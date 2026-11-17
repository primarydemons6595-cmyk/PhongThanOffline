// Named loose-only adapter. Never opens a PAK or an ID-addressed payload store.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

struct PtLooseEntry { unsigned long id; char path[MAX_PATH]; };
struct PtLooseState
{
    KPakList* owner;
    PtLooseEntry* entries;
    unsigned count;
    PtLooseState* next;
};
static PtLooseState* s_ptLooseStates = NULL;
static char s_ptLooseRoot[MAX_PATH] = "";
static bool s_ptLooseEnabled = false;
static unsigned long s_ptLooseMisses[4096];
static LONG s_ptLooseMissCount = 0;

static FILE* PtLooseLogFile(const char* name)
{
    if(PtDiagnostics::Quiet())return NULL;
    char root[MAX_PATH], path[MAX_PATH];
    g_GetRootPath(root);
    if (strlen(root) + strlen(name) + 2 >= sizeof(path)) return NULL;
    sprintf(path, "%s\\%s", root, name);
    return fopen(path, "a");
}

static void PtLooseEvent(const char* event, unsigned count)
{
    FILE* log = PtLooseLogFile("loose_loader.log");
    if (log)
    {
        SYSTEMTIME now; GetLocalTime(&now);
        fprintf(log, "%04u-%02u-%02u %02u:%02u:%02u pid=%lu %s count=%u pak_opened=0\n",
            now.wYear,now.wMonth,now.wDay,now.wHour,now.wMinute,now.wSecond,
            GetCurrentProcessId(),event,count);
        fclose(log);
    }
}

bool PtLooseOnlyEnabled()
{
    char root[MAX_PATH], marker[MAX_PATH];
    g_GetRootPath(root);
    if (strcmp(root,s_ptLooseRoot) != 0)
    {
        strcpy(s_ptLooseRoot,root);
        s_ptLooseEnabled = false;
        s_ptLooseMissCount = 0;
        if (strlen(root) + 18 < sizeof(marker))
        {
            sprintf(marker,"%s\\loose_only.flag",root);
            DWORD attr = GetFileAttributesA(marker);
            s_ptLooseEnabled = attr != (DWORD)-1 && !(attr & FILE_ATTRIBUTE_DIRECTORY);
        }
    }
    return s_ptLooseEnabled;
}

void PtLooseMissing(const char* caller, const char* path, unsigned long id)
{
    if (PtDiagnostics::Quiet() || !PtLooseOnlyEnabled()) return;
    unsigned long key = id ^ 2166136261UL;
    const unsigned char* p = (const unsigned char*)(path ? path : "");
    while (*p) { key = (key ^ *p++) * 16777619UL; }
    unsigned count=(unsigned)s_ptLooseMissCount;
    if (count>4096) count=4096;
    for (unsigned i=0;i<count;i++) if (s_ptLooseMisses[i]==key) return;
    if (count==4096) return;
    LONG slot=InterlockedIncrement(&s_ptLooseMissCount)-1;
    if (slot<0 || slot>=4096) return;
    s_ptLooseMisses[slot] = key;
    FILE* log = PtLooseLogFile("loose_missing_files.log");
    if (log)
    {
        SYSTEMTIME now; GetLocalTime(&now);
        fprintf(log,"%04u-%02u-%02u %02u:%02u:%02u pid=%lu caller=%s id=%08lX path=",
            now.wYear,now.wMonth,now.wDay,now.wHour,now.wMinute,now.wSecond,
            GetCurrentProcessId(),caller,id);
        p = (const unsigned char*)(path ? path : "<ID_ONLY>");
        while (*p) { if (*p<32 || *p==127) fprintf(log,"\\x%02X",*p); else fputc(*p,log); p++; }
        fprintf(log," result=MISSING no_pak_fallback=1\n");
        fclose(log);
    }
}

static PtLooseState* PtLooseStateFor(KPakList* owner)
{
    for (PtLooseState* s=s_ptLooseStates;s;s=s->next) if (s->owner==owner) return s;
    return NULL;
}

static void PtLooseClose(KPakList* owner)
{
    PtLooseState** link=&s_ptLooseStates;
    while (*link)
    {
        PtLooseState* state=*link;
        if (state->owner==owner)
        {
            *link=state->next;
            free(state->entries); free(state); return;
        }
        link=&state->next;
    }
}

static bool PtLooseSafePath(const char* path)
{
    if (path[0]!='\\' || !path[1] || path[1]=='\\') return false;
    const char* part=path+1;
    for (const unsigned char* p=(const unsigned char*)part;;p++)
    {
        if (*p=='\\' || !*p)
        {
            unsigned len=(const char*)p-part;
            if (!len || (len==1 && part[0]=='.') ||
                (len==2 && part[0]=='.' && part[1]=='.') ||
                part[len-1]=='.' || part[len-1]==' ') return false;
            if (!*p) break;
            part=(const char*)p+1;
        }
        else if (*p<32 || *p==127 || strchr(":/*?\"<>|",*p)) return false;
    }
    return true;
}

// ID-addressed fallback for payloads whose authentic filename was not
// recoverable. Offline recovery stores these as _loose_id\HH\HHHHHHHH.bin;
// the runtime still resolves the same native hash and never opens a PAK.
static bool PtLooseIdPath(unsigned long id,char* path)
{
    if (!path || !id) return false;
    sprintf(path,"\\_loose_id\\%02lX\\%08lX.bin",(id >> 24) & 0xFF,id);
    return true;
}

