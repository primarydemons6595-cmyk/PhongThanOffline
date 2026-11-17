// Reconstructed Linh thu runtime, distinct from the Di Nhan combat summons.
// Included once by ScriptFuns.cpp. Original pet.ini and item tables are inputs.
#include "../../../Headers/PhongThanPet.h"
#include "../../../Headers/PhongThanAccountStorage.h"
#include <limits.h>
struct PT_PET_FILE {DWORD Magic,Version;char Account[32],Role[32];PT_PET_DATA Data;DWORD PendingItem;int PendingCount;PT_PET_DATA Previous;DWORD Checksum;};
struct PT_PET_FILE_V4 {DWORD Magic,Version;char Account[32],Role[32];BYTE Data[300];DWORD PendingItem;int PendingCount;BYTE Previous[300];DWORD Checksum;};
struct PT_PET_FILE_V3 {DWORD Magic,Version;char Account[32],Role[32];BYTE Data[280];DWORD PendingItem;int PendingCount;BYTE Previous[280];DWORD Checksum;};
struct PT_PET_FILE_V2 {DWORD Magic,Version;char Account[32],Role[32];BYTE Data[200];DWORD PendingItem;int PendingCount;BYTE Previous[200];DWORD Checksum;};
struct PT_PET_RUNTIME {DWORD Owner,Actor,Tick,SaveTick,VisualId,ChatTick,AutomationTick;DWORD CombatSignature;int CombatSkills[4];int Loaded,Dirty,Remainder,Journal,Visual,PickupCursor;PT_PET_DATA Data;};
static PT_PET_RUNTIME ptPets[MAX_PLAYER];
static int PhongThanPetHopeCount(int);
static bool PhongThanPetHopeAdjust(int,int);
static bool PhongThanPetHopeRefund(int,int);
static int PhongThanSpawnNpc(int,int,int,int,int,BOOL);
static void PhongThanRemoveNpc(int);
static void PtPetRemoveVisual(PT_PET_RUNTIME*);
static void PtPetInitialAttributes(PT_PET_DATA&,int);
static void PtPetRefreshCombat(int,PT_PET_RUNTIME*);
static DWORD PtPetHash(const void* data,int count){const BYTE*p=(const BYTE*)data;DWORD h=2166136261U;for(int i=0;i<count;++i)h=(h^p[i])*16777619U;return h;}
static bool PtPetPath(int player,std::string& path){
 std::string dir,name;if(!PtAccountDirectory("CharacterStore",Player[player].AccountName,dir,true)||!PtRoleFileName(Player[player].Name,name))return false;
 path=dir+"\\"+name+".pet";return path.size()<MAX_PATH-5;
}
static bool PtPetSave(int player,const PT_PET_DATA& data,DWORD pending=0,int count=0,const PT_PET_DATA* previous=0){
 std::string path;if(!PtPetValid(data)||!PtPetPath(player,path))return false;
 PT_PET_FILE file;ZeroMemory(&file,sizeof(file));file.Magic=0x54455050;file.Version=5;
 lstrcpyn(file.Account,Player[player].AccountName,32);lstrcpyn(file.Role,Player[player].Name,32);file.Data=data;
 file.PendingItem=pending;file.PendingCount=count;if(previous)file.Previous=*previous;file.Checksum=PtPetHash(&file,sizeof(file)-4);
 std::string temp=path+".tmp";HANDLE h=CreateFileA(temp.c_str(),GENERIC_WRITE,0,0,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,0);if(h==INVALID_HANDLE_VALUE)return false;
 DWORD wrote=0;bool ok=WriteFile(h,&file,sizeof(file),&wrote,0)&&wrote==sizeof(file)&&FlushFileBuffers(h);CloseHandle(h);
 if(ok)ok=MoveFileExA(temp.c_str(),path.c_str(),MOVEFILE_REPLACE_EXISTING|MOVEFILE_WRITE_THROUGH)!=0;
 if(!ok)DeleteFileA(temp.c_str());return ok;
}
#include "PhongThanPetPairJournal.inl"
static PT_PET_RUNTIME* PtPetLoad(int player){
 if(player<=0||player>=MAX_PLAYER||!Player[player].m_PlayerDBLoad)return 0;
 int actor=Player[player].m_nIndex;if(actor<=0||actor>=MAX_NPC||!Npc[actor].m_dwID)return 0;
 PT_PET_RUNTIME& r=ptPets[player];
 if(r.Owner!=Player[player].m_dwID||r.Actor!=Npc[actor].m_dwID){PtPetRemoveVisual(&r);ZeroMemory(&r,sizeof(r));r.Owner=Player[player].m_dwID;r.Actor=Npc[actor].m_dwID;}
 if(!r.Loaded&&!PtPetRecoverPair(player,0))return 0;
 if(r.Loaded)return r.Loaded>0?&r:0;
 r.Loaded=-1;std::string path;if(!PtPetPath(player,path))return 0;
 HANDLE h=CreateFileA(path.c_str(),GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
 if(h==INVALID_HANDLE_VALUE){if(GetLastError()!=ERROR_FILE_NOT_FOUND)return 0;r.Data.PickExclude=1;r.Data.SaleKeep=2;r.Data.SaleMagic=3;r.Loaded=1;r.Tick=r.SaveTick=GetTickCount();return &r;}
 PT_PET_FILE file;ZeroMemory(&file,sizeof(file));DWORD got=0,size=GetFileSize(h,0);bool migrated=false,ok=false;
 if(size==sizeof(PT_PET_FILE_V2)){
  PT_PET_FILE_V2 old;ok=ReadFile(h,&old,sizeof(old),&got,0)&&got==sizeof(old)&&old.Version==2&&old.Checksum==PtPetHash(&old,sizeof(old)-4);
  if(ok){memcpy(&file,&old,72);memcpy(&file.Data,old.Data,200);memcpy(&file.Previous,old.Previous,200);file.PendingItem=old.PendingItem;file.PendingCount=old.PendingCount;
   file.Data.PickExclude=file.Previous.PickExclude=1;file.Data.SaleKeep=file.Previous.SaleKeep=2;file.Data.SaleMagic=file.Previous.SaleMagic=3;file.Version=5;file.Checksum=PtPetHash(&file,sizeof(file)-4);migrated=true;}
 }else if(size==sizeof(PT_PET_FILE_V3)){
  PT_PET_FILE_V3 old;ok=ReadFile(h,&old,sizeof(old),&got,0)&&got==sizeof(old)&&old.Version==3&&old.Checksum==PtPetHash(&old,sizeof(old)-4);
  if(ok){memcpy(&file,&old,72);memcpy(&file.Data,old.Data,280);memcpy(&file.Previous,old.Previous,280);file.PendingItem=old.PendingItem;file.PendingCount=old.PendingCount;
   file.Version=5;file.Checksum=PtPetHash(&file,sizeof(file)-4);migrated=true;}
 }else if(size==sizeof(PT_PET_FILE_V4)){
  PT_PET_FILE_V4 old;ok=ReadFile(h,&old,sizeof(old),&got,0)&&got==sizeof(old)&&old.Version==4&&old.Checksum==PtPetHash(&old,sizeof(old)-4);
  if(ok){memcpy(&file,&old,72);memcpy(&file.Data,old.Data,300);memcpy(&file.Previous,old.Previous,300);file.PendingItem=old.PendingItem;file.PendingCount=old.PendingCount;
   file.Version=5;file.Checksum=PtPetHash(&file,sizeof(file)-4);migrated=true;}
 }else ok=size==sizeof(file)&&ReadFile(h,&file,sizeof(file),&got,0)&&got==sizeof(file);CloseHandle(h);
 if(!ok||file.Magic!=0x54455050||file.Version!=5||file.Account[31]||file.Role[31]||
    _stricmp(file.Account,Player[player].AccountName)||_stricmp(file.Role,Player[player].Name)||
    file.Checksum!=PtPetHash(&file,sizeof(file)-4)||!PtPetValid(file.Data))return 0;
 if(migrated&&!CopyFileA(path.c_str(),(path+(size==sizeof(PT_PET_FILE_V2)?".v2.bak":size==sizeof(PT_PET_FILE_V3)?".v3.bak":".v4.bak")).c_str(),TRUE)&&GetLastError()!=ERROR_FILE_EXISTS)return 0;
 if(file.PendingItem){
  if(file.PendingCount<=0||!PtPetValid(file.Previous))return 0;
  if(file.PendingItem==0xffffffffUL){
   if(Player[player].m_cTask.GetSaveVal(TASKVALUE_PT_PET_TYPE)!=file.PendingCount)file.Data=file.Previous;
  }else{
   int item=Player[player].m_ItemList.SearchID(file.PendingItem);
   if(item>0&&item<MAX_ITEM&&Item[item].GetStackNum()>=file.PendingCount)file.Data=file.Previous;
  }
  if(!PtPetSave(player,file.Data))return 0;
 }
 if(migrated&&!PtPetSave(player,file.Data))return 0;
 r.Data=file.Data;r.Loaded=1;r.Tick=r.SaveTick=GetTickCount();return &r;
}
static void PtPetLog(PT_PET_DATA& d,int event,int value){
 memmove(d.Log+1,d.Log,7*sizeof(d.Log[0]));d.Log[0].Time=(unsigned int)time(0);d.Log[0].Event=event;d.Log[0].Value=value;
}
static int PtPetSetting(const char* section,const char* key,int fallback){
 static KIniFile ini;static bool loaded=false;if(!loaded){ini.Load("\\settings\\pet.ini");loaded=true;}
 int value=fallback;ini.GetInteger(section,key,fallback,&value);return value;
}
bool PhongThanPetRead(int player,PT_PET_DATA* out){PT_PET_RUNTIME*r=PtPetLoad(player);if(!r)return false;if(out)*out=r->Data;return true;}
int PhongThanPetType(int player){PT_PET_RUNTIME*r=PtPetLoad(player);return r&&r->Data.Owned?r->Data.Type:0;}
static bool PtPetCommit(int player,PT_PET_RUNTIME* r,const PT_PET_DATA& next){
 if(!PtPetSave(player,next))return false;r->Data=next;r->Dirty=0;r->SaveTick=GetTickCount();return true;
}
static bool PtPetCarried(int player,int item){
 if(item<=0||item>=MAX_ITEM||!Item[item].GetID()||Item[item].GetLock()->IsLock())return false;
 int slot=Player[player].m_ItemList.FindSame(item);if(slot<=0)return false;
 int place=Player[player].m_ItemList.m_Items[slot].nPlace;
 return place==pos_equiproom||place==pos_equiproomex||place==pos_ptbag2||place==pos_ptbag3||place==pos_immediacy;
}
static bool PtPetExchangeRows(int player,PT_PET_RUNTIME*r,PT_PET_DATA& next,PhongThanQuestExchangeRow*needs,int needCount,PhongThanQuestExchangeRow*rewards,int rewardCount,int cash){
 if(r->Journal||cash<0||!PtPetValid(next)||Player[player].m_ItemList.GetMoneyAmount()<cash)return false;
 int phase=Player[player].m_cTask.GetSaveVal(TASKVALUE_PT_PET_TYPE);if(phase<0||phase==INT_MAX)return false;
 if(!PtPetSave(player,next,0xffffffffUL,phase+1,&r->Data))return false;
 bool paid=cash==0||Player[player].Pay(cash);
 if(!paid||!PhongThanQuestExchange(player,TASKVALUE_PT_PET_TYPE,phase,phase+1,needs,needCount,rewards,rewardCount)){
  if(paid&&cash)Player[player].Earn(cash);
  if(!PtPetSave(player,r->Data))r->Loaded=-1;return false;
 }
 r->Data=next;r->Journal=r->Dirty=PtPetSave(player,next)?0:1;r->SaveTick=GetTickCount();return true;
}
static bool PtPetNeed(PhongThanQuestExchangeRow& row,int genre,int detail,int part,int count){
 ZeroMemory(&row,sizeof(row));row.count=count;
 if(count<=0||count>10000||!PhongThanDecodeQuestItemTuple(genre,detail,part,genre==item_skillbook?1:0,0,0,row.item))return false;
 // Original PC scripts create the same book at stored level 0 or 1.
 // Its identity is genre/detail/skill, not that legacy storage spelling.
 if(genre==item_skillbook)row.item.level=-1;return true;
}
static bool PtPetExchange(int player,PT_PET_RUNTIME*r,PT_PET_DATA& next,int genre,int detail,int particular,int count,int cash){
 PhongThanQuestExchangeRow need;if(count<0||(count&&!PtPetNeed(need,genre,detail,particular,count)))return false;
 return PtPetExchangeRows(player,r,next,&need,count?1:0,0,0,cash);
}
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
int PhongThanPetAction(int player,int operation,int value,const char* name){
 PT_PET_RUNTIME*r=PtPetLoad(player);if(!r||r->Journal)return -2;
 if(!r->Data.Owned)return -5;
 if(Player[player].GetLockState()||Player[player].CheckTrading())return -3;
 PT_PET_DATA next=r->Data;
 if(operation==PT_PET_RENAME){
  if(!name||!name[0])return -1;int length=0;while(length<32&&name[length])++length;if(length>=32)return -1;lstrcpyn(next.Name,name,32);
  if(!PtPetValid(next))return -1;PtPetLog(next,operation,0);
  // Original Pet_BtnChangeName tooltip specifies two Tuong Quan Lenh.
  return !strcmp(name,r->Data.Name)?1:PtPetExchange(player,r,next,3,100,0,2,0)?1:-10;
 }else if(operation==PT_PET_PICK_OPTIONS){
  if(value<0||value>255||!name)return -1;next.PickExclude=value;lstrcpyn(next.PickName,name,64);if(!PtPetValid(next))return -1;
 }else if(operation==PT_PET_PROFILE_OPTIONS){
  if(value<0||!name)return -1;next.PickExclude=value&255;next.SaleKeep=(value>>8)&255;next.SaleLevel=(value>>16)&255;next.SaleMagic=(value>>24)&255;
  lstrcpyn(next.PickName,name,64);if(!PtPetValid(next))return -1;
 }else if(operation==PT_PET_SALE_OPTIONS){
  if(value<0||(value&~0x00ffffff))return -1;next.SaleKeep=value&255;next.SaleLevel=(value>>8)&255;next.SaleMagic=(value>>16)&255;if(!PtPetValid(next))return -1;
 }else if(operation==PT_PET_TOGGLE){
  if(value<=0||(value&(value-1))||(value&~1023))return -1;
  // Unsupported automation is not enabled by a cosmetic checkbox.
  if(value==PT_PET_TRAIN)return -6;
  // Turning a previously enabled function off is always allowed.
  if(next.Flags&value){next.Flags&=~value;PtPetLog(next,operation,value);return PtPetCommit(player,r,next)?1:-2;}
  if(next.Strength<=0&&value!=PT_PET_REST)return -7;
  if(value==PT_PET_CHAT&&next.AgeSeconds<PtPetSetting("Chat","NeedTime",3600))return -8;
  if(value==PT_PET_PROTECT&&(next.Spirit<PtPetSetting("Protect","NeedSpirit",200)||next.Strength<PtPetSetting("Protect","NeedStr",104400)))return -9;
  const char* section=value==PT_PET_PICK?"AutoPick":value==PT_PET_SELL?"AutoSold":value==PT_PET_REPAIR?"AutoRepair":value==PT_PET_AUTO_SKILL?"AutoSkill":0;
  if(section&&next.AgeSeconds<PtPetSetting(section,"NeedTime",0))return -8;
  if(section&&next.Strength<PtPetSetting(section,"NeedStr",104400))return -7;
  if(value==PT_PET_SEE_LIFE&&next.Spirit<PtPetSetting("SeeBlood","NeedSpirit",150))return -9;
  next.Flags^=value;PtPetLog(next,operation,value);
 }else if(operation==PT_PET_FEED){
  int item=Player[player].m_ItemList.SearchID((DWORD)value);return PhongThanPetUseItem(player,item)?1:-5;
 }else return -1;
 return PtPetCommit(player,r,next)?1:-2;
}
static void PtPetRemoveVisual(PT_PET_RUNTIME*r){
 // The owner may already have left the NPC set. The pet's own serial
 // is the authority; never remove a reused NPC slot with a different ID.
 if(r->Visual>0&&r->Visual<MAX_NPC&&r->VisualId&&Npc[r->Visual].m_dwID==r->VisualId)
  PhongThanRemoveNpc(r->Visual);
 r->Visual=0;r->VisualId=0;
}
static void PtPetVisual(int player,PT_PET_RUNTIME*r){
 int actor=Player[player].m_nIndex,world=Npc[actor].m_SubWorldIndex;
 if(!r->Data.Owned||r->Data.Strength<=0||(r->Data.Flags&PT_PET_REST)||!Npc[actor].IsAlive()||world<0||world>=MAX_SUBWORLD){PtPetRemoveVisual(r);return;}
 int templateId=-1;KTabFile table;
 if(!table.Load("\\settings\\pets.txt")||r->Data.Type>=table.GetHeight()-1)return;
 table.GetInteger(r->Data.Type+2,2,0,&templateId);templateId-=2;
 if(templateId<0||templateId>g_NpcSetting.GetHeight()-2)return;
 // Retire the exact previous visual before replacing it. Dropping only
 // its handle leaves a live NPC behind and accumulates duplicate pets.
 if(r->Visual>0&&(r->Visual>=MAX_NPC||Npc[r->Visual].m_dwID!=r->VisualId||Npc[r->Visual].m_nOwnerIdx!=actor))PtPetRemoveVisual(r);
 if(r->Visual>0&&(Npc[r->Visual].m_SubWorldIndex!=world||Npc[r->Visual].m_NpcSettingIdx!=templateId))PtPetRemoveVisual(r);
 int x,y;Npc[actor].GetMpsPos(&x,&y);
 if(!r->Visual){
  r->Visual=PhongThanSpawnNpc(templateId,1,world,x+48,y+48,TRUE);if(!r->Visual)return;
  KNpc& pet=Npc[r->Visual];r->VisualId=pet.m_dwID;pet.m_nOwnerIdx=actor;pet.m_Kind=kind_dialoger;pet.m_AiMode=0;
  pet.m_ActionScriptID=pet.m_DeathScriptID=pet.m_DropScriptID=0;pet.ActionScript[0]=0;
  pet.m_Experience=pet.m_CurrentExperience=0;pet.m_FightMode=FALSE;
  pet.SendSyncData(0,TRUE);
 }
 KNpc& pet=Npc[r->Visual];const char* name=r->Data.Name[0]?r->Data.Name:Player[player].Name;
 if(strcmp(pet.Name,name)){lstrcpyn(pet.Name,name,sizeof(pet.Name));pet.SendSyncData(0,TRUE);}
 int distance=NpcSet.GetDistance(actor,r->Visual);
 if(distance>1000){PtPetRemoveVisual(r);return;}
 if(distance>100){pet.m_CurrentRunSpeed=Npc[actor].m_CurrentRunSpeed;pet.SendCommand(do_run,x+48,y+48);}
}
#ifndef PHONGTHAN_PET_NO_LUA
#include "PhongThanPetNearby.inl"
#include "PhongThanPetAutomation.inl"
#endif
void PhongThanPetTick(int player){
 PT_PET_RUNTIME*r=PtPetLoad(player);if(!r)return;PtPetRefreshCombat(player,r);if(!r->Data.Owned)return;
 DWORD now=GetTickCount(),elapsed=now-r->Tick;if(elapsed<1000)return;r->Tick=now-elapsed%1000;
 int seconds=(int)(elapsed/1000);if(seconds>60)seconds=60;
 PtPetVisual(player,r);
 if(r->Data.Strength>0&&!(r->Data.Flags&PT_PET_REST)){
  if(r->Data.AgeSeconds<=INT_MAX-seconds)r->Data.AgeSeconds+=seconds;
  r->Remainder+=seconds;int period=PtPetSetting("Normal","StrSecond",5);if(period<1)period=5;
  int steps=r->Remainder/period;r->Remainder%=period;int sub=PtPetSetting("Normal","StrSub",3);if(sub<0||sub>10000)sub=3;
  int loss=steps*sub;r->Data.Strength=r->Data.Strength>loss?r->Data.Strength-loss:0;r->Dirty=1;
  if(r->Data.Type==0&&r->Data.AgeSeconds>=PtPetSetting("Normal","PetAutohChangeTypeNeedTime",3600)){
   r->Data.Type=PtPetSetting("Normal",(g_Random(100)<PtPetSetting("Normal","PetAutoChangeTypeRate",95))?"PetAutoChangeType1":"PetAutoChangeType2",1);PtPetLog(r->Data,20,r->Data.Type);
  }
  SYSTEMTIME date;GetLocalTime(&date);int day=date.wYear*10000+date.wMonth*100+date.wDay;
  if(r->Data.Day!=day){r->Data.Day=day;r->Data.Protect=r->Data.Spirit>=PtPetSetting("Protect","NeedSpirit",200)?1:0;}
  if(!r->Journal&&(r->Data.Flags&PT_PET_AUTO_FEED)&&r->Data.Strength<PtPetSetting("Feed","NeedStr",122400)&&!Player[player].GetLockState()&&!Player[player].CheckTrading()){
   PlayerItem* item=Player[player].m_ItemList.GetFirstItem();int food=0;
   while(item){int index=item->nIdx;if(PtPetCarried(player,index)&&Item[index].GetGenre()==item_ibitem&&Item[index].GetDetailType()==416&&Item[index].GetParticular()==2){food=index;break;}item=Player[player].m_ItemList.GetNextItem();}
   if(food)PhongThanPetUseItem(player,food);
  }
 }
#ifndef PHONGTHAN_PET_NO_LUA
 PtPetAutomate(player,r);
#endif
 if(r->Dirty&&now-r->SaveTick>=30000&&PtPetSave(player,r->Data)){r->Dirty=r->Journal=0;r->SaveTick=now;}
}
void PhongThanPetRelease(int player){
 if(player<=0||player>=MAX_PLAYER)return;PT_PET_RUNTIME&r=ptPets[player];
 PtPetRemoveVisual(&r);
 if(r.Loaded==1&&r.Dirty&&r.Owner==Player[player].m_dwID)PtPetSave(player,r.Data);
 ZeroMemory(&r,sizeof(r));
}
bool PhongThanPetTryProtect(int player){
 PT_PET_RUNTIME*r=PtPetLoad(player);if(!r||r->Journal||!r->Data.Owned||
  !(r->Data.Flags&PT_PET_PROTECT)||(r->Data.Flags&PT_PET_REST)||r->Data.Protect<=0||
  r->Data.Strength<PtPetSetting("Protect","NeedStr",104400)||r->Data.Spirit<PtPetSetting("Protect","NeedSpirit",200))return false;
 PT_PET_DATA next=r->Data;--next.Protect;int loss=PtPetSetting("Protect","PerProtectStr",-10800);
 if(loss>0||loss<-10000000)loss=-10800;next.Strength+=loss;if(next.Strength<0)next.Strength=0;PtPetLog(next,30,next.Protect);
 if(!PtPetCommit(player,r,next))return false;
 int actor=Player[player].m_nIndex;Npc[actor].m_CurrentLife=1;Npc[actor].SendCommand(do_stand);Npc[actor].SendSyncData(0,TRUE);return true;
}
#ifndef PHONGTHAN_PET_NO_LUA
int LuaPetGetInfoRestored(Lua_State*L){
 PT_PET_DATA d;ZeroMemory(&d,sizeof(d));if(!PhongThanPetRead(GetPlayerIndex(L),&d))d.Owned=-1;
 Lua_PushNumber(L,d.Owned);Lua_PushNumber(L,d.Type);Lua_PushNumber(L,d.Strength);Lua_PushNumber(L,d.Spirit);Lua_PushNumber(L,d.AgeSeconds);Lua_PushNumber(L,d.Protect);
 for(int i=0;i<4;++i)Lua_PushNumber(L,d.Attributes[i]);Lua_PushNumber(L,d.Aptitude);Lua_PushNumber(L,d.Sex);Lua_PushString(L,d.Name);return 13;
}
int LuaPetAdoptCostRestored(Lua_State*L){
 KIniFile config;int amount=30,cash=1000000;config.Load("\\settings\\PhongThanPetRestore.ini");config.GetInteger("Adoption","LinhNguyen",30,&amount);config.GetInteger("Adoption","Silver",1000000,&cash);
 if(amount<1||amount>10000||cash<0){Lua_PushNumber(L,-1);Lua_PushNumber(L,-1);}else{Lua_PushNumber(L,amount);Lua_PushNumber(L,cash);}return 2;
}
int LuaPetIsAddRestored(Lua_State*L){PT_PET_DATA d;Lua_PushNumber(L,PhongThanPetRead(GetPlayerIndex(L),&d)&&d.Owned?1:0);return 1;}
int LuaPetIsSleepRestored(Lua_State*L){PT_PET_DATA d;Lua_PushNumber(L,PhongThanPetRead(GetPlayerIndex(L),&d)&&d.Owned&&(d.Strength<=0||(d.Flags&PT_PET_REST))?1:0);return 1;}
int LuaPetGetTimeRestored(Lua_State*L){PT_PET_DATA d;Lua_PushNumber(L,PhongThanPetRead(GetPlayerIndex(L),&d)&&d.Owned?d.AgeSeconds:0);return 1;}
int LuaPetSetTypeRestored(Lua_State*L){
 int player=GetPlayerIndex(L),type=Lua_GetTopIndex(L)>0?(int)Lua_ValueToNumber(L,1):-1;PT_PET_RUNTIME*r=PtPetLoad(player);KTabFile table;
 int ok=0;if(r&&r->Data.Owned&&type>=0&&table.Load("\\settings\\pets.txt")&&type<table.GetHeight()-1){PT_PET_DATA next=r->Data;next.Type=type;PtPetLog(next,20,type);ok=PtPetCommit(player,r,next)?1:0;}
 Lua_PushNumber(L,ok);return 1;
}
int LuaPetModifyProtectRestored(Lua_State*L){
 int player=GetPlayerIndex(L),delta=Lua_GetTopIndex(L)>0?(int)Lua_ValueToNumber(L,1):0;PT_PET_RUNTIME*r=PtPetLoad(player);int ok=0;
 if(r&&r->Data.Owned&&delta>=-255&&delta<=255&&r->Data.Protect+delta>=0&&r->Data.Protect+delta<=255){PT_PET_DATA next=r->Data;next.Protect+=delta;ok=PtPetCommit(player,r,next)?1:0;}
 Lua_PushNumber(L,ok);return 1;
}
int LuaUsePetItemRestored(Lua_State*L){Lua_PushNumber(L,PhongThanPetUseItem(GetPlayerIndex(L),Lua_GetTopIndex(L)>0?(int)Lua_ValueToNumber(L,1):0));return 1;}
#endif
int PhongThanPetAdopt(int player){
 int ok=0;PT_PET_RUNTIME*r=PtPetLoad(player);
 if(r&&!r->Data.Owned&&!r->Journal&&!Player[player].GetLockState()&&!Player[player].CheckTrading()){
  int actor=Player[player].m_nIndex,npc=Player[player].m_nLastNpcIndex;
  if(npc>0&&npc<MAX_NPC&&Npc[npc].m_RegionIndex>=0&&Npc[npc].m_SubWorldIndex==Npc[actor].m_SubWorldIndex&&NpcSet.GetDistance(actor,npc)<=400){
   DWORD script=Npc[npc].m_ActionScriptID;
   if(script==g_FileName2Id("\\script\\phongthan\\npc_restore\\1020_04.lua")||script==g_FileName2Id("\\script\\phongthan\\npc_restore\\1021_03.lua")||script==0xF1532FEEUL||script==0x1536A76FUL){
    PT_PET_DATA next;ZeroMemory(&next,sizeof(next));next.Owned=1;next.Type=PtPetSetting("Normal","NewPetType",0);next.Strength=PtPetSetting("Normal","NewPetStr",360000);next.PickExclude=1;next.SaleKeep=2;next.SaleMagic=3;lstrcpyn(next.Name,"Linh thu",32);PtPetLog(next,1,1);
    // Admission economics are an authored restoration rule, kept outside the
    // original item tables. Original scripts use IBBuff 418, not bag item 8/418/0.
    KIniFile config;int amount=30,cash=1000000;config.Load("\\settings\\PhongThanPetRestore.ini");config.GetInteger("Adoption","LinhNguyen",30,&amount);config.GetInteger("Adoption","Silver",1000000,&cash);
    if(amount>0&&amount<=1023&&cash>=0&&PhongThanPetHopeCount(player)>=amount&&Player[player].m_ItemList.GetMoneyAmount()>=cash){
     if(PhongThanPetHopeAdjust(player,-amount)){
      ok=PtPetExchange(player,r,next,0,0,0,0,cash)?1:0;
      if(ok){Player[player].m_cTask.SetSaveVal(4363,1,TRUE);Player[player].m_cTask.SetSaveVal(4360,0,TRUE);}
      if(!ok&&!PhongThanPetHopeRefund(player,amount))r->Loaded=-1;
     }
    }
   }
  }
 }
 return ok;
}
#ifndef PHONGTHAN_PET_NO_LUA
int LuaAdoptPetRestored(Lua_State*L){Lua_PushNumber(L,PhongThanPetAdopt(GetPlayerIndex(L)));return 1;}
#endif

// Called when serializing the existing authoritative NPC snapshot.
int PhongThanPetVisualStyle(int npc,int* hue,int* transparency){
 if(npc<=0||npc>=MAX_NPC)return 0;
 int actor=Npc[npc].m_nOwnerIdx;
 if(actor<=0||actor>=MAX_NPC)return 0;
 for(int p=1;p<MAX_PLAYER;++p){PT_PET_RUNTIME& r=ptPets[p];
  if(r.Loaded==1&&r.Visual==npc&&r.VisualId==Npc[npc].m_dwID&&r.Actor==Npc[actor].m_dwID&&r.Owner==Player[p].m_dwID){
   if(hue)*hue=r.Data.Hue;if(transparency)*transparency=r.Data.Transparency;return 1;
  }
 }return 0;
}
static bool PtPetAtSage(int player){
 int actor=Player[player].m_nIndex,npc=Player[player].m_nLastNpcIndex;
 if(npc<=0||npc>=MAX_NPC||Npc[npc].m_RegionIndex<0||Npc[npc].m_SubWorldIndex!=Npc[actor].m_SubWorldIndex||NpcSet.GetDistance(actor,npc)>400)return false;
 DWORD script=Npc[npc].m_ActionScriptID;
 return script==g_FileName2Id("\\script\\phongthan\\npc_restore\\1020_04.lua")||script==g_FileName2Id("\\script\\phongthan\\npc_restore\\1021_03.lua")||script==0xF1532FEEUL||script==0x1536A76FUL;
}
static int PtPetCosmeticCost(int mode){
 KIniFile ini;ini.Load("\\settings\\PhongThanPetRestore.ini");int count;
 ini.GetInteger("Cosmetics",mode==1?"BeautyTuongQuanLenh":"ColorTuongQuanLenh",mode==1?20:0,&count);
 return count>=0&&count<=10000?count:-1;
}
int PhongThanPetCosmeticRestored(int player,int mode,int value,int alpha){
 int ok=0;
 PT_PET_RUNTIME*r=PtPetLoad(player);
 if(r&&r->Data.Owned&&!r->Journal&&!Player[player].GetLockState()&&!Player[player].CheckTrading()&&PtPetAtSage(player)){
  PT_PET_DATA next=r->Data;bool valid=false;
  if(mode==1&&next.PurifyStage==0&&(value==1||value==2||value==21||value==29)){
   KTabFile types;valid=types.Load("\\settings\\pets.txt")&&value<types.GetHeight()-1;
   if(valid)next.Type=value;
  }else if(mode==2&&value>=0&&value<360&&alpha>=0&&alpha<=205){next.Hue=value;next.Transparency=alpha;valid=true;}
  int cost=PtPetCosmeticCost(mode);
  if(valid&&cost>=0){
   if(next.Type==r->Data.Type&&next.Hue==r->Data.Hue&&next.Transparency==r->Data.Transparency)ok=1;
   else{PtPetLog(next,mode==1?20:43,value);ok=PtPetExchange(player,r,next,3,100,0,cost,0)?1:0;
    if(ok)PtPetRemoveVisual(r);
   }
  }
 }
 return ok;
}
#ifndef PHONGTHAN_PET_NO_LUA
int LuaPetCosmeticCostRestored(Lua_State* L){
 int mode=(int)Lua_ValueToNumber(L,1);Lua_PushNumber(L,mode==1||mode==2?PtPetCosmeticCost(mode):-1);return 1;
}
int LuaPetCosmeticRestored(Lua_State* L){
 Lua_PushNumber(L,PhongThanPetCosmeticRestored(GetPlayerIndex(L),(int)Lua_ValueToNumber(L,1),(int)Lua_ValueToNumber(L,2),(int)Lua_ValueToNumber(L,3)));return 1;
}
#endif

#include "PhongThanPetTraining.inl"
#include "PhongThanPetCombat.inl"

int PhongThanPetAttributeBonus(int player,int index){PT_PET_RUNTIME*r=PtPetLoad(player);return r?PtPetAttributeBonus(r->Data,index):0;}
int PhongThanPetSkillBonus(int player,int skill){PT_PET_RUNTIME*r=PtPetLoad(player);return r?PtPetSkillBonus(r->Data,skill):0;}

#ifndef PHONGTHAN_PET_NO_LUA
int LuaSummonMorphAtSageRestored(Lua_State*L){
 int p=GetPlayerIndex(L),result=0;
 if(p>0&&p<MAX_PLAYER&&Lua_GetTopIndex(L)==1&&Lua_IsNumber(L,1)&&PtPetAtSage(p)&&
    !Player[p].GetLockState()&&!Player[p].CheckTrading()&&Player[p].m_cProfession.GetProfession()==2){
  double requested=Lua_ValueToNumber(L,1);int type=(int)requested,skill=0,old=0,owner=Player[p].m_nIndex;
  if(requested==type&&(type==359||type==408||type==409)&&PhongThanSummonInfo(owner,skill,old)){
   if(old==type)result=1;
   else if(Player[p].Pay(9000)){result=PhongThanSummonMorph(owner,skill,type)?1:0;if(!result)Player[p].Earn(9000);}
  }
 }
 Lua_PushNumber(L,result);return 1;
}
#endif
