#ifndef PHONGTHAN_STATUS_PAGES_H
#define PHONGTHAN_STATUS_PAGES_H
#include "PhongThanWorldProtocol.h"
#include <string.h>
#include "PhongThanPet.h"
enum {PT_STATUS_PAGES_QUERY=0x4121,PT_STATUS_PAGES_RESPONSE=0x4021,
 PT_VIEW_PROFILE_GET=0x50545650,PT_STATUS_PAGES_GET=0x50545350,PT_STATUS_PAGES_ACTION=0x50545351};
enum {PT_STATUS_READ=0,PT_STATUS_SAVE_PROFILE=1,PT_STATUS_SELECT_TITLE=2,
 PT_STATUS_HIDE_TITLE=3,PT_STATUS_CANCEL_ITEM_BUFF=4,PT_STATUS_SUMMON_LOCK=16,PT_STATUS_LIFE_OPEN=32,PT_STATUS_LIFE_MAKE=33,PT_STATUS_SOUL_OPEN=34};
#pragma pack(push,1)
struct PT_PERSONAL_PROFILE {
 int Sex,Age,BirthMonth,BirthDay,Star,Activity,Hour;
 char Province[32],Relationship[32],Memo[160];
};
struct PT_STATUS_PAGES_REQUEST {
 PHONGTHAN_WIRE_HEADER Header;PHONGTHAN_U32 ActorId,MapId;
 int Operation,Value;PT_PERSONAL_PROFILE Profile;
};
struct PT_STATUS_PAGES_STATE {
 PHONGTHAN_WIRE_HEADER Header;PHONGTHAN_U32 ActorId,MapId;
 int Result,CurrentTitle;unsigned int Titles[16];PT_PERSONAL_PROFILE Profile;
 PT_PET_DATA Pet;int PetResult;int SummonLocked;
};
#pragma pack(pop)
inline bool PtStatusSafeText(const char* s,int capacity){
 for(int i=0;i<capacity;++i){unsigned char c=(unsigned char)s[i];if(!c)return true;
  if(c<32||c==127||c=='<'||c=='>')return false;}return false;
}
inline bool PtStatusValidProfile(const PT_PERSONAL_PROFILE& p){
 if(p.Sex<0||p.Sex>2||p.Age<0||p.Age>150||p.BirthMonth<0||p.BirthMonth>12||
    p.BirthDay<0||p.BirthDay>31||p.Star<0||p.Star>12||p.Activity<0||p.Activity>7||p.Hour<0||p.Hour>23)return false;
 static const int days[13]={0,31,29,31,30,31,30,31,31,30,31,30,31};
 if((p.BirthMonth==0)!=(p.BirthDay==0)||p.BirthDay>days[p.BirthMonth])return false;
 return PtStatusSafeText(p.Province,sizeof(p.Province))&&PtStatusSafeText(p.Relationship,sizeof(p.Relationship))&&PtStatusSafeText(p.Memo,sizeof(p.Memo));
}
inline bool PtStatusHasTitle(const unsigned int* words,int title){
 return title==0||(title>0&&title<256&&(words[title/32]&(1U<<(title%32)))!=0);
}
#ifndef _SERVER
int PhongThanGetStatusPages(PT_STATUS_PAGES_STATE* state);
int PhongThanStatusPagesAction(const PT_STATUS_PAGES_REQUEST* request);
#endif
#endif
