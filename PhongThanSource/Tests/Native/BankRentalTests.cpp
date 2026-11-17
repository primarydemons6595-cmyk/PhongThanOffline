#include <stdio.h>
#include <string.h>
#include <time.h>
#define _SERVER
#define TRUE 1
typedef unsigned long DWORD;
enum {MAX_PLAYER=3,MAX_NPC=8,MAX_OBJECT=8,kind_dialoger=3,Obj_Kind_Box=1,Obj_Kind_Prop=2};
#include "../../Headers/PhongThanBank.h"
struct Lua_State {int args[2],values[2],count;};
int GetPlayerIndex(Lua_State*){return 1;}
double Lua_ValueToNumber(Lua_State* L,int i){return L->args[i-1];}
void Lua_PushNumber(Lua_State* L,int n){L->values[L->count++]=n;}
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
#include "../../Sources/Core/Src/PhongThanBank.inl"
int checks=0;
#define CHECK(c) do{if(!(c)){printf("FAIL line=%d %s\n",__LINE__,#c);return 1;}++checks;}while(0)
int Purchase(int page,int permanent){Lua_State L;memset(&L,0,sizeof(L));L.args[0]=page;L.args[1]=permanent;LuaBankPurchase(&L);return L.values[0];}
int main(){
 KPlayer& p=Player[1];p.m_PlayerDBLoad=1;p.m_nIndex=1;p.m_nLastNpcIndex=2;p.m_cTask.values[13]=2;
 Npc[1].alive=1;Npc[2].m_Kind=kind_dialoger;Npc[2].m_dwID=100;Npc[2].m_ActionScriptID=g_FileName2Id("\\script\\\263\257\270\350\\\264\242\316\357\317\344.lua");
 CHECK(PhongThanBankNearTreasury(1));
 strcpy(Npc[2].ActionScript,"\\script\\\263\257\270\350\\\264\242\316\357\317\344.lua");DWORD activeScript=Npc[2].m_ActionScriptID;Npc[2].m_ActionScriptID=0;CHECK(PhongThanBankNearTreasury(1));Npc[2].m_ActionScriptID=activeScript;Npc[2].ActionScript[0]=0;
 Npc[2].m_ActionScriptID=0;Npc[2].m_NpcSettingIdx=151;strcpy(Npc[2].Name,"\124\150\361\040\113\150\350");CHECK(PhongThanBankNearTreasury(1));Npc[2].Name[0]=0;CHECK(!PhongThanBankNearTreasury(1));Npc[2].m_NpcSettingIdx=0;Npc[2].m_ActionScriptID=activeScript;
 p.m_btRepositoryNum=1;PhongThanBankSync(1);
 p.m_nLastNpcIndex=0;CHECK(PhongThanBankNearTreasury(1));
 p.m_nLastNpcIndex=3;Npc[3].m_dwID=101;Npc[3].m_Kind=kind_dialoger;CHECK(PhongThanBankNearTreasury(1)); // cooking pot was clicked after treasury
 p.m_nLastNpcIndex=2;Npc[1].m_FightMode=1;CHECK(PhongThanBankNearTreasury(1));Npc[1].m_FightMode=0;
 DWORD originalScript=Npc[2].m_ActionScriptID;Npc[2].m_ActionScriptID=777;Npc[2].m_NpcSettingIdx=768;CHECK(PhongThanBankNearTreasury(1));Npc[2].m_NpcSettingIdx=0;Npc[2].m_ActionScriptID=originalScript;
 CHECK(p.m_cTask.values[5340]==PT_BANK_PERMANENT&&p.m_cTask.values[5341]==0);
 CHECK(PhongThanBankAccess(1,1)&&PhongThanBankAccess(1,2)&&!PhongThanBankAccess(1,3));
 coins=1000;CHECK(!Purchase(2,0)&&paid==0);CHECK(!Purchase(2,1)&&paid==0);
 int now=(int)time(NULL);CHECK(Purchase(3,0)&&paid==20&&coins==980);
 int expiry=p.m_cTask.values[5341];CHECK(expiry>=now+PT_BANK_RENT_SECONDS&&expiry<=now+PT_BANK_RENT_SECONDS+2);
 CHECK(PhongThanBankAccess(1,3)&&!PhongThanBankAccess(1,4));
 CHECK(Purchase(3,0)&&p.m_cTask.values[5341]==expiry+PT_BANK_RENT_SECONDS&&paid==40);
 CHECK(Purchase(5,1)&&p.m_cTask.values[5343]==PT_BANK_PERMANENT&&paid==189);
 CHECK(!PhongThanBankAccess(1,4)&&PhongThanBankAccess(1,5));
 CHECK(!Purchase(5,1)&&paid==189);
 coins=29;CHECK(!Purchase(4,0)&&coins==29&&p.m_cTask.values[5342]==0);
 coins=1000;p.m_cTask.values[13]='2';CHECK(Purchase(4,0)&&paid==219);p.m_cTask.values[13]=2;
 p.m_cTask.values[5342]=(int)time(NULL)-1;CHECK(!PhongThanBankAccess(1,4));
 CHECK(Purchase(4,1)&&p.m_cTask.values[5342]==PT_BANK_PERMANENT&&paid==318);
 p.locked=1;CHECK(!Purchase(3,0)&&paid==318);p.locked=0;
 p.m_cTask.values[13]=0;CHECK(!Purchase(3,0));p.m_cTask.values[13]=2;
 Npc[2].m_RegionIndex=-1;CHECK(!PhongThanBankNearTreasury(1));Npc[2].m_RegionIndex=0;
 NpcSet.distance=385;CHECK(!PhongThanBankNearTreasury(1));NpcSet.distance=0;
 Npc[1].m_SubWorldIndex=1;CHECK(!PhongThanBankNearTreasury(1));Npc[1].m_SubWorldIndex=0;
 Npc[2].m_ActionScriptID=0;CHECK(!PhongThanBankNearTreasury(1));
 Object[1].m_nIndex=1;Object[1].m_nKind=Obj_Kind_Box;Object[1].m_dwScriptID=g_FileName2Id("\\script\\\321\376\263\330\\\264\242\316\357\317\344.lua");
 CHECK(PhongThanBankNearTreasury(1));CHECK(Purchase(3,0));
 Object[1].distance=384*384;CHECK(PhongThanBankNearTreasury(1));Object[1].distance++;CHECK(!PhongThanBankNearTreasury(1));Object[1].distance=0;
 Object[1].m_nSubWorldID=1;CHECK(!PhongThanBankNearTreasury(1));Object[1].m_nSubWorldID=0;
 Object[1].m_nRegionIdx=-1;CHECK(!PhongThanBankNearTreasury(1));Object[1].m_nRegionIdx=0;
 Object[1].m_dwScriptID=123;CHECK(!PhongThanBankNearTreasury(1));Object[1].m_nIndex=0;
 Npc[2].m_ActionScriptID=g_FileName2Id("\\script\\\321\376\263\330\\\262\326\277\342\271\334\300\355\324\261.lua");CHECK(PhongThanBankNearTreasury(1));
 CHECK(PTBankPrice(2,0)==9&&PTBankPrice(2,1)==45&&PTBankPrice(4,1)==99);
 CHECK(!PTBankActive(now,now)&&PTBankActive(PT_BANK_PERMANENT,now));
 CHECK(PTBankExtend(PT_BANK_PERMANENT,now,0)==0&&PTBankExtend(2147483640,now,0)==0);
 memset(Object,0,sizeof(Object));memset(Npc+2,0,sizeof(Npc)-2*sizeof(Npc[0]));
 NpcSet.distance=9000;p.m_nLastNpcIndex=0;CHECK(!PhongThanBankNearTreasury(1));
 int before=coins;CHECK(Purchase(3,0)&&coins==before-20);
 before=coins;Npc[1].alive=0;CHECK(!Purchase(3,0)&&coins==before);Npc[1].alive=1;
 p.trading=1;CHECK(!Purchase(3,0)&&coins==before);p.trading=0;
 p.m_PlayerDBLoad=0;CHECK(!Purchase(3,0)&&coins==before);p.m_PlayerDBLoad=1;
 printf("PASS BANK_RENTAL checks=%d production_policy_and_purchase=1 independent_expiry=1\n",checks);return 0;
}
