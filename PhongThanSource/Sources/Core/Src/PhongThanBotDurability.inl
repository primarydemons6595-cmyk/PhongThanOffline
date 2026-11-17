// Use the same authoritative paid repair as players, including broken gear.
static void BotMaintainDurability(Runtime& r,DWORD now){
 static DWORD checked[Capacity],reported[Capacity];int slot=&r-bots;if(now-checked[slot]<5000)return;checked[slot]=now;
 KPlayer& p=Player[r.Player];if(!p.m_bIsBot||!SocialAlive(r.Player)||p.GetLockState()||p.CheckTrading()||p.m_ItemList.Hand())return;
 int selected=0,percent=101;
 for(PlayerItem* owned=p.m_ItemList.GetFirstItem();owned;owned=p.m_ItemList.GetNextItem()){
  int item=owned->nIdx;if((owned->nPlace!=pos_equip&&!BotBag(owned->nPlace))||item<=0||item>=MAX_ITEM||Item[item].GetGenre()!=item_equip)continue;
  int maxDur=Item[item].GetMaxDurability(),dur=Item[item].GetDurability();if(maxDur<=0||dur<0||(__int64)dur*100>(__int64)maxDur*50||!Item[item].CanBeRepaired())continue;
  int ratio=(int)((__int64)dur*100/maxDur);if(ratio<percent){selected=item;percent=ratio;}
 }
 if(!selected)return;int cost=Item[selected].GetRepairPrice(),before=Item[selected].GetDurability(),money=p.m_ItemList.GetEquipmentMoney();if(cost<=0)return;
 if(money<cost){if(now-reported[slot]>60000){reported[slot]=now;BotInventoryEvent(r.Config.Id,"REPAIR_NEEDS_MONEY",Item[selected].GetID(),before,cost,Item[selected].GetDetailType(),Item[selected].GetParticular());}return;}
 DWORD id=Item[selected].GetID();p.RepairItem(id);
 if(Item[selected].GetDurability()==Item[selected].GetMaxDurability()&&p.m_ItemList.GetEquipmentMoney()==money-cost){BotInventoryEvent(r.Config.Id,"PAID_REPAIR",id,before,Item[selected].GetDurability(),Item[selected].GetDetailType(),Item[selected].GetParticular());p.Save();}
}

// A stranded autonomous bot uses an owned scroll; never abandons a companion.
static bool BotReturnForTraining(Runtime& r,Training& t,DWORD now){
 static DWORD idle[Capacity],attempt[Capacity];int slot=&r-bots;
 if(t.Target>0||t.QuestTarget||companions[slot].Mode||companions[slot].Pending){idle[slot]=0;return false;}
 if(!idle[slot])idle[slot]=now;KPlayer& p=Player[r.Player];KNpc& actor=Npc[p.m_nIndex];
 if(actor.m_SubWorldIndex<0||actor.m_SubWorldIndex>=MAX_SUBWORLD)return false;int map=SubWorld[actor.m_SubWorldIndex].m_SubWorldID;
 if(!PTBotShouldReturnForTraining(now-idle[slot],now-attempt[slot],map,r.Config.Map,false,false,1,p.CheckTrading()||p.GetLockState()||p.m_ItemList.Hand()||p.m_nPaceBarTime))return false;
 for(PlayerItem* owned=p.m_ItemList.GetFirstItem();owned;owned=p.m_ItemList.GetNextItem())if(BotBag(owned->nPlace)&&owned->nIdx>0&&owned->nIdx<MAX_ITEM){
  KItem& item=Item[owned->nIdx];if(item.GetGenre()!=item_townportal||item.GetDetailType()!=0||item.GetParticular()!=0||item.GetStackNum()<1||item.GetLock()->nState||item.IsTemp()||(item.GetExpireTime()&&item.GetExpireTime()<=KSG_GetCurSec()))continue;
  DWORD serial=item.GetID();int before=item.GetStackNum();PHONGTHAN_ITEM_USE_REQUEST use;ZeroMemory(&use,sizeof(use));PhongThanInitializeWireHeader(&use.Header,PHONGTHAN_MSG_INVENTORY_USE_REQUEST,sizeof(use),PHONGTHAN_WIRE_FLAG_REQUEST,0);use.ItemId=serial;use.Container=owned->nPlace;use.SlotX=owned->nX;use.SlotY=owned->nY;attempt[slot]=now;p.EatItem((BYTE*)&use);
  int remaining=p.m_ItemList.SearchID(serial);if(!remaining||Item[remaining].GetStackNum()<before||p.m_nPaceBarTime){t.Path.clear();t.Selected=0;idle[slot]=now;lstrcpyA(r.State,"returning_town_to_reselect");r.Error[0]=0;BotInventoryEvent(r.Config.Id,"OWNED_TOWN_SCROLL_REQUESTED",serial,map,SubWorld[actor.m_SubWorldIndex].m_SubWorldID,0,0);p.Save();return true;}return false;
 }return false;
}
