#include <windows.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../../Headers/PhongThanPet.h"
#include "../../Headers/PhongThanOriginalItemBuffData.h"
enum{MAX_PLAYER=4,MAX_NPC=5,MAX_ITEM=8,item_ibitem=8,item_equip=0,TASKVALUE_PT_IBBUFF_BEGIN=4800,
 PHONGTHAN_MAX_IBBUFF=48,PHONGTHAN_IBBUFF_ID_MASK=0x3fff,PHONGTHAN_IBBUFF_LEVEL_MASK=255,PHONGTHAN_IBBUFF_TIMES_MASK=1023};
#define MESSAGE_SYSTEM_ANNOUCE_HEAD ""
DWORD now=1000;DWORD KSG_GetCurSec(){return now;}
struct KMagicAttrib{int nAttribType,nValue[3];};
struct Tasks{int v[5400];int GetSaveVal(int k){return v[k];}void SetSaveVal(int k,int v2,BOOL){v[k]=v2;}};
struct Lock{bool locked;bool IsLock(){return locked;}};
struct ItemType{int GetDetailTypeX(){return detail;}int genre,detail,part,count;DWORD serial;int param;bool carried;Lock lock;struct{int nExpireTime;}m_CommonAttrib;
 int GetParam(){return param;}void SetParam(int v){param=v;}void SetExpireTime(DWORD v){m_CommonAttrib.nExpireTime=v;}int GetGenre(){return genre;}int GetDetailType(){return detail;}int GetParticular(){return part;}}Item[MAX_ITEM];
enum{magic_life_v=87,magic_mana_v=91};
enum{SAVE_DOING=2,SAVE_REQUEST=1};bool removeFailure=false;
struct Bag{int money;int GetEquipmentMoney(){return money;}bool CostMoney(int n){if(money<n)return false;money-=n;return true;}void SyncItem(int){}bool RemoveItem(int item,int count){if(removeFailure||!Item[item].carried||Item[item].count<count)return false;Item[item].count-=count;if(!Item[item].count)Item[item].carried=false;return true;}};
struct Profession{int value;int GetProfession(){return value;}};
struct PlayerType{DWORD m_dwID;int m_nIndex,updates,notices,m_uMustSave;bool locked,trading;Tasks m_cTask;Bag m_ItemList;Profession m_cProfession;
 bool GetLockState(){return locked;}bool CheckTrading(){return trading;}void UpdataCurData();void SendSystemMessage(const char*,const char*){++notices;}}Player[MAX_PLAYER];
typedef PlayerType KPlayer;
struct NpcType{DWORD m_dwID;int m_CurrentLife,m_CurrentLifeMax,m_CurrentMana,m_CurrentManaMax,m_Index,m_Level,attrs[400];struct {int nTime;}m_FreezeState;void SendSyncData(int,BOOL){}bool IsPlayer(){return true;}int GetPlayerIdx(){return 1;}void ModifyAttrib(int,KMagicAttrib* a){assert(a->nAttribType>0&&a->nAttribType<400);attrs[a->nAttribType]+=a->nValue[0];}}Npc[MAX_NPC];
typedef NpcType KNpc;
struct PHONGTHAN_IBBUFF_ENTRY{int nId,nLevel,nTimes;DWORD dwExpireTime;};
struct PHONGTHAN_IBBUFF_STORE{int nOwnerIndex;DWORD dwOwnerId;PHONGTHAN_IBBUFF_ENTRY Entry[48];}g_PhongThanPlayerIBBuff[MAX_PLAYER];
PT_PET_DATA pets[MAX_PLAYER];bool petRead=true;
bool PhongThanPetRead(int p,PT_PET_DATA* out){*out=pets[p];return petRead;}
bool PhongThanIsLivePlayer(int p){return p>0&&p<MAX_PLAYER&&Player[p].m_nIndex>0&&Npc[Player[p].m_nIndex].m_dwID;}
bool PtPetCarried(int,int item){return item>0&&item<MAX_ITEM&&Item[item].carried&&Item[item].serial&&!Item[item].lock.IsLock();}
PHONGTHAN_IBBUFF_STORE* GetPlayerIBBuffStore(int p){PHONGTHAN_IBBUFF_STORE& s=g_PhongThanPlayerIBBuff[p];int n=0;for(int i=0;i<48;++i)if(s.Entry[i].nId&&(!s.Entry[i].dwExpireTime||s.Entry[i].dwExpireTime>now))s.Entry[n++]=s.Entry[i];for(;n<48;++n)ZeroMemory(&s.Entry[n],sizeof(s.Entry[n]));return &s;}
int GetIBBuffDefaultSeconds(int id,int){for(int i=0;i<2;++i)if(ptOriginalItemBuffs[i].id==id)return ptOriginalItemBuffs[i].seconds;return 0;}
void ApplyNativeIBBuffState(int,const PHONGTHAN_IBBUFF_ENTRY*){}
static bool PhongThanPetHopeAdjust(int,int);
void ExpireNativeIBBuffState(int,int){}
#include "SpecialBuffStore.inl"
#include "../../Sources/Core/Src/PhongThanSpecialBuffStats.inl"
#include "../../Sources/Core/Src/PhongThanLifeItemStats.inl"
void PlayerType::UpdataCurData(){++updates;memset(Npc[m_nIndex].attrs,0,sizeof(Npc[m_nIndex].attrs));PtOriginalItemBuffApply(*this);PtLifeItemBuffApply(*this);}
struct Lua_State{};
int Lua_GetTopIndex(Lua_State*){return 0;}int GetPlayerIndex(Lua_State*){return 1;}void Lua_SetTopIndex(Lua_State*,int){}
bool Lua_IsNumber(Lua_State*,int){return false;}double Lua_ValueToNumber(Lua_State*,int){return 0;}void Lua_PushNumber(Lua_State*,int){}
#include "../../Sources/Core/Src/PhongThanSpecialBuffUse.inl"
struct PtHandbookItem{int genre,detail,part,level,row,recipe;};
PtHandbookItem kPtHandbookItems[]={{0,0,0,0,0,0},{8,418,0,1,418,0},{3,66,0,1,0,0}};
bool book=true;bool PtHandbookAllowed(int){return book;}
#include "SpecialBuffHandbook.inl"
#include "../../Sources/Core/Src/PhongThanLifeItemUse.inl"
void Reset(){ZeroMemory(Player,sizeof(Player));ZeroMemory(Npc,sizeof(Npc));ZeroMemory(Item,sizeof(Item));ZeroMemory(g_PhongThanPlayerIBBuff,sizeof(g_PhongThanPlayerIBBuff));ZeroMemory(pets,sizeof(pets));Player[1].m_nIndex=1;Player[1].m_dwID=10;Npc[1].m_dwID=100;Npc[1].m_CurrentLife=100;Item[1].genre=8;Item[1].detail=7445;Item[1].part=2;Item[1].count=1;Item[1].serial=99;Item[1].carried=true;book=petRead=true;removeFailure=false;now=1000;}

