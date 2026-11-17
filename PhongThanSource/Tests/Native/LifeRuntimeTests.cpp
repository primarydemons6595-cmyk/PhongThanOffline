#include <windows.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "../../Headers/PhongThanLifeSkills.h"
enum{MAX_PLAYER=4,MAX_NPC=10,MAX_SUBWORLD=120,MAX_OBJECT=4096,kind_dialoger=3,GAME_FPS=18,OBJ_PROP_STATE_DISPLAY=0,OBJ_PROP_STATE_HIDE=1,SAVE_REQUEST=1};
enum{MAX_ITEM=10,itempart_weapon=0,item_equip=0};
struct PickItem{int id,particular;int GetID(){return id;}int GetGenre(){return 0;}int GetDetailType(){return 0;}int GetParticular(){return particular?particular:8;}}Item[MAX_ITEM];
struct Bag{int pick;int GetEquipment(int){return pick;}};
struct KIniFile{BOOL Load(const char*){return TRUE;}void GetInteger(const char*,const char*,int,int* v){*v=1000;}};
int g_Random(int){return 0;}
struct Task{int v[5400];int GetSaveVal(int k){assert(k>=0&&k<5400);return v[k];}void SetSaveVal(int k,int v2,BOOL){assert(k>=0&&k<5400);v[k]=v2;}};
struct SkillList{int mine;int GetLevel(int id){return id==62?mine:0;}};
struct NpcType{DWORD m_dwID,m_ActionScriptID;int m_Kind,m_SubWorldIndex,m_NpcSettingIdx,m_Level,m_CurrentLife,x,y;SkillList m_SkillList;
 bool IsPlayer(){return m_Kind==1;}void GetMpsPos(int* a,int* b){*a=x;*b=y;}void SendSyncData(int,BOOL){}
}Npc[MAX_NPC];
typedef NpcType KNpc;
struct PlayerType{BOOL m_PlayerDBLoad;DWORD m_dwID,m_dwTaskExcuteScriptId;int m_nIndex,m_nLastNpcIndex,m_nPaceBarTime,m_nPaceBarTimeMax,locked,trading,lastMenu,m_uMustSave;char m_szTaskExcuteFun[80];Task m_cTask;
 bool CheckTrading(){return trading!=0;}bool GetLockState(){return locked!=0;}BOOL ExecuteScript(char*,char*,int v){lastMenu=v;return TRUE;}
 Bag m_ItemList;
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
int PhongThanQuestExchange(int p,int task,int before,int after,PhongThanQuestExchangeRow*,int,PhongThanQuestExchangeRow* gift,int n,bool,int,bool){
 if(bagFailure||Player[p].m_cTask.GetSaveVal(task)!=before)return 0;assert(n>=1&&n<=2&&gift->count==1);++awards;lastDetail=gift->item.detail;extraDetail=n==2?gift[1].item.detail:0;Player[p].m_cTask.SetSaveVal(task,after,TRUE);return 1;
}
struct Lua_State{int count;double args[3],result;};int currentPlayer=1;
int GetPlayerIndex(Lua_State*){return currentPlayer;}int GetObjIndex(Lua_State*){return 1;}
int Lua_GetTopIndex(Lua_State* L){return L->count;}int Lua_IsNumber(Lua_State* L,int n){return n<=L->count;}double Lua_ValueToNumber(Lua_State* L,int n){return n<=L->count?L->args[n-1]:0;}void Lua_PushNumber(Lua_State* L,double v){L->result=v;}
time_t fakeTime=1793366400;time_t LifeTestTime(time_t*){return fakeTime;}
void PhongThanMiningTick(){}
void PhongThanMiningDamaged(int){}
#define time LifeTestTime
#include "../../Sources/Core/Src/PhongThanLifeRuntime.inl"
#undef time
void ResetPlayer(int p){ZeroMemory(&Player[p],sizeof(Player[p]));Player[p].m_PlayerDBLoad=TRUE;Player[p].m_dwID=p+20;Player[p].m_nIndex=p;
 ZeroMemory(&Npc[p],sizeof(Npc[p]));Npc[p].m_Kind=1;Npc[p].m_dwID=p+50;Npc[p].m_SubWorldIndex=3;Npc[p].m_Level=200;Npc[p].m_CurrentLife=100;Npc[p].m_ActionScriptID=777;
 Player[p].m_nLastNpcIndex=8;Npc[8].m_Kind=3;Npc[8].m_NpcSettingIdx=1361;Npc[8].m_SubWorldIndex=3;Npc[8].m_ActionScriptID=777;Npc[8].x=Npc[8].y=0;}
double Call(int(*fn)(Lua_State*),double a,double b,int count){Lua_State L;ZeroMemory(&L,sizeof(L));L.count=count;L.args[0]=a;L.args[1]=b;fn(&L);return L.result;}
void AtSite(int p,int at){int obj=PtLife::sites[at].index;Npc[p].x=Object[obj].x;Npc[p].y=Object[obj].y;Npc[p].m_SubWorldIndex=Object[obj].m_nSubWorldID;Player[p].m_cTask.SetSaveVal(PT_LIFE_LEVEL_BEGIN+PT_LIFE_NODES[at].skill,PT_LIFE_NODES[at].rank,TRUE);}
void Teacher(int skill){int map=(skill==1||skill==3)?1003:(skill==4||skill==6||skill==8)?1021:1020;Npc[1].m_SubWorldIndex=Npc[8].m_SubWorldIndex=map-1000;Npc[8].m_NpcSettingIdx=map==1003?1361:map==1021?1363:1362;}
int main(){
 for(int w=0;w<MAX_SUBWORLD;++w)SubWorld[w].m_SubWorldID=w+1000;
 ResetPlayer(1);ResetPlayer(2);
 const int ids[9]={1,3,4,5,6,7,8,9,10};
 for(int i=0;i<9;++i){Teacher(ids[i]);assert(Call(LuaAddLiveSkillCompat,ids[i],1,2)==1);assert(PtLife::Get(1,5100+ids[i])==1);assert(Call(LuaAddLiveSkillCompat,ids[i],1,2)==0);assert(Call(LuaAddLiveSkillCompat,ids[i],2,2)==0);
  PtLife::Set(1,5120+ids[i],10000);assert(Call(LuaAddLiveSkillCompat,ids[i],2,2)==1);assert(PtLife::Get(1,5120+ids[i])==0);}
 assert(Call(LuaAddLiveSkillCompat,2,1,2)==0&&Call(LuaAddLiveSkillCompat,1.5,1,2)==0);
 Teacher(1);assert(!PtLife::Trainer(1,4)&&!PtLife::Trainer(1,10));Teacher(4);assert(!PtLife::Trainer(1,1)&&!PtLife::Trainer(1,10));Teacher(10);assert(!PtLife::Trainer(1,1)&&!PtLife::Trainer(1,4));
 Npc[1].x=1000;assert(!PtLife::Trainer(1));Npc[1].x=0;Player[1].locked=1;assert(Call(LuaAddLiveSkillCompat,10,3,2)==0);Player[1].locked=0;
 PtLife::Refresh(1);int cap=PtLife::Limits(1)[0];assert(PtLife::Get(1,PT_LIFE_ENERGY)==cap);
 PtLife::Set(1,PT_LIFE_ENERGY,0);fakeTime+=600;PtLife::Refresh(1);assert(PtLife::Get(1,PT_LIFE_ENERGY)==PtLife::Limits(1)[2]);
 assert(Call(LuaLifeState,5121,20000,2)==10000);assert(Call(LuaLifeState,999,100,2)==0);
 assert(Call(LuaLifeState,5142,2147483647,2)==PtLife::Limits(1)[1]);
 int craters=0;for(unsigned int c=0;c<PT_LIFE_NODE_COUNT;++c)if(PhongThanIsMiningCrater(PT_LIFE_NODES[c].object))++craters;assert(craters==34);
 PtLife::Spawn();assert(ObjSet.count==PT_LIFE_NODE_COUNT-craters);PtLife::Spawn();assert(ObjSet.count==PT_LIFE_NODE_COUNT-craters);
 for(unsigned int at=0;at<PT_LIFE_NODE_COUNT;++at){if(PhongThanIsMiningCrater(PT_LIFE_NODES[at].object)){assert(PtLife::sites[at].index==0);continue;}Object[PtLife::sites[at].index].x=PT_LIFE_NODES[at].x;Object[PtLife::sites[at].index].y=PT_LIFE_NODES[at].y;}
 int herb=0;while(herb<PT_LIFE_NODE_COUNT&&PT_LIFE_NODES[herb].detail!=910)++herb;assert(herb<PT_LIFE_NODE_COUNT);
 int obj=PtLife::sites[herb].index;AtSite(1,herb);AtSite(2,herb);PtLife::Refresh(2);
 int strength=PtLife::Get(1,PT_LIFE_STRENGTH);assert(PtLife::Start(1,obj)==1);assert(PtLife::Start(1,obj)==-5);
 assert(PtLife::Finish(1)==0&&awards==0);g_SubWorldSet.tick+=54;Player[1].m_nPaceBarTime=0;
 bagFailure=1;assert(PtLife::Finish(1)==-6&&awards==0&&Object[obj].m_nState==0&&PtLife::Get(1,PT_LIFE_STRENGTH)==strength);bagFailure=0;
 assert(PtLife::Start(1,obj)==1&&PtLife::Start(2,obj)==1);g_SubWorldSet.tick+=54;Player[1].m_nPaceBarTime=Player[2].m_nPaceBarTime=0;
 assert(PtLife::Finish(1)==1&&awards==1&&lastDetail==910&&Object[obj].m_nState==1);assert(PtLife::Finish(2)==0&&awards==1);PtLife::Clear(2);
 assert(PtLife::Get(1,PT_LIFE_STRENGTH)==strength-12&&PtLife::Get(1,5121)==10000);
 Object[obj].m_nState=0;assert(PtLife::Start(1,obj)==1);Npc[1].x++;assert(!PtLife::Valid(1));PtLife::Clear(1);assert(awards==1);
 AtSite(1,herb);assert(PtLife::Start(1,obj)==1);assert(PhongThanLifeGatherDamaged(1)&&Player[1].m_nPaceBarTime==0);
 assert(PtLife::Start(1,obj)==1);++Npc[1].m_dwID;Player[1].m_nPaceBarTime=77;assert(!PtLife::Valid(1));assert(!PhongThanLifeGatherDamaged(1));assert(Player[1].m_nPaceBarTime==77);Player[1].m_nPaceBarTime=0;
 assert(PtLife::Start(1,obj)==1);Object[obj].m_nID++;assert(!PtLife::Valid(1));PtLife::Clear(1);
 ResetPlayer(1);assert(PhongThanLifeOpenMenu(1,7)&&Player[1].lastMenu==7&&Player[1].m_nLastNpcIndex==0);assert(!PhongThanLifeOpenMenu(1,2));
 int fish=0;while(fish<PT_LIFE_NODE_COUNT&&PT_LIFE_NODES[fish].skill!=4)++fish;assert(fish<PT_LIFE_NODE_COUNT);
 ResetPlayer(1);PtLife::Refresh(1);AtSite(1,fish);obj=PtLife::sites[fish].index;assert(PtLife::Start(1,obj)==-8);
 Player[1].m_ItemList.pick=1;Item[1].id=909;Item[1].particular=71;assert(PtLife::Start(1,obj)==1);PtLife::Clear(1);
 Npc[1].x+=512;assert(PtLife::Start(1,obj)==1);Item[1].particular=8;assert(!PtLife::Valid(1));PtLife::Clear(1);Item[1].particular=71;
 Npc[1].x++;assert(PtLife::Start(1,obj)==-4);Npc[1].x--;assert(PtLife::Start(1,obj)==1);
 int beforeAwards=awards;g_SubWorldSet.tick+=54;Player[1].m_nPaceBarTime=0;assert(PtLife::Finish(1)==1&&awards==beforeAwards+1&&lastDetail==PT_LIFE_NODES[fish].detail);
 assert(!PhongThanLifeMake(1,0)&&!PhongThanLifeMake(1,129));Player[1].locked=1;assert(!PhongThanLifeMake(1,1));Player[1].locked=0;assert(PhongThanLifeMake(1,128)&&Player[1].lastMenu==128);
 printf("PASS production life runtime: sites=%d craters=%d native_respawn_ticks=%d ranks/timing/inventory/concurrent_harvest/cancellation=PASS\n",PT_LIFE_NODE_COUNT,craters,PT_MINING_CRATER_RESPAWN_TICKS);return 0;
}
