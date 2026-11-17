#include "../../../Headers/PhongThanSoulState.h"
int& PtSoulUiNonce(){static int nonce=0;return nonce;}
static std::map<int,PT_SOUL_STATE>& PtSoulCache(){static std::map<int,PT_SOUL_STATE> cache;return cache;}
bool PtSoulRegister(int param,const PT_SOUL_STATE& state){
 if(!PtSoulParam(param)||!PtSoulValid(state))return false;
 std::map<int,PT_SOUL_STATE>& cache=PtSoulCache();std::map<int,PT_SOUL_STATE>::iterator found=cache.find(param);
 if(found!=cache.end())return !memcmp(&found->second,&state,sizeof(state));cache[param]=state;return true;
}
#ifdef _SERVER
struct PtSoulDisk {unsigned Magic,Version,Hash;int Param;PT_SOUL_STATE State;};
static void PtSoulFile(int param,char* path){sprintf(path,"settings\\phongthan\\soul_states\\%08X.bin",(unsigned)param);}
#endif
bool PtSoulRead(int param,PT_SOUL_STATE& state){
 if(!PtSoulParam(param))return false;
 std::map<int,PT_SOUL_STATE>::iterator found=PtSoulCache().find(param);if(found!=PtSoulCache().end()){state=found->second;return true;}
#ifdef _SERVER
 char path[128];PtSoulFile(param,path);FILE* file=fopen(path,"rb");if(!file)return false;
 PtSoulDisk disk;bool ok=fread(&disk,1,sizeof(disk),file)==sizeof(disk)&&fgetc(file)==EOF;fclose(file);
 if(!ok||disk.Magic!=0x4c535450||disk.Version!=2||disk.Param!=param||disk.Hash!=PtSocketHash(&disk.State,sizeof(disk.State))||!PtSoulRegister(param,disk.State))return false;
 state=disk.State;return true;
#else
 return false;
#endif
}
bool PtSoulStore(const PT_SOUL_STATE& state,int& param){
#ifdef _SERVER
 if(!PtSoulValid(state))return false;
 CreateDirectoryA("settings\\phongthan\\soul_states",0);unsigned key=PtSocketHash(&state,sizeof(state))&0xffffffU;
 for(unsigned probe=0;probe<65536;++probe){
  param=(int)(0xc3000000U|((key+probe)&0xffffffU));char path[128];PtSoulFile(param,path);
  if(GetFileAttributesA(path)!=(DWORD)-1){PT_SOUL_STATE old;if(!PtSoulRead(param,old))return false;if(!memcmp(&old,&state,sizeof(state)))return true;continue;}
  PtSoulDisk disk;ZeroMemory(&disk,sizeof(disk));disk.Magic=0x4c535450;disk.Version=2;disk.Param=param;disk.State=state;disk.Hash=PtSocketHash(&state,sizeof(state));
  char temporary[160];sprintf(temporary,"%s.tmp.%lu",path,GetCurrentProcessId());HANDLE file=CreateFileA(temporary,GENERIC_WRITE,0,0,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,0);if(file==INVALID_HANDLE_VALUE)return false;
  DWORD wrote=0;bool ok=WriteFile(file,&disk,sizeof(disk),&wrote,0)&&wrote==sizeof(disk)&&FlushFileBuffers(file);CloseHandle(file);if(ok)ok=MoveFileExA(temporary,path,MOVEFILE_WRITE_THROUGH)!=0;
  if(!ok){DeleteFileA(temporary);return false;}return PtSoulRegister(param,state);
 }return false;
#else
 return false;
#endif
}
bool PtSoulReplace(int original,const PT_SOUL_STATE& state,int& param){
 if(!PtSoulStore(state,param))return false;
 if(PtSocketParam(original)){PT_SOCKET_STATE sockets;if(!PtSocketRead(original,sockets))return false;sockets.BaseParam=param;return PtSocketStore(sockets,param);}
 return true;
}
