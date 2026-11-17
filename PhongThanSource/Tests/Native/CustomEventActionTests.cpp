#include <windows.h>
#include <stdio.h>
#include <assert.h>
#include "KWin32.h"
#include "KDebug.h"
#include "KLuaScript.h"
#include <limits.h>
#include "../../Headers/PhongThanReservoirs.h"
#ifdef PT_TEST_GIFTS
#include "KFilePath.h"
#include "KPakFile.h"
static int GiftMaxStack(int,int,int);
#endif
enum {MAX_PLAYER=3,MAX_NPC=64,MAX_ITEM=128,MAX_TASK=5400,MAX_ITEM_MAGICLEVEL=20,
      pos_immediacy=22,pos_equiproom=3,pos_equiproomex=4,pos_ptbag2=19,pos_ptbag3=20,item_equip=0,item_medicine=1,item_materials=3,item_task=4,item_townportal=5,item_magicscript=6,item_skillbook=7,item_ibitem=8,equip_detailnum=16};
#include "../../Sources/Core/Src/PhongThanQuestItemTuple.h"
struct Lock {bool IsLock(){return false;}};
struct KItem {
    int GetParam(){return m_CommonAttrib.nParam;}int GetExpireTime(){return 0;}

    const char* GetName(){return "Fixture item";}
    int id,genre,detail,part,level,series,stack,maxStack;struct {int nParam;} m_CommonAttrib;
    int GetID(){return id;} int GetGenre(){return genre;} int GetDetailType(){return detail;}
    int GetParticular(){return part;}int GetLevel(){return level;}int GetSeries(){return series;}
    int GetStackNum(){return stack;}int GetMaxStackNum(){return maxStack>0?maxStack:1;}int GetWidth(){return 1;}int GetHeight(){return 1;}
    bool IsStack(){return GetMaxStackNum()>1 && !PhongThanReservoirTooltip(genre,detail,part,GetMaxStackNum(),"NONE");}
    const char* GetScript(){return "NONE";}int GetExpirePoint(){return 0;}void SetExpireTime(int){}
    void SetStackNum(int value){stack=value;}Lock* GetLock(){static Lock lock;return &lock;}
} Item[MAX_ITEM];
struct ItemPos {int nPlace,nX,nY;};
struct PlayerItem {int nIdx,nPlace,nX,nY;};
struct KItemList {
    PlayerItem m_Items[MAX_ITEM];int capacity,cursor,cash;bool failSearch;int GetEquipmentMoney(){return cash;}bool CostMoney(int n){if(n<0||n>cash)return false;cash-=n;return true;}
    PlayerItem* GetFirstItem(){cursor=0;return GetNextItem();}
    PlayerItem* GetNextItem(){while(++cursor<MAX_ITEM)if(m_Items[cursor].nIdx)return &m_Items[cursor];return 0;}
    int FindSame(int index){return m_Items[index].nIdx?index:0;}
    bool Remove(int index){if(!FindSame(index))return false;m_Items[index].nIdx=0;return true;}
    void SyncItem(int){}
    bool SearchPosition(int,int,ItemPos* p,bool){
        if(failSearch)return false;
        for(int x=0;x<capacity;++x){bool used=false;for(int i=1;i<MAX_ITEM;++i)if(m_Items[i].nIdx && m_Items[i].nX==x)used=true;
            if(!used){p->nPlace=pos_equiproom;p->nX=x;p->nY=0;return true;}}
        return false;
    }
    int Add(int index,int place,int x,int y,bool){
        if(x<0 || x>=capacity || m_Items[index].nIdx)return 0;
        for(int i=1;i<MAX_ITEM;++i)if(m_Items[i].nIdx && m_Items[i].nX==x)return 0;
        m_Items[index].nIdx=index;m_Items[index].nPlace=place;m_Items[index].nX=x;m_Items[index].nY=y;return index;
    }
    int Count(){int count=0;for(int i=1;i<MAX_ITEM;++i)if(m_Items[i].nIdx)++count;return count;}
};
struct MockTasks {int value,slots[MAX_TASK];int GetSaveVal(int id){return id==20?value:slots[id];}void SetSaveVal(int id,int next,BOOL){if(id==20)value=next;else slots[id]=next;}};
struct KPhongThanScriptAction {int Operation,View,ServerOwned,ContentLength;char Content[1400];};
struct PHONGTHAN_CHARACTER_STATE_HEADER {int StateSize;unsigned long RoleTime;};
static bool PhongThanValidateCharacterState(const PHONGTHAN_CHARACTER_STATE_HEADER*,int){return true;}
enum{PHONGTHAN_SCRIPT_SHOW=1,UI_TOPMESSAGE=1};
struct PlayerMock {
    int m_nNetConnectIdx;
    bool m_PlayerDBLoad;char Name[32],AccountName[32];void* m_SaveBuffer;void Save(){}void DoScriptAction(KPhongThanScriptAction*){}BOOL ExecuteScript(DWORD,char*,int){return TRUE;}BOOL ExecuteScript2(char*,char*,int,int){return TRUE;}int m_nIndex,m_nLastNpcIndex;void* m_pStatusLoadPlayerInfo;bool locked,trading;MockTasks m_cTask;KItemList m_ItemList;
    unsigned long m_dwID;int cash,xp;struct {int m_nFlag;} m_cTong;
    bool Earn(int money){if(money<0||cash>2000000000-money)return false;cash+=money;m_ItemList.cash+=money;return true;}
    bool Pay(int money){if(money<0||cash<money)return false;cash-=money;return true;}
    void DirectAddExp(int amount){xp+=amount;}
    bool GetLockState(){return locked;}bool CheckTrading(){return trading;}
} Player[MAX_PLAYER];
int GetPlayerIndex(Lua_State*){return 1;}
static unsigned char shopPacket[60000];static unsigned int shopPacketSize=0;
struct ShopTransport {void PackDataToClient(int,const void* data,unsigned int size){assert(size<=sizeof(shopPacket));memcpy(shopPacket,data,size);shopPacketSize=size;}} shopTransport;
static ShopTransport* g_pServer=&shopTransport;
struct WorldSet {int GetGameVersion(){return 1;}int SearchWorld(int map); } g_SubWorldSet;
struct Items {
    int allocated,failAt;
    int Add(KItem* p){int i=Add(p->genre,p->series,p->level,0,p->detail,p->part,0,1);if(i){int id=Item[i].id;Item[i]=*p;Item[i].id=id;}return i;}
    int Add(int genre,int series,int level,int,int detail,int particular,int*,int){
        if(++allocated==failAt)return 0;
        for(int i=2;i<MAX_ITEM;++i)if(!Item[i].id){Item[i].id=i+100;Item[i].genre=genre;Item[i].detail=detail;
            Item[i].part=particular;Item[i].level=level;Item[i].series=series;Item[i].stack=1;
#ifdef PT_TEST_GIFTS
            Item[i].maxStack=GiftMaxStack(genre,detail,particular);
#endif
            return i;}
        return 0;
    }
    void Remove(int index){Item[index].id=0;}
} ItemSet;

