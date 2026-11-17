#include <windows.h>
#include <vector>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include "../../Headers/PhongThanStall.h"
enum {Capacity=2,MAX_PLAYER=8,MAX_ITEM=32,item_equip=0,do_sit=2,do_stand=1,pos_equiproom=3};
struct Runtime {struct {char Id[33];int Auto;}Config;int Player;char State[32];};static Runtime bots[Capacity];
struct Training {int Target,QuestTarget,Failures;std::vector<int> Path;};static Training training[Capacity];
struct Owned {int nIdx,nPlace;};typedef Owned PlayerItem;
struct TestItem {int id,price;bool useful,worthy,needed,reserved,learnable;int GetID(){return id;}int GetGenre(){return item_equip;}int GetTradePrice(){return price;}void SetTradePrice(int p){price=p;}};static TestItem Item[MAX_ITEM];
struct Inventory {Owned rows[16];int cursor;Owned* GetFirstItem(){cursor=0;return rows[0].nIdx?&rows[0]:0;}Owned* GetNextItem(){++cursor;return cursor<16&&rows[cursor].nIdx?&rows[cursor]:0;}int SearchID(int id){for(int i=0;i<16;++i)if(rows[i].nIdx&&Item[rows[i].nIdx].id==id)return rows[i].nIdx;return 0;}int Hand(){return 0;}};
static bool permit=true,travel=false,ticket=false;static int hops=0,travelCalls=0,moveCalls=0,purchases=0,opens=0,closes=0,equipped=0,learned=0;
static int closedVillage=0;
struct KIniFile {bool Load(const char*){return true;}void GetInteger(const char*,const char* key,int fallback,int* value){*value=atoi(key)==closedVillage?0:fallback;}};
struct KNpc {int m_SubWorldIndex,m_CurrentLife,m_CurrentLifeMax,m_Doing,m_Level,m_bRideHorse,x,y;void SendCommand(int c){m_Doing=c;}void GetMpsPos(int* a,int* b){*a=x;*b=y;}void SetFightMode(int){}void SwitchRideHorse(int b){m_bRideHorse=b;}};static KNpc Npc[MAX_PLAYER];
struct KPlayer {int m_nIndex,m_dwID;bool m_PlayerDBLoad;struct {int nTrade;}m_PTrade;Inventory m_ItemList;bool GetLockState(){return false;}bool CheckTrading(){return m_PTrade.nTrade!=0;}int GetPlayerIndex(){return this-PlayerBase();}static KPlayer* PlayerBase();void Save(){}int GetTradeCount(){return m_ItemList.rows[0].nIdx?1:0;}void SendTradeCancel(){m_PTrade.nTrade=0;++closes;}void TradeStart(BYTE*){if(permit){m_PTrade.nTrade=1;++opens;}}};static KPlayer Player[MAX_PLAYER];KPlayer* KPlayer::PlayerBase(){return Player;}
struct {int m_SubWorldID;}SubWorld[2];
struct TestNpcSet {int GetDistance(int a,int b){return abs(Npc[a].x-Npc[b].x)+abs(Npc[a].y-Npc[b].y);}}NpcSet;
struct {int Mode;}companions[Capacity];static int g_TaskGlobalValue[5];
struct PLAYER_TRADE_START_COMMAND {char Name[32];};
static bool BotBag(int place){return place==pos_equiproom;}
static bool BotMarketTradable(KPlayer&,int i){return i>0&&i<MAX_ITEM&&Item[i].id;}
static bool BotMarketWorthy(TestItem& i){return i.worthy;}
static bool BotEquipmentUseful(KPlayer&,int i){return Item[i].useful;}
static bool BotMarketReserved(int,int i){return Item[i].reserved;}
static bool BotMarketNeedsItem(KPlayer&,int i){return Item[i].needed;}
static bool BotLearnable(KPlayer&,TestItem& i){return i.learnable;}
static int BotRecoveryKind(TestItem&){return 0;}
static int BotMarketBuyBudget(KPlayer&){return 2000;}
static int BotMarketValue(TestItem&){return 1000;}
static int BotRoute(Runtime&,int,int,int* edge=0){if(edge)*edge=hops>0?0:-1;return hops;}
static bool BotTryTravel(Runtime&,int,DWORD){++travelCalls;return travel;}
static bool BotHasTravelItem(KPlayer&,int){return travel;}
static void MoveTraining(Runtime&,Training&,int,int,DWORD,int=0){++moveCalls;}
static int PTBotVttBand(int){return 1;}static int BotVttMission(int){return 0;}static bool BotVttTicket(Runtime&,int,int){return ticket;}
static bool PhongThanStarterSpawn(int profession,int& map,int& x,int& y){map=1001+profession;x=y=100;return true;}
bool PhongThanStallAllowed(int){return permit;}
bool PhongThanStallPurchase(int,int,int){++purchases;return false;}
static unsigned long Hash(const void*,int){return 123456;}
static void Log(const char*,const char*,const char*){}
static void BotLearnOwned(Runtime&,DWORD){++learned;}
struct {int X,Y;}PTBotEdges[1];
#include "../../Sources/Core/Src/PhongThanBotStall.inl"
static void BotManageEquipment(Runtime&,DWORD){++equipped;}
static Runtime& Reset(){ZeroMemory(Player,sizeof(Player));ZeroMemory(Npc,sizeof(Npc));ZeroMemory(Item,sizeof(Item));ZeroMemory(companions,sizeof(companions));ZeroMemory(g_TaskGlobalValue,sizeof(g_TaskGlobalValue));permit=true;ticket=travel=false;hops=0;travelCalls=moveCalls=purchases=opens=closes=0;botStalls[0]=BotStallJob();training[0]=Training();Runtime& r=bots[0];ZeroMemory(&r,sizeof(r));r.Player=1;r.Config.Auto=1;strcpy(r.Config.Id,"fixture");Player[1].m_PlayerDBLoad=true;Player[1].m_nIndex=1;Npc[1].m_CurrentLife=Npc[1].m_CurrentLifeMax=100;Npc[1].x=Npc[1].y=100;SubWorld[0].m_SubWorldID=1002;SubWorld[1].m_SubWorldID=1021;return r;}
static void Stock(int count){for(int i=0;i<count;++i){int n=5+i;Player[1].m_ItemList.rows[i].nIdx=n;Player[1].m_ItemList.rows[i].nPlace=pos_equiproom;Item[n].id=50+i;Item[n].worthy=true;}}
int main(){
 Runtime& r=Reset();Training& t=training[0];Stock(2);assert(!BotEconomyPlan(r,t,100000)&&!opens);
 Reset();Stock(3);assert(BotEconomyPlan(r,t,100000)&&opens==1);assert(botStalls[0].until>=3700000&&botStalls[0].until<=7300000);assert(Player[1].m_PTrade.nTrade&&Item[5].price==1000);
 DWORD until=botStalls[0].until;assert(BotStallActive(r,t,until-1)&&Npc[1].m_Doing==do_sit);assert(!BotStallActive(r,t,until));assert(closes==1&&!Item[5].price&&botStalls[0].next==until+14400000);
 for(int condition=0;condition<5;++condition){Reset();Stock(3);assert(BotEconomyPlan(r,t,100000));if(condition==0)companions[0].Mode=1;if(condition==1){ticket=true;g_TaskGlobalValue[1]=1;}if(condition==2)Npc[1].m_CurrentLife=59;if(condition==3)permit=false;if(condition==4)r.Config.Auto=0;assert(!BotStallActive(r,t,100001)&&closes==1&&!Player[1].m_PTrade.nTrade);}
 Reset();Stock(3);Item[5].useful=Item[6].useful=Item[7].useful=true;assert(!BotEconomyPlan(r,t,100000)&&!opens);
 Reset();Stock(3);t.QuestTarget=4;assert(!BotEconomyPlan(r,t,100000)&&!opens);t.QuestTarget=0;companions[0].Mode=1;assert(!BotEconomyPlan(r,t,100000));companions[0].Mode=0;Npc[1].m_CurrentLife=79;assert(!BotEconomyPlan(r,t,100000));
 Reset();hops=1;travel=true;assert(BotMarketMove(r,t,1021,1,1,100000,128)&&moveCalls==1&&travelCalls==0);
 Reset();hops=2;travel=true;assert(BotMarketMove(r,t,1021,1,1,100000,128)&&travelCalls==1&&!moveCalls);
 Reset();hops=-1;travel=true;assert(BotMarketMove(r,t,1021,1,1,100000,128)&&travelCalls==1);
 Reset();hops=-1;assert(!BotMarketMove(r,t,1021,1,1,100000,128)&&!moveCalls);
 Reset();hops=1;t.Failures=2;travel=true;assert(BotMarketMove(r,t,1021,1,1,100000,128)&&travelCalls==1&&!moveCalls);
 Reset();Stock(3);botStalls[0].seller=2;botStalls[0].item=123;botStalls[0].until=999999;assert(!BotBrowseStalls(r,t,100000)&&!purchases&&!botStalls[0].seller);
 Reset();Stock(3);closedVillage=1002;assert(!BotFreeVillage(1002)&&BotFreeVillage(1003));BotEconomyPlan(r,t,100000);assert(botStalls[0].map==1003&&!opens);closedVillage=0;
 puts("PASS production bot stall controller: valuable spare threshold, useful gear protection, 1-2 hour session, timed close/cooldown, event/health/companion priority, real travel decision and stale purchase rejection");return 0;
}