int main(){int tested=0;
 for(int i=0;i<sizeof(ptLifeItemUses)/sizeof(ptLifeItemUses[0]);++i){const PTLifeItemUse& d=ptLifeItemUses[i];Reset();Item[1].detail=d.detail;Item[1].part=d.part;Item[1].count=3;Npc[1].m_Index=1;Npc[1].m_Level=200;Npc[1].m_CurrentLifeMax=1000;Npc[1].m_CurrentLife=100;Npc[1].m_CurrentManaMax=1000;Npc[1].m_CurrentMana=100;
  Npc[1].m_Level=d.level-1;if(d.level>0)assert(!PhongThanUseLifeItem(1,1)&&Item[1].count==3);Npc[1].m_Level=200;
  Player[1].trading=true;assert(!PhongThanUseLifeItem(1,1)&&Item[1].count==3);Player[1].trading=false;
  removeFailure=true;assert(!PhongThanUseLifeItem(1,1)&&Item[1].count==3);removeFailure=false;
  if(d.kind==1){assert(PhongThanUseLifeItem(1,1)==1&&Item[1].count==2);int slot=FindIBBuffSlot(GetPlayerIBBuffStore(1),d.buff);assert(slot>=0&&g_PhongThanPlayerIBBuff[1].Entry[slot].dwExpireTime==now+d.seconds);
   for(int a=0;a<12;++a)if(d.attrs[a][0])assert(Npc[1].attrs[d.attrs[a][0]]==d.attrs[a][1]);
   Player[1].UpdataCurData();for(a=0;a<12;++a)if(d.attrs[a][0])assert(Npc[1].attrs[d.attrs[a][0]]==d.attrs[a][1]);
   assert(PhongThanUseLifeItem(1,1)==1&&Item[1].count==1);slot=FindIBBuffSlot(GetPlayerIBBuffStore(1),d.buff);assert(g_PhongThanPlayerIBBuff[1].Entry[slot].nTimes==1);
   for(a=0;a<12;++a)if(d.attrs[a][0])assert(Npc[1].attrs[d.attrs[a][0]]==d.attrs[a][1]);
   now+=d.seconds;Player[1].UpdataCurData();for(a=0;a<12;++a)if(d.attrs[a][0])assert(Npc[1].attrs[d.attrs[a][0]]==0);
  }else if(d.kind==2){assert(PhongThanUseLifeItem(1,1)==1&&Item[1].count==2);}
  else if(d.kind==3){assert(!PhongThanUseLifeItem(1,1)&&Item[1].count==3);Npc[1].m_FreezeState.nTime=100;assert(PhongThanUseLifeItem(1,1)==1&&Item[1].count==2&&!Npc[1].m_FreezeState.nTime);}
  else if(d.kind==4){assert(!PhongThanUseLifeItem(1,1)&&Item[1].count==3);assert(AddIBBuffToStore(GetPlayerIBBuffStore(1),1,1217,600,0));assert(PhongThanUseLifeItem(1,1)==1&&Item[1].count==2&&FindIBBuffSlot(GetPlayerIBBuffStore(1),1217)<0);}
  ++tested;
 }
 printf("PASS all %d crafted IB outputs: exact effects/time/particular, level/trade/remove failure, refresh without stacking, rebuild/expiry, instant recovery and cures\n",tested);return 0;
}