#include <string>
#include <vector>
#include <map>
#include <time.h>
#define _SERVER
#include "../../Sources/Core/Src/PhongThanCustomEvents.h"
#define MAX_SUBWORLD 2
#define kind_dialoger 3
#define camp_free 0
#define obj_npc 1
struct Region {void RemoveNpc(int){}void DecRef(int,int,int){}};
struct World {int m_SubWorldID,m_nTotalRegion;Region m_Region[1];int GetBarrier(int,int);};
static bool blocked=false,spawnFailed=false;int World::GetBarrier(int,int){return blocked?1:0;}
struct KNpc {DWORD m_dwID,m_ActionScriptID,m_DeathScriptID,m_TimerScriptID,m_DropScriptID;int m_NpcSettingIdx,m_Kind,m_SubWorldIndex,m_RegionIndex,m_MapX,m_MapY;char Name[32],ActionScript[120];bool human;bool IsPlayer(){return human;}bool IsAlive(){return true;}void SendDataToNearRegion(BYTE*,int){}void SetFightMode(int){}void SetCurrentCamp(int){}void GetMpsPos(int*x,int*y){*x=m_MapX;*y=m_MapY;}};
static KNpc Npc[MAX_NPC];static World SubWorld[MAX_SUBWORLD];
struct Npcs {int Add(int templateId,int w,int x,int y,BOOL){if(spawnFailed)return 0;for(int i=3;i<MAX_NPC;++i)if(!Npc[i].m_dwID){static DWORD next=100;KNpc& n=Npc[i];ZeroMemory(&n,sizeof(n));n.m_dwID=++next;n.m_NpcSettingIdx=HIWORD(templateId);n.m_SubWorldIndex=w;n.m_RegionIndex=0;n.m_MapX=x;n.m_MapY=y;return i;}return 0;}void Remove(int i){ZeroMemory(&Npc[i],sizeof(Npc[i]));}int GetDistance(int a,int b){int dx=abs(Npc[a].m_MapX-Npc[b].m_MapX),dy=abs(Npc[a].m_MapY-Npc[b].m_MapY);return dx+dy;}} NpcSet;
static DWORD g_FileName2Id(char* s){DWORD n=0;while(*s)n=n*31+(unsigned char)*s++;return n;}

