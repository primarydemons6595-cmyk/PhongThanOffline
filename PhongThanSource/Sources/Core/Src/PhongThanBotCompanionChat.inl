static bool CompanionChat(Runtime& r,int from,const char* raw,int mode,int hostility,char* answer,int size){
 if(mode==PTBC_SALE_ACCEPT&&CompanionOwns(r,from)&&companions[&r-bots].Mode==PTBC_GIFT)mode=PTBC_GIFT;
 if(BotMarketChat(r,from,raw,mode,hostility,answer,size))return true;

 CompanionState& c=companions[&r-bots];DWORD now=GetTickCount();if(c.Pending&&(int)(now-c.Until)>0)c.Pending=0;if(!mode)return false;if(mode==PTBC_CONFIRM&&(!CompanionOwns(r,from)||!c.Pending||(int)(now-c.Until)>0))return false;

 if((c.Mode||c.Pending)&&!CompanionOwns(r,from)&&SocialAlive(c.Owner)){lstrcpynA(answer,"\124\165\151\040\256\141\156\147\040\143\343\040\150\321\156\040\166\355\151\040\156\147\255\352\151\040\153\150\270\143\054\040\253\156\147\040\143\150\352\040\143\150\363\164\040\156\150\320\056",size);return true;}

 if(mode==PTBC_CANCEL){if(CompanionOwns(r,from)){CompanionEvent(r,"command_cancelled",raw);ResetCompanion(r);Npc[Player[r.Player].m_nIndex].SendCommand(do_stand);lstrcpynA(answer,"\117\153\054\040\164\165\151\040\161\165\141\171\040\154\271\151\040\154\265\155\040\166\151\326\143\040\143\361\141\040\164\165\151\040\156\150\320\056\040\103\307\156\040\164\150\327\040\147\344\151\056",size);return true;}return false;}

 if(expires<(unsigned long)time(0))return false;
 if(Player[r.Player].CheckTrading()||Player[r.Player].GetLockState()){lstrcpynA(answer,"\124\165\151\040\256\141\156\147\040\147\151\141\157\040\144\336\143\150\054\040\143\150\352\040\170\157\156\147\040\256\267\040\253\156\147\056",size);return true;}

 if(!SocialAlive(from)||!SocialAlive(r.Player)){lstrcpynA(answer,"\103\150\352\040\150\345\151\040\163\370\143\040\162\345\151\040\156\343\151\040\164\151\325\160\040\156\150\141\040\253\156\147\056",size);return true;}

 int actor=Player[r.Player].m_nIndex,other=Player[from].m_nIndex;

 if(!CompanionOwns(r,from)&&(Npc[actor].m_SubWorldIndex!=Npc[other].m_SubWorldIndex||NpcSet.GetDistance(actor,other)>1024)){lstrcpynA(answer,"\244\156\147\040\154\271\151\040\147\307\156\040\164\165\151\040\164\162\255\355\143\040\256\267\054\040\147\151\352\040\143\337\156\040\170\141\040\161\165\270\056",size);return true;}

 if(mode!=PTBC_GIFT&&hostility>=80){lstrcpynA(answer,"\103\363\164\054\040\142\151\325\156\040\143\150\157\040\153\150\165\312\164\040\155\276\164\056\040\247\270\156\150\040\164\165\151\040\162\345\151\040\143\337\156\040\156\150\352\040\166\266\040\265\077",size);return true;}

 if(mode!=PTBC_GIFT&&hostility>=40){lstrcpynA(answer,"\114\307\156\040\164\162\255\355\143\040\253\156\147\040\147\151\325\164\040\164\165\151\056\040\103\343\040\147\327\040\142\357\040\256\276\160\040\162\345\151\040\156\343\151\054\040\150\370\141\040\163\165\253\156\147\040\164\165\151\040\143\150\255\141\040\164\151\156\056",size);return true;}

 std::string key;if(!PlayerKey(from,key))return false;
 CompanionPrepareGift(r,from,mode);

 c.Owner=from;c.OwnerId=Player[from].m_dwID;lstrcpynA(c.Peer,key.c_str(),sizeof(c.Peer));

 if(mode==PTBC_INVITE){c.Pending=PTBC_FOLLOW;c.Until=now+120000;CompanionEvent(r,"follow_negotiating",raw);lstrcpynA(answer,"\247\151\040\256\251\165\040\256\312\171\040\253\156\147\077\040\247\151\040\164\162\141\151\156\040\150\141\171\040\256\270\156\150\040\143\157\156\040\156\265\157\077\040\116\343\151\040\162\342\040\162\345\151\040\164\165\151\040\164\150\145\157\056",size);return true;}

 if(mode==PTBC_CONFIRM){mode=c.Pending;c.Pending=0;}

 if(mode==PTBC_SHOP){

  // Resolve an actual shop item name before promising to purchase anything.

  const char* requested=forcedArgument[0]?forcedArgument:raw;BotLoadShops();const char* found=0;for(int b=0;b<(int)botShops.size()&&!found;++b)for(int s=0;s<BuySell.GetWidth();++s){KItem* item=BuySell.GetItem(BuySell.GetItemIndex(botShops[b].shop,s));if(item){const char* name=item->GetName();if(name[0]=='#')++name;if(name[0]&&strstr(requested,name)){found=name;break;}}}

  if(!found){char description[256];PTBotFoldPartySpeech(requested,description,sizeof(description));int recovery=strstr(description,"thuoc mau")||strstr(description,"hoi mau")||strstr(description,"binh mau")?1:strstr(description,"thuoc mana")||strstr(description,"thuoc noi luc")?2:0;int cheapest=2147483647;if(recovery)for(int b=0;b<(int)botShops.size();++b)for(int slot=0;slot<BuySell.GetWidth();++slot){KItem* item=BuySell.GetItem(BuySell.GetItemIndex(botShops[b].shop,slot));if(item&&BotRecoveryKind(*item)==recovery&&item->GetCurPrice()>0&&item->GetCurPrice()<cheapest){cheapest=item->GetCurPrice();const char* name=item->GetName();found=name[0]=='#'?name+1:name;}}}
  if(!found){lstrcpynA(answer,"\115\165\141\040\155\343\156\040\156\265\157\040\253\156\147\077\040\107\366\151\040\256\363\156\147\040\164\252\156\040\164\162\157\156\147\040\163\150\157\160\054\040\164\165\151\040\155\355\151\040\164\327\155\040\155\165\141\040\256\255\356\143\056",size);return true;}

  lstrcpynA(c.ItemName,found,sizeof(c.ItemName));c.PurchaseBefore=CompanionItemCount(r,c.ItemName);botShop[&r-bots].npc=0;botShop[&r-bots].next=now;

  if(Player[r.Player].m_ItemList.GetEquipmentMoney()<=0){lstrcpynA(answer,"\124\165\151\040\150\325\164\040\164\151\322\156\040\162\345\151\056\040\244\156\147\040\147\151\141\157\040\144\336\143\150\040\256\255\141\040\164\151\322\156\040\164\162\255\355\143\040\256\267\056",size);return true;}

 }

 c.Mode=mode;c.Target=c.TargetMap=c.TargetX=c.TargetY=c.TargetEntity=c.RetreatUntil=0;c.TargetExplicit=forcedArgument[0]?1:0;lstrcpynA(c.TargetName,forcedArgument[0]?forcedArgument:raw,sizeof(c.TargetName));c.Pending=0;c.Started=now;c.LastNotice=now;Training& t=training[&r-bots];t.Target=0;t.Identity=0;t.Path.clear();t.QuestTarget=t.QuestMap=0;Npc[actor].SendCommand(do_stand);CompanionEvent(r,"command_accepted",raw);

 if(mode==PTBC_GIFT){c.GiftUntil=now+120000;Npc[actor].SetFightMode(0);lstrcpynA(answer,"\244\156\147\040\164\276\164\040\143\150\151\325\156\040\256\312\165\040\162\345\151\040\155\353\040\147\151\141\157\040\144\336\143\150\056\040\124\165\151\040\156\150\313\156\040\161\165\265\054\040\253\156\147\040\153\151\323\155\040\164\162\141\040\166\265\040\170\270\143\040\156\150\313\156\040\156\150\320\056",size);}

 else if(mode==PTBC_WAIT)lstrcpynA(answer,"\365\054\040\164\165\151\040\256\370\156\147\040\143\150\352\040\353\040\256\251\171\056\040\113\150\151\040\156\265\157\040\256\151\040\164\151\325\160\040\164\150\327\040\147\344\151\056",size);

 else if(mode==PTBC_FIGHT)lstrcpynA(answer,"\365\054\040\164\165\151\040\256\151\040\143\357\156\147\040\166\265\040\256\270\156\150\040\161\165\270\151\040\147\307\156\040\253\156\147\056\040\335\164\040\155\270\165\040\164\165\151\040\163\317\040\162\363\164\040\142\254\155\040\164\150\165\350\143\056",size);

 else if(mode==PTBC_BOSS)lstrcpynA(answer,"\244\156\147\040\143\150\344\156\040\155\364\143\040\164\151\252\165\040\142\157\163\163\040\353\040\147\307\156\040\256\151\054\040\164\165\151\040\163\317\040\150\347\040\164\162\356\056\040\116\147\165\171\040\150\151\323\155\040\161\165\270\040\160\150\266\151\040\162\363\164\040\156\150\320\056",size);

 else if(mode>=PTBC_PLAYER&&mode<=PTBC_RAID_ESCORT)lstrcpynA(answer,"\124\165\151\040\163\317\040\170\270\143\040\256\336\156\150\040\256\363\156\147\040\155\364\143\040\164\151\252\165\040\162\345\151\040\153\151\323\155\040\164\162\141\040\120\113\056\040\116\325\165\040\142\336\040\143\150\306\156\040\164\165\151\040\142\270\157\040\253\156\147\056",size);
 else if(mode==PTBC_LOOT)lstrcpynA(answer,"\124\165\151\040\164\327\155\040\256\345\040\147\307\156\040\256\251\171\040\256\323\040\156\150\306\164\056\040\247\345\040\153\150\343\141\040\143\150\361\040\150\141\171\040\164\363\151\040\256\307\171\040\164\150\327\040\153\150\253\156\147\040\154\312\171\040\256\255\356\143\056",size);

 else if(mode==PTBC_SHOP)lstrcpynA(answer,"\124\165\151\040\256\151\040\164\327\155\040\163\150\157\160\040\142\270\156\040\256\363\156\147\040\155\343\156\040\253\156\147\040\156\150\352\056\040\115\165\141\040\256\255\356\143\040\164\165\151\040\163\317\040\142\270\157\056",size);

 else lstrcpynA(answer,"\365\054\040\164\165\151\040\164\150\145\157\040\253\156\147\056\040\115\165\350\156\040\144\365\156\147\040\143\150\352\040\164\150\327\040\156\343\151\040\156\150\320\056",size);

 return true;

}

