#include <windows.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "../../Headers/PhongThanLifeSkills.h"
enum{MAX_PLAYER=4,MAX_NPC=10,MAX_SUBWORLD=120,MAX_OBJECT=4096,kind_dialoger=3,GAME_FPS=18,OBJ_PROP_STATE_DISPLAY=0,OBJ_PROP_STATE_HIDE=1,SAVE_REQUEST=1};
enum{MAX_ITEM=10,itempart_weapon=0,item_equip=0};
struct PickItem{int id,particular;int GetID(){return id;}int GetGenre(){return 0;}int GetDetailType(){return 0;}int GetParticular(){return particular?particular:8;}char* GetName(){return "Copper";}}Item[MAX_ITEM];
struct PlayerItem {int nIdx;};PlayerItem inventory={1};
struct Bag{PlayerItem* GetFirstItem(){return &inventory;}PlayerItem* GetNextItem(){return 0;}int pick;int GetEquipment(int){return pick;}};
struct KIniFile{BOOL Load(const char*){return FALSE;}void GetInteger(const char*,const char*,int v,int* out){*out=v;}void GetString(const char*,const char*,const char*,char* out,int){out[0]=0;}};
int randomRoll=0;int g_Random(int range){return range>0?randomRoll%range:0;}
struct Task{int v[5400];int GetSaveVal(int k){assert(k>=0&&k<5400);return v[k];}void SetSaveVal(int k,int v2,BOOL){assert(k>=0&&k<5400);v[k]=v2;}};
struct SkillList{int mine;int GetLevel(int id){return id==62?mine:0;}};
struct NpcType{DWORD m_dwID,m_ActionScriptID;int m_Kind,m_SubWorldIndex,m_NpcSettingIdx,m_Level,m_CurrentLife,x,y,m_bRideHorse,m_CurrentLucky,lastCommand,m_Doing;SkillList m_SkillList;
 int GetPlayerIdx(){return 1;}void BeginMiningSit(){m_Doing=9;}void SendCommand(int command,int,int,int){lastCommand=command;}bool IsPlayer(){return m_Kind==1;}void GetMpsPos(int* a,int* b){*a=x;*b=y;}void SendSyncData(int,BOOL){}
}Npc[MAX_NPC];
typedef NpcType KNpc;
struct PlayerType{BOOL m_PlayerDBLoad;DWORD m_dwID,m_dwTaskExcuteScriptId;int m_nIndex,m_nLastNpcIndex,m_nPaceBarTime,m_nPaceBarTimeMax,locked,trading,lastMenu,m_uMustSave;char m_szTaskExcuteFun[80];Task m_cTask;
 bool CheckTrading(){return trading!=0;}bool GetLockState(){return locked!=0;}BOOL ExecuteScript(char*,char*,int v){lastMenu=v;return TRUE;}
 void SendSystemMessage(const char*,const char* text);Bag m_ItemList;
}Player[MAX_PLAYER];
struct World{int m_SubWorldID;void Mps2Map(int,int,int* r,int* x,int* y,int* ox,int* oy){*r=*x=*y=*ox=*oy=0;}}SubWorld[MAX_SUBWORLD];
struct Worlds{int tick;int GetGameTime(){return tick;}int SearchWorld(int map){return map>=1000&&map<1120?map-1000:-1;}}g_SubWorldSet;
DWORD g_FileName2Id(char* s){DWORD h=1;while(*s)h=h*33+(unsigned char)*s++;return h;}
void* g_GetScript(char*){return (void*)1;}BOOL ReLoadScript(char*){return TRUE;}
void g_StrCpyLen(char* dst,const char* src,int count){strncpy(dst,src,count);dst[count-1]=0;}
struct KMapPos{int nSubWorld,nRegion,nMapX,nMapY,nOffX,nOffY;};struct KObjItemInfo{int m_nMovieFlag;};
struct KObj{int m_nID,m_nDataID,m_nSubWorldID,m_nState,m_nLifeTime,x,y;DWORD m_dwScriptID;char m_szName[80];
 void GetMpsPos(int* a,int* b){*a=x;*b=y;}void SetScriptFile(char* s){m_dwScriptID=g_FileName2Id(s);}void SetState(int s){m_nState=s;}
}Object[MAX_OBJECT];
struct ObjSetType{int count;int Add(int tid,KMapPos pos,KObjItemInfo){int n=++count;assert(n<MAX_OBJECT);Object[n].m_nID=n+100;Object[n].m_nDataID=tid;Object[n].m_nSubWorldID=pos.nSubWorld;return n;}}ObjSet;
struct Tuple{int detail;};struct PhongThanQuestExchangeRow{Tuple item;int count;};
bool PhongThanDecodeQuestItemTuple(int genre,int detail,int,int,int,int,Tuple& t){assert(genre==3);t.detail=detail;return true;}
int bagFailure=0,awards=0,lastDetail=0,extraDetail=0;
int PhongThanQuestExchange(int p,int task,int before,int after,PhongThanQuestExchangeRow*,int,PhongThanQuestExchangeRow* gift,int n,bool,int,bool,int=0,bool=false,const int* =0,bool=false){
 if(bagFailure||Player[p].m_cTask.GetSaveVal(task)!=before)return 0;assert(n>=1&&n<=2&&gift->count==1);++awards;lastDetail=gift->item.detail;extraDetail=n==2?gift[1].item.detail:0;Player[p].m_cTask.SetSaveVal(task,after,TRUE);return 1;
}
struct Lua_State{int count;double args[3],result;};int currentPlayer=1;
int GetPlayerIndex(Lua_State*){return currentPlayer;}int GetObjIndex(Lua_State*){return 1;}
int Lua_GetTopIndex(Lua_State* L){return L->count;}int Lua_IsNumber(Lua_State* L,int n){return n<=L->count;}double Lua_ValueToNumber(Lua_State* L,int n){return n<=L->count?L->args[n-1]:0;}void Lua_PushNumber(Lua_State* L,double v){L->result=v;}
time_t fakeTime=1793366400;time_t LifeTestTime(time_t*){return fakeTime;}

