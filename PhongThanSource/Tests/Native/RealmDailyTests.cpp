#include <windows.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#define PT_REALM_DAILY_TEST
enum {MAX_PLAYER=3,MAX_NPC=80,MAX_ITEM=12,MAX_SUBWORLD=2,kind_dialoger=3,kind_normal=0,Obstacle_NULL=0,do_walk=1,item_magicscript=6,pos_equiproom=3,pos_equiproomex=11,pos_ptbag2=17,pos_ptbag3=18,pos_immediacy=4};
struct Lock {bool locked;bool IsLock(){return locked;}};
struct It {int detail;Lock lock;int GetGenre(){return 6;}int GetDetailType(){return detail;}int GetStackNum(){return 1;}Lock* GetLock(){return &lock;}} Item[MAX_ITEM];
struct PlayerItem {int nIdx,nPlace;};
struct Bag {PlayerItem m_Items[MAX_ITEM];int cursor;PlayerItem* GetFirstItem(){cursor=0;return GetNextItem();}PlayerItem* GetNextItem(){while(++cursor<MAX_ITEM)if(m_Items[cursor].nIdx)return m_Items+cursor;return 0;}int FindSame(int id){return m_Items[id].nIdx?id:0;}};
struct Task {int v[5000];int GetSaveVal(int n){return v[n];}void SetSaveVal(int n,int val,int){v[n]=val;}};
struct Actor {DWORD m_dwID,m_DeathScriptID,m_ActionScriptID,m_TimerScriptID;int m_SubWorldIndex,m_NpcSettingIdx,m_Kind,m_CurrentLife,m_AiMode,m_nNpcTimeout,x,y,syncs;char Name[80];void GetMpsPos(int* a,int* b){*a=x;*b=y;}void SendSyncData(int,int){++syncs;}void SendCommand(int,int a,int b){x=a;y=b;}} Npc[MAX_NPC];
typedef Actor KNpc;
struct World {int m_SubWorldID,barrier;int TestBarrier(int,int){return barrier;}} SubWorld[MAX_SUBWORLD];
struct Worlds {int SearchWorld(int id){return id==1047?0:-1;}int GetGameTime(){return 100;}} g_SubWorldSet;
struct Actors {int GetNextIdx(int n){while(++n<MAX_NPC)if(Npc[n].m_dwID)return n;return 0;}int GetDistance(int a,int b){int x=Npc[a].x-Npc[b].x,y=Npc[a].y-Npc[b].y;return (int)sqrt((double)x*x+(double)y*y);}} NpcSet;
struct Owner {int m_nIndex,m_PlayerDBLoad,locked,trading;DWORD m_dwID;Task m_cTask;Bag m_ItemList;bool CheckTrading(){return trading!=0;}int GetLockState(){return locked;}} Player[MAX_PLAYER];
namespace PtRealmWorldRuntime {int sequence,receipts;bool fail;static int Today(){return 20;}static int LastReceipt(int){return sequence;}static bool CompleteReputation(int,int s){if(fail)return false;if(s>sequence){sequence=s;++receipts;}return true;}}
struct PhongThanQuestItemTuple {int genre,detail,part;};struct PhongThanQuestExchangeRow{PhongThanQuestItemTuple item;int count;};
int coins=20,lastFee=0;bool room=true;
bool PhongThanDecodeQuestItemTuple(int g,int d,int p,int,int,int,PhongThanQuestItemTuple& t){t.genre=g;t.detail=d;t.part=p;return true;}
int PhongThanQuestExchange(int p,int task,int expected,int next,PhongThanQuestExchangeRow* need,int nc,PhongThanQuestExchangeRow* gift,int gc,bool,int=0,bool=true){
 int fee=nc?need[0].count:0;if(!room||coins<fee||Player[p].m_cTask.v[task]!=expected)return 0;coins-=fee;lastFee=fee;
 for(int n=0;n<gc;++n){int id=gift[n].item.part-409;Item[id].detail=gift[n].item.part;Player[p].m_ItemList.m_Items[id].nIdx=id;Player[p].m_ItemList.m_Items[id].nPlace=pos_equiproom;}
 Player[p].m_cTask.v[task]=next;return 1;
}
void g_StrCpyLen(char* a,const char* b,int n){strncpy(a,b,n-1);a[n-1]=0;}
void SendPhongThanTaskRecord(int,int,int,int,const char*){}
void PhongThanRemoveNpc(int n){Npc[n].m_dwID=0;}
int PhongThanSpawnNpc(int model,int,int world,int x,int y,int){for(int n=10;n<20;++n)if(!Npc[n].m_dwID){Npc[n].m_dwID=1000+n;Npc[n].m_NpcSettingIdx=model;Npc[n].m_SubWorldIndex=world;Npc[n].x=x;Npc[n].y=y;return n;}return 0;}
#include "../../Headers/PhongThanRealmQuestText.h"
#include "../../Sources/Core/Src/PhongThanRealmDaily.inl"
static void Reset(){ZeroMemory(Player,sizeof(Player));ZeroMemory(Npc,sizeof(Npc));ZeroMemory(SubWorld,sizeof(SubWorld));ZeroMemory(Item,sizeof(Item));
 PtRealmDaily::sessions[1]=PtRealmDaily::Session();PtRealmWorldRuntime::sequence=PtRealmWorldRuntime::receipts=0;PtRealmWorldRuntime::fail=false;room=true;coins=20;
 Player[1].m_nIndex=1;Player[1].m_PlayerDBLoad=1;Player[1].m_dwID=77;Player[1].m_cTask.v[4871]=10;
 Npc[1].m_dwID=88;Npc[1].m_CurrentLife=100;Npc[1].x=1000;
 Npc[2].m_dwID=22;Npc[2].m_Kind=kind_dialoger;Npc[2].m_NpcSettingIdx=149;SubWorld[0].m_SubWorldID=1047;
}
static void Mob(int id,int x,int y){Npc[id].m_dwID=2000+id;Npc[id].m_Kind=kind_normal;Npc[id].m_SubWorldIndex=0;Npc[id].x=x;Npc[id].y=y;Npc[id].m_CurrentLife=0;PtRealmDaily::Kill(id,1);}
int main(){using namespace PtRealmDaily;
 for(int side=1;side<=2;++side){Reset();int role=side+2,flag=side==1?1:2,perfume=side==1?3:4;
  for(int run=0;run<4;++run){
   room=false;assert(!Apply(1,1,role)&&Get(1,State)==0);room=true;assert(Apply(1,1,role)&&lastFee==run&&Get(1,Runs)==run+1);
   assert(!Apply(1,1,role));Mob(30,1000,0);assert(Get(1,Kills)==0);assert(Use(1,flag)==1&&Get(1,State)==side*10+2);
   assert(Entity(sessions[1].circle,sessions[1].circleId,755)&&Npc[sessions[1].circle].Name[0]);
   Mob(30,1601,0);assert(Get(1,Kills)==0);Mob(30,1500,0);assert(Get(1,Kills)==1);Kill(30,1);assert(Get(1,Kills)==1);
   for(int n=31;n<=34;++n)Mob(n,1200,0);assert(Get(1,Kills)==5&&Get(1,State)==side*10+3);
   assert(Entity(sessions[1].soul,sessions[1].soulId,side==1?756:782));assert(Use(1,perfume)==1&&Get(1,State)==side*10+4);
   sessions[1].published=GetTickCount()-3100;int circle=sessions[1].circle,prior=Npc[circle].syncs;Tick();assert(Npc[circle].syncs>prior);
   assert(!Apply(1,2,role));Npc[1].x=200;Tick();assert(Get(1,State)==side*10+5);
   PtRealmWorldRuntime::fail=true;assert(!Apply(1,2,role)&&Get(1,State)==side*10+5);PtRealmWorldRuntime::fail=false;
   assert(Apply(1,2,role)&&Get(1,side==1?TienCredit:MaCredit)==(run+1)*2&&!Apply(1,2,role));Npc[1].x=1000;
  }assert(!Apply(1,1,role)&&coins==14&&PtRealmWorldRuntime::receipts==4);
 }
 Reset();assert(Apply(1,1,3)&&Use(1,1));int circle=sessions[1].circle;Npc[circle].m_dwID=0;Tick();assert(Get(1,State)==11&&Get(1,Kills)==0);
 assert(Use(1,1));Set(1,End,(int)time(NULL)-1);Mob(30,1000,0);assert(Get(1,Kills)==0);Tick();assert(Get(1,State)==11);
 assert(Use(1,1));Npc[1].m_CurrentLife=0;Tick();assert(Get(1,State)==11&&!sessions[1].owner);Npc[1].m_CurrentLife=100;
 assert(Use(1,1));sessions[1].actor=999;Tick();assert(Get(1,State)==11&&!sessions[1].owner);
 assert(Use(1,1));Npc[1].m_SubWorldIndex=1;Tick();assert(Get(1,State)==11&&!sessions[1].owner);
 puts("PASS production reputation: both sides, 0/1/2/3 Xu, 4 daily runs, 2 points/run, 5 distinct souls near visible circle, follow/deliver, durable failure, duplicate kill/claim, expiry/death/map/relogin reset, stable resync");return 0;
}
