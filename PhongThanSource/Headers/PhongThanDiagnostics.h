#ifndef PHONGTHAN_DIAGNOSTICS_H
#define PHONGTHAN_DIAGNOSTICS_H
#include <windows.h>
#include <stdio.h>
#include <string.h>
// FILE* stays inside the caller's CRT. Disabled traces use NUL so legacy
// callers that require a valid stream keep working without creating files.
namespace PtDiagnostics {
inline bool Contains(const char* text,const char* part){
 if(!text||!part)return false;size_t n=strlen(part);for(;*text;++text)if(!_strnicmp(text,part,n))return true;return false;
}
inline bool DataPath(const char* path){
 return Contains(path,"CharacterStore")||Contains(path,"NativeDropPersistence")||Contains(path,"AccountStore")||Contains(path,"PetPairCommits")||Contains(path,".pthc")||Contains(path,"\\settings\\")||Contains(path,"/settings/")||Contains(path,"\\data\\")||Contains(path,"/data/");
}
inline bool LogPath(const char* path){
 if(!path||DataPath(path))return false;
 return Contains(path,".log")||Contains(path,".jsonl")||Contains(path,".trace")||
  (Contains(path,".csv")&&(Contains(path,"diag")||Contains(path,"trace")||Contains(path,"audit")));
}
inline bool ErrorPath(const char* path){return Contains(path,"runtime_errors.log")||Contains(path,"native_drop_serial_errors.log")||Contains(path,"fatal_errors.log")||Contains(path,"native_world_ready.log")||Contains(path,"reload.errors.log");}
struct Policy{bool scoped,enabled;DWORD maxBytes;char root[MAX_PATH];};
inline void Load(Policy& p){
 p.maxBytes=1048576;
 DWORD n=GetModuleFileNameA(0,p.root,sizeof(p.root));if(!n||n>=sizeof(p.root))return;
 char* slash=strrchr(p.root,'\\');if(!slash)return;*slash=0;
 const char* role=strrchr(p.root,'\\');if(!role||(_stricmp(role+1,"Server")&&_stricmp(role+1,"Client")))return;
 if(!Contains(p.root,"\\PhongThanRuntime-Loose\\"))return;
 char path[MAX_PATH];if(strlen(p.root)+48>=sizeof(path))return;
 sprintf(path,"%s\\loose_only.flag",p.root);DWORD attributes=GetFileAttributesA(path);
 if(attributes==(DWORD)-1||(attributes&FILE_ATTRIBUTE_DIRECTORY))return;
 p.scoped=true;sprintf(path,"%s\\settings\\phongthan\\Diagnostics.ini",p.root);
 p.enabled=GetPrivateProfileIntA("Diagnostics","Enabled",0,path)==1;
 DWORD limit=GetPrivateProfileIntA("Diagnostics","MaxFileBytes",1048576,path);
 if(limit>=65536&&limit<=8388608)p.maxBytes=limit;return;
}
inline Policy& Current(){
 static Policy p;static volatile LONG initialized=0;
 if(initialized==2)return p;
 if(InterlockedCompareExchange((PVOID*)&initialized,(PVOID)1,0)==0){Load(p);InterlockedExchange((LONG*)&initialized,2);}
 else while(initialized!=2)Sleep(0);
 return p;
}
inline bool Quiet(){Policy& p=Current();return p.scoped&&!p.enabled;}
inline void Rotate(const char* path,DWORD limit){
 WIN32_FILE_ATTRIBUTE_DATA data;if(!GetFileAttributesExA(path,GetFileExInfoStandard,&data)||data.dwFileAttributes&(FILE_ATTRIBUTE_DIRECTORY|FILE_ATTRIBUTE_REPARSE_POINT))return;
 if(!data.nFileSizeHigh&&data.nFileSizeLow<limit)return;
 char previous[MAX_PATH];if(strlen(path)+3>=sizeof(previous))return;sprintf(previous,"%s.1",path);
 MoveFileExA(path,previous,MOVEFILE_REPLACE_EXISTING|MOVEFILE_WRITE_THROUGH);
}
}
inline FILE* PtDiagnosticsFopen(const char* path,const char* mode){
 if(!path||!mode)return fopen(path,mode);
 PtDiagnostics::Policy& policy=PtDiagnostics::Current();
 bool writing=strchr(mode,'a')||strchr(mode,'w')||strchr(mode,'+');
 if(policy.scoped&&writing&&PtDiagnostics::LogPath(path)){
  if(!policy.enabled&&!PtDiagnostics::ErrorPath(path))return fopen("NUL",mode);
  PtDiagnostics::Rotate(path,policy.maxBytes);
 }
 return fopen(path,mode);
}
inline void PtRuntimeError(const char* component,const char* message){
 if(!PtDiagnostics::Current().scoped)return;
 FILE* f=PtDiagnosticsFopen("runtime_errors.log","a");if(!f)return;
 SYSTEMTIME t;GetLocalTime(&t);fprintf(f,"%04u-%02u-%02u %02u:%02u:%02u [%s] %s\n",t.wYear,t.wMonth,t.wDay,t.wHour,t.wMinute,t.wSecond,component?component:"runtime",message?message:"");fclose(f);
}
#endif
