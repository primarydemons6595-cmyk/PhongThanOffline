// Owned resources only. All mutations use the same validated routes as players.
static bool BotBag(int place){return place==pos_equiproom||place==pos_equiproomex||place==pos_ptbag2||place==pos_ptbag3||place==pos_immediacy;}
static bool BotRareEquipment(KItem& item){return item.GetGenre()==item_equip&&(item.GetPhongThanQuality()>0||item.m_CommonAttrib.nUpgradeLvl>0||item.m_CommonAttrib.nSetID);}
static bool BotLowWarriorSet(KItem& item){
 if(item.GetGenre()!=item_equip||item.GetPhongThanQuality()!=4||item.GetLevel()>3||item.GetUpgradeState())return false;
 for(int at=0;at<6;++at){KMagicAttrib* req=(KMagicAttrib*)item.GetRequirement(at);if(req&&(req->nAttribType==magic_requireseries||req->nAttribType==magic_requiremenpai)&&req->nValue[0]==3)return true;}return false;
}
static bool BotMarketWorthy(KItem& item){if(item.GetGenre()!=item_equip||BotLowWarriorSet(item))return false;int required=0;for(int q=0;q<6;++q)if(item.TooltipRequirements()[q].nAttribType==magic_requirelevel)required=item.TooltipRequirements()[q].nValue[0];PTBotMarketBonuses bonuses;for(int i=0;i<MAX_ITEM_MAGICATTRIB;++i)bonuses.Add(item.m_aryMagicAttrib[i].nAttribType,item.m_aryMagicAttrib[i].nValue[0]);return bonuses.Eligible(item.GetPhongThanQuality(),required);}
static bool BotEquipmentMismatch(KPlayer& p,KItem& item){
 KNpc& actor=Npc[p.m_nIndex];for(int i=0;i<32;++i){KMagicAttrib* req=(KMagicAttrib*)item.GetRequirement(i);if(!req)break;if((req->nAttribType==magic_requireseries&&req->nValue[0]==3&&p.m_cProfession.GetProfession()!=3)||(req->nAttribType==magic_requiremenpai&&req->nValue[0]!=p.m_cProfession.GetProfession())||(req->nAttribType==magic_requiresex&&req->nValue[0]!=actor.m_nSex)||(req->nAttribType==magic_requireseries&&req->nValue[0]!=actor.m_Series))return true;}return false;
}
static int BotGearScore(KPlayer& p,KItem& item){
 if(item.GetGenre()!=item_equip)return -1;__int64 score=0;int native=0;if(item.GetEquipmentPower(native)&&native>0)score+=(__int64)native*4;
 for(int i=0;i<MAX_ITEM_BASEATTRIB;++i)score+=PTBotGearAttributeScore(item.P0BaseAttributes()[i].nAttribType,item.P0BaseAttributes()[i].nValue,p.m_cProfession.GetProfession());
 for(i=0;i<MAX_ITEM_MAGICATTRIB;++i)score+=PTBotGearAttributeScore(item.m_aryMagicAttrib[i].nAttribType,item.m_aryMagicAttrib[i].nValue,p.m_cProfession.GetProfession());
 // Color is only a small tie-breaker; unusable equipment is rejected elsewhere.
 score+=item.GetPhongThanQuality()*2;return score>2147483000?2147483000:(int)score;
}
static bool BotEquipmentUseful(KPlayer& p,int item){
 if(BotEquipmentMismatch(p,Item[item]))return false;
 int score=BotGearScore(p,Item[item]);if(score<0)return true;
 for(int part=0;part<itempart_num;++part)if(p.m_ItemList.CanEquip(item,part)){int old=p.m_ItemList.GetEquipment(part),oldScore=old>0&&old<MAX_ITEM&&!BotEquipmentMismatch(p,Item[old])&&p.m_ItemList.CanEquip(old,part)&&!(Item[old].GetMaxDurability()>0&&Item[old].GetDurability()==0)?BotGearScore(p,Item[old]):-1;if(old<=0||score>oldScore)return true;}return false;
}
// Blue alone is not proof of a valuable roll. Keep upgrades, sets, gold and
// anything useful; only zero-power ordinary blue gear may be sold at an NPC.
static bool BotZeroPowerBlueSurplus(KPlayer& p,int index){KItem& item=Item[index];int power=0;return item.GetGenre()==item_equip&&item.GetPhongThanQuality()==1&&!item.m_CommonAttrib.nUpgradeLvl&&!item.m_CommonAttrib.nSetID&&item.GetEquipmentPower(power)&&power==0&&!BotEquipmentUseful(p,index);}
static bool BotImportantEquipment(KPlayer& p,int index){return BotRareEquipment(Item[index])&&!BotZeroPowerBlueSurplus(p,index);}
static int BotOwnedEquivalent(KPlayer& player,KItem& wanted){int count=0;for(PlayerItem* p=player.m_ItemList.GetFirstItem();p;p=player.m_ItemList.GetNextItem())if((BotBag(p->nPlace)||p->nPlace==pos_equip)&&p->nIdx>0&&p->nIdx<MAX_ITEM){KItem& item=Item[p->nIdx];if(item.GetGenre()==wanted.GetGenre()&&item.GetDetailType()==wanted.GetDetailType()&&item.GetParticular()==wanted.GetParticular()&&item.GetLevel()==wanted.GetLevel())count+=item.GetStackNum();}return count;}
static bool BotNpcReplacementWanted(KPlayer& player,KItem& offer){
 if(offer.GetGenre()!=item_equip||BotLowWarriorSet(offer)||BotEquipmentMismatch(player,offer))return false;
 const int parts[]={itempart_weapon,itempart_horse,itempart_body,itempart_head,itempart_foot,itempart_belt,itempart_pendant};
 for(int at=0;at<sizeof(parts)/sizeof(parts[0]);++at){int part=parts[at],current=player.m_ItemList.GetEquipment(part);
  if(current>0&&current<MAX_ITEM&&!BotEquipmentMismatch(player,Item[current])&&player.m_ItemList.CanEquip(current,part)&&!(Item[current].GetMaxDurability()>0&&Item[current].GetDurability()==0))continue;
  if(!player.m_ItemList.CanEquip(&offer,part))continue;bool spare=false;
  for(PlayerItem* owned=player.m_ItemList.GetFirstItem();owned;owned=player.m_ItemList.GetNextItem())if(BotBag(owned->nPlace)&&owned->nIdx>0&&owned->nIdx<MAX_ITEM&&player.m_ItemList.CanEquip(owned->nIdx,part)){spare=true;break;}
  if(!spare)return true;
 }return false;
}
static int BotRecoveryKind(KItem& item){
 if(item.GetGenre()==item_medicine){if(item.HasMedicineEffect(153))return 1;if(item.HasMedicineEffect(154))return 2;}
 if(item.GetGenre()==item_ibitem)return PhongThanConsumableRecoveryKind(item.GetGenre(),item.GetDetailType(),item.GetParticular(),item.GetScript());
 return 0;
}
static int BotOwnedRecoveryCount(KPlayer& p,int kind){int count=0;for(PlayerItem* owned=p.m_ItemList.GetFirstItem();owned;owned=p.m_ItemList.GetNextItem())if(BotBag(owned->nPlace)&&owned->nIdx>0&&owned->nIdx<MAX_ITEM&&BotRecoveryKind(Item[owned->nIdx])==kind)count+=Item[owned->nIdx].GetStackNum();return count;}
static int BotStoryRequiredCount(KPlayer& player,KItem& item);
static int BotStoryMissingCount(KPlayer& player,KItem& item);
static bool BotLearnable(KPlayer& player,KItem& item){
 int skill=item.GetParticular();if(item.GetGenre()==item_skillbook&&PhongThanIsLivingBook(item.GetDetailType(),skill))return Npc[player.m_nIndex].m_SkillList.GetLevel(skill)<=0;return item.GetGenre()==item_skillbook&&PhongThanCanLearnBook(item.GetDetailType(),skill,player.m_cProfession.GetProfession(),Npc[player.m_nIndex].m_Level,Npc[player.m_nIndex].m_SkillList.GetLevel(skill))&&PhongThanNativeCanLearn(skill,Npc[player.m_nIndex].m_SkillList,Npc[player.m_nIndex].m_Level);
}
// Emergency unloading uses the ordinary hand move and ground-drop requests.
// Never destroy items, discard equipped/rare/useful gear, or waive carry rules.
static bool BotRelieveCarry(Runtime& r,DWORD now,bool requested=false){
 KPlayer& p=Player[r.Player];if(!requested&&!PhongThanCarryBlocked(r.Player))return false;
 lstrcpyA(r.State,"overloaded");int before=PhongThanCarryWeight(r.Player),capacity=PhongThanCarryCapacity(r.Player);
 sprintf(r.Error,"carry_over_capacity_%d_of_%d",before,capacity);
 static DWORD last[Capacity];int slot=&r-bots;if(now-last[slot]<2000)return true;last[slot]=now;
 if(p.GetLockState()||p.CheckTrading()||p.m_ItemList.Hand())return true;
 int chosen=0,price=2147483647;ItemPos source;
 for(PlayerItem* owned=p.m_ItemList.GetFirstItem();owned;owned=p.m_ItemList.GetNextItem())if(BotBag(owned->nPlace)&&owned->nIdx>0&&owned->nIdx<MAX_ITEM){
  int index=owned->nIdx;KItem& item=Item[index];
  if(BotMarketReserved(r.Player,index)||item.GetGenre()!=item_equip||BotRareEquipment(item)||item.IsTemp()||item.GetExpireTime()>0||item.GetScript()[0]||item.GetWeight()<=0||item.GetLock()->nState||item.m_CommonAttrib.bLockSell||item.m_CommonAttrib.bLockTrade||BotEquipmentUseful(p,index)||BotStoryRequiredCount(p,item)>0)continue;
  if(item.GetSalePrice()<price){chosen=index;price=item.GetSalePrice();source.nPlace=owned->nPlace;source.nX=owned->nX;source.nY=owned->nY;}
 }
 if(!chosen){
  // A real owned town scroll is an escape action when no safe white drop or
  // nearby merchant can help. Never bypass travel restrictions or fabricate it.
  static DWORD returnAt[Capacity];if((int)(now-returnAt[slot])>=0){returnAt[slot]=now+60000;
   int portal=0,total=0;for(PlayerItem* owned=p.m_ItemList.GetFirstItem();owned;owned=p.m_ItemList.GetNextItem())if(BotBag(owned->nPlace)&&owned->nIdx>0&&owned->nIdx<MAX_ITEM){KItem& item=Item[owned->nIdx];if(item.GetGenre()==item_townportal&&item.GetLock()->nState==0&&!item.IsTemp()&&(!item.GetExpireTime()||item.GetExpireTime()>KSG_GetCurSec())){total+=item.GetStackNum();if(!portal)portal=owned->nIdx;}}
   if(portal&&total>1){int weight=Item[portal].GetWeight(),needed=PTBotCarryEscapeScrolls(before,capacity,weight,total);
    int map=SubWorld[Npc[p.m_nIndex].m_SubWorldIndex].m_SubWorldID,x=0,y=0;Npc[p.m_nIndex].GetMpsPos(&x,&y);
    int used=0;for(int attempt=0;attempt<needed;++attempt){int index=p.m_ItemList.SearchID(Item[portal].GetID());if(index<=0)break;int at=p.m_ItemList.FindSame(index);if(at<=0)break;PlayerItem& owned=p.m_ItemList.m_Items[at];DWORD serial=Item[index].GetID();int countBefore=Item[index].GetStackNum();PHONGTHAN_ITEM_USE_REQUEST use;ZeroMemory(&use,sizeof(use));PhongThanInitializeWireHeader(&use.Header,PHONGTHAN_MSG_INVENTORY_USE_REQUEST,sizeof(use),PHONGTHAN_WIRE_FLAG_REQUEST,0);use.ItemId=serial;use.Container=owned.nPlace;use.SlotX=owned.nX;use.SlotY=owned.nY;p.EatItem((BYTE*)&use);int remaining=p.m_ItemList.SearchID(serial);if(!remaining||Item[remaining].GetStackNum()<countBefore){++used;if(!remaining)break;}else break;if(!PhongThanCarryBlocked(r.Player))break;}
    if(used){char message[200];int nx=0,ny=0;Npc[p.m_nIndex].GetMpsPos(&nx,&ny);sprintf(message,"owned_town_scroll_recovery used=%d weight=%d->%d map=%d->%d position=%d,%d->%d,%d",used,before,PhongThanCarryWeight(r.Player),map,SubWorld[Npc[p.m_nIndex].m_SubWorldIndex].m_SubWorldID,x,y,nx,ny);Log("CARRY",r.Config.Id,message);p.Save();lstrcpyA(r.State,"carry_returning_town");return true;}
   }
  }
 }
 if(!chosen){static DWORD diagnostic[Capacity];if(now-diagnostic[slot]>=60000){diagnostic[slot]=now;for(PlayerItem* owned=p.m_ItemList.GetFirstItem();owned;owned=p.m_ItemList.GetNextItem())if(BotBag(owned->nPlace)&&owned->nIdx>0&&owned->nIdx<MAX_ITEM&&Item[owned->nIdx].GetGenre()==item_equip){KItem& item=Item[owned->nIdx];char detail[400];sprintf(detail,"carry_equipment_protection id=%lu weight=%d quality=%d useful=%d quest=%d locks=%d/%d/%d temporary=%d expiry=%lu script=%.100s",item.GetID(),item.GetWeight(),item.GetPhongThanQuality(),BotEquipmentUseful(p,owned->nIdx)?1:0,BotStoryRequiredCount(p,item),item.GetLock()->nState,item.m_CommonAttrib.bLockSell,item.m_CommonAttrib.bLockTrade,item.IsTemp()?1:0,item.GetExpireTime(),item.GetScript());Log("CARRY",r.Config.Id,detail);}}static DWORD reported[Capacity];if(now-reported[slot]>=60000){reported[slot]=now;Log("CARRY",r.Config.Id,"no_safe_unload_nearby_sale_failed_no_protected_items_discarded");}lstrcpyA(r.State,"blocked_overload");return true;}
 DWORD identity=Item[chosen].GetID();PHONGTHAN_ITEM_MOVE_REQUEST move;ZeroMemory(&move,sizeof(move));PhongThanInitializeWireHeader(&move.Header,PHONGTHAN_MSG_INVENTORY_MOVE_REQUEST,sizeof(move),PHONGTHAN_WIRE_FLAG_REQUEST,0);
 move.FromContainer=source.nPlace;move.FromX=source.nX;move.FromY=source.nY;move.ToContainer=source.nPlace;move.ToX=source.nX;move.ToY=source.nY;p.ServerMoveItem((BYTE*)&move);
 if(p.m_ItemList.Hand()!=chosen)return true;
 PHONGTHAN_ITEM_DROP_REQUEST drop;ZeroMemory(&drop,sizeof(drop));PhongThanInitializeWireHeader(&drop.Header,PHONGTHAN_MSG_INVENTORY_DROP_REQUEST,sizeof(drop),PHONGTHAN_WIRE_FLAG_REQUEST,0);drop.ItemId=identity;p.ServerThrowAwayItem((BYTE*)&drop);
 bool ground=false;for(int n=1;n<MAX_OBJECT;++n)if(Object[n].m_nID&&Object[n].m_nKind==Obj_Kind_Item&&Object[n].m_nItemDataID==chosen&&Item[chosen].GetID()==identity){ground=true;break;}
 if(!p.m_ItemList.SearchID(identity)&&ground){char message[160];sprintf(message,"surplus_white_dropped_to_unblock item=%lu weight=%d->%d capacity=%d",identity,before,PhongThanCarryWeight(r.Player),capacity);Log("CARRY",r.Config.Id,message);p.Save();}
 else if(p.m_ItemList.Hand()==chosen){move.FromContainer=source.nPlace;move.FromX=source.nX;move.FromY=source.nY;move.ToContainer=source.nPlace;move.ToX=source.nX;move.ToY=source.nY;p.ServerMoveItem((BYTE*)&move);Log("WARN",r.Config.Id,"carry_drop_rejected_inventory_kept");}
 return true;
}
static void BotLearnOwned(Runtime& r,DWORD now){
 static DWORD last[Capacity];int index=&r-bots;if(now-last[index]<5000)return;last[index]=now;KPlayer& p=Player[r.Player];
 if(!SocialAlive(r.Player)||p.GetLockState()||p.CheckTrading())return;
 for(PlayerItem* owned=p.m_ItemList.GetFirstItem();owned;owned=p.m_ItemList.GetNextItem())if(BotBag(owned->nPlace)&&owned->nIdx>0&&owned->nIdx<MAX_ITEM&&BotLearnable(p,Item[owned->nIdx])&&BotStoryRequiredCount(p,Item[owned->nIdx])==0){
  int skill=Item[owned->nIdx].GetParticular();DWORD id=Item[owned->nIdx].GetID();PHONGTHAN_ITEM_USE_REQUEST use;ZeroMemory(&use,sizeof(use));PhongThanInitializeWireHeader(&use.Header,PHONGTHAN_MSG_INVENTORY_USE_REQUEST,sizeof(use),PHONGTHAN_WIRE_FLAG_REQUEST,0);use.ItemId=id;use.Container=owned->nPlace;use.SlotX=owned->nX;use.SlotY=owned->nY;p.EatItem((BYTE*)&use);
  if(Npc[p.m_nIndex].m_SkillList.GetLevel(skill)>0&&!p.m_ItemList.SearchID(id)){char log[120];sprintf(log,"book_consumed_skill_learned skill=%d level=%d",skill,Npc[p.m_nIndex].m_SkillList.GetLevel(skill));Log("SKILL",r.Config.Id,log);p.Save();}return;
 }
}
static bool BotUseRecovery(Runtime& r,DWORD now,bool life,bool mana){
 Training& t=training[&r-bots];if(now-t.ConsumedAt<1000||(!life&&!mana))return false;KPlayer& p=Player[r.Player];KNpc& npc=Npc[p.m_nIndex];
 if(p.GetLockState()||p.CheckTrading())return false;
 for(PlayerItem* owned=p.m_ItemList.GetFirstItem();owned;owned=p.m_ItemList.GetNextItem())if(BotBag(owned->nPlace)&&owned->nIdx>0&&owned->nIdx<MAX_ITEM){int item=owned->nIdx,kind=BotRecoveryKind(Item[item]);if((kind!=1||!life)&&(kind!=2||!mana))continue;
  DWORD id=Item[item].GetID();int before=Item[item].GetStackNum(),hp=npc.m_CurrentLife,mp=npc.m_CurrentMana;PHONGTHAN_ITEM_USE_REQUEST use;ZeroMemory(&use,sizeof(use));PhongThanInitializeWireHeader(&use.Header,PHONGTHAN_MSG_INVENTORY_USE_REQUEST,sizeof(use),PHONGTHAN_WIRE_FLAG_REQUEST,0);use.ItemId=id;use.Container=owned->nPlace;use.SlotX=owned->nX;use.SlotY=owned->nY;use.Reserved=Item[item].GetGenre()==item_ibitem?1:0;p.EatItem((BYTE*)&use);t.ConsumedAt=now;
  if(!p.m_ItemList.SearchID(id)||Item[item].GetID()!=id||Item[item].GetStackNum()<before||npc.m_CurrentLife>hp||npc.m_CurrentMana>mp){char log[140];sprintf(log,"recovery_consumed kind=%d item=%lu life=%d->%d mana=%d->%d",kind,id,hp,npc.m_CurrentLife,mp,npc.m_CurrentMana);Log("ITEM",r.Config.Id,log);return true;}
 }
 return false;
}
static bool BotStoryWantedItem(KPlayer& player,KItem& item);
static std::vector<int> botDrops[256];
static void BotIndexDrops(){
 int equipment=0,recovery=0,money=0,books=0;
 for(int i=0;i<256;++i)botDrops[i].clear();
 for(int n=1;n<MAX_OBJECT;++n)if(Object[n].m_nID&&Object[n].m_nRegionIdx>=0&&(Object[n].m_nKind==Obj_Kind_Item||Object[n].m_nKind==Obj_Kind_Money)&&Object[n].m_nSubWorldID>=0&&Object[n].m_nSubWorldID<MAX_SUBWORLD){int map=SubWorld[Object[n].m_nSubWorldID].m_SubWorldID-1000;if(map>=0&&map<256)botDrops[map].push_back(n);if(Object[n].m_nKind==Obj_Kind_Money)++money;else{int item=Object[n].m_nItemDataID;if(item>0&&item<MAX_ITEM){if(Item[item].GetGenre()==item_equip)++equipment;if(BotRecoveryKind(Item[item]))++recovery;if(Item[item].GetGenre()==item_skillbook)++books;}}}
 static DWORD last=0;if(GetTickCount()-last>=10000){last=GetTickCount();char text[240];int size=sprintf(text,"{\"Pid\":%lu,\"UtcSeconds\":%lu,\"Equipment\":%d,\"Recovery\":%d,\"Money\":%d,\"Books\":%d}",GetCurrentProcessId(),(unsigned long)time(0),equipment,recovery,money,books);Atomic("BotAIgent\\loot-world.json",text,size);}
}
static bool BotLootAllowed(KPlayer& p,KObj& obj){
 if(obj.m_nBelong==-1||obj.m_nBelong==p.GetPlayerIndex())return true;
 return p.m_cTeam.m_nFlag&&p.m_cTeam.m_nID>=0&&p.m_cTeam.m_nID<MAX_TEAM&&g_Team[p.m_cTeam.m_nID].CheckIn(p.GetPlayerIndex(),obj.m_nBelong);
}
static bool BotLoot(Runtime& r,Training& t,DWORD now,bool urgent=false,bool requested=false){
 KPlayer& p=Player[r.Player];KNpc& npc=Npc[p.m_nIndex];if(!r.Config.Train||p.CheckTrading()||p.GetLockState()||p.m_ItemList.Hand()||npc.m_CurrentLife<=0)return false;
 static DWORD checked[Capacity],bad[Capacity],badUntil[Capacity],started[Capacity],goal[Capacity];int slot=&r-bots;if(now-checked[slot]<800)return !strcmp(r.State,"looting");checked[slot]=now;
 int map=SubWorld[npc.m_SubWorldIndex].m_SubWorldID-1000;if(map<0||map>=256)return false;int px=0,py=0;npc.GetMpsPos(&px,&py);int selected=0,best=-2147483647;
 int carry=PhongThanCarryWeight(r.Player),capacity=PhongThanCarryCapacity(r.Player);int hpStock=BotOwnedRecoveryCount(p,1),mpStock=BotOwnedRecoveryCount(p,2);
 for(int j=0;j<(int)botDrops[map].size();++j){int n=botDrops[map][j];KObj& obj=Object[n];if(!obj.m_nID||obj.m_nSubWorldID!=npc.m_SubWorldIndex||obj.m_nRegionIdx<0||!BotLootAllowed(p,obj)||(bad[slot]==(DWORD)obj.m_nID&&(int)(now-badUntil[slot])<0))continue;
  int x=0,y=0;obj.GetMpsPos(&x,&y);int distance=abs(px-x)+abs(py-y);if(distance>480)continue;int priority=100;
  if(obj.m_nKind==Obj_Kind_Item){int item=obj.m_nItemDataID;if(item<=0||item>=MAX_ITEM||!Item[item].GetID())continue;ItemPos pos;if(!p.m_ItemList.FindPickupPosition(obj.m_nItemDataID,&pos))continue;
   if(!requested&&Item[item].GetGenre()==item_equip&&BotEquipmentMismatch(p,Item[item])&&!BotMarketWorthy(Item[item])&&!BotStoryWantedItem(p,Item[item]))continue;
   int kind=BotRecoveryKind(Item[item]);if(urgent){bool life=npc.m_CurrentLifeMax>0&&(__int64)npc.m_CurrentLife*100<(__int64)npc.m_CurrentLifeMax*50;bool mana=npc.m_CurrentManaMax>0&&(__int64)npc.m_CurrentMana*100<(__int64)npc.m_CurrentManaMax*20;if((kind!=1||!life)&&(kind!=2||!mana))continue;if((__int64)npc.m_CurrentLife*100<(__int64)npc.m_CurrentLifeMax*25&&distance>160)continue;}
   if(kind&&!urgent&&!requested&&(kind==1?hpStock:mpStock)>=(kind==1?20:10))continue;
   if(kind)priority=urgent?1500:700;else if(BotLearnable(p,Item[item]))priority=800;else if(BotStoryWantedItem(p,Item[item]))priority=1000;else if(Item[item].GetGenre()==item_equip){priority=BotImportantEquipment(p,item)?1200:300;for(int part=0;part<itempart_num;++part)if(p.m_ItemList.CanEquip(item,part)){if(priority<600)priority=600;break;}}else if(requested)priority=200;else continue;
   int units=Item[item].GetInventoryCount();if(units<1)units=1;
   if(!PTBotCarryCanPick(carry,capacity,Item[item].GetWeight(),units,requested||urgent||BotStoryWantedItem(p,Item[item])||BotImportantEquipment(p,item)||BotLearnable(p,Item[item])||(Item[item].GetGenre()==item_equip&&BotEquipmentUseful(p,item))))continue;
  }else if(obj.m_nKind!=Obj_Kind_Money||urgent)continue;
  int score=priority-distance;if(score>best){selected=n;best=score;}
 }
 if(!selected){goal[slot]=0;return false;}KObj& obj=Object[selected];DWORD identity=obj.m_nID;if(goal[slot]!=identity){goal[slot]=identity;started[slot]=now;}
 if(now-started[slot]>10000){bad[slot]=identity;badUntil[slot]=now+20000;goal[slot]=0;t.Path.clear();return false;}
 int x=0,y=0;obj.GetMpsPos(&x,&y);if((px-x)*(px-x)+(py-y)*(py-y)>PLAYER_PICKUP_SERVER_DISTANCE){
  // Do not abandon a living nearby attacker for distant loot.
  if(!urgent&&t.Target>0&&Monster(p.m_nIndex,t.Target)&&NpcSet.GetDistance(p.m_nIndex,t.Target)<160)return false;
  lstrcpyA(r.State,"looting");MoveTraining(r,t,x,y,now,32);return true;
 }
 PHONGTHAN_ITEM_PICKUP_REQUEST request;ZeroMemory(&request,sizeof(request));PhongThanInitializeWireHeader(&request.Header,PHONGTHAN_MSG_INVENTORY_PICKUP_REQUEST,sizeof(request),PHONGTHAN_WIRE_FLAG_REQUEST,0);request.WorldObjectId=identity;
 if(obj.m_nKind==Obj_Kind_Item){ItemPos pos;if(!p.m_ItemList.FindPickupPosition(obj.m_nItemDataID,&pos))return false;request.DestinationContainer=pos.nPlace;request.DestinationX=pos.nX;request.DestinationY=pos.nY;}
 if(p.ServerPickUpItem((BYTE*)&request)){char message[100];sprintf(message,"pickup_confirmed object=%lu container=%d",identity,request.DestinationContainer);Log("LOOT",r.Config.Id,message);t.Path.clear();goal[slot]=0;return true;}
 bad[slot]=identity;badUntil[slot]=now+10000;return false;
}
