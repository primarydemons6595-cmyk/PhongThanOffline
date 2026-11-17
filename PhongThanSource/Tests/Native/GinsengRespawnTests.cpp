#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#define _SERVER
#define MAX_PLAYER 4
#define MAX_NPC 20
#define MAX_OBJECT 64
#define GAME_FPS 18
enum {kind_normal=0,kind_player=1,OBJ_PROP_STATE_DISPLAY=0,OBJ_PROP_STATE_HIDE=1};
static int purifyUntil=0;static int gameTime=1000,currentPlayer=1,currentObject=0,inventory=0,fullBag=0;static DWORD serial=100;
struct Tasks {int v[5000];int GetSaveVal(int t){return v[t];}void SetSaveVal(int t,int v1,BOOL){v[t]=v1;}};
struct Role {DWORD m_dwID,m_dwTaskExcuteScriptId;int m_nIndex,m_nPaceBarTime,m_nPaceBarTimeMax;char m_szTaskExcuteFun[80];Tasks m_cTask;bool lock,trade;bool GetLockState(){return lock;}bool CheckTrading(){return trade;}}Player[MAX_PLAYER];
struct KNpc {DWORD m_dwID;int m_Kind,m_SubWorldIndex,m_CurrentLife,m_Series,m_NpcSettingIdx,m_CurrentVisionRadius,m_AiMode,x,y;int syncCount;void SendSyncData(int,BOOL){++syncCount;}bool IsPlayer(){return m_Kind==kind_player;}void GetMpsPos(int*a,int*b){*a=x;*b=y;}}Npc[MAX_NPC];
struct KMapPos {int nSubWorld,nRegion,nMapX,nMapY,nOffX,nOffY;};
struct World {int m_SubWorldID;void Mps2Map(int x,int y,int*r,int*a,int*b,int*c,int*d){*r=1;*a=x;*b=y;*c=*d=0;}}SubWorld[3];
struct Worlds {int SearchWorld(int map){return map==1016?1:map==1014?2:-1;}int GetGameTime(){return gameTime;}}g_SubWorldSet;
static DWORD g_FileName2Id(char*){return 345;}
struct KObj {int m_nID,m_nDataID,m_nSubWorldID,m_nState,x,y;DWORD m_dwScriptID;void GetMpsPos(int*a,int*b){*a=x;*b=y;}void SetScriptFile(char*){m_dwScriptID=345;}void SetState(int s){m_nState=s;}}Object[MAX_OBJECT];
struct KObjItemInfo {int m_nMovieFlag;};
struct Objects {int next;int Add(int tid,KMapPos p,KObjItemInfo){int i=++next;Object[i].m_nID=++serial;Object[i].m_nDataID=tid;Object[i].m_nSubWorldID=p.nSubWorld;Object[i].x=p.nMapX;Object[i].y=p.nMapY;return i;}}ObjSet;
struct Npcs {int GetNextIdx(int at){while(++at<MAX_NPC)if(Npc[at].m_dwID)return at;return 0;}}NpcSet;
static void g_StrCpyLen(char*a,const char*b,int n){strncpy(a,b,n);a[n-1]=0;}
struct Tuple {int g,d,p;};struct PhongThanQuestExchangeRow {Tuple item;int count;};
static void PhongThanDecodeQuestItemTuple(int g,int d,int p,int,int,int,Tuple&t){t.g=g;t.d=d;t.p=p;}
static int PhongThanQuestExchange(int p,int task,int from,int to,void*,int,PhongThanQuestExchangeRow* reward,int n,bool){if(fullBag||Player[p].m_cTask.GetSaveVal(task)!=from||from==to||n!=1||reward->item.g!=4||(reward->item.d!=192&&reward->item.d!=201)||reward->count!=1)return 0;Player[p].m_cTask.SetSaveVal(task,to,TRUE);++inventory;return 1;}
struct Lua_State{};static int GetPlayerIndex(Lua_State*){return currentPlayer;}static int GetObjIndex(Lua_State*){return currentObject;}static int Lua_GetTopIndex(Lua_State*){return 0;}static int Lua_IsNumber(Lua_State*,int){return 0;}static double Lua_ValueToNumber(Lua_State*,int){return 0;}static void Lua_PushNumber(Lua_State*,int){}
#include "../../Sources/Core/Src/PhongThanEscortRules.h"
struct PHONGTHAN_IBBUFF_STORE{};static PHONGTHAN_IBBUFF_STORE buffs;static PHONGTHAN_IBBUFF_STORE* GetPlayerIBBuffStore(int){return &buffs;}static int FindIBBuffSlot(PHONGTHAN_IBBUFF_STORE*,int id){return id==336&&gameTime<purifyUntil?0:-1;}
#include "../../Sources/Core/Src/PhongThanGiapGather.inl"
static int checks=0;
#define CHECK(x) do{++checks;if(!(x)){printf("FAIL GINSENG line=%d check=%d %s\n",__LINE__,checks,#x);return 1;}}while(0)
static void Reset(){ZeroMemory(Player,sizeof(Player));ZeroMemory(Npc,sizeof(Npc));ZeroMemory(Object,sizeof(Object));for(int i=0;i<15;++i)PtGinseng::plants[i]=PtGinseng::Plant();for(i=0;i<MAX_PLAYER;++i)PtGinseng::sessions[i]=PtGinseng::Session();ObjSet.next=0;inventory=fullBag=0;gameTime=1000;purifyUntil=0;
 Player[1].m_dwID=5;Player[1].m_nIndex=1;Player[1].m_cTask.v[4550]=3;Npc[1].m_dwID=55;Npc[1].m_Kind=kind_player;Npc[1].m_SubWorldIndex=1;Npc[1].m_CurrentLife=100;SubWorld[1].m_SubWorldID=1016;SubWorld[2].m_SubWorldID=1014;PtGinseng::Plants();Npc[1].x=PT_GINSENG_POSITIONS[0][0];Npc[1].y=PT_GINSENG_POSITIONS[0][1];}
