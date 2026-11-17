#include <windows.h>
#include <initguid.h>
#include "IClient.h"
#include "PhongThanProtocol.h"
#include <stdio.h>
#include <string.h>
typedef HRESULT (WINAPI *CreateClient)(REFIID,void**);
int main(int argc,char**argv){
 if(argc!=2)return 1;
 HMODULE dll=LoadLibraryA("Rainbow.dll");if(!dll)return 2;
 CreateClient create=(CreateClient)GetProcAddress(dll,"CreateInterface");
 IClientFactory* factory=0;IClient* client=0;
 if(!create||FAILED(create(IID_IClientFactory,(void**)&factory)))return 3;
 factory->SetEnvironment(1024*1024);
 if(FAILED(factory->CreateClientInterface(IID_IESClient,(void**)&client)))return 4;
 factory->Release();client->Startup();
 if(FAILED(client->ConnectTo("127.0.0.1",5002)))return 5;
 Sleep(500);
 PHONGTHAN_SERVICE_GATEWAY_HELLO_REQUEST req;ZeroMemory(&req,sizeof(req));
 PhongThanInitializeWireHeader(&req.Header,PHONGTHAN_MSG_SERVICE_GATEWAY_HELLO,sizeof(req),PHONGTHAN_WIRE_FLAG_REQUEST,71030);
 req.RequestId=71030;req.Reconnect=1;
 strncpy((char*)req.GatewayName,argv[1],sizeof(req.GatewayName)-1);
 if(!GetEnvironmentVariableA("PT_GATEWAY_PROBE_PROOF",(char*)req.CredentialProof,sizeof(req.CredentialProof)))return 8;
 client->SendPackToServer(&req,sizeof(req));ZeroMemory(req.CredentialProof,sizeof(req.CredentialProof));
 DWORD start=GetTickCount();int result=6;
 while(GetTickCount()-start<10000){
  size_t size=0;const unsigned char* data=(const unsigned char*)client->GetPackFromServer(size);
  if(data&&size==sizeof(PHONGTHAN_SERVICE_GATEWAY_HELLO_RESPONSE)){
   const PHONGTHAN_SERVICE_GATEWAY_HELLO_RESPONSE*r=(const PHONGTHAN_SERVICE_GATEWAY_HELLO_RESPONSE*)data;
   if(PhongThanValidateWireHeader(&r->Header,size)&&r->RequestId==71030){printf("GATEWAY_HELLO name=%s result=%d game=%u elapsed=%lu\n",argv[1],r->Result,r->GameServiceId,GetTickCount()-start);result=r->Result?7:0;break;}
  }Sleep(10);
 }
 if(result==6)printf("GATEWAY_HELLO name=%s TIMEOUT\n",argv[1]);
 client->Shutdown();client->Cleanup();client->Release();return result;
}
