#include "../../Sources/MultiServer/Bishop/stdafx.h"
#include "../../Sources/MultiServer/Bishop/PlayerCreator.h"
#include "../../Sources/Engine/Src/KWin32.h"
#include "../../Sources/Engine/Src/KIniFile.h"
#include "../../Sources/Engine/Src/KFilePath.h"
#include "../../Headers/PhongThanSpawn.h"
#include <initguid.h>
#include "IClient.h"

static void CALLBACK ServiceEvent(void* context,const unsigned long& event) {
    InterlockedExchange((LONG*)context,event==enumServerConnectCreate?1:-1);
}
class Service {
    IClient* client; volatile LONG connected;
public:
    Service():client(0),connected(0){}
    ~Service(){if(client){client->Shutdown();client->Cleanup();client->Release();}}
    bool Open(){
        HMODULE dll=LoadLibraryA("Rainbow.dll");if(!dll)return false;
        typedef HRESULT(WINAPI* Factory)(REFIID,void**);
        Factory create=(Factory)GetProcAddress(dll,"CreateInterface");IClientFactory* factory=0;
        if(!create||FAILED(create(IID_IClientFactory,(void**)&factory)))return false;
        factory->SetEnvironment(512*1024);
        HRESULT result=factory->CreateClientInterface(IID_IESClient,(void**)&client);factory->Release();
        if(FAILED(result)||!client||FAILED(client->Startup()))return false;
        client->RegisterMsgFilter((void*)&connected,ServiceEvent);
        if(FAILED(client->ConnectTo("127.0.0.1",5001)))return false;
        for(int i=0;!connected&&i<1000;++i)Sleep(10);return connected==1;
    }
    bool Exchange(const void* request,unsigned size,unsigned short type,void* response,unsigned expected){
        if(FAILED(client->SendPackToServer(request,size)))return false;
        for(int i=0;i<1500;++i){size_t length=0;const void* data=client->GetPackFromServer(length);
            if(data&&length){const PHONGTHAN_WIRE_HEADER* header=(const PHONGTHAN_WIRE_HEADER*)data;
                if(length!=expected||!PhongThanValidateWireHeader(header,length)||header->MessageType!=type)return false;
                memcpy(response,data,length);return true;}
            Sleep(10);
        }return false;
    }
};

