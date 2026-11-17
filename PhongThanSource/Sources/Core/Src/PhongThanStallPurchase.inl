// Shared by players and bots, on the authoritative world thread.
bool PhongThanStallPurchase(int buyer,int seller,int index){
 if(buyer<=0||buyer>=MAX_PLAYER||seller<=0||seller>=MAX_PLAYER||buyer==seller||index<=0||index>=MAX_ITEM)return false;
 KPlayer& b=Player[buyer];KPlayer& s=Player[seller];
 if(!b.m_PlayerDBLoad||!s.m_PlayerDBLoad||b.m_nIndex<=0||s.m_nIndex<=0||b.CheckTrading()||b.GetLockState()||s.GetLockState()||b.m_ItemList.Hand()||!s.m_PTrade.nTrade)return false;
 KNpc& ba=Npc[b.m_nIndex];KNpc& sa=Npc[s.m_nIndex];
 if(ba.m_CurrentLife<=0||ba.m_FightMode||ba.m_SubWorldIndex!=sa.m_SubWorldIndex||NpcSet.GetDistance(b.m_nIndex,s.m_nIndex)>192)return false;
 if(!PhongThanStallAllowed(seller)){s.SendTradeCancel();return false;}
 KItem& item=Item[index];int slot=s.m_ItemList.FindSame(index),price=item.GetTradePrice();DWORD originalOwner=item.GetOwner();
 if(slot<=0||!item.GetID()||price<=0||price>200000000||item.GetLock()->IsLock()||item.GetLockTrade()||item.IsTemp()||(item.GetExpireTime()&&item.GetExpireTime()<=KSG_GetCurSec()))return false;
 PlayerItem old=s.m_ItemList.m_Items[slot];
 if(old.nPlace!=pos_equiproom&&old.nPlace!=pos_equiproomex&&old.nPlace!=pos_ptbag2&&old.nPlace!=pos_ptbag3)return false;
 if(b.m_ItemList.GetEquipmentMoney()<price||(__int64)s.m_ItemList.GetEquipmentMoney()+price>2000000000)return false;
 int place=0,x=0,y=0;if(!b.m_ItemList.CheckCanPlaceInEquipment(item.GetWidth(),item.GetHeight(),&place,&x,&y))return false;
 if(!b.Pay(price))return false;
 if(!s.m_ItemList.Remove(index)){b.Earn(price);return false;}
 item.SetTradePrice(0);
 if(!b.m_ItemList.Add(index,place,x,y,false)){
  item.SetOwner(originalOwner);item.SetTradePrice(price);s.m_ItemList.Add(index,old.nPlace,old.nX,old.nY,false);b.Earn(price);return false;
 }
 if(!s.Earn(price)){
  b.m_ItemList.Remove(index);item.SetOwner(originalOwner);item.SetTradePrice(price);s.m_ItemList.Add(index,old.nPlace,old.nX,old.nY,false);b.Earn(price);return false;
 }
 int sales=s.m_cTask.GetSaveVal(PT_STALL_SALES_TASK);if(sales<0)sales=0;int before=PTStallLevel(sales);if(sales<2000000000)++sales;
 s.m_cTask.SetSaveVal(PT_STALL_SALES_TASK,sales,TRUE);
 int turnover=s.m_cTask.GetSaveVal(PT_STALL_TURNOVER_TASK);if(turnover<0)turnover=0;
 s.m_cTask.SetSaveVal(PT_STALL_TURNOVER_TASK,turnover>2000000000-price?2000000000:turnover+price,TRUE);
 if(PTStallLevel(sales)>before){char message[128];sprintf(message,ptStallLevelUp,PTStallLevel(sales),sales);s.SendSystemMessage(MESSAGE_SYSTEM_ANNOUCE_HEAD,message);}
 s.SendTradeCount(buyer);b.m_uMustSave=s.m_uMustSave=SAVE_REQUEST;return true;
}
