
#include <windows.h>
#include <stdio.h>
#include <assert.h>
#include "../../Headers/PhongThanPet.h"
enum{item_ibitem=8,item_materials=3};
struct PetItem{int detail,count;int GetGenre(){return 3;}int GetDetailType(){return detail;}int GetParticular(){return 0;}int GetID(){return 101;}int GetStackNum(){return count;}}Item[3];
bool removeFailure=false,saveFailure=false;
struct PetBag{bool RemoveItem(int,int count){if(removeFailure)return false;Item[1].count-=count;return true;}};
struct PetPlayer{PetBag m_ItemList;bool GetLockState(){return false;}bool CheckTrading(){return false;}}Player[2];
struct PT_PET_RUNTIME{PT_PET_DATA Data;int Journal,Dirty,Loaded;DWORD SaveTick;}runtime;
PT_PET_RUNTIME* PtPetLoad(int){return &runtime;}
bool PtPetCarried(int,int){return Item[1].count>0;}
int PtPetSetting(const char*,const char*,int value){return value;}
void PtPetLog(PT_PET_DATA&,int,int){}
void PtPetInitialAttributes(PT_PET_DATA&,int){}
bool PtPetSave(int,const PT_PET_DATA&,DWORD=0,int=0,const PT_PET_DATA* =0);
bool PtPetSave(int,const PT_PET_DATA&,DWORD,int,const PT_PET_DATA*){return !saveFailure;}
int g_Random(int){return 0;}
int PhongThanPetUseItem(int player,int item){
 PT_PET_RUNTIME*r=PtPetLoad(player);if(!r||r->Journal||!r->Data.Owned||Player[player].GetLockState()||Player[player].CheckTrading()||!PtPetCarried(player,item))return 0;
 int genre=Item[item].GetGenre();
 if(!((genre==item_ibitem&&Item[item].GetParticular()==2)||(genre==item_materials&&(Item[item].GetDetailType()==114||Item[item].GetDetailType()==115||Item[item].GetDetailType()==1184||(Item[item].GetDetailType()>=5108&&Item[item].GetDetailType()<=5115)))))return 0;
 int detail=Item[item].GetDetailType();PT_PET_DATA next=r->Data;
 // Strength is the lifetime counter used by original pet.ini: 3600 units/point.
 // 416=Phac Ngoc, 417=Tu Linh Chau. Do not swap their original identities.
 // Original UI PetUI_Setting: PetUnitHealth=3600, PetMaxHealth=360000.
 if(genre==item_materials&&detail>=5108){
  if(next.Strength<=0||next.PurifyStage<1||next.Type<13||next.Type>20||detail!=5108+((next.Type-13)/2)*2)return 0;
  SYSTEMTIME date;GetLocalTime(&date);int day=date.wYear*10000+date.wMonth*100+date.wDay;
  if(next.FeedDay!=day){next.FeedDay=day;next.FeedCount=0;}if(next.FeedCount>=5)return 0;
  ++next.FeedCount;int result=g_Random(3);
  if(result==0){if(next.FeedValue<300)++next.FeedValue;}else if(result==1){int attr=g_Random(4);if(next.Attributes[attr]<10000)++next.Attributes[attr];}
 } else if(genre==item_materials&&detail==1184){
  if(next.Strength<=0||next.PurifyStage<1||next.FeedValue>=300)return 0;
  int gain=max(1,min(300,PtPetSetting("Feed","AdvancedEssenceGrowth",6)));next.FeedValue=min(300,next.FeedValue+gain);
 }
 else if(genre==item_materials){if(next.Strength<=0||next.Strength>=360000)return 0;next.Strength+=10*3600;if(next.Strength>360000)next.Strength=360000;}
 else if(detail==1056){
  if(next.Strength<=0||next.PurifyStage<1||next.FeedValue>=300)return 0;
  SYSTEMTIME date;GetLocalTime(&date);int day=date.wYear*10000+date.wMonth*100+date.wDay;
  if(next.FeedDay!=day){next.FeedDay=day;next.FeedCount=0;}
  if(next.FeedCount>=PtPetSetting("Feed","MaxFeedCount",5))return 0;
  next.FeedValue+=3;if(next.FeedValue>300)next.FeedValue=300;++next.FeedCount;
 }
 else if(detail==903){if(next.Strength<=0||next.PurifyStage<1)return 0;bool changed=next.FeedValue!=0;for(int a=0;a<4;++a){if(next.Attributes[a]!=next.BaseAttributes[a])changed=true;next.Attributes[a]=next.BaseAttributes[a];}if(!changed)return 0;PtPetInitialAttributes(next,(next.Type-13)/2);next.FeedValue=0;}
 else if(detail==416){if(next.Strength<=0||next.Strength>=360000)return 0;next.Strength+=50*3600;if(next.Strength>360000)next.Strength=360000;}
 else if(detail==417){if(next.Strength<=0||next.Spirit>=300)return 0;next.Spirit+=5;if(next.Spirit>300)next.Spirit=300;
  if(r->Data.Spirit<PtPetSetting("Protect","NeedSpirit",200)&&next.Spirit>=PtPetSetting("Protect","NeedSpirit",200))next.Protect=1;
 }
 else if(detail==414){if(next.Strength>0)return 0;next.Strength=PtPetSetting("Normal","NewPetStr",360000);next.Spirit=0;next.Flags&=~PT_PET_REST;}
 else if(detail==415){if(next.Strength<=0||next.Spirit==0)return 0;next.Spirit=0;}
 else return 0;
 PtPetLog(next,detail,detail==417?next.Spirit:next.Strength);
 // Durable intent carries the original serial/count and previous state. On a
 // restart the character inventory decides whether consumption was committed.
 // A pending intent prevents a second operation until it has been finalized.
 if(!PtPetSave(player,next,Item[item].GetID(),Item[item].GetStackNum(),&r->Data))return 0;
 if(!Player[player].m_ItemList.RemoveItem(item,1)){if(!PtPetSave(player,r->Data))r->Loaded=-1;return 0;}
 r->Data=next;r->Journal=r->Dirty=PtPetSave(player,next)?0:1;r->SaveTick=GetTickCount();return 1;
}

void Reset(){ZeroMemory(&runtime,sizeof(runtime));runtime.Data.Owned=1;runtime.Data.Strength=360000;runtime.Data.PurifyStage=1;runtime.Data.Type=13;Item[1].detail=1184;Item[1].count=3;removeFailure=saveFailure=false;}
int main(){Reset();assert(PhongThanPetUseItem(1,1)&&Item[1].count==2&&runtime.Data.FeedValue==6);assert(PhongThanPetUseItem(1,1)&&Item[1].count==1&&runtime.Data.FeedValue==12);
 Reset();runtime.Data.FeedValue=299;assert(PhongThanPetUseItem(1,1)&&runtime.Data.FeedValue==300&&Item[1].count==2);assert(!PhongThanPetUseItem(1,1)&&Item[1].count==2);
 Reset();removeFailure=true;assert(!PhongThanPetUseItem(1,1)&&runtime.Data.FeedValue==0&&Item[1].count==3);
 Reset();saveFailure=true;assert(!PhongThanPetUseItem(1,1)&&runtime.Data.FeedValue==0&&Item[1].count==3);
 Reset();runtime.Data.Strength=0;assert(!PhongThanPetUseItem(1,1)&&Item[1].count==3);
 Reset();runtime.Data.PurifyStage=0;assert(!PhongThanPetUseItem(1,1)&&Item[1].count==3);
 puts("PASS crafted pet essence: repeated growth, cap/no charge when full, invalid pet, durable-intent/save/inventory failure no consumption");return 0;}