static bool CompanionGiftEmpty(KPlayer& p){if(p.m_ItemList.GetMoney(room_trade)!=0)return false;for(PlayerItem* item=p.m_ItemList.GetFirstItem();item;item=p.m_ItemList.GetNextItem())if(item->nPlace==pos_traderoom)return false;return true;}

static void CompanionTradeTick(Runtime& r,DWORD now){
 if(BotMarketTradeTick(r,now))return;

 CompanionState& c=companions[&r-bots];KPlayer& p=Player[r.Player];if(!p.CheckTrading())return;int other=p.m_cTrade.m_nTradeDest;TRADE_DECISION_COMMAND decision;ZeroMemory(&decision,sizeof(decision));

 if(other<=0||other>=MAX_PLAYER||!CompanionOwns(r,other)||c.Mode!=PTBC_GIFT||(int)(now-c.GiftUntil)>0||!CompanionGiftEmpty(p)||p.m_cTrade.m_bTradeFolkGame){decision.m_btDecision=0;p.TradeDecision((BYTE*)&decision);CompanionEvent(r,"gift_cancelled","consent expired or outgoing consideration detected");return;}

 KPlayer& human=Player[other];if(!human.m_cTrade.m_nTradeLock)return;

 if(!p.m_cTrade.m_nTradeLock){decision.m_btDecision=2;p.TradeDecision((BYTE*)&decision);return;}

 if(!human.m_cTrade.m_nTradeState)return;

 bool hasGift=human.m_ItemList.GetMoney(room_trade)>0;for(PlayerItem* item=human.m_ItemList.GetFirstItem();item;item=human.m_ItemList.GetNextItem())if(item->nPlace==pos_traderoom)hasGift=true;

 if(!hasGift)return;

 // Normal engine checks bag space, money, locking, cooldown and durable saves.

 decision.m_btDecision=1;p.TradeDecision((BYTE*)&decision);

 if(!p.CheckTrading()&&!human.CheckTrading()){Remember(r,other,-20,10,"gift_received");CompanionEvent(r,"gift_received","normal player trade committed; relationship improved");CompanionTell(r,other,"\124\165\151\040\156\150\313\156\040\256\255\356\143\040\162\345\151\054\040\143\266\155\040\254\156\040\253\156\147\056\040\103\150\165\171\326\156\040\143\362\040\164\165\151\040\143\362\156\147\040\142\355\164\040\147\151\313\156\056");CompanionResumeGift(r,now);}

}

