// Executes the production conversation and gift controller against a deterministic
// engine fixture. This tests state decisions, not live-world delivery or economy.
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <vector>
#include "../../Headers/PhongThanBotCommand.h"
#include "../../Headers/PhongThanBotPartySpeech.h"
typedef unsigned long DWORD;typedef unsigned char BYTE;
static DWORD clockNow=100000;static unsigned long expires=0;
static DWORD GetTickCount(){return clockNow;}
static unsigned long time(int){return 100;}
static void ZeroMemory(void* p,int n){memset(p,0,n);}
static void lstrcpynA(char* to,const char* from,int n){strncpy(to,from,n-1);to[n-1]=0;}
enum {MAX_PLAYER=8,MAX_NPC=16,MAX_ITEM=16,room_trade=1,pos_traderoom=4,do_stand=0};
struct TRADE_DECISION_COMMAND {BYTE m_btDecision;};
struct PlayerItem {int nPlace;};
struct FakeItems {int money;bool offered;PlayerItem item;FakeItems():money(0),offered(false){item.nPlace=pos_traderoom;}int GetEquipmentMoney(){return money;}int GetMoney(int){return money;}PlayerItem* GetFirstItem(){return offered?&item:0;}PlayerItem* GetNextItem(){return 0;}};
struct FakeTrade {int m_nTradeDest,m_nTradeLock,m_nTradeState,m_bTradeFolkGame;FakeTrade():m_nTradeDest(0),m_nTradeLock(0),m_nTradeState(0),m_bTradeFolkGame(0){}};
struct KPlayer {int m_nIndex;DWORD m_dwID;bool alive,trading,locked;FakeTrade m_cTrade;FakeItems m_ItemList;KPlayer():m_nIndex(1),m_dwID(1),alive(true),trading(false),locked(false){}bool CheckTrading(){return trading;}bool GetLockState(){return locked;}void TradeDecision(BYTE* data);};
static KPlayer Player[MAX_PLAYER];static int decisions=0,remembered=0,hostilityDelta=0;
void KPlayer::TradeDecision(BYTE* data){TRADE_DECISION_COMMAND* command=(TRADE_DECISION_COMMAND*)data;++decisions;if(command->m_btDecision==2)m_cTrade.m_nTradeLock=1;else if(command->m_btDecision==0||command->m_btDecision==1){trading=false;Player[m_cTrade.m_nTradeDest].trading=false;}}
struct FakeNpc {int m_SubWorldIndex;DWORD m_dwID;int standing;FakeNpc():m_SubWorldIndex(0),m_dwID(1),standing(0){}void SendCommand(int){++standing;}void SetFightMode(int){}};
static FakeNpc Npc[MAX_NPC];struct FakeNpcSet {int GetDistance(int,int){return 100;}};static FakeNpcSet NpcSet;
struct Runtime {struct ConfigType {char Id[33];}Config;int Player;};static Runtime bots[2];
struct CompanionState {int Owner,Mode,Pending,PurchaseBefore,Target,TargetMap,TargetX,TargetY,TargetExplicit,LastLife;DWORD OwnerId,Until,GiftUntil,LastNotice,Started,TargetEntity,RetreatUntil;char Peer[160],ItemName[256],TargetName[256];CompanionState(){memset(this,0,sizeof(*this));}};
static CompanionState companions[2];static char forcedArgument[256]={0};struct Training {int Target;DWORD Identity;std::vector<int> Path;int QuestTarget,QuestMap;};static Training training[2];
struct ShopState {int npc;DWORD next;};static ShopState botShop[2];struct ShopBinding {int shop;};static std::vector<ShopBinding> botShops;
struct KItem {const char* GetName(){return "fixture potion";}int GetCurPrice(){return 10;}};struct FakeShop {int GetWidth(){return 1;}int GetItemIndex(int,int){return 1;}KItem* GetItem(int){static KItem item;return &item;}};static FakeShop BuySell;
static int BotRecoveryKind(KItem&){return 1;}
static bool PlayerKey(int player,std::string& key){key="aa-bb-100";return player>0&&player<MAX_PLAYER&&Player[player].alive;}
static bool SocialAlive(int player){return player>0&&player<MAX_PLAYER&&Player[player].alive;}
static bool CompanionOwns(Runtime& r,int player){CompanionState& c=companions[&r-bots];return c.Owner==player&&c.OwnerId==Player[player].m_dwID;}
static void ResetCompanion(Runtime& r);
static void CompanionEvent(Runtime&,const char*,const char*){}
static void CompanionTell(Runtime&,int,const char*){}
static int CompanionItemCount(Runtime&,const char*){return 0;}
static void BotLoadShops(){}
static bool Remember(Runtime&,int,int hostility,int,const char*){++remembered;hostilityDelta=hostility;return true;}
static bool BotMarketChat(Runtime&,int,const char*,int,int,char*,int){return false;}
static bool BotMarketTradeTick(Runtime&,DWORD){return false;}
#include "../../Sources/Core/Src/PhongThanBotCompanionResume.inl"
static void ResetCompanion(Runtime& r){companions[&r-bots]=CompanionState();companionResume[&r-bots]=CompanionState();}
#include "../../Sources/Core/Src/PhongThanBotCompanionChat.inl"
static int checks=0;
static void Check(bool value,const char* name){if(!value){printf("FAIL %s\n",name);exit(1);}++checks;}
static void Reset(){companions[0]=CompanionState();for(int i=0;i<MAX_PLAYER;++i)Player[i]=KPlayer();bots[0].Player=1;Player[1].m_nIndex=1;Player[2].m_nIndex=2;expires=200;decisions=remembered=0;clockNow=100000;}
int main(){char answer[256];Runtime& r=bots[0];Reset();
 Check(CompanionChat(r,2,"di cung toi",PTBC_INVITE,0,answer,sizeof(answer))&&companions[0].Pending==PTBC_FOLLOW&&companions[0].Mode==0,"neutral follow request asks destination before starting");
 Check(CompanionChat(r,2,"ok",PTBC_CONFIRM,0,answer,sizeof(answer))&&companions[0].Mode==PTBC_FOLLOW&&companions[0].Pending==0,"same player's confirmation starts real companion mode");
 CompanionChat(r,2,"cu theo sau toi",PTBC_FOLLOW,0,answer,sizeof(answer));Check(companions[0].Mode==PTBC_FOLLOW&&!companions[0].Pending,"direct follow order never waits for another verbal confirmation");
 CompanionChat(r,2,"dung cho",PTBC_WAIT,0,answer,sizeof(answer));Check(companions[0].Mode==PTBC_WAIT,"wait order replaces follow mode");
 CompanionChat(r,3,"di theo",PTBC_FOLLOW,0,answer,sizeof(answer));Check(companions[0].Owner==2&&companions[0].Mode==PTBC_WAIT,"other player cannot hijack active companion");
 CompanionChat(r,2,"danh quai",PTBC_FIGHT,0,answer,sizeof(answer));Check(companions[0].Mode==PTBC_FIGHT,"combat order is executable controller state");
 CompanionChat(r,2,"huy lenh",PTBC_CANCEL,0,answer,sizeof(answer));Check(companions[0].Mode==0&&companions[0].Owner==0,"cancel clears ownership and execution state");
 Reset();CompanionChat(r,2,"di cung",PTBC_INVITE,80,answer,sizeof(answer));Check(!companions[0].Mode&&!companions[0].Pending,"severe hostility refuses cooperation");
 CompanionChat(r,2,"di cung",PTBC_INVITE,40,answer,sizeof(answer));Check(!companions[0].Mode&&!companions[0].Pending,"hostile reward promise does not grant cooperation");
 Reset();CompanionChat(r,2,"di cung",PTBC_INVITE,0,answer,sizeof(answer));clockNow+=120001;Check(!CompanionChat(r,2,"ok",PTBC_CONFIRM,0,answer,sizeof(answer))&&companions[0].Mode==0,"expired confirmation cannot start follow");
 CompanionChat(r,3,"di cung",PTBC_INVITE,0,answer,sizeof(answer));Check(companions[0].Owner==3,"expired negotiation releases ownership");
 Reset();CompanionChat(r,2,"giao dich",PTBC_GIFT,80,answer,sizeof(answer));Check(companions[0].Mode==PTBC_GIFT&&companions[0].GiftUntil>clockNow,"gift request grants bounded receiving consent even when hostile");
 Player[1].trading=Player[2].trading=true;Player[1].m_cTrade.m_nTradeDest=2;Player[2].m_cTrade.m_nTradeDest=1;Player[2].m_ItemList.money=1000;
 CompanionTradeTick(r,clockNow);Check(decisions==0&&remembered==0,"unlocked human offer never commits or improves relationship");
 Player[2].m_cTrade.m_nTradeLock=1;CompanionTradeTick(r,clockNow);Check(Player[1].m_cTrade.m_nTradeLock==1&&remembered==0,"bot locks only after player locked the offer");
 CompanionTradeTick(r,clockNow);Check(remembered==0,"player lock without final confirmation is not payment");
 Player[2].m_cTrade.m_nTradeState=1;CompanionTradeTick(r,clockNow);Check(remembered==1&&hostilityDelta<0&&companions[0].Mode==PTBC_NONE,"confirmed committed gift improves relationship exactly after engine success");
 Reset();CompanionChat(r,2,"giao dich",PTBC_GIFT,0,answer,sizeof(answer));Player[1].trading=Player[2].trading=true;Player[1].m_cTrade.m_nTradeDest=2;Player[1].m_ItemList.offered=true;CompanionTradeTick(r,clockNow);Check(!Player[1].trading&&remembered==0,"outgoing bot item aborts receive-only gift trade");
 Reset();CompanionChat(r,2,"giao dich",PTBC_GIFT,0,answer,sizeof(answer));Player[1].trading=Player[2].trading=true;Player[1].m_cTrade.m_nTradeDest=2;Player[2].m_dwID++;CompanionTradeTick(r,clockNow);Check(!Player[1].trading&&remembered==0,"reused player slot cannot inherit gift consent");
 Reset();CompanionChat(r,2,"go wait",PTBC_GOTO,0,answer,sizeof(answer));companions[0].Target=7;companions[0].TargetEntity=700;
 CompanionChat(r,2,"receive travel scroll",PTBC_GIFT,0,answer,sizeof(answer));Check(companions[0].Mode==PTBC_GIFT&&companionResume[0].Mode==PTBC_GOTO,"giving supplies suspends the actual current travel job");
 Player[1].trading=Player[2].trading=true;Player[1].m_cTrade.m_nTradeDest=2;Player[2].m_cTrade.m_nTradeDest=1;Player[2].m_ItemList.money=1000;Player[2].m_cTrade.m_nTradeLock=1;Player[2].m_cTrade.m_nTradeState=1;
 CompanionTradeTick(r,clockNow);CompanionTradeTick(r,clockNow);Check(companions[0].Mode==PTBC_GOTO&&companions[0].Target==7&&companions[0].TargetEntity==700,"normal gift commit resumes destination and target identity without another player command");
 CompanionChat(r,2,"receive more supplies",PTBC_GIFT,0,answer,sizeof(answer));Player[2].m_dwID++;Check(!CompanionResumeGift(r,clockNow)&&companions[0].Mode==PTBC_NONE,"reused player slot cannot resume another role's suspended command");
 Reset();CompanionChat(r,2,"receive gift",PTBC_GIFT,0,answer,sizeof(answer));
 Check(!CompanionResumeGift(r,clockNow)&&!companions[0].Mode&&!companions[0].Owner,"gift without a suspended human job returns to autonomous work");
 Reset();CompanionChat(r,2,"wait here",PTBC_WAIT,0,answer,sizeof(answer));CompanionChat(r,2,"receive gift",PTBC_GIFT,0,answer,sizeof(answer));
 Check(CompanionResumeGift(r,clockNow)&&companions[0].Mode==PTBC_WAIT,"an explicit wait order survives a supply interruption until released");
 CompanionChat(r,2,"khong can nua",PTBC_CANCEL,0,answer,sizeof(answer));Check(!companions[0].Mode&&!companions[0].Owner&&!companionResume[0].Mode,"release after a resumed wait cannot revive a stale order");
 printf("PASS %d production native companion decision checks (engine fixture)\n",checks);return 0;
}
