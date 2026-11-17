#include <windows.h>
#include <stdio.h>
#include <assert.h>
#include <time.h>
#include <limits.h>
enum{MAX_PLAYER=4,MAX_NPC=512,MAX_ITEM=32,MAX_SUBWORLD=8,MAX_ITEM_MAGICLEVEL=20,GAME_FPS=18,MAX_TASK=5400,kind_dialoger=3,item_equip=0,item_medicine=1,item_event=2,item_materials=3,item_task=4,item_townportal=5,item_magicscript=6,item_skillbook=7,item_ibitem=8,equip_detailnum=15};
#define MAX_INT INT_MAX
#define MESSAGE_SYSTEM_ANNOUCE_HEAD ""
static time_t utc=1700000000;static DWORD tick=1000;static int frame,rng,spawned,removed,bagFail,blocked,member[MAX_PLAYER],lastQty,lastGenre,lastDetail,lastPart,lastLevel,lastSeries,prototypeUsed;
static bool xich=true;
struct Tasks{int v[MAX_TASK];int GetSaveVal(int k){assert(k>=0&&k<MAX_TASK);return v[k];}void SetSaveVal(int k,int n,BOOL){assert(k>=0&&k<MAX_TASK);v[k]=n;}};
struct PK{int value;int GetNormalPKState(){return value;}};
struct Profession{int value;int GetProfession(){return value;}};
struct Actor{DWORD m_dwID,m_ActionScriptID,m_DeathScriptID;int m_NpcSettingIdx,m_Level,m_CurrentLife,m_FightMode,m_SubWorldIndex,m_Kind,m_AiMode,x,y;char Name[40],ActionScript[160];void GetMpsPos(int* a,int* b){*a=x;*b=y;}void SendSyncData(int,BOOL){}}Npc[MAX_NPC];
struct KPlayer{DWORD m_dwID,m_dwTaskExcuteScriptId;BOOL m_PlayerDBLoad;int m_nIndex,m_nLastNpcIndex,m_nPaceBarTime,m_nPaceBarTimeMax,cash;char m_szTaskExcuteFun[80];bool locked,trading;Tasks m_cTask;PK m_cPK;Profession m_cProfession;
 bool GetLockState(){return locked;}bool CheckTrading(){return trading;}bool Earn(int n){if(n<0||cash>MAX_INT-n)return false;cash+=n;return true;}bool Pay(int n){if(n<0||cash<n)return false;cash-=n;return true;}void SendSystemMessage(const char*,const char*){}
}Player[MAX_PLAYER];
struct KItem{int genre,detail,part,level,series;DWORD id;struct{int nExpireTime;}m_CommonAttrib;int GetGenre(){return genre;}int GetDetailType(){return detail;}int GetParticular(){return part;}int GetLevel(){return level;}int GetSeries(){return series;}DWORD GetID(){return id;}}Item[MAX_ITEM];
struct Items{int Add(int g,int s,int l,int,int d,int p,int*,int,int,int){Item[2].genre=g;Item[2].series=s;Item[2].level=l;Item[2].detail=d;Item[2].part=p;return 2;}void Remove(int){}}ItemSet;
struct World{int m_SubWorldID;void Mps2Map(int,int,int* r,int*,int*,int*,int*){*r=0;}BYTE TestBarrier(int,int){return blocked?1:0;}}SubWorld[MAX_SUBWORLD];
struct Worlds{int GetGameTime(){return frame;}int GetGameVersion(){return 1;}}g_SubWorldSet;
struct Npcs{int Add(DWORD packed,int w,int x,int y,BOOL){int n=++spawned+20;assert(n<MAX_NPC);Npc[n].m_dwID=1000+n;Npc[n].m_NpcSettingIdx=HIWORD(packed);Npc[n].m_SubWorldIndex=w;Npc[n].x=x;Npc[n].y=y;return n;}}NpcSet;
struct KMission{int generation,v[100],world;int GetMissionValue(int n){return v[n];}int AddNpc(int n,DWORD,int){return n;}int GetMissionNpc_DataIndex(int n,DWORD serial){return n>0&&Npc[n].m_dwID==serial?n:0;}}missions[5];
static int g_TaskGlobalValue[5],inventory[MAX_PLAYER][100000];
static DWORD g_FileName2Id(char* s){DWORD h=0;while(*s)h=h*33+(unsigned char)*s++;return h;}
static DWORD g_FileName2Id(const char* s){return g_FileName2Id((char*)s);}
static int g_GetScript(char*){return 1;}static int ReLoadScript(char*){return 1;}
static void g_StrCpyLen(char* d,const char* s,int n){strncpy(d,s,n-1);d[n-1]=0;}
static bool PtEscortNear(int x,int y,int a,int b,int r){return (x-a)*(x-a)+(y-b)*(y-b)<=r*r;}
static void PhongThanRemoveNpc(int n){Npc[n].m_dwID=0;++removed;}
static int g_Random(int limit){return rng++%limit;}
static DWORD KSG_GetCurSec(){return (DWORD)utc;}
static bool PtPetCarried(int,int n){return n>0&&n<MAX_ITEM&&Item[n].id;}
static bool PhongThanNearXichTungTu(int){return xich;}
namespace PtVtt{static bool Live(int p){return p>0&&p<MAX_PLAYER&&Player[p].m_nIndex>0&&Npc[Player[p].m_nIndex].m_dwID;}static KMission* Mission(int id,int* w=0){if(id<1||id>4||!missions[id].generation)return 0;if(w)*w=id;return &missions[id];}static int Generation(KMission* m){return m?m->generation:0;}}
namespace PtArena{struct Entity{int index;DWORD serial;bool broken;Entity():index(0),serial(0),broken(false){}};static bool Live(const Entity& e){return e.index>0&&e.index<MAX_NPC&&Npc[e.index].m_dwID==e.serial;}static void Remove(Entity& e){if(Live(e)){Npc[e.index].m_dwID=0;++removed;}e=Entity();}static bool Member(int p,KMission*){return member[p]!=0;}}
#include "../../Sources/Core/Src/PhongThanQuestItemTuple.h"
struct PhongThanQuestExchangeRow{PhongThanQuestItemTuple item;int count;};
static int Key(const PhongThanQuestItemTuple& t){return t.genre*10000+t.detail;}
static int PhongThanQuestExchange(int p,int task,int expected,int next,PhongThanQuestExchangeRow* needs,int nc,PhongThanQuestExchangeRow* rewards,int rc,bool=false,KItem* prototype=0,bool=false){
 if(bagFail||Player[p].locked||Player[p].trading||expected==next||Player[p].m_cTask.v[task]!=expected)return 0;
 for(int i=0;i<nc;++i)if(inventory[p][Key(needs[i].item)]<needs[i].count)return 0;
 for(i=0;i<nc;++i)inventory[p][Key(needs[i].item)]-=needs[i].count;
 for(i=0;i<rc;++i){inventory[p][Key(rewards[i].item)]+=rewards[i].count;lastQty=rewards[i].count;lastGenre=rewards[i].item.genre;lastDetail=rewards[i].item.detail;lastPart=rewards[i].item.particular;lastLevel=rewards[i].item.level;lastSeries=rewards[i].item.series;prototypeUsed=prototype!=0;}
 Player[p].m_cTask.v[task]=next;return 1;
}
struct Lua_State{};static double arg[3];static int argcLua,lastLua;
static int GetPlayerIndex(Lua_State*){return 1;}static int Lua_GetTopIndex(Lua_State*){return argcLua;}static bool Lua_IsNumber(Lua_State*,int){return true;}static double Lua_ValueToNumber(Lua_State*,int n){return arg[n-1];}static void Lua_PushNumber(Lua_State*,int n){lastLua=n;}
static time_t MockTime(time_t*){return utc;}static DWORD MockTick(){return tick;}
#define time MockTime
#define GetTickCount MockTick
#include "../../Sources/Core/Src/PhongThanTgptVtt.inl"
#undef time
#undef GetTickCount
static void Reset(int arena=1){
 ZeroMemory(Player,sizeof(Player));ZeroMemory(Npc,sizeof(Npc));ZeroMemory(Item,sizeof(Item));ZeroMemory(inventory,sizeof(inventory));ZeroMemory(missions,sizeof(missions));ZeroMemory(member,sizeof(member));ZeroMemory(g_TaskGlobalValue,sizeof(g_TaskGlobalValue));spawned=removed=frame=rng=bagFail=blocked=lastQty=0;tick+=1000;xich=true;
 for(int i=0;i<4;++i)PtTgpt::loot[i]=PtTgpt::Loot();for(i=0;i<MAX_PLAYER;++i)PtTgpt::sessions[i]=PtTgpt::Session();
 for(i=1;i<=2;++i){Player[i].m_PlayerDBLoad=TRUE;Player[i].m_dwID=100+i;Player[i].m_nIndex=i;Player[i].cash=1000000;Player[i].m_cPK.value=1;Npc[i].m_dwID=200+i;Npc[i].m_SubWorldIndex=arena;Npc[i].m_CurrentLife=100;Npc[i].m_FightMode=1;member[i]=1;}
 for(i=1;i<=4;++i)SubWorld[i].m_SubWorldID=1066+i;SubWorld[0].m_SubWorldID=1021;SubWorld[5].m_SubWorldID=1052;
 missions[arena].generation=123;missions[arena].world=arena;g_TaskGlobalValue[arena]=2;
 PtTgpt::Loot& l=PtTgpt::loot[arena-1];l.active=true;l.generation=123;l.world=arena;PtTgpt::SpawnAll(arena,&missions[arena]);
 Item[1].genre=6;Item[1].detail=12;Item[1].id=99;
}
static void Approach(int p,int arena,int slot){int n=PtTgpt::loot[arena-1].chests[slot].npc.index;Player[p].m_nLastNpcIndex=n;Npc[p].x=Npc[n].x;Npc[p].y=Npc[n].y;}
static void Complete(int p){frame=PtTgpt::sessions[p].deadline;Player[p].m_nPaceBarTime=0;}
static void Merchant(int type=0){Npc[1].m_SubWorldIndex=0;Npc[10].m_SubWorldIndex=0;Npc[10].m_Kind=3;Npc[10].m_NpcSettingIdx=155;Npc[10].x=Npc[1].x;Npc[10].y=Npc[1].y;Player[1].m_nLastNpcIndex=10;if(type==2)Npc[10].m_ActionScriptID=g_FileName2Id("\\script\\phongthan\\van_tien\\ung_tiem_thuong.lua");}
static bool Admission(int id,int npc){
#include "TgptAdmissionActual.inl"
 return level>=minLevel&&level<=maxLevel;
}
int main(){
 const int low[]={30,51,71,91},high[]={50,70,90,120};for(int entry=1;entry<=4;++entry){Npc[1].m_Level=low[entry-1]-1;assert(!Admission(entry,1));Npc[1].m_Level=low[entry-1];assert(Admission(entry,1));Npc[1].m_Level=high[entry-1];assert(Admission(entry,1));Npc[1].m_Level=high[entry-1]+1;assert(!Admission(entry,1));}
 for(int id=1;id<=4;++id){Reset(id);assert(spawned==5);PtTgpt::SpawnAll(id,&missions[id]);assert(spawned==5);
  for(int i=0;i<5;++i){int qty=PtTgpt::loot[id-1].chests[i].pearls;assert(qty==i+1);Approach(1,id,i);Player[1].m_cTask.v[PtTgpt::Cooldown]=0;assert(PtTgpt::Start(1)==1);assert(Player[1].m_nPaceBarTime==20*GAME_FPS);assert(!PtTgpt::Finish(1));Complete(1);assert(PtTgpt::Finish(1)==1&&lastQty==qty&&lastDetail==PT_TGPT_PEARLS[id-1]);assert(!PtTgpt::Finish(1));}
  assert(inventory[1][30000+PT_TGPT_PEARLS[id-1]]==15);PtTgpt::SpawnAll(id,&missions[id]);assert(spawned==5);printf("PASS arena %d: exactly 5 chests, stable 1..5 pearls, timed claims, no duplicates\n",id);
 }
 Reset();Approach(1,1,0);Approach(2,1,0);assert(PtTgpt::Start(1)==1&&PtTgpt::Start(2)==-3);Npc[1].x+=32;PhongThanTgptVttTick();assert(!PtTgpt::sessions[1].owner&&PtTgpt::Start(2)==1);assert(PhongThanTgptVttDamaged(2)&&!PtTgpt::sessions[2].owner);
 Reset();Approach(1,1,0);Npc[1].m_FightMode=0;assert(!PtTgpt::Start(1));Npc[1].m_FightMode=1;Player[1].m_cPK.value=0;assert(!PtTgpt::Start(1));Player[1].m_cPK.value=1;assert(PtTgpt::Start(1)==1);int qty=PtTgpt::loot[0].chests[0].pearls;Complete(1);bagFail=1;assert(PtTgpt::Finish(1)==-4&&!PtTgpt::loot[0].chests[0].claimed&&!Player[1].m_cTask.v[PtTgpt::Cooldown]);bagFail=0;assert(PtTgpt::Start(1)==1&&PtTgpt::loot[0].chests[0].pearls==qty);Complete(1);assert(PtTgpt::Finish(1)==1);Approach(1,1,1);assert(PtTgpt::Start(1)==-2);
 Reset();Approach(1,1,0);assert(PtTgpt::Start(1)==1);++Player[1].m_dwID;PhongThanTgptVttTick();assert(!PtTgpt::sessions[1].owner&&!PtTgpt::loot[0].chests[0].player);
 Reset();g_TaskGlobalValue[1]=0;tick+=1000;PhongThanTgptVttTick();assert(!PtTgpt::loot[0].active&&removed==5);
 for(id=1;id<=4;++id)for(int profession=0;profession<3;++profession){Reset();Merchant();int key=30000+PT_TGPT_PEARLS[id-1];inventory[1][key]=PT_TGPT_COSTS[id-1];bagFail=1;assert(!PtTgpt::Weapon(1,id,profession)&&inventory[1][key]==PT_TGPT_COSTS[id-1]);bagFail=0;assert(PtTgpt::Weapon(1,id,profession)==1&&inventory[1][key]==0&&lastPart==PT_TGPT_WEAPONS[id-1][profession].part&&lastLevel==PT_TGPT_WEAPONS[id-1][profession].level&&lastSeries==profession&&prototypeUsed);assert(!PtTgpt::Weapon(1,id,profession));}
 Reset();Merchant();for(int tier=1;tier<=3;++tier){inventory[1][30061+tier]=2;assert(PtTgpt::Merge(1,tier)==1&&inventory[1][30061+tier]==0&&lastDetail==62+tier);}assert(!PtTgpt::Merge(1,4));
 Reset();Merchant();Player[1].m_cTask.v[405]=7;Player[1].m_cTask.v[406]=1;Player[1].m_cTask.v[408]=100000;assert(PtTgpt::Extra(1,0)==1);Player[1].m_cTask.v[405]=8;bagFail=1;assert(!PtTgpt::Extra(1,1)&&Player[1].m_cTask.v[PtTgpt::ExtraPending]==(1<<7));bagFail=0;assert(PtTgpt::Extra(1,1)==1&&lastGenre==6&&lastDetail==278);assert(!PtTgpt::Extra(1,1));Player[1].m_cTask.v[406]=1;assert(PtTgpt::Green(1,4)==1&&lastGenre==0&&lastDetail==7&&lastPart==5&&lastLevel==7);int count=inventory[1][7];assert(PtTgpt::Green(1,4)==1&&inventory[1][7]==count);
 Reset();Merchant(2);Npc[1].m_Level=5;int cash=Player[1].cash;bagFail=1;assert(!PtTgpt::BuyLottery(1,5)&&Player[1].cash==cash);bagFail=0;assert(PtTgpt::BuyLottery(1,5)==1&&Player[1].cash==cash-5000&&inventory[1][60012]==5);assert(!PtTgpt::BuyLottery(1,2));
 int earth=0,water=0,repeat=0,money100=0,money20=0,empty=0;
 for(int draw=1;draw<=1000;++draw){Reset();inventory[1][60012]=1;Player[1].m_cTask.v[PtTgpt::LotteryDraw]=draw;cash=Player[1].cash;assert(PhongThanUseTgptLottery(1,1)==1&&Player[1].m_cTask.v[PtTgpt::LotteryDraw]==0);
  if(inventory[1][30062])++earth;else if(inventory[1][30063])++water;else if(inventory[1][60012])++repeat;else if(Player[1].cash==cash+100000)++money100;else if(Player[1].cash==cash+20000)++money20;else ++empty;
 }
 assert(earth==100&&water==50&&repeat==100&&money100==1&&money20==5&&empty==744);
 Reset();inventory[1][60012]=1;Player[1].m_cTask.v[PtTgpt::LotteryDraw]=107;bagFail=1;assert(!PhongThanUseTgptLottery(1,1)&&inventory[1][60012]==1&&Player[1].m_cTask.v[PtTgpt::LotteryDraw]==107);bagFail=0;assert(PhongThanUseTgptLottery(1,1)==1&&inventory[1][30062]==1&&!inventory[1][60012]);
 Reset();Merchant();Player[1].m_cTask.v[405]=9;Player[1].m_cTask.v[406]=1;Player[1].m_cTask.v[408]=100000;Player[1].m_cTask.v[PtTgpt::ExtraPending]=(1<<7)|(1<<8);assert(PtTgpt::Extra(1,0)==1);Player[1].m_cTask.v[405]=10;for(int k=0;k<3;++k)assert(PtTgpt::Extra(1,1)==1);assert(!PtTgpt::Extra(1,1)&&Player[1].m_cTask.v[PtTgpt::ExtraPending]==0);
 utc=1700000000-86400;LuaEventVttExpMultiplier(0);assert(lastLua==2);utc+=86400;LuaEventVttExpMultiplier(0);assert(lastLua==1);
 Reset();inventory[1][60012]=1;Player[1].m_cTask.v[PtTgpt::LotteryDraw]=1;bagFail=1;cash=Player[1].cash;assert(!PhongThanUseTgptLottery(1,1)&&Player[1].cash==cash&&inventory[1][60012]==1);
 puts("PASS actual native VTT: concurrency, movement/damage/PK/bag/relog/cooldown guards, cleanup, 12 exact gold weapons, merge costs, green/blueprint receipts, lottery buy rollback and exhaustive 1000-outcome distribution");
 return 0;
}