static bool PtLooseOpen(KPakList* owner)
{
    char root[MAX_PATH], full[MAX_PATH];
    g_GetRootPath(root);
    if (strlen(root)+20>=sizeof(full)) return false;
    sprintf(full,"%s\\loose_index.bin",root);
    FILE* file=fopen(full,"rb");
    if (!file) { PtLooseEvent("INDEX_MISSING",0); return false; }
    char magic[8]; unsigned count=0;
    PtLooseState* state=NULL;
    bool ok=fread(magic,1,8,file)==8 && memcmp(magic,"PTLOOSE1",8)==0 &&
        fread(&count,4,1,file)==1 && count>0 && count<=400000;
    if (ok)
    {
        state=(PtLooseState*)calloc(1,sizeof(PtLooseState));
        if (state) state->entries=(PtLooseEntry*)calloc(count,sizeof(PtLooseEntry));
        ok=state && state->entries;
    }
    for (unsigned i=0;ok && i<count;i++)
    {
        PtLooseEntry& entry=state->entries[i]; unsigned short len=0;
        ok=fread(&entry.id,4,1,file)==1 && fread(&len,2,1,file)==1 &&
            entry.id!=0 && len>1 && len<MAX_PATH && strlen(root)+len<MAX_PATH;
        if (ok) ok=fread(entry.path,1,len,file)==len;
        if (ok) ok=memchr(entry.path,0,len)==NULL && PtLooseSafePath(entry.path) &&
            (!i || entry.id>state->entries[i-1].id);
    }
    if (ok) ok=fgetc(file)==EOF && !ferror(file);
    fclose(file);
    if (!ok)
    {
        if (state) { free(state->entries); free(state); }
        PtLooseEvent("INDEX_INVALID_FAIL_CLOSED",count); return false;
    }
    state->owner=owner;state->count=count;state->next=s_ptLooseStates;s_ptLooseStates=state;
    PtLooseEvent("LOOSE_ONLY_READY",count);
    return true;
}

bool PtLooseAliasPath(KPakList* owner,const XPackElemFileRef& ref,char* path)
{
    if (ref.nPackIndex == -3)
        return PtLooseIdPath(ref.uId,path);
    PtLooseState* state=PtLooseStateFor(owner);
    if (!state || ref.nPackIndex!=-2 || ref.nElemIndex<0 ||
        (unsigned)ref.nElemIndex>=state->count || state->entries[ref.nElemIndex].id!=ref.uId)
        return false;
    strcpy(path,state->entries[ref.nElemIndex].path);
    return true;
}

static bool PtLooseFind(KPakList* owner,unsigned long id,XPackElemFileRef& ref)
{
    PtLooseState* state=PtLooseStateFor(owner);
    if (!state) { PtLooseMissing("FindElemFile/index_not_open",NULL,id); return false; }
    unsigned low=0,high=state->count;
    while (low<high)
    {
        unsigned mid=low+(high-low)/2;
        if (state->entries[mid].id<id) low=mid+1; else high=mid;
    }
    if (low==state->count || state->entries[low].id!=id)
    {
        char idPath[MAX_PATH],full[MAX_PATH]; long idSize=-1;
        if (PtLooseIdPath(id,idPath))
        {
            g_GetFullPath(full,idPath);
            FILE* idFile=fopen(full,"rb");
            if (idFile) { if (fseek(idFile,0,SEEK_END)==0) idSize=ftell(idFile); fclose(idFile); }
        }
        if (idSize >= 0)
        {
            memset(&ref,0,sizeof(ref));ref.uId=id;ref.nPackIndex=-3;
            ref.nElemIndex=-1;ref.nCacheIndex=-1;ref.nSize=(int)idSize;
            PtLooseEvent("ID_STORE_HIT",1);
            return true;
        }
        PtLooseMissing("FindElemFile/ID",NULL,id); return false;
    }
    char full[MAX_PATH];g_GetFullPath(full,state->entries[low].path);
    FILE* file=fopen(full,"rb");
    long size=-1;
    if (file) { if (fseek(file,0,SEEK_END)==0) size=ftell(file); fclose(file); }
    if (size<0) { PtLooseMissing("FindElemFile/file",state->entries[low].path,id); return false; }
    memset(&ref,0,sizeof(ref));ref.uId=id;ref.nPackIndex=-2;
    ref.nElemIndex=(int)low;ref.nCacheIndex=-1;ref.nSize=(int)size;
    return true;
}

static int PtLooseRead(KPakList* owner,XPackElemFileRef& ref,void* buffer,unsigned size)
{
    char path[MAX_PATH],full[MAX_PATH];
    if (!buffer || ref.nOffset<0 || ref.nOffset>ref.nSize || !PtLooseAliasPath(owner,ref,path)) return 0;
    unsigned remaining=(unsigned)(ref.nSize-ref.nOffset);
    if (size>remaining) size=remaining;
    g_GetFullPath(full,path);
    FILE* file=fopen(full,"rb");
    if (!file) { PtLooseMissing("ElemFileRead",path,ref.uId); return 0; }
    int read=0;
    if (fseek(file,ref.nOffset,SEEK_SET)==0) read=(int)fread(buffer,1,size,file);
    fclose(file);ref.nOffset+=read;
    return read;
}
