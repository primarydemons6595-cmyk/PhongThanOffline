#include "../../../Headers/PhongThanStallMessages.h"
static void BotManageEquipment(Runtime&,DWORD);
// Economy jobs use the same start/purchase gates as human players. Owned items only.
struct BotStallJob {
 DWORD next,started,until,owner,item,scan,repriced;int map,x,y,seller,price;bool selling,pressure;
 BotStallJob():next(0),started(0),until(0),owner(0),item(0),scan(0),repriced(0),map(0),x(0),y(0),seller(0),price(0),selling(false),pressure(false){}
};
static BotStallJob botStalls[Capacity];
static bool BotFreeVillage(int map){
 if(map<1002||map>1004)return false;KIniFile policy;int mode=2;char key[24];sprintf(key,"%d",map);
 if(policy.Load("\\settings\\phongthan\\StallPolicy.ini"))policy.GetInteger("Maps",key,2,&mode);
 return mode==2;
}
static bool BotStallSpare(KPlayer& p,int index){return BotMarketTradable(p,index)&&Item[index].GetGenre()==item_equip&&BotMarketWorthy(Item[index])&&!BotEquipmentUseful(p,index)&&!BotMarketReserved(p.GetPlayerIndex(),index);}
static void BotStallClear(Runtime& r,DWORD now){
 KPlayer& p=Player[r.Player];if(p.m_PTrade.nTrade)p.SendTradeCancel();
 for(PlayerItem* owned=p.m_ItemList.GetFirstItem();owned;owned=p.m_ItemList.GetNextItem())if(owned->nIdx>0&&owned->nIdx<MAX_ITEM&&BotBag(owned->nPlace))Item[owned->nIdx].SetTradePrice(0);
 BotStallJob& job=botStalls[&r-bots];job=BotStallJob();job.next=now+14400000;training[&r-bots].Path.clear();p.Save();Log("MARKET",r.Config.Id,"stall_closed_resuming_cycle");
}
static bool BotStallActive(Runtime& r,Training& t,DWORD now){
 KPlayer& p=Player[r.Player];KNpc& actor=Npc[p.m_nIndex];BotStallJob& job=botStalls[&r-bots];if(!p.m_PTrade.nTrade&&!job.selling)return false;
 int band=PTBotVttBand(actor.m_Level);bool event=band&&(g_TaskGlobalValue[band]==1||g_TaskGlobalValue[band]==2)&&BotVttTicket(r,band,BotVttMission(band));
 if(!job.selling||!p.m_PTrade.nTrade||(int)(now-job.until)>=0||companions[&r-bots].Mode||!r.Config.Auto||actor.m_CurrentLife<actor.m_CurrentLifeMax*3/5||event||!PhongThanStallAllowed(r.Player)||!p.GetTradeCount()){
  BotStallClear(r,now);return false;
 }
 if(job.pressure&&!BotBagPressure(p)){BotStallClear(r,now);return false;}
 if(now-job.repriced>=600000){
  job.repriced=now;std::vector<int> stock;for(PlayerItem* owned=p.m_ItemList.GetFirstItem();owned;owned=p.m_ItemList.GetNextItem())if(BotBag(owned->nPlace))stock.push_back(owned->nIdx);
  for(int i=0;i<(int)stock.size();++i){int n=stock[i];if(n<=0||n>=MAX_ITEM||!BotStallSpare(p,n)||Item[n].GetTradePrice()<=0)continue;PTBotQuote quote=BotQuote(Item[n]);int price=Item[n].GetTradePrice();if(quote.reference>0&&price>quote.minimum){int reduced=price-price/20;if(reduced<quote.minimum)reduced=quote.minimum;Item[n].SetTradePrice(PTBotWholeVanMoney(reduced));}}
  Log("MARKET",r.Config.Id,"unsold_stock_repriced_within_reference_floor");
 }
 t.Path.clear();t.Target=0;actor.SendCommand(do_sit);lstrcpyA(r.State,"selling_stall");return true;
}
static bool BotMarketMove(Runtime& r,Training& t,int map,int x,int y,DWORD now,int approach){
 KNpc& actor=Npc[Player[r.Player].m_nIndex];int current=SubWorld[actor.m_SubWorldIndex].m_SubWorldID;
 if(current!=map){int edge=-1,hops=BotRoute(r,current,map,&edge);
  if((hops<0||hops>=2||t.Failures>=2)&&BotTryTravel(r,map,now))return true;
  if(hops<1||edge<0)return false;lstrcpyA(r.State,"shopping_travel");MoveTraining(r,t,PTBotEdges[edge].X,PTBotEdges[edge].Y,now);return true;
 }
 lstrcpyA(r.State,"shopping_travel");MoveTraining(r,t,x,y,now,approach);return true;
}
static bool BotBrowseStalls(Runtime& r,Training& t,DWORD now){
 KPlayer& p=Player[r.Player];KNpc& actor=Npc[p.m_nIndex];BotStallJob& job=botStalls[&r-bots];int current=SubWorld[actor.m_SubWorldIndex].m_SubWorldID;
 if(job.seller){
  int seller=job.seller,index=seller>0&&seller<MAX_PLAYER?Player[seller].m_ItemList.SearchID(job.item):0;
  if((int)(now-job.until)>=0||seller<=0||seller>=MAX_PLAYER||Player[seller].m_dwID!=job.owner||!Player[seller].m_PTrade.nTrade||!index||Item[index].GetTradePrice()!=job.price||!BotMarketNeedsItem(p,index)||job.price>BotMarketCashBudget(p)||!PTBotQuoteAccepts(BotQuote(Item[index]),job.price)||!PhongThanStallAllowed(seller)){
   job=BotStallJob();job.scan=now+60000;t.Path.clear();return false;
  }
  KNpc& vendor=Npc[Player[seller].m_nIndex];int map=SubWorld[vendor.m_SubWorldIndex].m_SubWorldID,x=0,y=0;vendor.GetMpsPos(&x,&y);
  if(actor.m_SubWorldIndex!=vendor.m_SubWorldIndex||NpcSet.GetDistance(p.m_nIndex,Player[seller].m_nIndex)>160){if(!BotMarketMove(r,t,map,x,y,now,128)){job=BotStallJob();job.scan=now+60000;return false;}return true;}
  actor.SetFightMode(0);t.Path.clear();DWORD serial=job.item;bool bought=PhongThanStallPurchase(r.Player,seller,index);job=BotStallJob();job.scan=now+60000;
  if(bought&&p.m_ItemList.SearchID(serial)){Log("MARKET",r.Config.Id,"useful_stall_item_purchased_via_authoritative_settlement");BotLearnOwned(r,now);BotManageEquipment(r,now);p.Save();}return bought;
 }
 if((int)(now-job.scan)<0)return false;job.scan=now+30000+(Hash(r.Config.Id,32)%15000);int budget=BotMarketCashBudget(p);if(budget<100)return false;
 int chosen=0,best=0,bestScore=-2147483647;
 for(int seller=1;seller<MAX_PLAYER;++seller){KPlayer& vendor=Player[seller];if(seller==r.Player||!vendor.m_PlayerDBLoad||!vendor.m_PTrade.nTrade||vendor.m_nIndex<=0||!PhongThanStallAllowed(seller))continue;
  int dest=SubWorld[Npc[vendor.m_nIndex].m_SubWorldIndex].m_SubWorldID,hops=BotRoute(r,current,dest);if(hops<0&&!BotHasTravelItem(p,dest))continue;
  std::vector<int> stock;for(PlayerItem* owned=vendor.m_ItemList.GetFirstItem();owned;owned=vendor.m_ItemList.GetNextItem())if(BotBag(owned->nPlace))stock.push_back(owned->nIdx);
  for(int j=0;j<(int)stock.size();++j){int index=stock[j];if(index<=0||index>=MAX_ITEM||!BotMarketTradable(vendor,index)||!BotMarketNeedsItem(p,index))continue;int price=Item[index].GetTradePrice();if(price<1||price>budget||!PTBotQuoteAccepts(BotQuote(Item[index]),price))continue;
   int priority=BotRecoveryKind(Item[index])?12000:BotLearnable(p,Item[index])?10000:6000;int score=priority-price*2000/(budget?budget:1)-(hops<0?4:hops)*500;
   if(actor.m_SubWorldIndex==Npc[vendor.m_nIndex].m_SubWorldIndex)score-=NpcSet.GetDistance(p.m_nIndex,vendor.m_nIndex)/16;
   if(score>bestScore){bestScore=score;chosen=seller;best=index;}
  }
 }
 if(!chosen)return false;job.seller=chosen;job.owner=Player[chosen].m_dwID;job.item=Item[best].GetID();job.price=Item[best].GetTradePrice();job.until=now+180000;return true;
}
static bool BotEconomyPlan(Runtime& r,Training& t,DWORD now){
 KPlayer& p=Player[r.Player];KNpc& actor=Npc[p.m_nIndex];BotStallJob& job=botStalls[&r-bots];
 if(!r.Config.Auto||companions[&r-bots].Mode||p.CheckTrading()||p.GetLockState()||p.m_ItemList.Hand()||t.QuestTarget||actor.m_CurrentLife<actor.m_CurrentLifeMax*4/5)return false;
 bool pressure=botPolicy.bag&&BotBagPressure(p);
 if(!pressure){job.pressure=false;if(BotBrowseStalls(r,t,now))return true;}
 if(pressure&&!job.pressure){job.pressure=true;job.next=now;}
 if((int)(now-job.next)<0)return false;
 std::vector<int> spare;for(PlayerItem* owned=p.m_ItemList.GetFirstItem();owned;owned=p.m_ItemList.GetNextItem())if(BotBag(owned->nPlace)&&BotStallSpare(p,owned->nIdx))spare.push_back(owned->nIdx);
 if(spare.size()<(pressure?1:3)){job.next=now+60000;job.map=0;return false;}
 if(!job.map){int current=SubWorld[actor.m_SubWorldIndex].m_SubWorldID,best=2147483647;for(int profession=1;profession<=3;++profession){int map=0,x=0,y=0;if(!PhongThanStarterSpawn(profession,map,x,y)||!BotFreeVillage(map))continue;int hops=BotRoute(r,current,map);if(hops<0&&!BotHasTravelItem(p,map))continue;int cost=hops<0?3:hops;if(map==current)cost=0;if(cost<best){best=cost;job.map=map;job.x=x;job.y=y;}}job.started=now;}
 if(!job.map||now-job.started>180000){job.map=0;job.next=now+600000;return false;}
 int x=0,y=0;actor.GetMpsPos(&x,&y);if(SubWorld[actor.m_SubWorldIndex].m_SubWorldID!=job.map||abs(x-job.x)+abs(y-job.y)>256)return BotMarketMove(r,t,job.map,job.x,job.y,now,192);
 actor.SetFightMode(0);if(actor.m_bRideHorse){actor.SwitchRideHorse(FALSE);return true;}
 if(!PhongThanStallAllowed(r.Player)){job.map=0;job.next=now+600000;return false;}
 for(int i=0;i<(int)spare.size()&&i<12;++i){int price=BotMarketValue(Item[spare[i]]);Item[spare[i]].SetTradePrice(price>0&&price<=200000000?price:0);}
 PLAYER_TRADE_START_COMMAND start;ZeroMemory(&start,sizeof(start));lstrcpynA(start.Name,ptBotStallName,sizeof(start.Name));p.TradeStart((BYTE*)&start);
 if(p.m_PTrade.nTrade){job.selling=true;job.until=now+3600000+(Hash(r.Config.Id,32)^now)%3600001;job.started=job.repriced=now;job.pressure=pressure;t.Path.clear();t.Target=0;Log("MARKET",r.Config.Id,pressure?"full_bag_fair_stall_opened_until_space_recovers":"starter_town_stall_opened_for_one_to_two_hours");return true;}
 BotStallClear(r,now);return false;
}
