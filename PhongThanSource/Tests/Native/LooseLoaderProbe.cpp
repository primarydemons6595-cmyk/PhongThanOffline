#include "KWin32.h"
#include "KFilePath.h"
#include "KPakList.h"
#include "KPakFile.h"
#include "KIniFile.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(x) do { if (!(x)) { fprintf(stderr,"FAIL line=%d expression=%s\n",__LINE__,#x); return 1; } } while (0)

int main(int argc,char** argv)
{
    if (argc!=4) return 2;
    g_SetRootPath(argv[1]);g_SetFilePath("\\");
    KPakList list;
    if (!strcmp(argv[3],"reject"))
    { CHECK(!list.Open("\\package.ini")); puts("PASS invalid index rejected without PAK fallback"); return 0; }
    CHECK(list.Open("\\package.ini"));
    XPackElemFileRef ref;
    CHECK(list.FindElemFile(argv[2],ref));
    bool packed=!strcmp(argv[3],"pak");
    CHECK(packed ? ref.nPackIndex>=0 : ref.nPackIndex==-2);
    CHECK(ref.nSize>4 && ref.nSize<16*1024*1024);
    unsigned size=ref.nSize;
    char* indexed=(char*)malloc(size);
    char* named=(char*)malloc(size);
    CHECK(indexed && named);
    CHECK(list.ElemFileRead(ref,indexed,3)==3);
    CHECK(ref.nOffset==3);
    CHECK(list.ElemFileRead(ref,indexed+3,size-3)==(int)size-3);
    CHECK(list.ElemFileRead(ref,indexed,1)==0);
    XPackElemFileRef byId;
    CHECK(list.FindElemFile(ref.uId,byId));
    CHECK(byId.nOffset==0 && byId.nSize==(int)size);
    KPakFile file;
    g_SetPakFileMode(1); // server's PAK-first setting cannot override loose-only.
    CHECK(file.Open(argv[2]));
    CHECK(file.IsFileInPak()==packed);
    CHECK(file.Size()==size);
    CHECK(file.Read(named,size)==size);
    CHECK(!memcmp(indexed,named,size));
    CHECK(file.Seek(1,FILE_BEGIN)==1 && file.Tell()==1);
    CHECK(file.Read(named,3)==3 && !memcmp(indexed+1,named,3));
    KIniFile ini;
    CHECK(ini.LoadPakEntry(ref.uId));
    CHECK(!list.FindElemFile(0xDEADBEEFUL,byId));
    CHECK(!file.Open("\\settings\\__loose_probe_missing__.ini"));
    free(indexed);free(named);
    puts(packed ? "PASS legacy PAK mode unchanged" : "PASS named/ID/INI/read/seek/missing/no-PAK-fallback");
    return 0;
}
