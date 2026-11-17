// Per-snapshot gear identity; global type is required by the legacy compiler.
struct SeenGear{DWORD id;int place,detail,particular;};
// Uses canonical equipment power and normal inventory movement; never grants items.
static void BotInventorySnapshot(Runtime& r){
 CreateDirectoryA("BotAIgent\\inventory",0);char path[180],temp[185];sprintf(path,"BotAIgent\\inventory\\%s.json",r.Config.Id);sprintf(temp,"%s.tmp",path);FILE* f=fopen(temp,"wb");if(!f)return;KPlayer& player=Player[r.Player];
 fprintf(f,"{\"CarryWeight\":%d,\"CarryCapacity\":%d,",PhongThanCarryWeight(r.Player),PhongThanCarryCapacity(r.Player));
 fprintf(f,"\"Version\":1,\"Bot\":\"%s\",\"UtcSeconds\":%lu,\"Money\":%d,\"Items\":[",r.Config.Id,(unsigned long)time(0),player.m_ItemList.GetEquipmentMoney());int countItems=0;
 static std::vector<SeenGear> previous[Capacity];static DWORD owners[Capacity];std::vector<SeenGear> current;int auditSlot=&r-bots;
 if(owners[auditSlot]!=player.m_dwID){owners[auditSlot]=player.m_dwID;previous[auditSlot].clear();}
 PlayerItem* owned=player.m_ItemList.GetFirstItem();while(owned&&countItems<128){int index=owned->nIdx;if(index>0&&index<MAX_ITEM&&Item[index].GetID()){
  KItem& item=Item[index];char name[161];int j=0;for(;item.GetName()[j]&&j<80;++j)sprintf(name+j*2,"%02x",(unsigned char)item.GetName()[j]);name[j*2]=0;int power=0;bool known=item.GetGenre()==item_equip&&item.GetEquipmentPower(power);
  if(item.GetGenre()==item_equip){SeenGear gear;gear.id=item.GetID();gear.place=owned->nPlace;gear.detail=item.GetDetailType();gear.particular=item.GetParticular();current.push_back(gear);}
  bool locked=item.m_CommonAttrib.bLockTrade||item.GetLock()->nState!=0||item.IsTemp();
  fprintf(f,"%s{\"Id\":%lu,\"NameHex\":\"%s\",\"Genre\":%d,\"Detail\":%d,\"Particular\":%d,\"Level\":%d,\"Count\":%d,\"Place\":%d,\"X\":%d,\"Y\":%d,\"PowerKnown\":%s,\"Power\":%d,\"Equipped\":%s,\"TradeLocked\":%s,\"NpcSalePrice\":%d,\"Quality\":%d,\"Durability\":%d,\"MaxDurability\":%d,\"Weight\":%d}",countItems?",":"",item.GetID(),name,item.GetGenre(),item.GetDetailType(),item.GetParticular(),item.GetLevel(),item.GetStackNum(),owned->nPlace,owned->nX,owned->nY,known?"true":"false",power,owned->nPlace==pos_equip?"true":"false",locked?"true":"false",item.GetSalePrice(),item.GetPhongThanQuality(),item.GetDurability(),item.GetMaxDurability(),item.GetWeight());++countItems;
 }owned=player.m_ItemList.GetNextItem();}
 if(!owned){
  for(int old=0;old<(int)previous[auditSlot].size();++old){const SeenGear& a=previous[auditSlot][old];int found=-1;for(int at=0;at<(int)current.size();++at)if(current[at].id==a.id){found=at;break;}
   if(found<0)BotInventoryEvent(r.Config.Id,"MISSING_FROM_OWNER",a.id,a.place,-1,a.detail,a.particular);
   else if(current[found].place!=a.place)BotInventoryEvent(r.Config.Id,"MOVED",a.id,a.place,current[found].place,a.detail,a.particular);
  }previous[auditSlot]=current;
 }
 fprintf(f,"],\"Truncated\":%s,\"Skills\":[",owned?"true":"false");bool firstSkill=true;KNpc& actor=Npc[player.m_nIndex];for(int k=1;k<MAX_NPCSKILL;++k){NPCSKILL& skill=actor.m_SkillList.m_Skills[k];if(skill.SkillLevel<=0)continue;fprintf(f,"%s{\"Id\":%d,\"Level\":%d,\"Exp\":%d,\"Next\":%d}",firstSkill?"":",",skill.SkillId,skill.SkillLevel,skill.SkillExp,skill.NextSkillExp);firstSkill=false;}fprintf(f,"],\"Mana\":%d,\"MaxMana\":%d,\"HandId\":%lu,\"EquipmentSlots\":[",actor.m_CurrentMana,actor.m_CurrentManaMax,player.m_ItemList.Hand()>0&&player.m_ItemList.Hand()<MAX_ITEM?Item[player.m_ItemList.Hand()].GetID():0);
 for(int part=0;part<itempart_num;++part){int index=player.m_ItemList.GetEquipment(part);fprintf(f,"%s%lu",part?",":"",index>0&&index<MAX_ITEM?Item[index].GetID():0);}fprintf(f,"]}");bool ok=!ferror(f);if(fclose(f))ok=false;if(ok)MoveFileExA(temp,path,MOVEFILE_REPLACE_EXISTING);
}
#include "../../../Headers/PhongThanBotPriceText.h"
#include "PhongThanBotMarketSpeech.inl"
static void BotAdvertiseRare(Runtime& r,DWORD now){
 static DWORD next[Capacity];static int previous[Capacity];int slot=&r-bots;if((int)(now-next[slot])<0)return;next[slot]=now+45000+(Hash(r.Config.Id,32)^now)%90000;KPlayer& p=Player[r.Player];
 if(!r.Config.Train||p.CheckTrading()||p.GetLockState()||companions[slot].Mode>=PTBC_SALE_QUOTE||!GetPrivateProfileIntA("Market","SellAds",1,"BotAIgent\\market-policy.ini"))return;
 for(PlayerItem* owned=p.m_ItemList.GetFirstItem();owned;owned=p.m_ItemList.GetNextItem())if(BotBag(owned->nPlace)&&owned->nIdx>0&&owned->nIdx<MAX_ITEM){KItem& item=Item[owned->nIdx];if(!BotMarketWorthy(item)||!BotEquipmentMismatch(p,item)||!BotMarketTradable(p,owned->nIdx))continue;
  unsigned long seed=Hash(r.Config.Id,32)^now^item.GetID();char body[240],speech[256],label[67];if(!PTFormatItemChatLabel(label,sizeof(label),item.GetName()))continue;
  int variant=-1,price=BotMarketValue(item);if(price<=0)continue;unsigned long fingerprint=0;char displayedPrice[64];PTBotPriceText(price,displayedPrice,sizeof(displayedPrice));
  for(int attempt=0;attempt<24;++attempt){int candidate=(seed+attempt)%24;if(candidate==previous[slot]-1||!botAdLedger.Available(candidate,0,now))continue;
   _snprintf(body,sizeof(body)-1,botSaleFormats[candidate],label,displayedPrice);body[sizeof(body)-1]=0;const char* ending=botSaleEndings[(seed/29+attempt)%(sizeof(botSaleEndings)/sizeof(botSaleEndings[0]))];if(strlen(body)+strlen(ending)<sizeof(body)-1)strcat(body,ending);unsigned long hash=Hash(body,strlen(body));if(!botAdLedger.Available(candidate,hash,now))continue;variant=candidate;fingerprint=hash;break;
  }
  if(variant<0)return;int length=PTBotStyleAd(speech,sizeof(speech),body,strlen(body),seed);if(!length)return;
  BYTE style=(BYTE)(0x80|(seed%4==0?0x40:0));bool sent=::PhongThanBotEmitItemLink(r.Player,PT_CHAT_TRADE,owned->nIdx,speech,length,style);
  if(sent){BotMarketList(r,owned->nIdx);botAdLedger.Sent(variant,fingerprint,now);previous[slot]=variant+1;next[slot]=now+900000+(Hash(r.Config.Id,32)^now)%900000;char log[200];sprintf(log,"owned_unused_rare_advertised item=%lu price=%d variant=%d style=%u mode=item_link",item.GetID(),price,variant,(unsigned)style);Log("MARKET",r.Config.Id,log);}return;
 }
}
struct BotEquipInventory {
 KPlayer& player;BotEquipInventory(KPlayer& p):player(p){}int Hand(){return player.m_ItemList.Hand();}int Equipped(int part){return player.m_ItemList.GetEquipment(part);}
 void Click(const PTBotInventoryCell& cell){PHONGTHAN_ITEM_MOVE_REQUEST move;ZeroMemory(&move,sizeof(move));PhongThanInitializeWireHeader(&move.Header,PHONGTHAN_MSG_INVENTORY_MOVE_REQUEST,sizeof(move),PHONGTHAN_WIRE_FLAG_REQUEST,0);move.FromContainer=move.ToContainer=cell.place;move.FromX=move.ToX=cell.x;move.FromY=move.ToY=cell.y;player.ServerMoveItem((BYTE*)&move);}
};
static void BotAdvertiseBuy(Runtime& r,DWORD now){
 static DWORD next[Capacity];int slot=&r-bots;if((int)(now-next[slot])<0)return;next[slot]=now+45000+(Hash(r.Config.Id,32)^now)%90000;KPlayer& p=Player[r.Player];
 if(!r.Config.Train||companions[slot].Mode||p.CheckTrading()||p.GetLockState()||!GetPrivateProfileIntA("Market","BuyAds",1,"BotAIgent\\market-policy.ini"))return;
 int budget=BotMarketBuyBudget(p);if(budget<300)return;bool medicine=BotPotionCount(p,153)<5,gear=BotNeedsBossGear(r);if(!medicine&&!gear)return;
 char text[256];const char* career=r.Config.Profession==1?"\247\271\157\040\123\334":r.Config.Profession==2?"\104\336\040\116\150\251\156":"\107\151\270\160\040\123\334";int variant=40+(medicine?0:1);
 if(medicine)_snprintf(text,255,"\124\165\151\040\163\276\160\040\150\325\164\040\164\150\165\350\143\040\155\270\165\054\040\143\307\156\040\155\165\141\040\164\150\252\155\056\040\101\151\040\143\343\040\147\366\151\040\154\151\156\153\040\166\265\040\147\151\270\040\162\151\252\156\147\040\156\150\320\054\040\164\165\151\040\170\145\155\040\256\363\156\147\040\154\157\271\151\040\162\345\151\040\147\151\141\157\040\144\336\143\150\056");
 else _snprintf(text,255,"\103\307\156\040\164\327\155\040\256\345\040\154\364\143\040\150\157\306\143\040\150\157\265\156\147\040\153\151\155\040\144\357\156\147\040\256\255\356\143\040\143\150\157\040\045\163\040\143\312\160\040\045\144\054\040\256\345\040\256\141\156\147\040\155\306\143\040\143\337\156\040\171\325\165\056\040\101\151\040\142\270\156\040\147\366\151\040\154\151\252\156\040\153\325\164\040\166\265\040\147\151\270\040\143\150\157\040\164\165\151\040\170\145\155\040\156\150\320\056",career,Npc[p.m_nIndex].m_Level);
 text[255]=0;DWORD hash=Hash(text,strlen(text));if(!botAdLedger.Available(variant,hash,now))return;
 if(::PhongThanBotEmitPublic(r.Player,PT_CHAT_TRADE,text,strlen(text))){botAdLedger.Sent(variant,hash,now);botMarket[slot].WantUntil=now+1200000;botMarket[slot].BuyBudget=budget;next[slot]=now+1200000+(Hash(r.Config.Id,32)^now)%600000;Log("MARKET",r.Config.Id,medicine?"buy_ad_for_missing_recovery_stock":"buy_ad_for_actual_weak_equipment");}
}
static bool BotStoreHand(KPlayer& p){
 int hand=p.m_ItemList.Hand();if(hand<=0)return true;if(hand>=MAX_ITEM||!Item[hand].GetID())return false;
 ItemPos position;bool bank=false;
 if(!p.m_ItemList.FindBagPosition(Item[hand].GetWidth(),Item[hand].GetHeight(),&position)){
  // Emergency recovery for a server bot holding an item with every bag full.
  // Use an existing unlocked storage cell and a server-owned recovery transaction.
  // No item deletion, capacity increase, replacement or new item generation.
  if(!p.m_bIsBot||!p.m_ItemList.FindBotRecoveryStorePosition(Item[hand].GetWidth(),Item[hand].GetHeight(),&position))return false;
  bank=true;
 }
 DWORD serial=Item[hand].GetID();if(bank){if(!p.m_ItemList.RecoverBotHandToStore())return false;}else{BotEquipInventory inventory(p);inventory.Click(PTBotInventoryCell(position.nPlace,position.nX,position.nY));}
 bool stored=p.m_ItemList.Hand()==0&&Item[hand].GetID()==serial&&p.m_ItemList.FindSame(hand)>0;
 if(stored&&bank){FILE* f=fopen("BotAIgent\\hand-recovery.tsv","ab");if(f){fprintf(f,"%lu\t%s\t%lu\t%d\t%d\t%d\n",(unsigned long)time(0),p.Name,serial,position.nPlace,position.nX,position.nY);fclose(f);}p.Save();}
 return stored;
}
static void BotManageEquipment(Runtime& r,DWORD now){
 static DWORD checked[Capacity];int slot=&r-bots;if(now-checked[slot]<5000+(Hash(r.Config.Id,32)%5000))return;checked[slot]=now;
 KPlayer& player=Player[r.Player];if(!SocialAlive(r.Player)||player.GetLockState()||player.CheckTrading())return;
 if(player.m_ItemList.Hand()&&!BotStoreHand(player)){lstrcpyA(r.Error,"equipment_hand_needs_bag_space");BotInventorySnapshot(r);return;}
 if(GetPrivateProfileIntA("Policy","AutoEquip",1,"BotAIgent\\equipment-policy.ini")){
  int best=0,part=-1,gain=0;PTBotInventoryCell source,displaced;PlayerItem* item=player.m_ItemList.GetFirstItem();
  while(item){int index=item->nIdx;bool bag=item->nPlace==pos_equiproom||item->nPlace==pos_equiproomex||item->nPlace==pos_ptbag2||item->nPlace==pos_ptbag3;
   if(bag&&index>0&&index<MAX_ITEM&&!BotEquipmentMismatch(player,Item[index])&&!BotMarketReserved(r.Player,index)&&Item[index].GetGenre()==item_equip&&BotStoryRequiredCount(player,Item[index])==0){int score=BotGearScore(player,Item[index]);
    for(int p=0;p<itempart_num;++p){if(!player.m_ItemList.CanEquip(index,p))continue;int current=player.m_ItemList.GetEquipment(p),oldScore=current>0&&current<MAX_ITEM&&!BotEquipmentMismatch(player,Item[current])&&player.m_ItemList.CanEquip(current,p)&&!(Item[current].GetMaxDurability()>0&&Item[current].GetDurability()==0)?BotGearScore(player,Item[current]):-1;
     int carry=PhongThanCarryWeight(r.Player),after=carry-Item[index].GetWeight()+(current>0&&current<MAX_ITEM?Item[current].GetWeight():0);if(after>PhongThanCarryCapacity(r.Player)&&after>=carry)continue;
     PTBotInventoryCell back(item->nPlace,item->nX,item->nY);if(current>0&&current<MAX_ITEM&&(Item[current].GetWidth()>Item[index].GetWidth()||Item[current].GetHeight()>Item[index].GetHeight())){ItemPos space;if(!player.m_ItemList.FindBagPosition(Item[current].GetWidth(),Item[current].GetHeight(),&space))continue;back=PTBotInventoryCell(space.nPlace,space.nX,space.nY);}
     if((current<=0||score>oldScore)&&(best==0||score-oldScore>gain)){best=index;part=p;gain=score-oldScore;source=PTBotInventoryCell(item->nPlace,item->nX,item->nY);displaced=back;}
    }
   }item=player.m_ItemList.GetNextItem();
  }
  if(best>0&&part>=0){BotEquipInventory inventory(player);bool completed=PTBotEquipTransaction(inventory,best,part,source,PTBotInventoryCell(pos_equip,part,0),displaced);if(!completed)BotStoreHand(player);
   if(player.m_ItemList.GetEquipment(part)==best&&!player.m_ItemList.Hand()){char message[160];sprintf(message,"better_owned_equipment_confirmed item=%lu part=%d score_gain=%d",Item[best].GetID(),part,gain);Log("EQUIP",r.Config.Id,message);player.Save();}else Log("EQUIP",r.Config.Id,"equipment_change_rejected_or_needs_bag_space");
  }
 }
 BotAdvertiseRare(r,now);BotAdvertiseBuy(r,now);BotInventorySnapshot(r);
}