int WorldSet::SearchWorld(int map){return map==1?0:-1;}
static PTCustomEvent eventConfig;
const PTCustomEvent* PhongThanCustomEvent(int id){return id==eventConfig.id?&eventConfig:0;}
const PTCustomRecipe* PhongThanCustomRecipe(int id){for(unsigned int i=0;i<eventConfig.recipes.size();++i)if(eventConfig.recipes[i].id==id)return &eventConfig.recipes[i];return 0;}
static bool shopAccessAllowed=true;
bool PhongThanCustomNpcAccess(int,int,int,int){return shopAccessAllowed;}
int PhongThanConsumableInitialCharges(int,int,int,int,const char*){return 0;}
#define PT_QUEST_ITEM_NOW() 0
#include "../../Sources/Core/Src/PhongThanQuestExchange.inl"
static int CreateVngNormalItem(int g,int d,int p,int l,int s,int luck){PhongThanQuestItemTuple t;if(!PhongThanDecodeQuestItemTuple(g,d,p,l,s,luck,t))return 0;return ItemSet.Add(t.genre,t.series,t.level,t.luck,t.detail,t.particular,0,1);}
static unsigned long clockNow=100000;static time_t TestTime(time_t*){return clockNow;}
#define time TestTime
#include "../../Sources/Core/Src/PhongThanCustomEventActions.inl"
#undef time
#define CHECK(c) do{if(!(c)){printf("FAIL line %d %s\n",__LINE__,#c);return 1;}++checks;}while(0)
static void Reset(){ZeroMemory(Player,sizeof(Player));ZeroMemory(Item,sizeof(Item));ZeroMemory(Npc,sizeof(Npc));ItemSet.allocated=ItemSet.failAt=0;Player[1].m_nIndex=1;Player[1].m_PlayerDBLoad=true;Player[1].m_ItemList.capacity=8;Player[1].m_ItemList.cash=100;static PHONGTHAN_CHARACTER_STATE_HEADER state={8,42};Player[1].m_SaveBuffer=&state;strcpy(Player[1].Name,"alice");strcpy(Player[1].AccountName,"account");Npc[1].m_dwID=1;Npc[1].human=true;Npc[1].m_MapX=Npc[1].m_MapY=1000;
 int i=ItemSet.Add(3,0,0,0,42,0,0,1);Item[i].stack=10;Item[i].maxStack=100;Player[1].m_ItemList.Add(i,pos_equiproom,0,0,false);}
