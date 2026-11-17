struct BotBagPlan {DWORD npc,started,next,last;BotBagPlan():npc(0),started(0),next(0),last(0){}};
static BotBagPlan botBagPlans[Capacity];
static bool BotEconomyPlan(Runtime&,Training&,DWORD);
static bool BotBagPressure(KPlayer& p){ItemPos free;return p.m_ItemList.Hand()!=0||!p.m_ItemList.FindBagPosition(2,3,&free);}
static int BotStorableSurplus(Runtime& r){
 KPlayer& p=Player[r.Player];std::vector<int> entries;
 for(PlayerItem* owned=p.m_ItemList.GetFirstItem();owned;owned=p.m_ItemList.GetNextItem())if(BotBag(owned->nPlace)&&owned->nPlace!=pos_immediacy)entries.push_back(owned->nIdx);
 for(int i=0;i<(int)entries.size();++i){int n=entries[i];if(n<=0||n>=MAX_ITEM||!BotMarketTradable(p,n)||BotMarketReserved(r.Player,n))continue;
  KItem& item=Item[n];if(BotRecoveryKind(item)||BotLearnable(p,item)||BotStoryRequiredCount(p,item)||BotActivityWants(r,item)||(item.GetGenre()==item_task&&(item.GetDetailType()==46||item.GetDetailType()==47)))continue;
  if(item.GetGenre()==item_equip&&BotEquipmentUseful(p,n))continue;
  return n;
 }return 0;
}
static bool BotInventoryPlan(Runtime& r,Training& route,DWORD now){
 KPlayer& p=Player[r.Player];int slot=&r-bots;BotBagPlan& plan=botBagPlans[slot];
 if(!r.Config.Auto||!BotBagPressure(p)||p.CheckTrading()||p.GetLockState()||p.m_PTrade.nTrade){plan.npc=0;return false;}
 if(p.m_ItemList.Hand()){if(BotStoreHand(p)){r.Error[0]=0;Log("RECOVERY",r.Config.Id,"held_item_stored_inventory_recovered");}return false;}
 // Do not interrupt an agreed trade or a human combat order for a routine bank trip.
 if(companions[slot].Mode)return false;
 // A full owned vault is not a reason to sell rare equipment at NPC prices.
 // Keep trying a fair player sale while the treasury route is on cooldown.
 if((int)(now-plan.next)<0)return BotEconomyPlan(r,route,now);
 if(BotSurplusEquipment(p)||BotSurplusPotion(p)){
  int previous=botDeclutterPlayer;botDeclutterPlayer=r.Player;bool shop=BotShopPlan(r,route,now);botDeclutterPlayer=previous;if(shop)return true;
 }
 int selected=BotStorableSurplus(r);if(!selected){plan.next=now+30000;lstrcpyA(r.Error,"inventory_full_only_protected_or_needed_items");return false;}
 // Preserve the real novice permission and password/entitlement checks.
 bool permitted=false;int tasks[3]={13,23,33};for(int t=0;t<3;++t)if(p.m_cTask.GetSaveVal(tasks[t])==2||!strcmp(p.m_cTask.GetSaveStr(tasks[t]),"2"))permitted=true;
 if(!permitted){plan.next=now+30000;lstrcpyA(r.Error,"bank_unlock_quest_required");return false;}
 if(PhongThanBankLocked(r.Player)){plan.next=now+30000;lstrcpyA(r.Error,"bank_saved_password_required");return false;}
 KNpc& actor=Npc[p.m_nIndex];int map=SubWorld[actor.m_SubWorldIndex].m_SubWorldID;
 if(!plan.npc){int best=2147483647,npc=0;
  for(int n=NpcSet.GetNextIdx(0);n;n=NpcSet.GetNextIdx(n))if(Npc[n].m_Kind==kind_dialoger&&Npc[n].IsAlive()&&Npc[n].m_SubWorldIndex>=0&&Npc[n].m_SubWorldIndex<MAX_SUBWORLD&&(Npc[n].m_NpcSettingIdx==768||(Npc[n].m_NpcSettingIdx==151&&!strcmp(Npc[n].Name,"\124\150\361\040\113\150\350")))){
   int dest=SubWorld[Npc[n].m_SubWorldIndex].m_SubWorldID,hops=BotRoute(r,map,dest);if(hops<0)continue;int distance=dest==map?NpcSet.GetDistance(p.m_nIndex,n):0;if(distance<0)continue;int score=hops*100000+distance;if(score<best){best=score;npc=n;}
  }
  if(!npc){plan.next=now+60000;lstrcpyA(r.Error,"inventory_no_reachable_treasury");return false;}plan.npc=Npc[npc].m_dwID;plan.started=now;plan.last=0;route.Path.clear();
 }
 int banker=NpcSet.SearchID(plan.npc);
 if(banker<=0||banker>=MAX_NPC||now-plan.started>180000||route.Failures>=3){plan.npc=0;plan.next=now+60000;route.Path.clear();lstrcpyA(r.Error,"inventory_bank_route_backoff");return false;}
 int dest=SubWorld[Npc[banker].m_SubWorldIndex].m_SubWorldID,x,y;Npc[banker].GetMpsPos(&x,&y);
 if(map!=dest){int edge=-1;if(BotRoute(r,map,dest,&edge)<1||edge<0){plan.npc=0;plan.next=now+30000;return false;}lstrcpyA(r.State,"bank_travel");MoveTraining(r,route,PTBotEdges[edge].X,PTBotEdges[edge].Y,now);return true;}
 if(NpcSet.GetDistance(p.m_nIndex,banker)>128){lstrcpyA(r.State,"bank_travel");MoveTraining(r,route,x,y,now,96);return true;}
 route.Path.clear();actor.SendCommand(do_stand);actor.SetFightMode(0);lstrcpyA(r.State,"bank_deposit");if(now-plan.last<1500)return true;plan.last=now;
 if(!PhongThanBankNearTreasury(r.Player))return false;
 ItemPos storage;bool found=false;for(int page=0;page<6;++page){POINT point;if(PhongThanBankAccess(r.Player,page+1)&&p.m_ItemList.m_Room[room_repository+page].FindRoom(Item[selected].GetWidth(),Item[selected].GetHeight(),&point)){storage.nPlace=pos_repositoryroom+page;storage.nX=point.x;storage.nY=point.y;found=true;break;}}
 if(!found){plan.npc=0;plan.next=now+60000;lstrcpyA(r.Error,"inventory_owned_storage_full");Log("RECOVERY",r.Config.Id,r.Error);return BotEconomyPlan(r,route,now);}
 int entry=p.m_ItemList.FindSame(selected);if(entry<=0)return false;PlayerItem source=p.m_ItemList.m_Items[entry];DWORD serial=Item[selected].GetID();
 BotMarketClick(p,source.nPlace,source.nX,source.nY);if(p.m_ItemList.Hand()!=selected)return true;
 BotMarketClick(p,storage.nPlace,storage.nX,storage.nY);if(p.m_ItemList.Hand())BotMarketClick(p,source.nPlace,source.nX,source.nY);
 entry=p.m_ItemList.FindSame(selected);
 if(!p.m_ItemList.Hand()&&entry>0&&Item[selected].GetID()==serial&&p.m_ItemList.m_Items[entry].nPlace==storage.nPlace){
  BotInventoryEvent(r.Config.Id,"DEPOSITED_AT_TREASURY",serial,source.nPlace,storage.nPlace,Item[selected].GetDetailType(),Item[selected].GetParticular());
  if(!p.Save())lstrcpyA(r.Error,"inventory_deposit_save_pending");else r.Error[0]=0;
  Log("RECOVERY",r.Config.Id,"surplus_item_deposited_at_real_treasury");return true;
 }
 plan.npc=0;plan.next=now+30000;lstrcpyA(r.Error,"inventory_deposit_rejected_no_item_deleted");return false;
}
