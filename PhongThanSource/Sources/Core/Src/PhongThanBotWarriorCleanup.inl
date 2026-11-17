// Explicit administrator-requested maintenance, restricted to server bots.
// A per-bot receipt prevents later acquisitions being deleted by this request.
static bool BotUnusableWarriorLoot(KPlayer& player,KItem& item){
 if(item.GetGenre()!=item_equip||player.m_cProfession.GetProfession()==3||item.GetPhongThanQuality()!=4||item.GetLevel()>3||item.GetUpgradeState())return false;
 for(int i=0;i<6;++i){KMagicAttrib* req=(KMagicAttrib*)item.GetRequirement(i);if(req&&(req->nAttribType==magic_requireseries||req->nAttribType==magic_requiremenpai)&&req->nValue[0]==3)return true;}
 return false;
}
static void BotCleanupWarriorLoot(Runtime& r){
 KPlayer& player=Player[r.Player];if(!player.m_bIsBot||!SocialAlive(r.Player)||player.CheckTrading()||player.GetLockState())return;
 const char* path="BotAIgent\\maintenance.ini";int job=GetPrivateProfileIntA("Cleanup","WarriorLoot",0,path);if(job<=0)return;
 if(GetPrivateProfileIntA("Completed",r.Config.Id,0,path)==job)return;
 bool pendingProtected=false;std::vector<int> remove;
 for(PlayerItem* entry=player.m_ItemList.GetFirstItem();entry;entry=player.m_ItemList.GetNextItem()){
  int index=entry->nIdx;if(index<=0||index>=MAX_ITEM||!Item[index].GetID()||BotMarketReserved(r.Player,index))continue;
  if(BotUnusableWarriorLoot(player,Item[index])&&!BotMarketWorthy(Item[index])&&!Item[index].IsTemp()&&!Item[index].GetExpireTime()&&BotStoryRequiredCount(player,Item[index])==0){if(entry->nPlace==pos_equip||entry->nPlace==pos_hand){pendingProtected=true;continue;}remove.push_back(index);}
 }
 if(!GetPrivateProfileIntA("Cleanup","ApplyWarriorLoot",0,path)){
  static int jobs[Capacity];int slot=&r-bots;if(jobs[slot]!=job){jobs[slot]=job;FILE* f=fopen("BotAIgent\\warrior-cleanup-plan.tsv","ab");if(f){for(int at=0;at<(int)remove.size();++at){KItem& item=Item[remove[at]];fprintf(f,"%d\t%s\t%lu\t%d\t%d\t%d\t%d\n",job,r.Config.Id,item.GetID(),item.GetDetailType(),item.GetParticular(),item.GetLevel(),item.GetPhongThanQuality());}fclose(f);}}return;
 }
 if(!remove.empty()&&!player.Save())return;
 for(int i=0;i<(int)remove.size();++i){int index=remove[i];DWORD id=Item[index].GetID();int detail=Item[index].GetDetailType(),particular=Item[index].GetParticular();
  if(player.m_ItemList.Remove(index)){ItemSet.Remove(index);FILE* audit=fopen("BotAIgent\\warrior-cleanup.tsv","ab");if(audit){fprintf(audit,"%lu\t%d\t%s\t%lu\t%d\t%d\n",(unsigned long)time(0),job,r.Config.Id,id,detail,particular);fclose(audit);}}
 }
 if(pendingProtected&&!remove.empty())player.Save();
 if(!pendingProtected&&(remove.empty()||player.Save())){char value[24];sprintf(value,"%d",job);WritePrivateProfileStringA("Completed",r.Config.Id,value,path);r.Error[0]=0;}
}
