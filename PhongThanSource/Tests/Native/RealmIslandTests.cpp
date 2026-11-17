#include <windows.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#define PT_REALM_ISLAND_TEST
enum {MAX_PLAYER=3,MAX_NPC=20,MAX_ITEM=4,kind_normal=0,camp_animal=4,item_materials=3,pos_equiproom=3,pos_equiproomex=4,pos_ptbag2=18,pos_ptbag3=19,Obstacle_NULL=0};
struct Lock{bool IsLock(){return false;}};
struct ItemType {int detail,count;Lock lock;int GetGenre(){return 3;}int GetDetailType(){return detail;}int GetStackNum(){return count;}Lock* GetLock(){return &lock;}} Item[MAX_ITEM];
struct PlayerItem{int nIdx,nPlace;};struct Bag{int money,cursor;PlayerItem items[4];PlayerItem* GetFirstItem(){cursor=0;return GetNextItem();}PlayerItem* GetNextItem(){while(++cursor<4)if(items[cursor].nIdx)return items+cursor;return 0;}int GetEquipmentMoney(){return money;}};
struct Task{int v[5000];int GetSaveVal(int n){return v[n];}void SetSaveVal(int n,int v0,int){v[n]=v0;}};
int landingBlocked=0,warpFail=0;struct World{int m_SubWorldID;int TestBarrier(int,int){return landingBlocked;}} SubWorld[2];
struct WorldSet{int SearchWorld(int){return 0;}int GetGameTime(){return 0;}}g_SubWorldSet;
struct Actor{DWORD m_dwID,m_DeathScriptID;int m_NpcSettingIdx,m_CurrentLife,m_Level,m_SubWorldIndex,m_Kind,m_Camp,m_CurrentCamp,m_AiMode,m_nNpcTimeout;int ChangeWorld(int map,int,int){if(warpFail)return 0;SubWorld[m_SubWorldIndex].m_SubWorldID=map;return 1;}void GetMpsPos(int* a,int* b){*a=1000;*b=1000;}void SendSyncData(int,int){}}Npc[MAX_NPC];
struct Owner{DWORD m_dwID;int m_PlayerDBLoad,m_nIndex;Task m_cTask;Bag m_ItemList;bool CheckTrading(){return false;}int GetLockState(){return 0;}int Pay(int n){if(m_ItemList.money<n)return 0;m_ItemList.money-=n;return 1;}void Earn(int n){m_ItemList.money+=n;}}Player[MAX_PLAYER];
struct PhongThanQuestItemTuple{int detail;};struct PhongThanQuestExchangeRow{PhongThanQuestItemTuple item;int count;};
bool room=true;bool PhongThanDecodeQuestItemTuple(int,int d,int,int,int,int,PhongThanQuestItemTuple& t){t.detail=d;return true;}
int PhongThanQuestExchange(int p,int task,int from,int to,PhongThanQuestExchangeRow* needs,int nc,PhongThanQuestExchangeRow*,int gc,bool,int=0,bool=true){if(!room||Player[p].m_cTask.v[task]!=from||nc&&Item[1].count<needs[0].count)return 0;if(nc)Item[1].count-=needs[0].count;if(gc){Item[2].detail=180;Item[2].count=1;Player[p].m_ItemList.items[2].nIdx=2;Player[p].m_ItemList.items[2].nPlace=3;}Player[p].m_cTask.v[task]=to;return 1;}
void PhongThanRemoveNpc(int n){Npc[n].m_dwID=0;}int PhongThanSpawnNpc(int model,int,int,int,int,int){Npc[5].m_dwID=123;Npc[5].m_NpcSettingIdx=model;return 5;}
#include "../../Headers/PhongThanRealmQuestText.h"
void SendPhongThanTaskRecord(int,int,int,int,const char*){}
#include "../../Sources/Core/Src/PhongThanRealmIsland.inl"
int main(){using namespace PtRealmIsland;
 Player[1].m_dwID=8;Player[1].m_PlayerDBLoad=1;Player[1].m_nIndex=1;Npc[1].m_dwID=9;Npc[1].m_CurrentLife=100;Npc[1].m_Level=119;Player[1].m_ItemList.money=2000000;
 assert(!Apply(1,1,8));Npc[1].m_Level=120;assert(Apply(1,1,8));assert(!Apply(1,5,10));
 landingBlocked=1;assert(!Apply(1,4,9)&&Player[1].m_ItemList.money==2000000);landingBlocked=0;warpFail=1;assert(!Apply(1,4,9)&&Player[1].m_ItemList.money==2000000);warpFail=0;
 assert(Apply(1,4,9)&&Player[1].m_ItemList.money==0);assert(!Apply(1,4,9));
 Item[1].detail=177;Item[1].count=9;Player[1].m_ItemList.items[1].nIdx=1;Player[1].m_ItemList.items[1].nPlace=3;assert(!Apply(1,2,10));Item[1].count=10;assert(Apply(1,2,10)&&Item[1].count==0&&!Apply(1,2,10));
 PhongThanRealmIslandTick();assert(Entity(hunts[1]));PhongThanRealmIslandKill(6,1);assert(Get(1,Lure)==2);PhongThanRealmIslandKill(5,1);assert(Get(1,Lure)==3);
 room=false;assert(!Apply(1,3,10)&&Get(1,Lure)==3);room=true;assert(Apply(1,3,10)&&Has(1,180,1)&&!Apply(1,3,10));
 assert(Apply(1,5,10)&&SubWorld[0].m_SubWorldID==1056&&Item[2].count==1);Npc[1].m_Level=119;assert(!Apply(1,5,10));
 puts("PASS island travel/lure: level 120, walkable landing, 200-van fee/refund, 10 arrows once, owned dragon identity, full-bag reward rollback and whisker eligibility");return 0;
}
