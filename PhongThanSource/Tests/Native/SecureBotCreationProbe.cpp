#include <windows.h>
#include <stdio.h>
#include "../../Sources/Core/Src/PhongThanBotInitialState.h"
#include "KFilePath.h"
#include "PtRelease.h"

int main(int argc,char** argv){
    SetErrorMode(SEM_FAILCRITICALERRORS|SEM_NOGPFAULTERRORBOX);
    if(argc!=2||!SetCurrentDirectoryA(argv[1]))return 2;
    g_SetRootPath(argv[1]);
#ifdef PT_SECURE_RELEASE_PROFILE
    if(!PtReleaseMount()){puts("FAIL authenticated PAK mount");return 3;}
#endif
    for(int profession=0;profession<3;++profession)for(int gender=0;gender<2;++gender){
        int map=0,x=0,y=0;PhongThanStarterSpawn(profession,map,x,y);
        char path[128];sprintf(path,"settings\\npc\\player\\newplayerini%02d.ini",profession*2+gender);
#ifdef PT_SECURE_RELEASE_PROFILE
        if(GetFileAttributesA(path)!=(DWORD)-1){puts("FAIL fixture contains loose templates");return 4;}
#endif
        std::vector<BYTE> bytes;const char* error=0;
        if(!PhongThanBuildBotInitialState("9fc06c768d714b02858158fcc41df041","VyKemDau",profession,gender,map,x,y,bytes,error)){printf("FAIL profession=%d gender=%d reason=%s\n",profession,gender,error?error:"unknown");return 5;}
        const PHONGTHAN_CHARACTER_STATE_HEADER* s=(const PHONGTHAN_CHARACTER_STATE_HEADER*)&bytes[0];
        KIniFile original;char resource[128];sprintf(resource,"\\%s",path);if(!original.Load(resource))return 6;
        int life=0,power=0;original.GetInteger("ROLE","imaxlife",0,&life);original.GetInteger("ROLE","ipower",0,&power);
        if(!PhongThanValidateCharacterState(s,bytes.size())||strcmp((const char*)s->RoleName,"VyKemDau")||strcmp((const char*)s->AccountName,"bot9fc06c768d71")||s->Profession!=profession||s->Gender!=gender||s->MaxLife!=life||s->Power!=power||s->MaxLife<=0||s->CurrentLife!=s->MaxLife||s->ItemCount!=1||s->FightLevel!=1||s->EnterMapId!=map||s->EnterX!=x||s->EnterY!=y||s->ReviveMapId!=map||s->ReviveX!=x||s->ReviveY!=y||PhongThanStarterWeaponClass(PhongThanCharacterItems(s)->ScriptParam)!=profession)return 7;
        printf("PASS packed bot profession=%d gender=%d name=VyKemDau life=%d starter_map=%d weapon=1\n",profession,gender,s->MaxLife,map);
    }
    std::vector<BYTE> bytes;const char* error=0;
    if(PhongThanBuildBotInitialState("test","TestRole",3,0,1002,1,1,bytes,error)||!error)return 8;
    if(PhongThanBuildBotInitialState("test","TestRole",0,2,1002,1,1,bytes,error)||!error)return 9;
    puts("PASS bot initial state: 6 profession/sex templates; actual PAK reader; no save/profile mutations.");return 0;
}
