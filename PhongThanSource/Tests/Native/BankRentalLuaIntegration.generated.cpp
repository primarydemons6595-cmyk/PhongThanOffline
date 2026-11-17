#include "KWin32.h"
#include "LuaLib.h"
#include "KLuaScript.h"
#include <stdio.h>
#include <string.h>
#include <time.h>
#define _SERVER
#define TRUE 1
typedef unsigned long DWORD;
enum {MAX_PLAYER=3,MAX_NPC=8,MAX_OBJECT=8,kind_dialoger=3,Obj_Kind_Box=1,Obj_Kind_Prop=2};
#include "../../Headers/PhongThanBank.h"
int GetPlayerIndex(Lua_State* L);
DWORD g_FileName2Id(const char* s){DWORD hash=0;while(*s)hash=hash*131+(unsigned char)*s++;return hash;}
struct Task {int values[5400];int GetSaveVal(int i){return values[i];}const char* GetSaveStr(int i){return (const char*)&values[i];}void SetSaveVal(int i,int v,int){values[i]=v;}};
struct Bag {int hand;int Hand(){return hand;}};
struct MoveLock {int bLock;};
struct KPlayer {
 int m_PlayerDBLoad,m_nIndex,m_nLastNpcIndex,m_btRepositoryNum,m_nPaceBarTime,saves,locked,trading;
 Task m_cTask;Bag m_ItemList;MoveLock move;
 int GetLockState(){return locked;}int CheckTrading(){return trading;}MoveLock* GetLockMove(){return &move;}
 void Save(){++saves;}void SetExpandBoxNum(int n){m_btRepositoryNum=n;}
} Player[MAX_PLAYER];
struct NpcStub {char ActionScript[256],Name[32];int m_SubWorldIndex,m_FightMode,m_RegionIndex,m_Kind,m_NpcSettingIdx,alive,player;DWORD m_ActionScriptID,m_dwID;int IsAlive(){return alive;}int IsPlayer(){return player;}} Npc[MAX_NPC];
struct NpcSetStub {int distance;int GetDistance(int,int){return distance;}int GetNextIdx(int n){return n+1<MAX_NPC?n+1:0;}} NpcSet;
struct ObjectStub {int m_nIndex,m_nRegionIdx,m_nSubWorldID,m_nKind,distance;DWORD m_dwScriptID;int GetDistanceSquare(int){return distance;}} Object[MAX_OBJECT];
struct PhongThanQuestExchangeRow {int item,count;};
int coins=0,paid=0;
int PhongThanDecodeQuestItemTuple(int g,int d,int p,int l,int s,int luck,int& item){item=g==4&&d==47&&!p&&!l&&!s&&!luck;return item;}
int PhongThanQuestExchange(int p,int task,int old,int next,PhongThanQuestExchangeRow* needs,int count,void*,int){
 if(count!=1||!needs->item||Player[p].m_cTask.GetSaveVal(task)!=old||coins<needs->count)return 0;
 coins-=needs->count;paid+=needs->count;Player[p].m_cTask.SetSaveVal(task,next,TRUE);return 1;
}
bool testVaultLocked=false;bool PhongThanBankLocked(int){return testVaultLocked;}
#define SCRIPT_PLAYERINDEX "PlayerIndex"
int GetPlayerIndex(Lua_State* L)
{
    const int top=Lua_GetTopIndex(L);
    Lua_GetGlobal(L,SCRIPT_PLAYERINDEX);
    const int index=lua_isnumber(L,-1)?(int)Lua_ValueToNumber(L,-1):-1;
    Lua_SetTopIndex(L,top);
    if(index<=0 || index>=MAX_PLAYER)return -1;
    const int npc=Player[index].m_nIndex;
    return npc>0 && npc<MAX_NPC?index:-1;
}


#include "../../Sources/Core/Src/PhongThanBank.inl"

