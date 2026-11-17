#ifndef PHONGTHAN_BOT_INITIAL_STATE_H
#define PHONGTHAN_BOT_INITIAL_STATE_H
#include <vector>
#include <time.h>
#include "KWin32.h"
#include "KIniFile.h"
#include "../../../Headers/PhongThanCharacter.h"
#include "../../../Headers/PhongThanStarterWeapon.h"
#include "../../../Headers/PhongThanStarterSpawn.h"

// Uses Engine's resource reader: disk in Dev, authenticated PAK in Release.
// Character saves remain ordinary mutable files; never recreate a corrupt save.
inline bool PhongThanBuildBotInitialState(const char* id,const char* name,int profession,
    int gender,int map,int x,int y,std::vector<BYTE>& bytes,const char*& error)
{
    error="new_character_template_invalid";
    if(profession<0||profession>=PHONGTHAN_CREATABLE_PROFESSION_COUNT||gender<0||gender>1)return false;
    PHONGTHAN_CHARACTER_STATE_HEADER probe;ZeroMemory(&probe,sizeof(probe));probe.ItemCount=1;
    bytes.resize(PhongThanCharacterExpectedStateSize(&probe));ZeroMemory(&bytes[0],bytes.size());
    PHONGTHAN_CHARACTER_STATE_HEADER* s=(PHONGTHAN_CHARACTER_STATE_HEADER*)&bytes[0];
    s->SchemaVersion=1;s->StateSize=bytes.size();s->Revision=1;s->RoleTime=(DWORD)time(0);
    s->UseRevive=1;s->HelmResource=1;s->ItemCount=1;
    lstrcpynA((char*)s->RoleName,name,32);sprintf((char*)s->AccountName,"bot%.12s",id);
    if(!PhongThanInitializeStarterWeapon(*PhongThanCharacterItems(s),profession))return false;
    s->Gender=(BYTE)gender;s->Profession=(BYTE)profession;s->EnterMapId=map;s->EnterX=x;s->EnterY=y;
    s->FightLevel=s->LeadershipLevel=1;
    int home=0,hx=0,hy=0;if(!PhongThanStarterSpawn(profession,home,hx,hy))return false;
    s->ReviveMapId=home;s->ReviveX=hx;s->ReviveY=hy;
    char path[128];sprintf(path,"\\settings\\npc\\player\\newplayerini%02d.ini",profession*2+gender);
    KIniFile ini;if(!ini.Load(path)){error="new_character_template_missing";return false;}
    if(!ini.GetInteger("ROLE","ipower",0,&s->Power)||!ini.GetInteger("ROLE","iagility",0,&s->Agility)||
       !ini.GetInteger("ROLE","iouter",0,&s->Physique)||!ini.GetInteger("ROLE","iinside",0,&s->Wisdom)||
       !ini.GetInteger("ROLE","iluck",0,&s->Luck)||!ini.GetInteger("ROLE","imaxlife",0,&s->MaxLife)||
       !ini.GetInteger("ROLE","imaxinner",0,&s->MaxMana)||!ini.GetInteger("ROLE","imaxstamina",0,&s->MaxStamina)||
       s->MaxLife<=0||s->MaxMana<0||s->MaxStamina<0)return false;
    s->CurrentLife=s->MaxLife;s->CurrentMana=s->MaxMana;s->CurrentStamina=s->MaxStamina;
    if(!PhongThanValidateCharacterState(s,s->StateSize))return false;
    error=0;return true;
}
#endif
