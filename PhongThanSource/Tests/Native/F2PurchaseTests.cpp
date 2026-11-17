#include <stdio.h>
#include <assert.h>
#include <limits.h>
#include "../../Headers/PhongThanF2Balance.h"
enum{MAX_PLAYER=3,TASKVALUE_PT_BOUND_MONEY=4847,TASKVALUE_PT_XU_EXCHANGE=4960,TRUE=1};
struct Task{int bound,token;int GetSaveVal(int t){return t==TASKVALUE_PT_BOUND_MONEY?bound:token;}void SetSaveVal(int t,int v,int){if(t==TASKVALUE_PT_BOUND_MONEY)bound=v;else token=v;}};
struct Bag{int money;int CostMoney(int n){if(n<0||money<n)return 0;money-=n;return 1;}};
struct Actor{Task m_cTask;Bag m_ItemList;int saves;void Save(){++saves;}void Earn(int n){m_ItemList.money+=n;}}Player[MAX_PLAYER];
struct KItem{int expires;int GetGenre(){return 8;}int GetDetailType(){return 300;}int GetParticular(){return 2;}int GetLevel(){return 0;}int GetSeries(){return 5;}int GetExpirePoint(){return 0;}void SetExpireTime(int n){expires=n;}};
struct Tuple{int genre,detail,particular,level,series,luck;};struct PhongThanQuestExchangeRow{Tuple item;int count;};
PTF2Currency config;const PTF2Currency* PhongThanF2CurrencyConfig(int slot){return slot==1||slot==2?&config:0;}
int physical=100,calls,result=1,lastNeeds,lastNeedCount,lastRewardCount,lastDetail;
int PhongThanF2CurrencyCount(int p,int){return config.Query.Type==1?Player[p].m_ItemList.money:config.Query.Type==3?Player[p].m_cTask.bound:physical;}
int PhongThanDecodeQuestItemTuple(int g,int d,int part,int l,int s,int luck,Tuple& t){t.genre=g;t.detail=d;t.particular=part;t.level=l;t.series=s;t.luck=luck;return d!=999;}
int KSG_GetCurSec(){return 100;}
int PhongThanQuestExchange(int p,int task,int old,int next,PhongThanQuestExchangeRow* needs,int n,PhongThanQuestExchangeRow* reward,int rewards,bool stack,KItem* prototype,bool carried,int exact,bool bagOnly){
 ++calls;assert(stack&&carried&&bagOnly&&exact==0&&rewards==1&&prototype);assert(reward->item.genre==8&&reward->item.detail==300&&reward->item.particular==2&&reward->item.series==5);
 lastNeeds=n;lastNeedCount=needs->count;lastRewardCount=reward->count;lastDetail=n?needs->item.detail:0;
 if(!result)return 0;if(n)physical-=needs->count;Player[p].m_cTask.SetSaveVal(task,next,1);return 1;
}
#include "../../Sources/Core/Src/PhongThanF2Purchase.inl"
int main(){KItem item;config.Query.Type=0;assert(PhongThanBuyWithF2Currency(1,item,2,7,1));assert(physical==86&&lastNeedCount==14&&lastRewardCount==2&&lastDetail==47);
 config.Query.Type=2;config.Query.Genre=4;config.Query.Detail=236;assert(PhongThanBuyWithF2Currency(1,item,3,2,2));assert(physical==80&&lastDetail==236&&lastNeedCount==6);
 result=0;assert(!PhongThanBuyWithF2Currency(1,item,1,3,2)&&physical==80);
 config.Query.Type=1;Player[1].m_ItemList.money=100;assert(!PhongThanBuyWithF2Currency(1,item,2,10,1)&&Player[1].m_ItemList.money==100);result=1;assert(PhongThanBuyWithF2Currency(1,item,2,10,1)&&Player[1].m_ItemList.money==80&&lastNeeds==0);
 config.Query.Type=3;Player[1].m_cTask.bound=100;assert(PhongThanBuyWithF2Currency(1,item,3,10,2)&&Player[1].m_cTask.bound==70&&lastNeeds==0);result=0;assert(!PhongThanBuyWithF2Currency(1,item,3,10,2)&&Player[1].m_cTask.bound==70);
 int old=calls;assert(!PhongThanBuyWithF2Currency(1,item,1,71,2));assert(!PhongThanBuyWithF2Currency(1,item,255,INT_MAX,2));assert(!PhongThanBuyWithF2Currency(1,item,1,0,2));assert(!PhongThanBuyWithF2Currency(1,item,1,1,3));assert(!PhongThanBuyWithF2Currency(0,item,1,1,1));assert(calls==old);
 config.Query.Type=2;config.Query.Detail=999;assert(!PhongThanBuyWithF2Currency(1,item,1,1,1)&&calls==old);
 puts("PASS actual F2 payment function: Xu, chosen item, silver and bound silver; exact prototype identity, bundle price, insufficient funds, no-space/failure refund, slot/zero/overflow guards; four-bag-only consumption");return 0;}
