// Included inside PtStudioQuest, after identity and inventory helpers.
namespace SealOps {
static Seal* Find(int id){for(unsigned i=0;i<seals.size();++i)if(seals[i].id==id)return &seals[i];return 0;}
static int BuffId(int id){return PT_QUEST_SEAL_FIRST_BUFF+id-1;}
static bool Bag(int place){return place==pos_equiproom||place==pos_equiproomex||place==pos_ptbag2||place==pos_ptbag3;}
static bool Has(int p,int id){
 if(!id)return true;if(!Live(p))return false;Seal* s=Find(id);if(!s||!s->enabled)return false;DWORD now=KSG_GetCurSec();
 if(s->kind==0){PHONGTHAN_IBBUFF_STORE* store=GetPlayerIBBuffStore(p);int at=FindIBBuffSlot(store,BuffId(id));return at>=0&&store->Entry[at].nTimes>0&&store->Entry[at].dwExpireTime>now;}
 for(PlayerItem* owned=Player[p].m_ItemList.GetFirstItem();owned;owned=Player[p].m_ItemList.GetNextItem()){
  int i=owned->nIdx;if(Bag(owned->nPlace)&&i>0&&i<MAX_ITEM&&::Item[i].GetID()&&::Item[i].GetParam()==PhongThanQuestSealParam(id)&&PhongThanQuestItemMatches(::Item[i],s->item.item)&&::Item[i].GetExpireTime()>(int)now&&::Item[i].GetStackNum()>0&&!::Item[i].GetLock()->IsLock())return true;
 }return false;
}
static bool TaskRoom(int p,int extra){int used=0;for(int i=0;i<MAX_TASK;++i)if(Player[p].m_cTask.GetSaveStr(i)[0])++used;return used+extra<=254;}
static int Remaining(int p,int id){
 if(!Has(p,id)||!id)return 0;Seal* s=Find(id);DWORD now=KSG_GetCurSec();
 if(s->kind==0){PHONGTHAN_IBBUFF_STORE* store=GetPlayerIBBuffStore(p);int at=FindIBBuffSlot(store,BuffId(id));return at>=0?(int)(store->Entry[at].dwExpireTime-now):0;}
 int best=0;for(PlayerItem* owned=Player[p].m_ItemList.GetFirstItem();owned;owned=Player[p].m_ItemList.GetNextItem()){int i=owned->nIdx;if(Bag(owned->nPlace)&&i>0&&i<MAX_ITEM&&::Item[i].GetID()&&::Item[i].GetParam()==PhongThanQuestSealParam(id)&&PhongThanQuestItemMatches(::Item[i],s->item.item)){int left=::Item[i].GetExpireTime()-(int)now;if(left>best)best=left;}}return best;
}
static int Grant(int p,int id,int task,int expected,int next){
 if(!Live(p)||Player[p].m_cTask.GetSaveVal(task)!=expected||Player[p].GetLockState()||Player[p].CheckTrading())return 0;
 if(!id||Has(p,id)){Player[p].m_cTask.SetSaveVal(task,next,TRUE);return 1;}
 Seal* s=Find(id);if(!s||!s->enabled)return 0;
 if(s->kind==0){
  if(!TaskRoom(p,Player[p].m_cTask.GetSaveStr(task)[0]?2:3))return 0;
  PHONGTHAN_IBBUFF_STORE* store=GetPlayerIBBuffStore(p);if(!store)return 0;PHONGTHAN_IBBUFF_STORE old=*store;
  if(!AddIBBuffToStore(store,Player[p].m_nIndex,BuffId(id),s->seconds,0))return 0;
  int slot=FindIBBuffSlot(store,BuffId(id));if(slot<0){*store=old;return 0;}store->Entry[slot].nTimes=1;
  Player[p].m_cTask.SetSaveVal(task,next,TRUE);PersistPlayerIBBuffStore(p);return 1;
 }
 int levels[MAX_ITEM_MAGICLEVEL];ZeroMemory(levels,sizeof(levels));PhongThanQuestItemTuple& t=s->item.item;
 if(KSG_GetCurSec()>2147483647UL-(DWORD)s->seconds)return 0;
 int prototype=ItemSet.Add(t.genre,t.series,t.level,t.luck,t.detail,t.particular,levels,g_SubWorldSet.GetGameVersion());if(prototype<=0)return 0;
 ::Item[prototype].SetParam(PhongThanQuestSealParam(id));::Item[prototype].SetExpireTime((int)KSG_GetCurSec()+s->seconds);::Item[prototype].SetLockTrade(TRUE);::Item[prototype].SetLockSell(TRUE);::Item[prototype].SetLockDrop(TRUE);
 PhongThanQuestExchangeRow reward=s->item;reward.count=1;
 int result=PhongThanQuestExchange(p,task,expected,next,0,0,&reward,1,false,&::Item[prototype],true,0,true);
 ItemSet.Remove(prototype);return result;
}
static int Exchange(int p,int id,bool consume,int task,int expected,int next,PhongThanQuestExchangeRow* needs,int count,PhongThanQuestExchangeRow* rewards,int rewardCount,bool stack=true,KItem* prototype=0){
 if(count<0||count>16||!Has(p,id))return 0;PhongThanQuestExchangeRow input[16];int params[16];ZeroMemory(params,sizeof(params));for(int n=0;n<count;++n)input[n]=needs[n];
 Seal* seal=id?Find(id):0;if(seal&&consume&&seal->kind==1){if(count==16)return 0;input[count]=seal->item;input[count].count=1;params[count]=PhongThanQuestSealParam(id);++count;}
 int result=PhongThanQuestExchange(p,task,expected,next,input,count,rewards,rewardCount,stack,prototype,true,0,true,params);
 if(result&&seal&&consume&&seal->kind==0){RemoveIBBuffFromStore(GetPlayerIBBuffStore(p),Player[p].m_nIndex,BuffId(id));PersistPlayerIBBuffStore(p);}return result;
}
}
