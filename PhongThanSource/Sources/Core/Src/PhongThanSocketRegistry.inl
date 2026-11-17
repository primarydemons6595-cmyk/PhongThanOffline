#include "../../../Headers/PhongThanSocketState.h"
int& PtSocketComposeMode(){static int mode=1;return mode;}
std::map<int,PT_SOCKET_STATE>& PtSocketCache(){static std::map<int,PT_SOCKET_STATE> cache;return cache;}
bool PtSocketRegister(int param,const PT_SOCKET_STATE& state){
 if(!PtSocketParam(param)||!PtSocketValid(state))return false;
 std::map<int,PT_SOCKET_STATE>& cache=PtSocketCache();std::map<int,PT_SOCKET_STATE>::iterator found=cache.find(param);
 if(found!=cache.end())return memcmp(&found->second,&state,sizeof(state))==0;
 cache[param]=state;return true;
}
#ifdef _SERVER
struct PtSocketDisk {unsigned Magic,Version,Hash;int Param;PT_SOCKET_STATE State;};
static void PtSocketFile(int param,char* path){sprintf(path,"settings\\phongthan\\socket_states\\%08X.bin",(unsigned)param);}
#endif
bool PtSocketRead(int param,PT_SOCKET_STATE& state){
 if(!PtSocketParam(param))return false;
 std::map<int,PT_SOCKET_STATE>::iterator found=PtSocketCache().find(param);
 if(found!=PtSocketCache().end()){state=found->second;return true;}
#ifdef _SERVER
 char path[128];PtSocketFile(param,path);FILE* f=fopen(path,"rb");if(!f)return false;
 PtSocketDisk d;bool ok=fread(&d,1,sizeof(d),f)==sizeof(d)&&fgetc(f)==EOF;fclose(f);
 if(!ok||d.Magic!=0x53535450||d.Version!=1||d.Param!=param||d.Hash!=PtSocketHash(&d.State,sizeof(d.State))||!PtSocketRegister(param,d.State))return false;
 state=d.State;return true;
#else
 return false;
#endif
}
bool PtSocketStore(const PT_SOCKET_STATE& state,int& param){
#ifdef _SERVER
 if(!PtSocketValid(state))return false;
 CreateDirectoryA("settings\\phongthan\\socket_states",0);
 unsigned key=PtSocketHash(&state,sizeof(state))&0xffffffU;
 for(unsigned probe=0;probe<65536;++probe){
  param=(int)(0xc2000000U|((key+probe)&0xffffffU));char path[128];PtSocketFile(param,path);
  if(GetFileAttributesA(path)!=(DWORD)-1){PT_SOCKET_STATE old;if(!PtSocketRead(param,old))return false;if(!memcmp(&old,&state,sizeof(state)))return true;continue;}
  PtSocketDisk d;ZeroMemory(&d,sizeof(d));d.Magic=0x53535450;d.Version=1;d.Param=param;d.State=state;d.Hash=PtSocketHash(&state,sizeof(state));
  char temporary[160];sprintf(temporary,"%s.tmp.%lu",path,GetCurrentProcessId());
  HANDLE f=CreateFileA(temporary,GENERIC_WRITE,0,0,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,0);if(f==INVALID_HANDLE_VALUE)return false;
  DWORD wrote=0;bool ok=WriteFile(f,&d,sizeof(d),&wrote,0)&&wrote==sizeof(d)&&FlushFileBuffers(f);CloseHandle(f);
  if(ok)ok=MoveFileExA(temporary,path,MOVEFILE_WRITE_THROUGH)!=0;
  if(!ok){DeleteFileA(temporary);return false;}
  return PtSocketRegister(param,state);
 }return false;
#else
 return false;
#endif
}
