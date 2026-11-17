#include <windows.h>
#include <stdio.h>
#include "LegacyDropEngine.h"
typedef int (__cdecl *Preflight)(const char*,unsigned,unsigned long,int,int*,int*,unsigned long*,unsigned long*);
typedef int (__cdecl *ResolvePath)(const char*,unsigned,char*,unsigned);
int main(int argc,char** argv){
    if(argc!=4)return 2;SetErrorMode(SEM_FAILCRITICALERRORS|SEM_NOOPENFILEERRORBOX);
    HMODULE core=LoadLibraryA(argv[1]);if(!core){printf("CORE_LOAD_FAIL winerror=%lu\n",GetLastError());return 3;}
    Preflight preflight=(Preflight)GetProcAddress(core,"LegacyDropPreflight");
    ResolvePath resolve=(ResolvePath)GetProcAddress(core,"LegacyDropResolvePath");
    if(!preflight)preflight=(Preflight)GetProcAddress(core,"_LegacyDropPreflight");
    if(!resolve)resolve=(ResolvePath)GetProcAddress(core,"_LegacyDropResolvePath");
    if(!preflight||!resolve){puts("CORE_EXPORT_MISSING");return 4;}
    char pattern[2048];sprintf(pattern,"%s\\*.ini",argv[2]);WIN32_FIND_DATAA data;HANDLE handle=FindFirstFileA(pattern,&data);if(handle==INVALID_HANDLE_VALUE)return 5;
    int files=0,rejected=0,failed=0;
    do{if(data.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)continue;char file[2048];sprintf(file,"%s\\%s",argv[2],data.cFileName);FILE* f=fopen(file,"rb");if(!f)return 6;
        std::string bytes;char chunk[4096];size_t count;while((count=fread(chunk,1,sizeof(chunk),f))!=0)bytes.append(chunk,count);fclose(f);
        LegacyDrop::Table table=LegacyDrop::Parse(bytes);LegacyDrop::PreparedTable local;std::string error;bool valid=local.Prepare(table,error);
        int remoteCount=0,remoteRange=0;unsigned long remoteTotal=0,remoteTrace=0;
        int result=preflight(bytes.c_str(),bytes.size(),20261017,1000,&remoteCount,&remoteRange,&remoteTotal,&remoteTrace);
        if(result!=(valid?1:0)){printf("CORE_PARSE_MISMATCH file=%s result=%d\n",data.cFileName,result);++failed;}
        if(!valid){++rejected;}else{
            LegacyDrop::Random rng(20261017);std::vector<int> selected;unsigned long total=0,trace=2166136261UL;
            for(int d=0;d<1000;++d){if(!local.Draw(rng,selected,error)){++failed;break;}total+=selected.size();trace=(trace^(unsigned long)selected.size())*16777619UL;for(size_t i=0;i<selected.size();++i)trace=(trace^(unsigned long)(selected[i]+1))*16777619UL;}
            if(remoteCount!=table.count||remoteRange!=table.range||remoteTotal!=total||remoteTrace!=trace){printf("CORE_SELECTION_MISMATCH file=%s\n",data.cFileName);++failed;}
        }
        ++files;
    }while(FindNextFileA(handle,&data));FindClose(handle);
    FILE* refs=fopen(argv[3],"rb");if(!refs)return 7;int npc,slot,paths=0;char hex[2048],raw[1024];
    while(fscanf(refs,"%d %d %2047s",&npc,&slot,hex)==3){int length=strlen(hex)/2;if(length>=sizeof(raw))return 8;for(int k=0;k<length;++k){unsigned int byte;sscanf(hex+k*2,"%2x",&byte);raw[k]=(char)byte;}raw[length]=0;
        LegacyDrop::NpcSnapshot snapshot;snapshot.slots[slot]=raw;std::string expected;bool ok=LegacyDrop::Resolve(snapshot,slot,expected);char actual[1024];int result=resolve(raw,length,actual,sizeof(actual));
        if(result!=(ok?1:0)||(ok&&expected!=actual)){++failed;printf("CORE_RESOLVER_MISMATCH npc=%d slot=%d\n",npc,slot);}++paths;
    }fclose(refs);
    printf("CORE_SHARED_RESULT load=PASS files=%d rejected=%d resolved_paths=%d selection_draws_per_valid_table=1000 failed=%d no_world_init=1\n",files,rejected,paths,failed);
    FreeLibrary(core);return failed||files!=478?1:0;
}