#define MESSAGE_SYSTEM_ANNOUCE_HEAD ""
enum {do_skill=6,do_sit=9};DWORD fakeTick=1000;DWORD MiningTestTick(){return fakeTick;}char lastMessage[512];
void PlayerType::SendSystemMessage(const char*,const char* text){strcpy(lastMessage,text);}
bool PhongThanQuestItemMatches(const PickItem&,const Tuple&){return true;}
#define time LifeTestTime
#define GetTickCount MiningTestTick
#include "../../Sources/Core/Src/PhongThanMiningRuntime.inl"
#undef time
int main(){
 assert(PtMiningChance(2000,10,0)==2000&&PtMiningChance(2000,10,1000)==10000&&PtMiningChance(2000,10,-5)==2000);
 assert(PtMiningNear(0,0,512,0,512)&&!PtMiningNear(0,0,513,0,512));
 PtMining::loaded=PtMining::enabled=PtMining::stateValid=true;PtMining::siteCount=PtMining::lootCount=1;PtMining::chance=10000;
 PTMiningSite& s=PtMining::sites[0];ZeroMemory(&s,sizeof(s));s.id=1;s.map=1057;s.x=1000;s.y=2000;s.capacity=3;s.respawn=600;s.reach=512;s.enabled=1;
 assert(PtMiningSiteValid(s));PTMiningLoot& l=PtMining::loot[0];ZeroMemory(&l,sizeof(l));l.genre=3;l.detail=3;l.weight=1000;l.minimum=l.maximum=1;assert(PtMiningLootValid(l));
 strcpy(PtMining::prefix,"You mined ");strcpy(PtMining::between," ");strcpy(PtMining::suffix,".");
 Player[1].m_PlayerDBLoad=true;Player[1].m_dwID=20;Player[1].m_nIndex=1;Player[1].m_ItemList.pick=1;Item[1].id=30;
 Npc[1].m_dwID=21;Npc[1].m_Kind=1;Npc[1].m_CurrentLife=100;Npc[1].m_SubWorldIndex=57;Npc[1].m_SkillList.mine=1;Npc[1].x=1000;Npc[1].y=2000;
 SubWorld[57].m_SubWorldID=1057;PtMining::Update();assert(PtMining::state.count==1&&PtMining::state.mines[0].remaining==3);
 assert(PhongThanMiningCanCast(1,1000,2000));assert(PhongThanMiningClick(1,PtMining::objects[0].index)&&Npc[1].lastCommand==do_skill&&Player[1].m_nPaceBarTime==0);
 Npc[1].x+=512;assert(PtMiningRoll(1,1000,2000)&&awards==1&&PtMining::state.mines[0].remaining==2&&Player[1].m_nPaceBarTime==0);
 assert(strstr(lastMessage,"<color=red>1<color>")&&strstr(lastMessage,"<color=yellow>Copper<color>"));
 Npc[1].x++;assert(!PtMiningRoll(1,1000,2000)&&awards==1);Npc[1].x--;
 bagFailure=1;assert(!PtMiningRoll(1,1000,2000)&&PtMining::state.mines[0].remaining==2);bagFailure=0;
 Item[1].particular=71;assert(!PtMiningRoll(1,1000,2000));Item[1].particular=8;
 PtMining::chance=0;assert(PtMiningRoll(1,1000,2000)&&awards==1&&PtMining::state.mines[0].remaining==2);PtMining::chance=10000;
 assert(PtMiningRoll(1,1000,2000)&&PtMiningRoll(1,1000,2000)&&PtMining::state.mines[0].remaining==0);
 assert(!PtMiningRoll(1,1000,2000)&&awards==3);PtMining::LoadState();assert(PtMining::stateValid&&PtMining::state.mines[0].remaining==0);
 fakeTime+=599;PtMining::Update();assert(PtMining::state.mines[0].remaining==0);++fakeTime;PtMining::Update();assert(PtMining::state.mines[0].remaining==3);
 PtMining::state.mines[0].remaining=10;PtMining::sites[0].capacity=10;Npc[1].x=1000;int oldAwards=awards;
 assert(PhongThanMiningCast(1,1000,2000)&&Npc[1].m_Doing==do_sit&&PtMining::sessions[1].owner&&awards==oldAwards+1);
 assert(PhongThanMiningCast(1,1000,2000)&&awards==oldAwards+1);
 fakeTick+=1000;PhongThanMiningTick();assert(awards==oldAwards+2&&Npc[1].m_Doing==do_sit&&Player[1].m_nPaceBarTime==0);
 Npc[1].x++;fakeTick+=1000;PhongThanMiningTick();assert(!PtMining::sessions[1].owner&&awards==oldAwards+2);
 Npc[1].x--;assert(PhongThanMiningCast(1,1000,2000)&&PtMining::sessions[1].owner);PhongThanMiningDamaged(1);assert(!PtMining::sessions[1].owner);
 PtMining::chance=0;fakeTick+=1000;assert(PhongThanMiningCast(1,1000,2000)&&Npc[1].m_Doing==do_sit);oldAwards=awards;fakeTick+=1000;PhongThanMiningTick();assert(awards==oldAwards&&PtMining::sessions[1].owner);Npc[1].m_Doing=0;fakeTick+=1000;PhongThanMiningTick();assert(!PtMining::sessions[1].owner);
 fakeTick+=1000;PtMining::chance=10000;assert(PhongThanMiningCast(1,1000,2000));oldAwards=awards;
 Item[1].particular=71;fakeTick+=1000;PhongThanMiningTick();assert(!PtMining::sessions[1].owner&&awards==oldAwards);Item[1].particular=8;
 fakeTick+=1000;assert(PhongThanMiningCast(1,1000,2000));oldAwards=awards;Player[1].m_dwID++;fakeTick+=1000;PhongThanMiningTick();assert(!PtMining::sessions[1].owner&&awards==oldAwards);
 fakeTick+=1000;assert(PhongThanMiningCast(1,1000,2000));oldAwards=awards;Player[1].trading=1;fakeTick+=1000;PhongThanMiningTick();assert(!PtMining::sessions[1].owner&&awards==oldAwards);Player[1].trading=0;
 puts("PASS actual mining runtime: active seated sessions, repeated random rolls, movement/action/damage cancellation, rim range, chance/luck, no progress, shared reserves, full-bag rollback, exhaustion/restart/refill, colored messages");return 0;
}
