#include "../../../Headers/PhongThanPetSalePolicy.h"
// Uses the existing authoritative pickup/sale/repair operations; pet packets
// never supply prices, rewards, item templates or a remote target position.
static bool PtPetPickExcluded(int item,const PT_PET_DATA& d){
 int genre=Item[item].GetGenre(),mask=d.PickExclude;
 if(genre==item_equip){int quality=Item[item].GetQuality();if(mask&(quality==equip_normal?1:quality==equip_magic?2:4))return true;}
 if((genre==item_materials&&(mask&8))||(genre==item_medicine&&(mask&16))||(genre==item_skillbook&&(mask&32)))return true;
 return d.PickName[0]&&strstr(Item[item].GetName(),d.PickName)==0;
}
static bool PtPetSaleKeep(int item,const PT_PET_DATA& d){
 if(PtSoulParam(PtSoulInnerParam(Item[item].GetParam())))return true;
 if(!PhongThanPetJunkFamily(Item[item].GetGenre(),Item[item].GetDetailType(),Item[item].GetSetID(),PhongThanForgeRecipe(Item[item].GetParam()),Item[item].GetUpgradeState()))return true;
 int quality=Item[item].GetQuality();
 if(quality!=equip_normal&&quality!=equip_magic)return true;
 if(d.SaleKeep&(quality==equip_normal?1:2))return true;
 if(d.SaleLevel>0&&Item[item].GetLevel()>=d.SaleLevel)return true;
 int magic=0;for(int i=0;i<MAX_ITEM_MAGICATTRIB;++i)if(Item[item].m_aryMagicAttrib[i].nAttribType>0)++magic;
 return d.SaleMagic>0&&magic>=d.SaleMagic;
}
static void PtPetAutomate(int player,PT_PET_RUNTIME*r){
 if(r->Journal||Player[player].GetLockState()||Player[player].CheckTrading())return;
 PT_PET_DATA& d=r->Data;int actor=Player[player].m_nIndex;
 if(!Npc[actor].IsAlive()||d.Strength<=0||(d.Flags&PT_PET_REST))return;
 DWORD now=GetTickCount();
 if((d.Flags&PT_PET_CHAT)&&now-r->ChatTick>=60000&&d.AgeSeconds>=PtPetSetting("Chat","NeedTime",3600)){
  r->ChatTick=now;KIniFile chat;if(chat.Load("\\settings\\petchat.ini")){
   int count=0;chat.GetInteger("Sentence","Count",0,&count);if(count>0&&count<1000){char key[16],text[256];sprintf(key,"%d",g_Random(count));chat.GetString("Sentence",key,"",text,sizeof(text));
    const char* value=text[0]=='$'?text+1:text;if(value[0])KPlayerChat::SendSystemInfo(1,player,MESSAGE_BROADCAST_ANNOUCE_HEAD,(char*)value,strlen(value),-1);
   }
  }
 }
 if((d.Flags&PT_PET_PICK)&&d.AgeSeconds>=PtPetSetting("AutoPick","NeedTime",7200)&&d.Strength>=PtPetSetting("AutoPick","NeedStr",104400)){
  int x,y;Npc[actor].GetMpsPos(&x,&y);
  int candidates[512];const int nearby=PtPetNearbyObjects(actor,candidates,512);
  for(int scanned=0;scanned<nearby;++scanned){
   KObj& object=Object[candidates[scanned]];
   if(!object.m_nID||object.m_nRegionIdx<0||object.m_nSubWorldID!=Npc[actor].m_SubWorldIndex||object.m_nKind!=Obj_Kind_Item)continue;
   int item=object.m_nItemDataID;if(item<=0||item>=MAX_ITEM)continue;
   if(PtPetPickExcluded(item,d))continue;
   int ox,oy;object.GetMpsPos(&ox,&oy);int dx=ox-x,dy=oy-y;
   if(abs(dx)>200||abs(dy)>200||dx*dx+dy*dy>PLAYER_PICKUP_SERVER_DISTANCE)continue;
   ItemPos destination;if(!Player[player].m_ItemList.FindPickupPosition(item,&destination))continue;
   PHONGTHAN_ITEM_PICKUP_REQUEST request;ZeroMemory(&request,sizeof(request));
   PhongThanInitializeWireHeader(&request.Header,PHONGTHAN_MSG_INVENTORY_PICKUP_REQUEST,sizeof(request),PHONGTHAN_WIRE_FLAG_REQUEST,0);request.WorldObjectId=object.m_nID;request.DestinationContainer=destination.nPlace;request.DestinationX=destination.nX;request.DestinationY=destination.nY;
   if(Player[player].ServerPickUpItem((BYTE*)&request)){d.Strength-=PtPetSetting("AutoPick","StrSub",4);r->Dirty=1;if(d.PickExclude&128)PtPetLog(d,40,item);break;}
  }
 }
 if(now-r->AutomationTick<5000)return;r->AutomationTick=now;
 PlayerItem* owned=Player[player].m_ItemList.GetFirstItem();int sell=0,repair=0;
 while(owned){
  int item=owned->nIdx;
  if(item>0&&item<MAX_ITEM&&Item[item].GetGenre()==item_equip&&!Item[item].GetLock()->IsLock()){
   if(!sell&&(d.Flags&PT_PET_SELL)&&owned->nPlace==pos_equiproom&&
      d.AgeSeconds>=PtPetSetting("AutoSold","NeedTime",10800)&&d.Strength>=PtPetSetting("AutoSold","NeedStr",104400)&&
      !PtPetSaleKeep(item,d)&&
      !Item[item].GetUpgradeState()&&!Item[item].GetLockSell()&&!Item[item].GetLockTrade())sell=item;
   if(!repair&&(d.Flags&PT_PET_REPAIR)&&owned->nPlace==pos_equip&&
      d.AgeSeconds>=PtPetSetting("AutoRepair","NeedTime",18000)&&d.Strength>=PtPetSetting("AutoRepair","NeedStr",104400)&&
      Item[item].GetDurability()>0&&Item[item].GetDurability()<Item[item].GetMaxDurability())repair=item;
  }
  owned=Player[player].m_ItemList.GetNextItem();
 }
 if(sell&&BuySell.Sell(player,0,sell,1)){d.Strength-=PtPetSetting("AutoSold","StrSub",4);PtPetLog(d,41,1);r->Dirty=1;}
 if(repair){int cost=Item[repair].GetRepairPrice();
  if(cost>0&&Player[player].Pay(cost)){Item[repair].SetDurability(Item[repair].GetMaxDurability());Player[player].m_ItemList.SyncItemDurability(repair);
   d.Strength+=PtPetSetting("AutoRepair","RepairStr",-2880);PtPetLog(d,42,cost);r->Dirty=1;}
 }
 if(d.Strength<0)d.Strength=0;
}
