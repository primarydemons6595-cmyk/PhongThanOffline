#include "../../../Headers/PhongThanAuction.h"
#ifndef _SERVER
static PTAuctionPage ptAuctionPage;static PTAuctionCommand ptAuctionLast;
static DWORD ptAuctionSequence=0,ptAuctionReceived=0;
int PhongThanAuctionAction(const PTAuctionCommand* source){
 int a=Player[CLIENT_PLAYER_INDEX].m_nIndex;
 if(!source||!g_pClient||a<=0||a>=MAX_NPC||!PTAuctionValidCommand(*source))return 0;
 PTAuctionCommand q=*source;PhongThanInitializeWireHeader(&q.Header,PT_AUCTION_QUERY,sizeof(q),PHONGTHAN_WIRE_FLAG_REQUEST,++ptAuctionSequence);
 q.ActorId=Npc[a].m_dwID;q.MapId=SubWorld[0].m_SubWorldID;ptAuctionLast=q;ptAuctionReceived=0;g_pClient->SendPackToServer(&q,sizeof(q));return 1;
}
int PhongThanAuctionGet(PTAuctionPage* out){
 int a=Player[CLIENT_PLAYER_INDEX].m_nIndex;
 if(!out||a<=0||a>=MAX_NPC||!ptAuctionReceived||ptAuctionPage.ActorId!=Npc[a].m_dwID||ptAuctionPage.MapId!=(DWORD)SubWorld[0].m_SubWorldID)return 0;
 *out=ptAuctionPage;return 1;
}
static void PhongThanAuctionReceive(const PTAuctionPage& page){
 int a=Player[CLIENT_PLAYER_INDEX].m_nIndex;
 if(a>0&&a<MAX_NPC&&page.ActorId==Npc[a].m_dwID&&page.MapId==(DWORD)SubWorld[0].m_SubWorldID&&page.Header.Sequence==ptAuctionSequence&&PTAuctionValidPage(page)){
  ptAuctionPage=page;ptAuctionReceived=GetTickCount();}
}
#else
#include "PhongThanAuctionStore.h"
static PTAuctionStore ptAuctionStore;
static const char* ptAuctionFile="CharacterStore\\auction.dat";
enum {PT_AUCTION_RECEIPT=TASKVALUE_PT_AUCTION_RECEIPT};
static PTAuctionIdentity PTAuctionOwner(int p){PTAuctionIdentity id;memset(&id,0,sizeof(id));lstrcpyn(id.Account,Player[p].AccountName,32);lstrcpyn(id.Role,Player[p].Name,32);return id;}
// 1: character save confirms the debit/claim. -1: a new fully loaded session
// confirms it was never saved. Offline absence alone is NOT proof of rollback.
static int PTAuctionReceipt(const PTAuctionIdentity& owner,unsigned int receipt){
 int saved=0;if(PtPsSavedReceipt(owner.Account,owner.Role,PT_AUCTION_RECEIPT,saved)&&(unsigned int)saved>=receipt)return 1;
 int p=PtPsOnline(owner.Role);
 if(p&&!strcmp(Player[p].AccountName,owner.Account)&&!Player[p].m_pStatusLoadPlayerInfo&&Player[p].m_uMustSave!=SAVE_DOING&&
  (unsigned int)Player[p].m_cTask.GetSaveVal(PT_AUCTION_RECEIPT)<receipt)return -1;
 return 0;
}
static bool PTAuctionFinish(PTAuctionLot& l){
 if(ptAuctionStore.FreeClaims()<2||ptAuctionStore.Book.NextId>=1999999995)return false;
 bool ok;
 if(l.Kind)ok=ptAuctionStore.Deliver(l.Owner,l.Price,0);
 else if(l.Bidder.Role[0])ok=ptAuctionStore.Deliver(l.Owner,l.Price,0)&&ptAuctionStore.Deliver(l.Bidder,0,&l.Item);
 else ok=ptAuctionStore.Deliver(l.Owner,0,&l.Item);
 if(ok){l.State=PT_AL_FINISHED;++l.Revision;}return ok;
}
static bool PTAuctionSettle(){
 if(!ptAuctionStore.Load(ptAuctionFile))return false;
 PTAuctionBook* before=new PTAuctionBook;*before=ptAuctionStore.Book;bool changed=false;
 for(int i=0;i<PT_AUCTION_LOTS;++i){PTAuctionLot& l=ptAuctionStore.Book.Lots[i];
  if(l.State==PT_AL_POSTING||l.State==PT_AL_BIDDING){int proof=PTAuctionReceipt(l.State==PT_AL_POSTING?l.Owner:l.Pending,l.Receipt);
   if(!proof)continue;
   if(proof<0){l.State=l.State==PT_AL_POSTING?PT_AL_ABORTED:PT_AL_LIVE;++l.Revision;changed=true;continue;}
   if(l.State==PT_AL_POSTING){l.State=PT_AL_LIVE;++l.Revision;changed=true;}
   else if(ptAuctionStore.FreeClaims()>=3&&ptAuctionStore.Book.NextId<1999999995){
    if(l.PendingOperation==PT_AUCTION_SUPPLY){
     if(!ptAuctionStore.Deliver(l.Owner,0,&l.PendingItem)||!ptAuctionStore.Deliver(l.Pending,l.Price,0))break;
     l.State=PT_AL_FINISHED;
    }else{
     if(l.Bidder.Role[0]&&!ptAuctionStore.Deliver(l.Bidder,l.Price,0))break;
     l.Bidder=l.Pending;l.Price=l.PendingPrice;l.State=PT_AL_LIVE;
     if(l.PendingOperation==PT_AUCTION_BUY){if(!PTAuctionFinish(l))break;}
    }++l.Revision;changed=true;
   }
  }
  if(l.State==PT_AL_LIVE&&l.Expires<=(unsigned int)KSG_GetCurSec()&&PTAuctionFinish(l))changed=true;
 }
 for(int c=0;c<PT_AUCTION_CLAIMS;++c){PTAuctionDelivery& d=ptAuctionStore.Book.Claims[c];if(d.State!=2)continue;
  int proof=PTAuctionReceipt(d.Owner,d.Receipt);if(proof){d.State=proof>0?3:1;changed=true;}}
 bool ok=!changed||ptAuctionStore.Save(ptAuctionFile);if(!ok)ptAuctionStore.Book=*before;delete before;return ok;
}
static bool PTAuctionSaveReceipt(int p,unsigned int receipt){
 int previous=Player[p].m_cTask.GetSaveVal(PT_AUCTION_RECEIPT);unsigned int save=Player[p].m_uMustSave;
 Player[p].m_cTask.SetSaveVal(PT_AUCTION_RECEIPT,receipt,TRUE);Player[p].m_uMustSave=SAVE_REQUEST;
 if(Player[p].Save())return true;
 Player[p].m_cTask.SetSaveVal(PT_AUCTION_RECEIPT,previous,TRUE);Player[p].m_uMustSave=save;return false;
}
static int PTAuctionInventory(int p,unsigned int serial){
 int index=Player[p].m_ItemList.SearchID(serial);if(index<=0||index>=MAX_ITEM)return 0;
 int own=Player[p].m_ItemList.FindSame(index);if(own<=0||own>=MAX_PLAYER_ITEM)return 0;
 int place=Player[p].m_ItemList.m_Items[own].nPlace;
 if(place!=pos_equiproom&&place!=pos_equiproomex&&place!=pos_ptbag2&&place!=pos_ptbag3)return 0;
 if(Item[index].GetLockTrade()||Item[index].GetLock()->IsLock()||Item[index].GetExpireTime()&&Item[index].GetExpireTime()<=KSG_GetCurSec())return 0;
 return index;
}
static int PTAuctionMutate(int p,const PTAuctionCommand& q){
 PTAuctionIdentity owner=PTAuctionOwner(p);
 if(Player[p].m_pStatusLoadPlayerInfo||Player[p].CheckTrading()||Player[p].GetLockState()||Player[p].m_uMustSave==SAVE_DOING||ptAuctionStore.Pending(owner))return -2;
 if(q.Operation==PT_AUCTION_POST||q.Operation==PT_AUCTION_ORDER){
  int slot=ptAuctionStore.FreeLot(),item=PTAuctionInventory(p,q.Serial);if(slot<0||!item||q.Price<1||q.Hours<1||q.Hours>72||q.Buyout&&q.Buyout<q.Price)return -3;
  if(q.Operation==PT_AUCTION_ORDER&&(Item[item].GetGenre()==0||q.Count<1||q.Count>Item[item].GetMaxStackNum()||q.Buyout))return -3;
  int money=q.Operation==PT_AUCTION_ORDER?q.Price:0;if(Player[p].m_ItemList.GetEquipmentMoney()<money)return -4;
  PTAuctionLot& l=ptAuctionStore.Book.Lots[slot];memset(&l,0,sizeof(l));l.Id=ptAuctionStore.Next();if(!l.Id)return -5;
  l.Revision=1;l.State=PT_AL_POSTING;l.Kind=q.Operation==PT_AUCTION_ORDER;l.Owner=owner;l.Receipt=l.Id;
  l.Price=q.Price;l.Buyout=q.Buyout;l.Expires=KSG_GetCurSec()+q.Hours*3600;PtPsCaptureItem(item,l.Item);
  if(l.Kind)l.Item.Stack=q.Count;
  if(!ptAuctionStore.Save(ptAuctionFile))return -5;
  int own=Player[p].m_ItemList.FindSame(item);PlayerItem old=Player[p].m_ItemList.m_Items[own];
  bool applied=l.Kind?Player[p].Pay(money)!=0:Player[p].m_ItemList.Remove(item)!=0;
  if(!applied)return -5;
  if(!PTAuctionSaveReceipt(p,l.Receipt)){
   if(l.Kind)Player[p].Earn(money);else Player[p].m_ItemList.Add(item,old.nPlace,old.nX,old.nY,false);return -5;}
  if(!l.Kind)ItemSet.Remove(item);return 2;
 }
 if(q.Operation==PT_AUCTION_CLAIM){
  int n=ptAuctionStore.Claim(q.Id);if(n<0)return -3;PTAuctionDelivery& d=ptAuctionStore.Book.Claims[n];
  if(d.State!=1||!PTAuctionSame(d.Owner,owner)||Player[p].m_ItemList.GetEquipmentMoney()>2000000000-d.Money)return -3;
  int item=0;ItemPos pos;if(d.HasItem&&(!d.Item.Expire||d.Item.Expire>KSG_GetCurSec())){item=PtPsCreateItem(d.Item);if(!item)return -6;
   if(Item[item].GetStackNum()!=d.Item.Stack){ItemSet.Remove(item);return -6;}
   if(!Player[p].m_ItemList.SearchPosition(Item[item].GetWidth(),Item[item].GetHeight(),&pos,true)){ItemSet.Remove(item);return -6;}}
  d.State=2;d.Receipt=ptAuctionStore.Next();if(!d.Receipt||!ptAuctionStore.Save(ptAuctionFile)){if(item)ItemSet.Remove(item);return -5;}
  bool earned=false,added=false;if(d.Money){earned=Player[p].Earn(d.Money)!=0;if(!earned){if(item)ItemSet.Remove(item);return -5;}}
  if(item){added=Player[p].m_ItemList.Add(item,pos.nPlace,pos.nX,pos.nY,false)!=0;if(!added){if(earned)Player[p].Pay(d.Money);ItemSet.Remove(item);return -5;}}
  if(!PTAuctionSaveReceipt(p,d.Receipt)){if(added){Player[p].m_ItemList.Remove(item);ItemSet.Remove(item);}if(earned)Player[p].Pay(d.Money);return -5;}return 2;
 }
 int n=ptAuctionStore.Lot(q.Id);if(n<0)return -3;PTAuctionLot& l=ptAuctionStore.Book.Lots[n];
 if(l.State!=PT_AL_LIVE||l.Revision!=q.Revision||l.Expires<=(unsigned int)KSG_GetCurSec())return -7;
 if(q.Operation==PT_AUCTION_CANCEL){
  if(!PTAuctionSame(l.Owner,owner)||l.Bidder.Role[0])return -3;
  if(!PTAuctionFinish(l)||!ptAuctionStore.Save(ptAuctionFile))return -5;return 1;
 }
 if(PTAuctionSame(l.Owner,owner)||PTAuctionSame(l.Bidder,owner)||ptAuctionStore.FreeClaims()<3)return -3;
 int money=0,item=0,originalStack=0;PlayerItem old;
 if(q.Operation==PT_AUCTION_SUPPLY){
  if(!l.Kind)return -3;item=PTAuctionInventory(p,q.Serial);if(!item)return -3;
  KItem& it=Item[item];if(it.GetGenre()!=l.Item.Genre||it.GetDetailType()!=l.Item.Detail||it.GetParticularType()!=l.Item.Particular||it.GetLevel()!=l.Item.Level||it.GetSeries()!=l.Item.Series||it.GetRow()!=l.Item.Row||it.GetStackNum()<l.Item.Stack)return -3;
  PtPsCaptureItem(item,l.PendingItem);l.PendingItem.Stack=l.Item.Stack;originalStack=it.GetStackNum();old=Player[p].m_ItemList.m_Items[Player[p].m_ItemList.FindSame(item)];
 }else{
  if(l.Kind||q.Operation!=PT_AUCTION_BID&&q.Operation!=PT_AUCTION_BUY)return -3;
  money=q.Operation==PT_AUCTION_BUY?l.Buyout:q.Price;
  if(money<1||money<l.Price||l.Bidder.Role[0]&&money<=l.Price||q.Operation==PT_AUCTION_BID&&l.Buyout&&money>=l.Buyout)return -3;
  if(Player[p].m_ItemList.GetEquipmentMoney()<money)return -4;
 }
 l.State=PT_AL_BIDDING;l.Pending=owner;l.PendingOperation=q.Operation;l.PendingPrice=money;l.Receipt=ptAuctionStore.Next();
 if(!l.Receipt||!ptAuctionStore.Save(ptAuctionFile))return -5;
 bool applied=money?Player[p].Pay(money)!=0:originalStack==l.Item.Stack?Player[p].m_ItemList.Remove(item)!=0:Item[item].SetStackNum(originalStack-l.Item.Stack)==0;
 if(!applied)return -5;
 if(!PTAuctionSaveReceipt(p,l.Receipt)){
  if(money)Player[p].Earn(money);else if(originalStack==l.Item.Stack)Player[p].m_ItemList.Add(item,old.nPlace,old.nX,old.nY,false);else Item[item].SetStackNum(originalStack);return -5;}
 if(item&&originalStack==l.Item.Stack)ItemSet.Remove(item);
 else if(item)Player[p].m_ItemList.SyncItem(item);
 return 2;
}
static int PTAuctionCategory(const PT_PS_ITEM& item){
 if(item.Genre==item_equip){if(item.Detail==equip_meleeweapon||item.Detail==equip_rangeweapon)return 3;if(item.Detail==equip_amulet)return 2;if(item.Detail==equip_horse)return 5;return 4;}
 if(item.Genre==item_materials||item.Genre==item_task)return 1;
 if(item.Genre==item_skillbook)return 6;if(item.Genre==item_ibitem)return 8;if(item.Genre==item_medicine)return 9;return 10;
}
static void PhongThanAuctionRequest(int p,const PTAuctionCommand& q){
 if(p<=0||p>=MAX_PLAYER||!Player[p].m_PlayerDBLoad||!q.Header.Sequence||!PTAuctionValidCommand(q))return;
 int a=Player[p].m_nIndex;if(a<=0||a>=MAX_NPC||Npc[a].m_dwID!=q.ActorId||Player[p].m_nNetConnectIdx<0)return;
 int w=Npc[a].m_SubWorldIndex;if(w<0||w>=MAX_SUBWORLD||q.MapId!=(DWORD)SubWorld[w].m_SubWorldID)return;
 static DWORD last[MAX_PLAYER],session[MAX_PLAYER],sequence[MAX_PLAYER];static BYTE tickets[MAX_PLAYER][PHONGTHAN_SESSION_TICKET_SIZE];static PTAuctionPage* replies[MAX_PLAYER];DWORD now=GetTickCount();
 if(session[p]!=Player[p].m_dwID||memcmp(tickets[p],Player[p].m_SessionTicket,sizeof(tickets[p]))){session[p]=Player[p].m_dwID;sequence[p]=0;last[p]=0;memcpy(tickets[p],Player[p].m_SessionTicket,sizeof(tickets[p]));}
 if(q.Header.Sequence==sequence[p]&&replies[p]){g_pServer->PackDataToClient(Player[p].m_nNetConnectIdx,replies[p],sizeof(PTAuctionPage));return;}
 if(q.Header.Sequence<sequence[p])return;
 if(now-last[p]<200)return;last[p]=now;
 PTAuctionPage* out=new PTAuctionPage;memset(out,0,sizeof(*out));out->ActorId=q.ActorId;out->MapId=q.MapId;out->Operation=q.Operation;out->View=q.View;out->Category=q.Category;out->Page=q.Page;
 PhongThanInitializeWireHeader(&out->Header,PT_AUCTION_REPLY,sizeof(*out),PHONGTHAN_WIRE_FLAG_RESPONSE,q.Header.Sequence);
 PTAuctionIdentity owner=PTAuctionOwner(p);
 if(!PTAuctionSettle())out->Result=-5;
 else{
  out->Result=1;if(q.Operation){PTAuctionBook* before=new PTAuctionBook;*before=ptAuctionStore.Book;out->Result=PTAuctionMutate(p,q);
   if(out->Result<0){ptAuctionStore.Book=*before;/* persisted pending writes are settled from receipts on the next request */ptAuctionStore.Ready=false;}
   delete before;}
  out->Pending=ptAuctionStore.Pending(owner)?1:0;
  int total=0;
  for(int i=0;i<(q.View==PT_AUCTION_DELIVERIES?PT_AUCTION_CLAIMS:PT_AUCTION_LOTS);++i){PTAuctionRow row;memset(&row,0,sizeof(row));
   if(q.View==PT_AUCTION_DELIVERIES){const PTAuctionDelivery& d=ptAuctionStore.Book.Claims[i];if(!d.Id||d.State==3||!PTAuctionSame(d.Owner,owner))continue;
    row.Id=d.Id;row.State=d.State;row.Money=d.Money;row.Item=d.Item;lstrcpyn(row.Owner,d.Owner.Role,32);}
   else{const PTAuctionLot& l=ptAuctionStore.Book.Lots[i];if(l.State!=PT_AL_LIVE&&l.State!=PT_AL_BIDDING&&l.State!=PT_AL_POSTING)continue;
    bool own=PTAuctionSame(l.Owner,owner),bid=PTAuctionSame(l.Bidder,owner)||l.State==PT_AL_BIDDING&&PTAuctionSame(l.Pending,owner);
    if(q.View==PT_AUCTION_BROWSE&&(l.Kind||l.State!=PT_AL_LIVE)||q.View==PT_AUCTION_MY_BIDS&&(!bid||l.Kind)||q.View==PT_AUCTION_OWN&&(!own||l.Kind)||q.View==PT_AUCTION_ORDERS&&(!l.Kind||l.State!=PT_AL_LIVE)||q.View==PT_AUCTION_MY_ORDERS&&(!own||!l.Kind))continue;
    if(q.Category&&PTAuctionCategory(l.Item)!=q.Category||q.Search[0]&&!strstr(l.Item.Name,q.Search))continue;
    row.Id=l.Id;row.Revision=l.Revision;row.Expires=l.Expires;row.Kind=l.Kind;row.State=l.State;row.Price=l.Price;row.Buyout=l.Buyout;row.Item=l.Item;lstrcpyn(row.Owner,l.Owner.Role,32);lstrcpyn(row.Bidder,l.Bidder.Role,32);}
   if(total>=q.Page*PT_AUCTION_ROWS&&out->Count<PT_AUCTION_ROWS)out->Rows[out->Count++]=row;++total;
  }out->Total=total;
  for(int c=0;c<PT_AUCTION_CLAIMS;++c)if(ptAuctionStore.Book.Claims[c].Id&&ptAuctionStore.Book.Claims[c].State!=3&&PTAuctionSame(ptAuctionStore.Book.Claims[c].Owner,owner))++out->Deliveries;
 }
 if(!replies[p])replies[p]=new PTAuctionPage;*replies[p]=*out;sequence[p]=q.Header.Sequence;
 g_pServer->PackDataToClient(Player[p].m_nNetConnectIdx,out,sizeof(*out));delete out;
}
#endif
