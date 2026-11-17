// Travel through an owned item's ordinary, nonce-bound dialog. No ChangeWorld,
// free item creation, overwritten compass slots or direct Lua mutation here.
struct BotTravelJob {DWORD serial,next;int destination,item,option,compass;BotTravelJob():serial(0),next(0),destination(0),item(0),option(0),compass(0){}};
static BotTravelJob botTravelJobs[Capacity];
static int BotTravelOption(KPlayer& p,KItem& item,int destination,int& compass){
 compass=0;int profile=PhongThanTravelProfile(item.GetGenre(),item.GetDetailType(),item.GetParticular());
 static const int tasks[5]={714,837,840,843,846};
 if(profile==12){for(int n=0;n<5;++n){int map=p.m_cTask.GetSaveVal(tasks[n]);if(map>0&&map<100)map+=1000;if(map==destination&&p.m_cTask.GetSaveVal(tasks[n]+1)>0&&p.m_cTask.GetSaveVal(tasks[n]+2)>0){compass=1;return n+1;}}}
 else for(int n=0;n<PT_TRAVEL_DESTINATION_COUNT;++n){const PtTravelDestination& d=ptTravelDestinations[n];if(d.map==destination&&(d.profile==profile||(profile==9&&d.profile>=2&&d.profile<=5)||(profile==10&&d.profile>=6&&d.profile<=7)))return n+1;}
 return 0;
}
static bool BotHasTravelItem(KPlayer& p,int destination){
 for(PlayerItem* owned=p.m_ItemList.GetFirstItem();owned;owned=p.m_ItemList.GetNextItem())if(BotBag(owned->nPlace)&&owned->nIdx>0&&owned->nIdx<MAX_ITEM){KItem& item=Item[owned->nIdx];int compass=0;
  if(!item.GetLock()->nState&&item.GetStackNum()>0&&(!item.GetExpireTime()||item.GetExpireTime()>KSG_GetCurSec())&&BotTravelOption(p,item,destination,compass))return true;
 }return false;
}
static bool BotTryTravel(Runtime& r,int destination,DWORD now){
 KPlayer& p=Player[r.Player];KNpc& actor=Npc[p.m_nIndex];int current=SubWorld[actor.m_SubWorldIndex].m_SubWorldID;if(current==destination)return false;
 if(p.GetLockState()||p.CheckTrading()||p.m_ItemList.Hand()||actor.m_CurrentLife<=0)return false;
 BotTravelJob& job=botTravelJobs[&r-bots];if(job.destination!=destination)job=BotTravelJob();if((int)(now-job.next)<0)return job.serial!=0;
 if(job.serial&&job.item>0&&job.item<MAX_ITEM&&Item[job.item].GetID()==job.serial&&p.m_ItemList.SearchID(job.serial)==job.item){
  if(p.m_UiDialogToken){char callback[40];sprintf(callback,job.compass?"pt_compass_go%d":"pt_travel_%d",job.option);int choice=QuestChoice(p,callback);if(choice<0&&job.compass)choice=QuestChoice(p,"pt_compass_go");if(choice<0&&!job.compass)choice=QuestChoice(p,"pt_travel_page2");
   if(choice>=0){PHONGTHAN_UI_CHOICE_REQUEST select;ZeroMemory(&select,sizeof(select));PhongThanInitializeWireHeader(&select.Header,PHONGTHAN_MSG_UI_CHOICE_REQUEST,sizeof(select),PHONGTHAN_WIRE_FLAG_REQUEST,0);select.MapId=current;select.DialogToken=p.m_UiDialogToken;select.View=p.m_UiDialogView;select.Selection=choice;p.ProcessPlayerSelectFromUI((BYTE*)&select);job.next=now+1500;
    if(SubWorld[actor.m_SubWorldIndex].m_SubWorldID==destination){Log("TRAVEL",r.Config.Id,"owned_travel_item_destination_confirmed");job=BotTravelJob();p.Save();}return true;}
  }job=BotTravelJob();job.next=now+10000;return false;
 }
 job=BotTravelJob();job.destination=destination;
 for(PlayerItem* owned=p.m_ItemList.GetFirstItem();owned;owned=p.m_ItemList.GetNextItem())if(BotBag(owned->nPlace)&&owned->nIdx>0&&owned->nIdx<MAX_ITEM){KItem& item=Item[owned->nIdx];if(item.GetLock()->nState||item.GetStackNum()<1||(item.GetExpireTime()&&item.GetExpireTime()<=KSG_GetCurSec()))continue;int compass=0,option=BotTravelOption(p,item,destination,compass);
  if(!option)continue;job.item=owned->nIdx;job.serial=item.GetID();job.option=option;job.compass=compass;job.next=now+1500;
  // Reserved=1 is auto-dose for recovery consumables, NOT ordinary IB use.
  // Travel must open the item's real dialog, including its second page.
  DWORD previousDialog=p.m_UiDialogToken;PHONGTHAN_ITEM_USE_REQUEST use;ZeroMemory(&use,sizeof(use));PhongThanInitializeWireHeader(&use.Header,PHONGTHAN_MSG_INVENTORY_USE_REQUEST,sizeof(use),PHONGTHAN_WIRE_FLAG_REQUEST,0);use.ItemId=item.GetID();use.Container=owned->nPlace;use.SlotX=owned->nX;use.SlotY=owned->nY;p.EatItem((BYTE*)&use);
  if(!p.m_UiDialogToken||p.m_UiDialogToken==previousDialog){job=BotTravelJob();job.next=now+10000;return false;}training[&r-bots].Path.clear();actor.SendCommand(do_stand);lstrcpyA(r.State,"using_travel_item");return true;
 }job.next=now+10000;return false;
}