static int Plant(int i){return PtGinseng::plants[i].index;}
static void Elapse(){gameTime+=54;Player[1].m_nPaceBarTime=0;}
int main(){
 Reset();CHECK(PtGinseng::Start(1,Plant(0))==1);CHECK(Npc[1].syncCount==1);
 gameTime+=3;Player[1].m_nPaceBarTime-=3;PhongThanGiapGatherTick();CHECK(Npc[1].syncCount==2);
 PhongThanGiapGatherDamaged(1);CHECK(Npc[1].syncCount==3&&Player[1].m_nPaceBarTime==0);
 Reset();for(int i=0;i<15;++i)CHECK(PtGinseng::PlantLive(i)&&Object[Plant(i)].m_nDataID==PT_GINSENG_POSITIONS[i][3]);
 CHECK(PtGinseng::Start(1,Plant(0))==1);CHECK(Player[1].m_nPaceBarTime==54&&Player[1].m_nPaceBarTimeMax==54&&inventory==0);
 CHECK(PtGinseng::Finish(1)==0&&inventory==0);CHECK(PtGinseng::Start(1,Plant(1))==0);
 Elapse();CHECK(PtGinseng::Finish(1)==1&&inventory==1&&Player[1].m_cTask.v[4553]==1&&Object[Plant(0)].m_nState==OBJ_PROP_STATE_HIDE);CHECK(PtGinseng::Finish(1)==0&&inventory==1);
 CHECK(PtGinseng::Start(1,Plant(0))==0);Object[Plant(0)].m_nState=0;CHECK(PtGinseng::Start(1,Plant(0))==1);Elapse();CHECK(PtGinseng::Finish(1)==1&&inventory==2&&Player[1].m_cTask.v[4553]==0);
 Reset();CHECK(PtGinseng::Start(1,Plant(0))==1);PhongThanGiapGatherDamaged(1);Elapse();CHECK(PtGinseng::Finish(1)==0&&inventory==0&&Player[1].m_cTask.v[4553]==0);
 Reset();CHECK(PtGinseng::Start(1,Plant(0))==1);Npc[1].x++;PhongThanGiapGatherTick();Elapse();CHECK(PtGinseng::Finish(1)==0&&inventory==0);
 Reset();CHECK(PtGinseng::Start(1,Plant(0))==1);Player[1].m_nPaceBarTime=0;PhongThanGiapGatherTick();Elapse();CHECK(PtGinseng::Finish(1)==0);
 Reset();CHECK(PtGinseng::Start(1,Plant(0))==1);Npc[1].m_SubWorldIndex=0;Elapse();CHECK(PtGinseng::Finish(1)==0);
 Reset();CHECK(PtGinseng::Start(1,Plant(0))==1);Player[1].m_dwID=6;Elapse();CHECK(PtGinseng::Finish(1)==0);
 Reset();CHECK(PtGinseng::Start(1,Plant(0))==1);Object[Plant(0)].m_nID++;Elapse();CHECK(PtGinseng::Finish(1)==0&&inventory==0);
 Reset();CHECK(PtGinseng::Start(1,Plant(0))==1);fullBag=1;Elapse();CHECK(PtGinseng::Finish(1)==0&&inventory==0&&Player[1].m_cTask.v[4553]==0);fullBag=0;CHECK(PtGinseng::Start(1,Plant(0))==1);
 Reset();CHECK(PtGinseng::Start(1,Plant(0))==1);gameTime+=53;Player[1].m_nPaceBarTime=0;CHECK(PtGinseng::Finish(1)==1);
 Reset();Player[1].lock=true;CHECK(PtGinseng::Start(1,Plant(0))==0);Player[1].lock=false;Player[1].trade=true;CHECK(PtGinseng::Start(1,Plant(0))==0);
 Reset();Npc[1].m_Series=4;CHECK(PtGinseng::Start(1,Plant(0))==0);Npc[1].m_Series=0;Player[1].m_cTask.v[4550]=4;CHECK(PtGinseng::Start(1,Plant(0))==0);
 Reset();Npc[1].x+=1000;CHECK(PtGinseng::Start(1,Plant(0))==0);
 Reset();for(i=9;i>=0;--i){Npc[1].x=PT_GINSENG_POSITIONS[i][0];Npc[1].y=PT_GINSENG_POSITIONS[i][1];CHECK(PtGinseng::Start(1,Plant(i))==1);Elapse();CHECK(PtGinseng::Finish(1)==1);}CHECK(inventory==10&&Player[1].m_cTask.v[4553]==1023);
 Reset();Npc[3].m_dwID=77;Npc[3].m_Kind=kind_normal;Npc[3].m_SubWorldIndex=1;Npc[3].m_NpcSettingIdx=608;Npc[3].x=198*256;Npc[3].y=204*512;PtGinseng::Plants();CHECK(Npc[3].m_CurrentVisionRadius==800&&Npc[3].m_AiMode==1);
 Reset();Player[1].m_cTask.v[4550]=5;Npc[1].m_SubWorldIndex=2;Npc[1].x=PT_GINSENG_POSITIONS[10][0];Npc[1].y=PT_GINSENG_POSITIONS[10][1];CHECK(PtGinseng::Start(1,Plant(10))==0);purifyUntil=gameTime+540;CHECK(PtGinseng::Start(1,Plant(10))==1);Elapse();CHECK(PtGinseng::Finish(1)==2&&inventory==1&&Player[1].m_cTask.v[4553]==1024);
 CHECK(PtGinseng::Start(1,Plant(10))==0);Object[Plant(10)].m_nState=0;CHECK(PtGinseng::Start(1,Plant(10))==1);Elapse();CHECK(PtGinseng::Finish(1)==2&&inventory==2);
 Reset();Player[1].m_cTask.v[4550]=5;Npc[1].m_SubWorldIndex=2;Npc[1].x=PT_GINSENG_POSITIONS[10][0];Npc[1].y=PT_GINSENG_POSITIONS[10][1];purifyUntil=gameTime+10;CHECK(PtGinseng::Start(1,Plant(10))==1);Elapse();CHECK(PtGinseng::Finish(1)==0&&inventory==0);
 Reset();Player[1].m_cTask.v[4550]=5;Npc[1].m_SubWorldIndex=2;purifyUntil=gameTime+540;for(i=10;i<15;++i){Npc[1].x=PT_GINSENG_POSITIONS[i][0];Npc[1].y=PT_GINSENG_POSITIONS[i][1];CHECK(PtGinseng::Start(1,Plant(i))==1);Elapse();CHECK(PtGinseng::Finish(1)==2);}CHECK(inventory==5&&Player[1].m_cTask.v[4553]==31744);
 // Shared herb stays available to a second eligible character, with its own receipt.
 Reset();Player[1].m_cTask.v[4550]=5;Npc[1].m_SubWorldIndex=2;Npc[1].x=PT_GINSENG_POSITIONS[10][0];Npc[1].y=PT_GINSENG_POSITIONS[10][1];purifyUntil=gameTime+540;
 CHECK(PtGinseng::Start(1,Plant(10))==1);Elapse();CHECK(PtGinseng::Finish(1)==2);CHECK(Object[Plant(10)].m_nState==OBJ_PROP_STATE_HIDE);
 Player[2]=Player[1];Player[2].m_dwID=8;Player[2].m_nIndex=2;Player[2].m_cTask.v[4553]=0;Npc[2]=Npc[1];Npc[2].m_dwID=88;
 CHECK(PtGinseng::Start(2,Plant(10))==0);Object[Plant(10)].m_nState=OBJ_PROP_STATE_DISPLAY;CHECK(PtGinseng::Start(2,Plant(10))==1);gameTime+=54;Player[2].m_nPaceBarTime=0;CHECK(PtGinseng::Finish(2)==2);CHECK(inventory==2&&Player[1].m_cTask.v[4553]==1024&&Player[2].m_cTask.v[4553]==1024);
 // Two players can channel together, but only one receives the shared plant.
 Reset();Player[2]=Player[1];Player[2].m_dwID=8;Player[2].m_nIndex=2;Npc[2]=Npc[1];Npc[2].m_dwID=88;
 CHECK(PtGinseng::Start(1,Plant(0))==1);CHECK(PtGinseng::Start(2,Plant(0))==1);
 gameTime+=54;Player[1].m_nPaceBarTime=Player[2].m_nPaceBarTime=0;
 CHECK(PtGinseng::Finish(1)==1);CHECK(PtGinseng::Finish(2)==0&&inventory==1);
 CHECK(Object[Plant(0)].m_nState==OBJ_PROP_STATE_HIDE);
 // A completed lifetime makes the same object available despite the old receipt.
 Object[Plant(0)].SetState(OBJ_PROP_STATE_DISPLAY);
 CHECK(PtGinseng::Start(1,Plant(0))==1);Elapse();CHECK(PtGinseng::Finish(1)==1&&inventory==2);
 printf("PASS GINSENG production object/channel engine checks=%d: real props, progress timer, damage/move/map/relog interruption, bag rollback, replay and ten picks\n",checks);return 0;
}
