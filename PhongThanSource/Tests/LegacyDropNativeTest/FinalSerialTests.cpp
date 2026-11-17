#include "NativeDropSerialReservation.h"
#include <stdio.h>
static int checks=0,failed=0;
void Check(bool ok,const char*s){++checks;if(!ok){++failed;printf("FAIL %s\n",s);}}
void Write(const std::string& file,DWORD end){DWORD v[4]={0x3153444eUL,1,end,0};v[3]=v[0]^v[1]^v[2]^0xa55a5aa5UL;FILE*f=fopen(file.c_str(),"wb");if(f){fwrite(v,1,sizeof(v),f);fclose(f);}}
int main(int argc,char**argv){
 if(argc!=3)return 2;std::string root=argv[2],file=root+"\\serial-reservation.v1",error;CreateDirectoryA(root.c_str(),0);DWORD cursor=100,id=0;
 NativeDropRollout::SerialState state;
 if(!strcmp(argv[1],"write")){
  if(GetFileAttributesA(file.c_str())!=(DWORD)-1)return 3;Write(file,100);
  bool unique=true;DWORD last=0;for(int i=0;i<260;++i){if(!NativeDropRollout::ReserveSerial(root,state,cursor,id,error)||id<=last)unique=false;last=id;}
  Check(unique&&last==359,"native serials monotonic across reserved block boundary");DWORD end=0;Check(NativeDropRollout::ReadSerialCounter(root,end)&&end>last,"reservation durable before serial returned");
 }else{
  Check(NativeDropRollout::ReserveSerial(root,state,cursor,id,error)&&id>=612,"restart cannot reuse offline character serials");
  DWORD end=0;Check(NativeDropRollout::ReadSerialCounter(root,end),"valid counter");
  HANDLE lock=CreateFileA(file.c_str(),GENERIC_READ,0,0,OPEN_EXISTING,0,0);Check(lock!=INVALID_HANDLE_VALUE,"inject counter IO failure");NativeDropRollout::SerialState blocked;DWORD original=cursor;
  Check(!NativeDropRollout::ReserveSerial(root,blocked,cursor,id,error)&&!id&&cursor==original,"counter IO failure returns no serial");if(lock!=INVALID_HANDLE_VALUE)CloseHandle(lock);
  DWORD preserved=0;Check(NativeDropRollout::ReadSerialCounter(root,preserved)&&preserved==end,"counter failure preserves committed bound");
  std::string held=file+".held";Check(MoveFileA(file.c_str(),held.c_str())!=0,"hide reservation");Check(!NativeDropRollout::ReserveSerial(root,blocked,cursor,id,error)&&!id,"missing reservation fails closed");Check(MoveFileA(held.c_str(),file.c_str())!=0,"restore reservation");
  Write(file,99);Check(!NativeDropRollout::ReserveSerial(root,blocked,cursor,id,error)&&!id,"corrupt reservation rejected");Write(file,end);
  cursor=0xfffffefeUL;Check(!NativeDropRollout::ReserveSerial(root,blocked,cursor,id,error)&&!id,"overflow boundary rejects");cursor=100;
  Check(NativeDropRollout::ReserveSerial(root,blocked,cursor,id,error)&&id>=end,"recovery stays above persisted bound");
 }
 printf("FINAL_SERIAL_RESULT operation=%s pid=%lu checks=%d failed=%d item_generation=0 deaths=0\n",argv[1],GetCurrentProcessId(),checks,failed);return failed?1:0;
}