static int notices,choices,confirms;
static int TaskSet(Lua_State* L){int p=GetPlayerIndex(L),id=(int)Lua_ValueToNumber(L,1);Player[p].m_cTask.values[id]=(int)Lua_ValueToNumber(L,2);return 0;}
static int TaskGet(Lua_State* L){Lua_PushNumber(L,Player[GetPlayerIndex(L)].m_cTask.GetSaveVal((int)Lua_ValueToNumber(L,1)));return 1;}
static int Notice(Lua_State*){++notices;return 0;}static int Choice(Lua_State*){++choices;return 0;}static int Confirm(Lua_State*){++confirms;return 0;}
#define CHECK(c) do{if(!(c)){printf("FAIL integration line=%d %s\n",__LINE__,#c);return 1;}}while(0)
int main(int argc,char**argv){CHECK(argc==2);FILE* f=fopen(argv[1],"rb");CHECK(f);char code[24000];int size=fread(code,1,sizeof(code),f);fclose(f);
 KPlayer& p=Player[1];p.m_PlayerDBLoad=1;p.m_nIndex=1;p.m_nLastNpcIndex=0;p.m_cTask.values[13]=2;Npc[1].alive=1;
 Object[1].m_nIndex=1;Object[1].m_nKind=Obj_Kind_Box;Object[1].m_dwScriptID=g_FileName2Id("\\script\\\321\376\263\330\\\264\242\316\357\317\344.lua");coins=1000;
 KLuaScript vm;CHECK(vm.Init());vm.RegisterFunction("SetTask",TaskSet);vm.RegisterFunction("GetTask",TaskGet);vm.RegisterFunction("BankInfo",LuaBankInfo);vm.RegisterFunction("BankPurchase",LuaBankPurchase);vm.RegisterFunction("Talk",Notice);vm.RegisterFunction("SayButtons",Choice);vm.RegisterFunction("MsgBox",Confirm);vm.RegisterFunction("CloseDialog",Notice);CHECK(vm.LoadBuffer((PBYTE)code,size)&&vm.ExecuteCode());
 Lua_PushNumber(vm.m_LuaState,1);vm.SetGlobalName(SCRIPT_PLAYERINDEX);
 for(int page=2;page<=5;++page){CHECK(vm.CallFunction("main",0,"d",page));CHECK(choices==page-1&&notices==0);CHECK(vm.CallFunction("buy",0,""));CHECK(vm.CallFunction("rent",0,""));CHECK(confirms==(page-1)*2);CHECK(vm.CallFunction("confirm_rent",0,""));CHECK(PTBankActive(p.m_cTask.values[PT_BANK_FIRST_TASK+page-2],(int)time(NULL)));--notices;}
 CHECK(paid==104);
 // No keeper, no chest, and a distant NPC: rental must remain available.
 memset(Object,0,sizeof(Object));memset(Npc+2,0,sizeof(Npc)-2*sizeof(Npc[0]));NpcSet.distance=9000;p.m_nLastNpcIndex=0;
 CHECK(!PhongThanBankNearTreasury(1));
 for(int farPage=2;farPage<=5;++farPage){
  int before=coins,previous=p.m_cTask.values[PT_BANK_FIRST_TASK+farPage-2];
  CHECK(vm.CallFunction("main",0,"d",farPage));CHECK(vm.CallFunction("rent",0,""));CHECK(vm.CallFunction("confirm_rent",0,""));
  CHECK(coins==before-PTBankPrice(farPage,0));CHECK(p.m_cTask.values[PT_BANK_FIRST_TASK+farPage-2]==previous+PT_BANK_RENT_SECONDS);
  CHECK(vm.CallFunction("buy",0,""));CHECK(vm.CallFunction("confirm_buy",0,""));CHECK(p.m_cTask.values[PT_BANK_FIRST_TASK+farPage-2]==PT_BANK_PERMANENT);
  before=coins;CHECK(vm.CallFunction("confirm_buy",0,""));CHECK(coins==before);
 }
 p.m_cTask.values[PT_BANK_FIRST_TASK]=0;coins=8;
 CHECK(vm.CallFunction("main",0,"d",2));CHECK(vm.CallFunction("confirm_rent",0,""));CHECK(coins==8&&p.m_cTask.values[PT_BANK_FIRST_TASK]==0);
 coins=100;p.trading=1;CHECK(vm.CallFunction("confirm_rent",0,""));CHECK(coins==100);p.trading=0;
 p.locked=1;CHECK(vm.CallFunction("confirm_rent",0,""));CHECK(coins==100);p.locked=0;
 Npc[1].alive=0;CHECK(vm.CallFunction("confirm_rent",0,""));CHECK(coins==100);Npc[1].alive=1;
 puts("PASS actual Lua VM + production BankInfo/BankPurchase: pages2-5 near/far dialogs, rent, buy, expiry, insufficient Xu, repeat purchase, locked/trading/dead rejection");return 0;
}
