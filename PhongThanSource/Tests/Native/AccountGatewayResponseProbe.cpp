#include <windows.h>
#include <initguid.h>
#include "IClient.h"
#include "PhongThanProtocol.h"
#include <stdio.h>
#include <string.h>
typedef HRESULT (WINAPI *CreateClient)(REFIID, void**);
int main(int argc,char** argv){
    bool listOnly=argc>2&&!strcmp(argv[2],"--character-list"),authenticated=false;
    HMODULE dll=LoadLibraryA("Rainbow.dll");if(!dll)return 2;
    CreateClient create=(CreateClient)GetProcAddress(dll,"CreateInterface");
    IClientFactory* factory=0;IClient* client=0;
    if(!create||FAILED(create(IID_IClientFactory,(void**)&factory)))return 3;
    factory->SetEnvironment(1024*1024);
    if(FAILED(factory->CreateClientInterface(IID_IESClient,(void**)&client)))return 4;
    factory->Release();client->Startup();
    unsigned short port=argc>1?(unsigned short)atoi(argv[1]):5622;
    if(FAILED(client->ConnectTo("127.0.0.1",port)))return 5;
    Sleep(500);
    PHONGTHAN_SESSION_AUTHENTICATE_REQUEST request;ZeroMemory(&request,sizeof(request));
    PhongThanInitializeWireHeader(&request.Header,PHONGTHAN_MSG_SESSION_AUTHENTICATE,
        sizeof(request),PHONGTHAN_WIRE_FLAG_REQUEST,9018);
    // Deliberately nonexistent identity: do not log in or mutate any player account.
    strcpy((char*)request.AccountName,"ptprobe9018");
    strcpy((char*)request.PasswordProof,"00000000000000000000000000000000");
    if(listOnly){
        ZeroMemory(request.AccountName,sizeof(request.AccountName));ZeroMemory(request.PasswordProof,sizeof(request.PasswordProof));
        if(!GetEnvironmentVariableA("PT_TEST_ACCOUNT",(char*)request.AccountName,sizeof(request.AccountName))||
            !GetEnvironmentVariableA("PT_TEST_PROOF",(char*)request.PasswordProof,sizeof(request.PasswordProof)))return 8;
    }
    DWORD start=GetTickCount();client->SendPackToServer(&request,sizeof(request));int result=6;
    ZeroMemory(request.PasswordProof,sizeof(request.PasswordProof));
    while(GetTickCount()-start<15000){
        size_t size=0;const unsigned char* data=(const unsigned char*)client->GetPackFromServer(size);
        size_t offset=0;
        while(data&&offset<=size&&size-offset>=sizeof(PHONGTHAN_WIRE_HEADER)){
            const PHONGTHAN_WIRE_HEADER* h=(const PHONGTHAN_WIRE_HEADER*)(data+offset);
            if(!PhongThanValidateWireHeader(h,size-offset)||h->PacketSize<sizeof(*h))break;
            if(h->MessageType==PHONGTHAN_MSG_SESSION_AUTHENTICATE&&h->PacketSize==sizeof(PHONGTHAN_SESSION_AUTHENTICATE_RESPONSE)){
                const PHONGTHAN_SESSION_AUTHENTICATE_RESPONSE* response=(const PHONGTHAN_SESSION_AUTHENTICATE_RESPONSE*)h;
                if(listOnly){
                    authenticated=response->Result==PHONGTHAN_AUTH_SUCCESS&&h->Flags==PHONGTHAN_WIRE_FLAG_RESPONSE&&
                        !memcmp(response->AccountName,request.AccountName,sizeof(request.AccountName));
                    printf("CHARACTER_LIST_AUTH result=%d elapsed_ms=%lu\n",response->Result,GetTickCount()-start);
                    if(!authenticated){result=9;goto done;}
                    offset+=h->PacketSize;continue;
                }
                bool valid=h->Flags==PHONGTHAN_WIRE_FLAG_RESPONSE&&response->Result==PHONGTHAN_AUTH_INVALID_CREDENTIALS&&
                    !memcmp(response->AccountName,request.AccountName,sizeof(request.AccountName));
                printf("GATEWAY_AUTH_RESPONSE result=%d expected_invalid_credentials=%d elapsed_ms=%lu valid=%d\n",
                    response->Result,PHONGTHAN_AUTH_INVALID_CREDENTIALS,GetTickCount()-start,valid?1:0);
                result=valid?0:7;goto done;
            }
            if(listOnly&&authenticated&&h->MessageType==PHONGTHAN_MSG_SESSION_CHARACTER_LIST&&h->PacketSize==sizeof(PHONGTHAN_SESSION_CHARACTER_LIST_RESPONSE)){
                const PHONGTHAN_SESSION_CHARACTER_LIST_RESPONSE* response=(const PHONGTHAN_SESSION_CHARACTER_LIST_RESPONSE*)h;
                result=response->CharacterCount<=3&&h->Flags==PHONGTHAN_WIRE_FLAG_RESPONSE?0:10;
                printf("CHARACTER_LIST_RESPONSE count=%u elapsed_ms=%lu valid=%d\n",response->CharacterCount,GetTickCount()-start,result==0?1:0);goto done;
            }
            offset+=h->PacketSize;
        }
        Sleep(10);
    }
    printf("TIMEOUT_GATEWAY_AUTH_RESPONSE\n");
done:
    client->Shutdown();client->Cleanup();client->Release();return result;
}