int main(int argc,char**argv){if(argc!=2)return 2;CreateDirectoryA(argv[1],0);SetCurrentDirectoryA(argv[1]);CreateDirectoryA("EventService",0);using namespace PtCustomActions;int checks=0;Reset();eventConfig.id=1;eventConfig.enabled=true;
 PTCustomRecipe recipe;recipe.id=1;recipe.eventId=1;recipe.cook=30;recipe.burn=60;recipe.silver=10;recipe.output.genre=6;recipe.output.detail=1;recipe.output.part=100;recipe.output.count=1;PTCustomItem input;input.genre=3;input.detail=42;input.count=2;recipe.needs.push_back(input);eventConfig.recipes.push_back(recipe);
 CHECK(Start(1,recipe)==1&&Item[2].stack==8&&Player[1].m_ItemList.cash==90);CHECK(Start(1,recipe)==-4&&Item[2].stack==8);CHECK(Claim(1,recipe,false)==-3);
 char path[400];CHECK(JobPath(1,1,1,path)&&ActiveJob(1,path));clockNow+=31;Player[1].m_ItemList.failSearch=true;CHECK(Claim(1,recipe,false)==-2&&ActiveJob(1,path));Player[1].m_ItemList.failSearch=false;
 CHECK(Claim(1,recipe,false)==3&&!ActiveJob(1,path));CHECK(Claim(1,recipe,false)==-2);bool marked=false;for(int i=1;i<MAX_ITEM;++i)if(Player[1].m_ItemList.FindSame(i)&&Item[i].m_CommonAttrib.nParam==(Tag|1))marked=true;CHECK(marked);
 CHECK(Start(1,recipe)==1);clockNow+=100;int before=Player[1].m_ItemList.Count();CHECK(Claim(1,recipe,false)==4&&Player[1].m_ItemList.Count()==before&&!ActiveJob(1,path));
 Reset();PTCustomRecipe instant=recipe;instant.cook=0;Player[1].m_ItemList.failSearch=true;CHECK(Start(1,instant)==-2&&Item[2].stack==10&&Player[1].m_ItemList.cash==100&&Player[1].m_cTask.GetSaveVal(Task)==0);
 Player[1].m_ItemList.failSearch=false;instant.needs[0].detail=43;CHECK(Start(1,instant)==-2&&Item[2].stack==10&&Player[1].m_ItemList.cash==100);instant.needs[0].detail=42;CHECK(Start(1,instant)==2&&Item[2].stack==8&&Player[1].m_ItemList.cash==90);
 Reset();PTCustomRecipe other=recipe;other.id=2;eventConfig.recipes.push_back(other);CHECK(Start(1,recipe)==1&&Start(1,other)==1);char path2[400];CHECK(JobPath(1,1,2,path2)&&ActiveJob(1,path)&&ActiveJob(1,path2));clockNow+=31;CHECK(Claim(1,recipe,false)==3&&ActiveJob(1,path2));CHECK(Claim(1,other,false)==3);
 Reset();recipe.hasTool=true;recipe.tool.genre=6;recipe.tool.detail=1;recipe.tool.part=200;CHECK(Start(1,recipe)==-2&&Item[2].stack==10);
 Reset();eventConfig.toolScript="\\script\\phongthan\\custom_events\\e1\\v1\\tool.lua";eventConfig.toolNpc=261;eventConfig.toolName="Pot";eventConfig.recipes[0]=recipe;SubWorld[0].m_SubWorldID=1;SubWorld[0].m_nTotalRegion=1;
 int tool=ItemSet.Add(6,0,0,0,200,0,0,1);Item[tool].m_CommonAttrib.nParam=ToolTag|1;Player[1].m_ItemList.Add(tool,pos_equiproom,1,0,false);
 CHECK(PlacePot(1,eventConfig)&&pots.size()==1);Player[1].m_nLastNpcIndex=pots[0].index;CHECK(Start(1,recipe)==1&&!Item[tool].GetID()&&Item[2].stack==8&&pots[0].recipe==recipe.id);CHECK(!PlacePot(1,eventConfig));
 CHECK(ToolAccess(1,eventConfig));unsigned long deadline=pots[0].burn;RemovePot(pots[0]);pots.clear();potsLoaded=false;TickPots();CHECK(pots.size()==1&&pots[0].recipe==recipe.id&&pots[0].burn==deadline&&LivePot(pots[0]));Player[1].m_nLastNpcIndex=pots[0].index;
 clockNow+=31;Player[1].m_ItemList.failSearch=true;CHECK(Claim(1,recipe,false)==-2&&pots.size()==1);Player[1].m_ItemList.failSearch=false;CHECK(Claim(1,recipe,false)==3&&pots.empty());
 tool=ItemSet.Add(6,0,0,0,200,0,0,1);Item[tool].m_CommonAttrib.nParam=ToolTag|1;Player[1].m_ItemList.Add(tool,pos_equiproom,3,0,false);CHECK(PlacePot(1,eventConfig));Player[1].m_nLastNpcIndex=pots[0].index;CHECK(Start(1,recipe)==1);int bagBefore=Player[1].m_ItemList.Count();clockNow+=100;TickPots();CHECK(pots.empty()&&Player[1].m_ItemList.Count()==bagBefore&&!Item[tool].GetID());ExpireJobs(1);CHECK(!ActiveJob(1,path));
 Reset();PTCustomGoods goods;goods.id=1;goods.item.genre=3;goods.item.detail=99;goods.item.count=1;goods.barter=true;goods.payment.genre=3;goods.payment.detail=42;goods.payment.count=3;
 CHECK(Buy(1,eventConfig,goods)==2&&Item[2].stack==7&&Player[1].m_ItemList.cash==100);goods.payment.detail=43;CHECK(Buy(1,eventConfig,goods)==-2&&Item[2].stack==7);goods.payment.detail=42;Player[1].m_ItemList.failSearch=true;CHECK(Buy(1,eventConfig,goods)==-2&&Item[2].stack==7);Player[1].m_ItemList.failSearch=false;
 Reset();Item[2].genre=4;Item[2].detail=47;goods.payment.genre=4;goods.payment.detail=47;goods.payment.count=4;CHECK(Buy(1,eventConfig,goods)==2&&Item[2].stack==6&&Player[1].m_ItemList.cash==100);goods.payment.count=7;CHECK(Buy(1,eventConfig,goods)==-2&&Item[2].stack==6);
 Reset();goods.barter=false;goods.silver=25;CHECK(Buy(1,eventConfig,goods)==2&&Player[1].m_ItemList.cash==75&&Item[2].stack==10);goods.silver=100;CHECK(Buy(1,eventConfig,goods)==-2&&Player[1].m_ItemList.cash==75);
 PTCustomRecipe randomRecipe=instant;randomRecipe.rewards.clear();PTCustomItem prize;prize.genre=3;for(int option=0;option<3;++option){prize.detail=43+option;prize.count=2+option;randomRecipe.rewards.push_back(prize);}int seen=0;
 for(int draw=0;draw<20;++draw){Reset();int cake=CreateVngNormalItem(6,1,100,0,0,0);Item[cake].stack=3;Item[cake].maxStack=100;Item[cake].m_CommonAttrib.nParam=Tag|randomRecipe.id;Player[1].m_ItemList.Add(cake,pos_equiproom,1,0,false);g_RandomSeed(1001+draw);
  CHECK(Consume(1,randomRecipe,cake)==1&&Item[cake].stack==2);int amounts[3]={0,0,0};for(int ix=1;ix<MAX_ITEM;++ix)if(Player[1].m_ItemList.FindSame(ix)&&Item[ix].genre==3&&Item[ix].detail>=43&&Item[ix].detail<=45)amounts[Item[ix].detail-43]+=Item[ix].stack;int awarded=0;for(int type=0;type<3;++type)if(amounts[type]){++awarded;seen|=1<<type;CHECK(amounts[type]==2+type);}CHECK(awarded==1&&Item[2].stack==10);
 }
 CHECK(seen==7);Reset();int cake=CreateVngNormalItem(6,1,100,0,0,0);Item[cake].stack=3;Item[cake].maxStack=100;Player[1].m_ItemList.Add(cake,pos_equiproom,1,0,false);Player[1].m_ItemList.capacity=2;CHECK(!Consume(1,randomRecipe,cake)&&Item[cake].stack==3&&Player[1].m_cTask.GetSaveVal(Task)==0);randomRecipe.rewards.clear();CHECK(!Consume(1,randomRecipe,cake)&&Item[cake].stack==3);
 Reset();cake=CreateVngNormalItem(6,1,100,0,0,0);Item[cake].stack=3;Item[cake].maxStack=100;Player[1].m_ItemList.Add(cake,pos_equiproom,1,0,false);randomRecipe.useExperience=123;randomRecipe.useSilver=10000;CHECK(Consume(1,randomRecipe,cake)==1&&Item[cake].stack==2&&Player[1].xp==123&&Player[1].m_ItemList.cash==10100);
 Reset();cake=CreateVngNormalItem(6,1,100,0,0,0);Item[cake].stack=3;Item[cake].maxStack=100;Player[1].m_ItemList.Add(cake,pos_equiproom,1,0,false);Player[1].m_ItemList.capacity=2;randomRecipe.rewards.push_back(prize);CHECK(!Consume(1,randomRecipe,cake)&&Item[cake].stack==3&&Player[1].xp==0&&Player[1].m_ItemList.cash==100);randomRecipe.rewards.clear();Player[1].m_ItemList.cash=2147483600;CHECK(!Consume(1,randomRecipe,cake)&&Item[cake].stack==3&&Player[1].xp==0);
 int moneyDraws=0,itemDraws=0;randomRecipe.rewards.push_back(prize);randomRecipe.randomSilver=20000;for(int pick=0;pick<20;++pick){Reset();cake=CreateVngNormalItem(6,1,100,0,0,0);Item[cake].stack=3;Item[cake].maxStack=100;Player[1].m_ItemList.Add(cake,pos_equiproom,1,0,false);g_RandomSeed(1000+pick);CHECK(Consume(1,randomRecipe,cake)==1&&Player[1].xp==123&&Item[cake].stack==2);if(Player[1].m_ItemList.cash==30100){++moneyDraws;CHECK(Player[1].m_ItemList.Count()==2);}else{++itemDraws;CHECK(Player[1].m_ItemList.cash==10100&&Player[1].m_ItemList.Count()>2);}}CHECK(moneyDraws>0&&itemDraws>0);
 Reset();PTCustomRecipe base=recipe;base.hasTool=false;base.cook=0;base.silver=0;base.output.count=10;base.needs[0].count=10;PTCustomRecipe scaled;
 CHECK(PTCustomScaleRecipe(base,1,scaled)&&scaled.needs[0].count==1&&scaled.output.count==1);
 CHECK(PTCustomScaleRecipe(base,10,scaled)&&scaled.needs[0].count==10&&scaled.output.count==10);
 CHECK(PTCustomScaleRecipe(base,100,scaled)&&scaled.needs[0].count==100&&scaled.output.count==100);
 CHECK(!PTCustomScaleRecipe(base,2,scaled));base.batchMask=3;CHECK(!PTCustomScaleRecipe(base,100,scaled));base.batchMask=7;
 base.needs[0].count=3;CHECK(!PTCustomScaleRecipe(base,1,scaled));base.needs[0].count=10;
 CHECK(PTCustomScaleRecipe(base,100,scaled));Item[2].stack=100;Item[2].maxStack=100;Player[1].m_ItemList.capacity=110;
 CHECK(Start(1,scaled)==2&&Player[1].m_ItemList.Count()==100&&!Item[2].GetID());
 Reset();Item[2].stack=100;Player[1].m_ItemList.capacity=1;CHECK(Start(1,scaled)==-2&&Item[2].stack==100&&Player[1].m_ItemList.Count()==1);
 Reset();goods.silver=25;goods.barter=false;eventConfig.goods.clear();eventConfig.goods.push_back(goods);Player[1].m_nLastNpcIndex=3;Npc[3].m_dwID=300;SubWorld[0].m_SubWorldID=1;
 CHECK(OpenShop(1,eventConfig,1)==1&&shopPacketSize>0&&PTValidateEventShop(shopPacket,shopPacketSize));
 PTEventShopData snapshot;memset(&snapshot,0,sizeof(snapshot));memcpy(&snapshot,shopPacket,shopPacketSize);CHECK(snapshot.Count==1&&snapshot.Rows[0].Id==1&&snapshot.NpcEntity==300);
 PTEventShopBuy request;memset(&request,0,sizeof(request));PhongThanInitializeWireHeader(&request.Header,PT_EVENT_SHOP_BUY,sizeof(request),PHONGTHAN_WIRE_FLAG_REQUEST,0);request.MapId=1;request.ActorId=1;request.EventId=1;request.Token=snapshot.Token;request.Sequence=snapshot.Sequence;request.GoodsId=1;request.Lots=1;
 CHECK(PTValidateEventShopBuy(&request,sizeof(request))&&!PTValidateEventShopBuy(&request,sizeof(request)-1));
 ShopRequest(1,request);CHECK(Player[1].m_ItemList.cash==75);ShopRequest(1,request);CHECK(Player[1].m_ItemList.cash==75);
 request.Sequence++;request.MapId=2;ShopRequest(1,request);CHECK(Player[1].m_ItemList.cash==75);request.MapId=1;
 shopAccessAllowed=false;ShopRequest(1,request);CHECK(Player[1].m_ItemList.cash==75&&((PTEventShopData*)shopPacket)->Count==0);shopAccessAllowed=true;
 Player[1].locked=true;ShopRequest(1,request);CHECK(Player[1].m_ItemList.cash==75);Player[1].locked=false;
 request.GoodsId=999;ShopRequest(1,request);CHECK(Player[1].m_ItemList.cash==75);request.GoodsId=1;request.Sequence++;
 Player[1].m_ItemList.failSearch=true;ShopRequest(1,request);CHECK(Player[1].m_ItemList.cash==75);Player[1].m_ItemList.failSearch=false;
 snapshot.Rows[1]=snapshot.Rows[0];snapshot.Count=2;PhongThanInitializeWireHeader(&snapshot.Header,PT_EVENT_SHOP,PTEventShopSize(2),PHONGTHAN_WIRE_FLAG_RESPONSE,0);CHECK(!PTValidateEventShop(&snapshot,PTEventShopSize(2)));
 snapshot.Count=20;CHECK(!PTValidateEventShop(&snapshot,sizeof(snapshot)));snapshot.Count=1;PhongThanInitializeWireHeader(&snapshot.Header,PT_EVENT_SHOP,PTEventShopSize(1),PHONGTHAN_WIRE_FLAG_RESPONSE,0);CHECK(PTValidateEventShop(&snapshot,PTEventShopSize(1))&&!PTValidateEventShop(&snapshot,PTEventShopSize(1)-1));
 snapshot.Rows[0].Name[79]='x';CHECK(!PTValidateEventShop(&snapshot,PTEventShopSize(1)));snapshot.Rows[0].Name[79]=0;snapshot.Rows[0].Genre=9;CHECK(!PTValidateEventShop(&snapshot,PTEventShopSize(1)));
 char money[80];PTEventShopSilver(10000,money);CHECK(!strcmp(money,"1 \166\271\156"));PTEventShopSilver(100000,money);CHECK(!strcmp(money,"10 \166\271\156"));PTEventShopSilver(15000,money);CHECK(!strcmp(money,"1,5 \166\271\156"));PTEventShopSilver(9999,money);CHECK(!strcmp(money,"9999 \154\255\356\156\147"));
 CHECK(PTEventShopMaxLots(1,25)==1000&&PTEventShopMaxLots(64,0)==156&&PTEventShopMaxLots(1,1000000000)==1&&PTEventShopMaxLots(0,0)==0);
 Reset();goods.item.count=2;goods.silver=25;eventConfig.goods[0]=goods;CHECK(OpenShop(1,eventConfig,1)==1);request.Token=shopSessions[1].token;request.Sequence=1;request.Lots=3;request.GoodsId=1;ShopRequest(1,request);CHECK(Player[1].m_ItemList.cash==25&&Player[1].m_ItemList.Count()==7);ShopRequest(1,request);CHECK(Player[1].m_ItemList.cash==25&&Player[1].m_ItemList.Count()==7);
 request.Sequence=2;request.Lots=2;ShopRequest(1,request);CHECK(Player[1].m_ItemList.cash==25&&Player[1].m_ItemList.Count()==7);
 Reset();goods.item.count=1;goods.silver=0;goods.barter=true;goods.payment.genre=4;goods.payment.detail=47;goods.payment.count=3;eventConfig.goods[0]=goods;Item[2].genre=4;Item[2].detail=47;CHECK(OpenShop(1,eventConfig,1)==1);CHECK(((PTEventShopData*)shopPacket)->Rows[0].PaymentKind==1&&((PTEventShopData*)shopPacket)->Rows[0].UnitPrice==3);request.Token=shopSessions[1].token;request.Sequence=1;request.Lots=3;ShopRequest(1,request);CHECK(Item[2].stack==1&&Player[1].m_ItemList.cash==100);ShopRequest(1,request);CHECK(Item[2].stack==1);
 Reset();goods.payment.genre=3;goods.payment.detail=42;eventConfig.goods[0]=goods;CHECK(OpenShop(1,eventConfig,1)==1);request.Token=shopSessions[1].token;request.Sequence=1;request.Lots=3;Player[1].m_ItemList.failSearch=true;ShopRequest(1,request);CHECK(Item[2].stack==10);Player[1].m_ItemList.failSearch=false;request.Sequence=2;ShopRequest(1,request);CHECK(Item[2].stack==1);
 request.Sequence=3;request.Lots=0;CHECK(!PTValidateEventShopBuy(&request,sizeof(request)));ShopRequest(1,request);CHECK(Item[2].stack==1);request.Sequence=4;request.Lots=1001;CHECK(!PTValidateEventShopBuy(&request,sizeof(request)));ShopRequest(1,request);CHECK(Item[2].stack==1);request.Sequence=5;request.Lots=1;eventConfig.revision++;ShopRequest(1,request);CHECK(Item[2].stack==1&&((PTEventShopData*)shopPacket)->Count==0);
 printf("PASS CUSTOM_EVENT_ACTIONS checks=%d production_transactions_and_random_reward=1\n",checks);return 0;}

