#include <windows.h>
#include <stdio.h>
#include <assert.h>
#define MAX_NPC 16
#define CLIENT_PLAYER_INDEX 0
enum {kind_normal=0,kind_player=1,kind_dialoger=3,obj_npc=1};
struct Resource{int m_PetStyle;};
struct Actor{int m_Kind,m_RegionIndex,m_MapX,m_MapY;DWORD m_dwID,m_SyncSignal;bool summon;Resource res;Resource* GetNpcRes(){return &res;}} Npc[MAX_NPC];
struct Region{int removed,refs;void RemoveNpc(int){++removed;}void DecRef(int,int,int){--refs;}};
struct World{DWORD m_dwCurrentTime;Region m_Region[1];} SubWorld[1];
struct PlayerInfo{int m_nIndex;} Player[1];
struct Indices{bool used[MAX_NPC];int GetNext(int i){while(++i<MAX_NPC)if(used[i])return i;return 0;}void Remove(int i){used[i]=false;}void Insert(int i){used[i]=true;}};
struct KNpcSet{Indices m_UseIdx,m_RequestUseIdx,m_RequestFreeIdx;struct Request{DWORD dwRequestId,dwRequestTime;}m_RequestNpc[MAX_NPC];int removed;
 void Remove(int i){++removed;m_UseIdx.Remove(i);Npc[i].m_dwID=0;}
 void CheckBalance();
} NpcSet;
static bool PhongThanIsClientSummon(int i){return Npc[i].summon;}
// Generated directly from the production KNpcSet.cpp by the test runner.
#include "OwnedNpcClientBalance.inl"
static void Reset(){ZeroMemory(Npc,sizeof(Npc));ZeroMemory(&NpcSet,sizeof(NpcSet));ZeroMemory(SubWorld,sizeof(SubWorld));Player[0].m_nIndex=1;SubWorld[0].m_dwCurrentTime=1000;}
static void Add(int i,int kind,DWORD sync,bool summon=false,int style=0){Npc[i].m_dwID=i+100;Npc[i].m_Kind=kind;Npc[i].m_SyncSignal=sync;Npc[i].summon=summon;Npc[i].res.m_PetStyle=style;NpcSet.m_UseIdx.Insert(i);++SubWorld[0].m_Region[0].refs;}
int main(){
 Reset();Add(1,kind_player,900);Add(2,kind_normal,1);Add(3,kind_dialoger,1);Add(4,kind_normal,909,true);Add(5,kind_dialoger,909,false,1);Add(6,kind_normal,910,true);Add(7,kind_dialoger,995,false,1);NpcSet.CheckBalance();
 assert(Npc[1].m_dwID&&Npc[2].m_dwID&&Npc[3].m_dwID);assert(!Npc[4].m_dwID&&!Npc[5].m_dwID);assert(Npc[6].m_dwID&&Npc[7].m_dwID);assert(NpcSet.removed==2&&SubWorld[0].m_Region[0].removed==2);
 // A pet packet newer than the local-player packet must not unsigned-underflow.
 assert(Npc[7].m_SyncSignal>Npc[1].m_SyncSignal);SubWorld[0].m_dwCurrentTime=1086;NpcSet.CheckBalance();assert(!Npc[6].m_dwID&&!Npc[7].m_dwID&&Npc[2].m_dwID&&Npc[3].m_dwID);
 Reset();Player[0].m_nIndex=0;Add(2,kind_normal,0,true);SubWorld[0].m_dwCurrentTime=1001;NpcSet.CheckBalance();assert(!Npc[2].m_dwID);
 Reset();SubWorld[0].m_dwCurrentTime=20;Add(1,kind_player,20);Add(2,kind_normal,0xFFFFFFF0UL,true);NpcSet.CheckBalance();assert(Npc[2].m_dwID);SubWorld[0].m_dwCurrentTime=90;NpcSet.CheckBalance();assert(!Npc[2].m_dwID);
 puts("PASS production client cleanup: stale summons/pets expire; static NPCs/local player stay; fresh packets, no local actor and clock wrap handled");return 0;
}
