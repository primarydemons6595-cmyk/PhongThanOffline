// Durable native serial reservation. Item identities/catalog IDs are unchanged.
#ifndef PT_NATIVE_DROP_SERIAL_RESERVATION_H
#define PT_NATIVE_DROP_SERIAL_RESERVATION_H
#include "NativeDropRolloutScope.h"
namespace NativeDropRollout {
struct SerialState {bool initialized;DWORD limit;SerialState():initialized(false),limit(0){}};
inline bool ReserveSerial(const std::string& root,SerialState& state,DWORD& cursor,DWORD& serial,std::string& error){
 serial=0;
 if(!state.initialized||cursor>=state.limit){
  unsigned long hash=2166136261UL;for(size_t i=0;i<root.size();++i){unsigned char c=root[i];if(c>='A'&&c<='Z')c+='a'-'A';hash=(hash^c)*16777619UL;}
  char name[80];sprintf(name,"Local\\PhongThanSerialV1-%08lx",hash);HANDLE mutex=CreateMutexA(0,FALSE,name);if(!mutex){error="SERIAL_MUTEX_CREATE_FAILED";return false;}
  DWORD wait=WaitForSingleObject(mutex,5000);if(wait!=WAIT_OBJECT_0&&wait!=WAIT_ABANDONED){CloseHandle(mutex);error="SERIAL_MUTEX_TIMEOUT";return false;}
  DWORD previous=0;bool ok=ReadSerialCounter(root,previous);if(!ok)error="SERIAL_COUNTER_MISSING_OR_CORRUPT";
  if(ok){if(cursor<previous)cursor=previous;if(cursor<100||cursor>=0xfffffefeUL){ok=false;error="SERIAL_SPACE_EXHAUSTED";}}
  DWORD end=cursor+256;
  if(ok){
   DWORD fields[4]={0x3153444eUL,1,end,0};fields[3]=fields[0]^fields[1]^fields[2]^0xa55a5aa5UL;
   std::string file=root+"\\serial-reservation.v1",temp=file+".tmp";
   HANDLE f=CreateFileA(temp.c_str(),GENERIC_WRITE,0,0,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,0);DWORD written=0;
   ok=f!=INVALID_HANDLE_VALUE&&WriteFile(f,fields,sizeof(fields),&written,0)&&written==sizeof(fields)&&FlushFileBuffers(f);if(f!=INVALID_HANDLE_VALUE)CloseHandle(f);
   if(ok)ok=MoveFileExA(temp.c_str(),file.c_str(),MOVEFILE_REPLACE_EXISTING|MOVEFILE_WRITE_THROUGH)!=0;
   if(!ok){DeleteFileA(temp.c_str());error="SERIAL_RESERVATION_COMMIT_FAILED";}
   else{state.limit=end;state.initialized=true;}
  }
  ReleaseMutex(mutex);CloseHandle(mutex);if(!ok)return false;
 }
 if(cursor>=0xfffffffeUL){error="SERIAL_SPACE_EXHAUSTED";return false;}
 serial=cursor++;return true;
}
inline bool MainSerial(DWORD& cursor,DWORD& serial,std::string& error){
 std::string root;int admission=Root(root,error);if(admission<0)return false;
 if(!admission){serial=cursor++;return true;}
 static SerialState state;return ReserveSerial(root,state,cursor,serial,error);
}
}
#endif
