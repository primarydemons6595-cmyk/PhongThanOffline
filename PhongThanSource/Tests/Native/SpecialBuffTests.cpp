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
struct ItemType{int genre,detail,part,count;DWORD serial;int param;bool carried;Lock lock;struct{int nExpireTime;}m_CommonAttrib;
 int GetParam(){return param;}void SetParam(int v){param=v;}void SetExpireTime(DWORD v){m_CommonAttrib.nExpireTime=v;}int GetGenre(){return genre;}int GetDetailType(){return detail;}int GetParticular(){return part;}}Item[MAX_ITEM];
enum{SAVE_DOING=2,SAVE_REQUEST=1};bool removeFailure=false;
struct Bag{int money;int GetEquipmentMoney(){return money;}bool CostMoney(int n){if(money<n)return false;money-=n;return true;}void SyncItem(int){}bool RemoveItem(int item,int count){if(removeFailure||!Item[item].carried||Item[item].count<count)return false;Item[item].count-=count;if(!Item[item].count)Item[item].carried=false;return true;}};
struct Profession{int value;int GetProfession(){return value;}};
struct PlayerType{DWORD m_dwID;int m_nIndex,updates,notices,m_uMustSave;bool locked,trading;Tasks m_cTask;Bag m_ItemList;Profession m_cProfession;
 bool GetLockState(){return locked;}bool CheckTrading(){return trading;}void UpdataCurData();void SendSystemMessage(const char*,const char*){++notices;}}Player[MAX_PLAYER];
