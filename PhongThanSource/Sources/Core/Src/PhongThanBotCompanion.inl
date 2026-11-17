// Commands are session-bound and executed on the world thread, never by an LLM.
struct CompanionState {int Owner,Mode,Pending,PurchaseBefore,Target,TargetMap,TargetX,TargetY,PreviousCamp,LastLife,TargetExplicit;DWORD OwnerId,Until,GiftUntil,LastNotice,Started,TargetEntity,RetreatUntil;char Peer[160],ItemName[256],TargetName[256];CompanionState():Owner(0),Mode(0),Pending(0),PurchaseBefore(0),Target(0),TargetMap(0),TargetX(0),TargetY(0),PreviousCamp(0),LastLife(0),TargetExplicit(0),OwnerId(0),Until(0),GiftUntil(0),LastNotice(0),Started(0),TargetEntity(0),RetreatUntil(0){Peer[0]=ItemName[0]=TargetName[0]=0;}};
static CompanionState companions[Capacity];
static void AppendCompanionStatus(FILE* file,Runtime& r){CompanionState& c=companions[&r-bots];fprintf(file,",\"Command\":%d,\"CommandOwner\":%d,\"PendingCommand\":%d",c.Mode,c.Owner,c.Pending);}

static void CompanionTell(Runtime& r,int to,const char* answer){
 if(to<=0||to>=MAX_PLAYER||!Player[to].m_PlayerDBLoad||Player[to].m_nNetConnectIdx<0)return;
 char packet[sizeof(tagExtendProtoHeader)+sizeof(CHAT_SOMEONECHAT_SYNC)+255];ZeroMemory(packet,sizeof(packet));int len=strlen(answer);if(len>255)len=255;int total=sizeof(tagExtendProtoHeader)+sizeof(CHAT_SOMEONECHAT_SYNC)+len;
 tagExtendProtoHeader* h=(tagExtendProtoHeader*)packet;h->ProtocolType=s2c_extendchat;h->wLength=total-1;CHAT_SOMEONECHAT_SYNC* msg=(CHAT_SOMEONECHAT_SYNC*)(h+1);msg->ProtocolType=chat_someonechat;msg->wSize=sizeof(*msg)+len-1;msg->sentlen=(BYTE)len;lstrcpynA(msg->someone,r.Config.Name,sizeof(msg->someone));memcpy(msg+1,answer,len);g_pServer->PackDataToClient(Player[to].m_nNetConnectIdx,packet,total);
}
static void CompanionEvent(Runtime& r,const char* kind,const char* detail){
 CompanionState& c=companions[&r-bots];if(!c.Peer[0])return;CreateDirectoryA("BotAIgent\\chat",0);CreateDirectoryA("BotAIgent\\chat\\in",0);static DWORD serial=0;char path[180],data[1400];sprintf(path,"BotAIgent\\chat\\in\\%08lx%08lx.experience",GetTickCount(),++serial);int n=sprintf(data,"PT_BOT_EXPERIENCE\t1\t%lu\t%s\t%s\t%s\t",(unsigned long)time(0),r.Config.Id,c.Peer,kind);for(int i=0;detail[i]&&i<255;++i)n+=sprintf(data+n,"%02x",(unsigned char)detail[i]);data[n++]='\n';Atomic(path,data,n);Log("COMMAND",r.Config.Id,kind);
}
static bool CompanionOwns(Runtime& r,int player){CompanionState& c=companions[&r-bots];std::string key;return c.Owner==player&&c.OwnerId==Player[player].m_dwID&&PlayerKey(player,key)&&key==c.Peer;}
#include "PhongThanBotCompanionResume.inl"
static void ResetCompanion(Runtime& r){CompanionState& c=companions[&r-bots];if(c.PreviousCamp&&r.Player)PhongThanBotRestorePk(r.Player,c.PreviousCamp);c=CompanionState();companionResume[&r-bots]=CompanionState();Training& t=training[&r-bots];t.Path.clear();t.Target=0;t.Identity=0;t.QuestTarget=t.QuestMap=0;r.GoalMap=r.GoalLevel=0;r.GoalEntity=0;}
static bool CompanionShopping(Runtime& r){return companions[&r-bots].Mode==PTBC_SHOP;}
static bool CompanionWants(Runtime& r,KItem& item){CompanionState& c=companions[&r-bots];const char* name=item.GetName();if(name[0]=='#')++name;return c.Mode==PTBC_SHOP&&c.ItemName[0]&&name[0]&&!strcmp(c.ItemName,name);}
static void CompanionNotice(Runtime& r,const char* kind,const char* text,DWORD now){
 struct Gate {DWORD job,kinds[12];int used;};static Gate gates[Capacity];int slot=&r-bots;CompanionState& c=companions[slot];Gate& gate=gates[slot];
 if(gate.job!=c.Started){ZeroMemory(&gate,sizeof(gate));gate.job=c.Started;}
 DWORD key=Hash(kind,strlen(kind));for(int i=0;i<gate.used;++i)if(gate.kinds[i]==key)return;
 if(now-c.LastNotice<10000)return;if(gate.used<12)gate.kinds[gate.used++]=key;else return;
 c.LastNotice=now;CompanionTell(r,c.Owner,text);CompanionEvent(r,kind,text);
}
static int CompanionItemCount(Runtime& r,const char* name){int count=0;KPlayer& p=Player[r.Player];for(PlayerItem* owned=p.m_ItemList.GetFirstItem();owned;owned=p.m_ItemList.GetNextItem())if(BotBag(owned->nPlace)&&owned->nIdx>0&&owned->nIdx<MAX_ITEM){const char* n=Item[owned->nIdx].GetName();if(n[0]=='#')++n;if(!strcmp(n,name)){int units=Item[owned->nIdx].GetInventoryCount();count+=units>0?units:1;}}return count;}
static bool CompanionPurchaseOwned(Runtime& r){CompanionState& c=companions[&r-bots];return c.ItemName[0]&&CompanionItemCount(r,c.ItemName)>c.PurchaseBefore;}
struct CompanionTrainScope {Runtime& bot;int previous;CompanionTrainScope(Runtime& r):bot(r),previous(r.Config.Train){bot.Config.Train=1;}~CompanionTrainScope(){bot.Config.Train=previous;}};
#include "PhongThanBotCombatOrder.inl"
#include "PhongThanBotMarket.inl"
static bool CompanionTick(Runtime& r,Training& t,DWORD now){
 CompanionState& c=companions[&r-bots];if(!c.Mode)return false;
 if(c.PreviousCamp&&!PTBotCombatCommand(c.Mode)&&PhongThanBotRestorePk(r.Player,c.PreviousCamp))c.PreviousCamp=0;
 if(!CompanionOwns(r,c.Owner)||Player[c.Owner].m_nNetConnectIdx<0){CompanionEvent(r,"owner_left","companion command cancelled: owner disconnected, changed character or died");ResetCompanion(r);return false;}
 CompanionTrainScope trainScope(r);KPlayer& p=Player[r.Player];KNpc& npc=Npc[p.m_nIndex];int leader=Player[c.Owner].m_nIndex;
 // Human-directed follow/combat persists until that live owner changes the
 // order or disconnects. Trade and gift consent keep their own bounded timers.
 if(c.Mode==PTBC_GIFT&&(int)(now-c.GiftUntil)>0&&!p.CheckTrading()){CompanionResumeGift(r,now);CompanionEvent(r,"gift_wait_expired","supply wait expired; previous job restored only for same live owner");if(!c.Mode)return false;}
 if(c.Mode==PTBC_WAIT||c.Mode==PTBC_GIFT){t.Path.clear();t.Target=0;t.Identity=0;r.GoalEntity=0;if(Recover(r,t,now))return true;if(npc.m_Doing!=do_stand)npc.SendCommand(do_stand);lstrcpyA(r.State,c.Mode==PTBC_GIFT?"awaiting_gift":"companion_wait");return true;}
 if(BotMarketTick(r,t,now))return true;
 if(!PTBotCombatCommand(c.Mode)&&Recover(r,t,now))return true;
 if(c.Mode==PTBC_SHOP){
  if(CompanionPurchaseOwned(r)){CompanionNotice(r,"purchase_confirmed","\115\165\141\040\256\255\356\143\040\155\343\156\040\253\156\147\040\156\150\352\040\162\345\151\054\040\164\165\151\040\161\165\141\171\040\154\271\151\040\256\251\171\056",now);c.Mode=PTBC_FOLLOW;c.ItemName[0]=0;}
  else if(BotShopPlan(r,t,now))return true;
  else{CompanionNotice(r,"purchase_blocked",p.m_ItemList.GetEquipmentMoney()<=0?"\124\165\151\040\150\325\164\040\164\151\322\156\040\162\345\151\056\040\244\156\147\040\147\151\141\157\040\144\336\143\150\040\256\255\141\040\164\151\322\156\040\147\151\363\160\040\164\165\151\040\156\150\320\056":"\103\150\255\141\040\155\165\141\040\256\255\356\143\072\040\143\307\156\040\256\363\156\147\040\164\252\156\040\155\343\156\054\040\163\150\157\160\040\143\343\040\142\270\156\040\166\265\040\256\361\040\143\150\347\040\164\162\157\156\147\040\164\363\151\056",now);lstrcpyA(r.State,"purchase_blocked");t.Path.clear();if(npc.m_Doing!=do_stand)npc.SendCommand(do_stand);return true;}
 }
 if(CompanionCombatOrder(r,t,now))return true;
 int map=SubWorld[npc.m_SubWorldIndex].m_SubWorldID,dest=SubWorld[Npc[leader].m_SubWorldIndex].m_SubWorldID;
 if(map!=dest){int edge=-1;if(BotRoute(r,map,dest,&edge)<1||edge<0){CompanionNotice(r,"follow_blocked","\124\165\151\040\143\150\255\141\040\164\327\155\040\256\255\356\143\040\256\255\352\156\147\040\161\165\141\040\256\343\056\040\244\156\147\040\143\150\352\040\164\165\151\040\143\150\363\164\056",now);t.Path.clear();lstrcpyA(r.State,"path_blocked");return true;}lstrcpyA(r.State,"party_follow");MoveTraining(r,t,PTBotEdges[edge].X,PTBotEdges[edge].Y,now);return true;}
 int lx=0,ly=0;Npc[leader].GetMpsPos(&lx,&ly);int distance=NpcSet.GetDistance(p.m_nIndex,leader);
 if(distance>(c.Mode==PTBC_FOLLOW?160:320)){t.Target=0;t.Identity=0;lstrcpyA(r.State,"party_follow");MoveTraining(r,t,lx,ly,now,128);return true;}
 if(c.Mode==PTBC_LOOT){if(BotLoot(r,t,now,false,true))return true;CompanionNotice(r,"loot_no_eligible_item","\103\150\255\141\040\143\343\040\256\345\040\156\150\306\164\040\256\255\356\143\040\147\307\156\040\256\251\171\054\040\150\157\306\143\040\164\363\151\040\256\307\171\054\040\256\345\040\256\141\156\147\040\164\150\165\351\143\040\156\147\255\352\151\040\153\150\270\143\056",now);}
 t.Target=0;t.Identity=0;t.Path.clear();r.GoalEntity=0;if(npc.m_Doing!=do_stand)npc.SendCommand(do_stand);lstrcpyA(r.State,"companion_wait");return true;
}