static bool Persist(Service& service,const PHONGTHAN_CHARACTER_STATE_HEADER& state,int index) {
    PHONGTHAN_SERVICE_CHARACTER_CREATE_REQUEST request;ZeroMemory(&request,sizeof(request));
    PhongThanInitializeWireHeader(&request.Header,PHONGTHAN_MSG_SERVICE_CHARACTER_CREATE,sizeof(request),PHONGTHAN_WIRE_FLAG_REQUEST,index+1);
    request.RequestId=index+1;
    sprintf((char*)request.AccountName,"SecureTest%lu_%d",GetCurrentProcessId(),index);
    sprintf((char*)request.RoleName,"PakTest%lu_%d",GetCurrentProcessId(),index);
    request.Gender=state.Gender;request.Profession=state.Profession;request.Portrait=1;request.NativePlaceId=1001;
    request.SpawnMapId=state.ReviveMapId;request.SpawnX=state.ReviveX;request.SpawnY=state.ReviveY;
    request.FightLevel=state.FightLevel;request.MaxLife=state.MaxLife;request.MaxStamina=state.MaxStamina;request.MaxMana=state.MaxMana;
    request.Power=state.Power;request.Agility=state.Agility;request.Physique=state.Physique;request.Wisdom=state.Wisdom;request.Luck=state.Luck;
    request.RemainingAttributePoints=state.RemainingAttributePoints;request.RemainingSkillPoints=state.RemainingSkillPoints;
    request.LeadershipLevel=state.LeadershipLevel;
    request.SkillCount=state.FightSkillCount;const PHONGTHAN_CHARACTER_SKILL_RECORD* skills=(const PHONGTHAN_CHARACTER_SKILL_RECORD*)(&state+1);
    for(int i=0;i<request.SkillCount;++i){request.Skills[i].SkillId=skills[i].SkillId;request.Skills[i].SkillLevel=skills[i].Level;request.Skills[i].SkillValue=skills[i].Value;}
    PHONGTHAN_SERVICE_CHARACTER_CREATE_RESPONSE result;
    if(!service.Exchange(&request,sizeof(request),PHONGTHAN_MSG_SERVICE_CHARACTER_CREATE,&result,sizeof(result))||result.Result!=PHONGTHAN_CHARACTER_OPERATION_SUCCESS)return false;
    // A repeated request must retain real duplicate-name protection.
    if(!service.Exchange(&request,sizeof(request),PHONGTHAN_MSG_SERVICE_CHARACTER_CREATE,&result,sizeof(result))||result.Result!=PHONGTHAN_CHARACTER_OPERATION_NAME_IN_USE)return false;
    PHONGTHAN_SERVICE_CHARACTER_LIST_REQUEST list;ZeroMemory(&list,sizeof(list));
    PhongThanInitializeWireHeader(&list.Header,PHONGTHAN_MSG_SERVICE_CHARACTER_LIST,sizeof(list),PHONGTHAN_WIRE_FLAG_REQUEST,index+1);
    list.RequestId=request.RequestId;memcpy(list.AccountName,request.AccountName,sizeof(list.AccountName));
    PHONGTHAN_SERVICE_CHARACTER_LIST_RESPONSE listed;
    if(!service.Exchange(&list,sizeof(list),PHONGTHAN_MSG_SERVICE_CHARACTER_LIST,&listed,sizeof(listed))||listed.Result||listed.CharacterCount!=1||strcmp((char*)listed.Characters[0].Name,(char*)request.RoleName))return false;
    PHONGTHAN_SERVICE_CHARACTER_DELETE_REQUEST remove;ZeroMemory(&remove,sizeof(remove));
    PhongThanInitializeWireHeader(&remove.Header,PHONGTHAN_MSG_SERVICE_CHARACTER_DELETE,sizeof(remove),PHONGTHAN_WIRE_FLAG_REQUEST,index+1);
    remove.RequestId=request.RequestId;memcpy(remove.AccountName,request.AccountName,sizeof(remove.AccountName));memcpy(remove.RoleName,request.RoleName,sizeof(remove.RoleName));
    PHONGTHAN_SERVICE_CHARACTER_DELETE_RESPONSE removed;
    if(!service.Exchange(&remove,sizeof(remove),PHONGTHAN_MSG_SERVICE_CHARACTER_DELETE,&removed,sizeof(removed))||removed.Result)return false;
    printf("PASS service create/duplicate/list/delete profession=%d gender=%d\n",state.Profession,state.Gender);return true;
}

int main(int argc,char** argv) {
    if(argc<2 || !SetCurrentDirectoryA(argv[1])) return 2;
#ifdef PT_SECURE_RELEASE_PROFILE
    if(!PtReleaseMount()) return 3;
#else
    g_SetRootPath(argv[1]);
#endif
    Service service;bool persist=argc>2&&!strcmp(argv[2],"--service");
    if(persist&&!service.Open())return 4;
    CPlayerCreator creator;
    for(int profession=0;profession<PHONGTHAN_CREATABLE_PROFESSION_COUNT;++profession)
    for(int gender=0;gender<PHONGTHAN_GENDER_COUNT;++gender) {
        CPlayerCreator::ROLEPARAM request; ZeroMemory(&request,sizeof(request));
        strcpy(request.szAccName,"SecureCreationTest"); strcpy(request.szName,"SecureRoleTest");
        request.nProfession=profession; request.nSex=gender; request.nMapID=1001;
        unsigned int length=0;
        char path[128];sprintf(path,"\\settings\\npc\\player\\newplayerini%02d.ini",profession*2+gender);
        KIniFile source;if(!source.Load(path)) return 20;
        POINT spawn;if(!PhongThanResolveRevivalPoint(1001,0,&spawn)) {puts("FAIL packed revival point");return 22;}
        const PHONGTHAN_CHARACTER_STATE_HEADER* state=creator.GetRoleData(length,&request);
        if(!state || !PhongThanValidateCharacterState(state,length)) {printf("FAIL template profession=%d gender=%d bytes=%u\n",profession,gender,length);return 10+profession*2+gender;}
        int expected=0;source.GetInteger("ROLE","imaxlife",0,&expected);
        if(expected<=0 || state->MaxLife!=expected || state->FightLevel!=1 || state->ReviveX<=0 || state->ReviveY<=0) return 21;
        if(persist&&!Persist(service,*state,profession*2+gender))return 30+profession*2+gender;
        printf("PASS profession=%d gender=%d life=%d map=%u spawn=%d,%d skills=%u\n",profession,gender,state->MaxLife,state->ReviveMapId,state->ReviveX,state->ReviveY,state->FightSkillCount);
    }
    puts("SECURE_CHARACTER_TEMPLATES_PASS cases=6");return 0;
}