typedef PlayerType KPlayer;
struct NpcType{DWORD m_dwID;int m_CurrentLife,attrs[400];bool IsPlayer(){return true;}int GetPlayerIdx(){return 1;}void ModifyAttrib(int,KMagicAttrib* a){assert(a->nAttribType>0&&a->nAttribType<400);attrs[a->nAttribType]+=a->nValue[0];}}Npc[MAX_NPC];
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
void PlayerType::UpdataCurData(){++updates;memset(Npc[m_nIndex].attrs,0,sizeof(Npc[m_nIndex].attrs));PtOriginalItemBuffApply(*this);}
struct Lua_State{};
int Lua_GetTopIndex(Lua_State*){return 0;}int GetPlayerIndex(Lua_State*){return 1;}void Lua_SetTopIndex(Lua_State*,int){}
bool Lua_IsNumber(Lua_State*,int){return false;}double Lua_ValueToNumber(Lua_State*,int){return 0;}void Lua_PushNumber(Lua_State*,int){}
#include "../../Sources/Core/Src/PhongThanSpecialBuffUse.inl"
struct PtHandbookItem{int genre,detail,part,level,row,recipe;};
PtHandbookItem kPtHandbookItems[]={{0,0,0,0,0,0},{8,418,0,1,418,0},{3,66,0,1,0,0}};
bool book=true;bool PtHandbookAllowed(int){return book;}
#include "SpecialBuffHandbook.inl"
void Reset(){ZeroMemory(Player,sizeof(Player));ZeroMemory(Npc,sizeof(Npc));ZeroMemory(Item,sizeof(Item));ZeroMemory(g_PhongThanPlayerIBBuff,sizeof(g_PhongThanPlayerIBBuff));ZeroMemory(pets,sizeof(pets));Player[1].m_nIndex=1;Player[1].m_dwID=10;Npc[1].m_dwID=100;Npc[1].m_CurrentLife=100;Item[1].genre=8;Item[1].detail=7445;Item[1].part=2;Item[1].count=1;Item[1].serial=99;Item[1].carried=true;book=petRead=true;removeFailure=false;now=1000;}
int main(){
 Reset();Item[1].param=1793549096;Item[1].m_CommonAttrib.nExpireTime=342572696;now=341968000;
 int oldUses,oldDay;assert(PtWeeklyDragonState(Item[1].param,oldUses,oldDay)&&oldUses==0&&oldDay==-1);
 assert(PhongThanUseOriginalBuffItem(1,1)==1&&Item[1].carried&&Player[1].m_ItemList.money==0&&Item[1].m_CommonAttrib.nExpireTime==342572696);
 assert(PtWeeklyDragonState(Item[1].param,oldUses,oldDay)&&oldUses==1);assert(!PhongThanUseOriginalBuffItem(1,1));
 Reset();Item[1].param=1793549096;now=342572697;assert(!PhongThanUseOriginalBuffItem(1,1)&&Item[1].param==1793549096&&Item[1].carried);
 assert(PtWeeklyDragonExpiry(1793549096,0,now)==342572696);assert(PtWeeklyDragonExpiry(0,12345,now)==12345);
 Reset();Item[1].param=42;assert(!PhongThanUseOriginalBuffItem(1,1)&&Item[1].param==42);
 puts("PASS real legacy Unix token parameter: first activation works, expiry unchanged, repeat denied, undated old token derives purchase deadline, invalid state not reset");

 Reset();assert(PtHandbookGive(1,1,10)==1&&PhongThanPetHopeCount(1)==10&&Item[1].count==1);
 assert(PtHandbookGive(1,1,21)==-8&&PhongThanPetHopeCount(1)==10);assert(PtHandbookGive(1,1,20)==1&&PhongThanPetHopeCount(1)==30);assert(PtHandbookGive(1,1,1)==-8);book=false;assert(PtHandbookGive(1,1,1)==-1);
 Reset();pets[1].Owned=1;assert(PtHandbookGive(1,1,1)==-8&&Player[1].m_cTask.GetSaveVal(4363)==1);pets[1].Owned=0;assert(PtHandbookGive(1,1,1)==-8);
 Reset();Item[1].detail=418;Item[1].part=0;Item[1].count=2;assert(PhongThanUseOriginalBuffItem(1,1)==1&&PhongThanPetHopeCount(1)==1&&Item[1].count==1);
 removeFailure=true;assert(PhongThanUseOriginalBuffItem(1,1)==0&&PhongThanPetHopeCount(1)==1&&Item[1].count==1);removeFailure=false;
 ZeroMemory(&g_PhongThanPlayerIBBuff[1],sizeof(g_PhongThanPlayerIBBuff[1]));RestorePhongThanIBBuffs(1);assert(PhongThanPetHopeCount(1)==1);
 for(int f=0;f<4;++f){Reset();Player[1].m_cProfession.value=f;assert(PhongThanUseOriginalBuffItem(1,1)==1&&Item[1].carried);int id=f==1||f==2?7447:7446;assert(FindIBBuffSlot(GetPlayerIBBuffStore(1),id)==0);assert(g_PhongThanPlayerIBBuff[1].Entry[0].dwExpireTime==now+86400);
  const PTOriginalItemBuffDef& d=ptOriginalItemBuffs[id==7447?1:0];assert(Npc[1].attrs[198]==5&&Npc[1].attrs[204]==5&&!Npc[1].attrs[291]&&!Npc[1].attrs[146]&&Npc[1].attrs[86]==10&&Npc[1].attrs[111]==10&&Npc[1].attrs[115]==15&&Npc[1].attrs[116]==15);assert(!Npc[1].attrs[126]&&!Npc[1].attrs[162]&&!Npc[1].attrs[114]&&!Npc[1].attrs[233]);for(int a=0;a<6;++a)assert(Npc[1].attrs[d.attrs[a][0]]==d.attrs[a][1]);
  Player[1].UpdataCurData();for(a=0;a<6;++a)assert(Npc[1].attrs[d.attrs[a][0]]==d.attrs[a][1]);
  Item[1].carried=true;Item[1].count=1;assert(PhongThanUseOriginalBuffItem(1,1)==0&&Item[1].count==1);
  ++Player[1].m_dwID;++Npc[1].m_dwID;ZeroMemory(&g_PhongThanPlayerIBBuff[1],sizeof(g_PhongThanPlayerIBBuff[1]));ZeroMemory(Npc[1].attrs,sizeof(Npc[1].attrs));RestorePhongThanIBBuffs(1);PtOriginalItemBuffTick(Player[1]);assert(Npc[1].attrs[d.attrs[0][0]]==d.attrs[0][1]);
  now+=86400;PtOriginalItemBuffTick(Player[1]);for(a=0;a<400;++a)assert(Npc[1].attrs[a]==0);
 }
 Reset();assert(PhongThanUseOriginalBuffItem(1,1)==1&&Item[1].carried&&Player[1].m_ItemList.money==0);
 int uses,day;assert(PtWeeklyDragonState(Item[1].param,uses,day)&&uses==1&&day==0);
 assert(!PhongThanUseOriginalBuffItem(1,1));now+=86400;
 assert(!PhongThanUseOriginalBuffItem(1,1)&&Item[1].param==PtWeeklyDragonPack(1,0));
 Player[1].m_ItemList.money=6000000;
 for(int d=1;d<=6;++d){now=1000+d*86400;assert(PhongThanUseOriginalBuffItem(1,1)==1);assert(!PhongThanUseOriginalBuffItem(1,1));assert(Player[1].m_ItemList.money==(6-d)*1000000);assert(PtWeeklyDragonState(Item[1].param,uses,day)&&uses==d+1&&day==d);}
 now=1000+604800;assert(!PhongThanUseOriginalBuffItem(1,1)&&Item[1].carried);
 Reset();Player[1].m_uMustSave=SAVE_DOING;assert(!PhongThanUseOriginalBuffItem(1,1)&&Item[1].param==0);
 Reset();now=86399;assert(PhongThanUseOriginalBuffItem(1,1));now=86400;Player[1].m_ItemList.money=1000000;assert(PhongThanUseOriginalBuffItem(1,1)&&Player[1].m_ItemList.money==0);
 Reset();for(int i=0;i<48;++i){g_PhongThanPlayerIBBuff[1].Entry[i].nId=100+i;g_PhongThanPlayerIBBuff[1].Entry[i].nTimes=1;}assert(!PhongThanUseOriginalBuffItem(1,1)&&Item[1].carried);
 Reset();Item[1].genre=3;Item[1].detail=66;assert(PhongThanUseOriginalBuffItem(1,1)==-1&&Item[1].carried);
 Reset();Item[1].lock.locked=true;assert(!PhongThanUseOriginalBuffItem(1,1)&&Item[1].carried);Item[1].lock.locked=false;Player[1].trading=true;assert(!PhongThanUseOriginalBuffItem(1,1)&&Item[1].carried);Player[1].trading=false;Item[1].m_CommonAttrib.nExpireTime=999;assert(!PhongThanUseOriginalBuffItem(1,1)&&Item[1].carried);
 Reset();Item[1].detail=9444;Item[1].part=2;
 assert(PhongThanUseOriginalBuffItem(1,1)==1&&!Item[1].carried&&PtOriginalExperienceLocked(Player[1]));
 assert(g_PhongThanPlayerIBBuff[1].Entry[0].dwExpireTime==now+604800);
 Item[1].carried=true;Item[1].count=1;assert(!PhongThanUseOriginalBuffItem(1,1)&&Item[1].count==1);
 ZeroMemory(&g_PhongThanPlayerIBBuff[1],sizeof(g_PhongThanPlayerIBBuff[1]));RestorePhongThanIBBuffs(1);assert(PtOriginalExperienceLocked(Player[1]));
 now+=604799;assert(PtOriginalExperienceLocked(Player[1]));++now;assert(!PtOriginalExperienceLocked(Player[1]));
 Reset();Item[1].detail=9444;removeFailure=true;assert(!PhongThanUseOriginalBuffItem(1,1)&&Item[1].carried&&!PtOriginalExperienceLocked(Player[1]));
 puts("PASS weekly dragon: free first use, local midnight, six paid days, same-day denial, expiry, money, save guard, relog, full slots");
 puts("PASS original Hoa Than Toa 8.9444.2 -> buff 9445, seven real days, relog, exact expiry, repeat denial and removal rollback");
 puts("PASS actual GM hope counter, eligibility/cap, legacy F4 conversion/rollback, real buff persistence, 4 professions, requested six attributes, no stacking, expiry, relog, full slots and item guards");return 0;
}
