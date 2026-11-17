#ifndef _SERVER
#include "../../../Headers/PhongThanEventShop.h"
#include "PhongThanQuestItemTuple.h"
static PTEventShopData s_EventShop;static bool s_EventShopActive=false,s_EventShopPending=false;static unsigned int s_EventShopDismissed=0;
struct PTEventShopLock {CRITICAL_SECTION lock;PTEventShopLock(){InitializeCriticalSection(&lock);}~PTEventShopLock(){DeleteCriticalSection(&lock);}};
static PTEventShopLock s_EventShopLock;
struct PTEventShopGuard {PTEventShopGuard(){EnterCriticalSection(&s_EventShopLock.lock);}~PTEventShopGuard(){LeaveCriticalSection(&s_EventShopLock.lock);}};
void PhongThanReceiveEventShop(const void* bytes,unsigned int size){
 if(!PTValidateEventShop(bytes,size))return;const PTEventShopData& d=*(const PTEventShopData*)bytes;
 {PTEventShopGuard guard;
 int actor=Player[CLIENT_PLAYER_INDEX].m_nIndex;if(actor<=0||actor>=MAX_NPC||d.ActorId!=Npc[actor].m_dwID||d.Token==s_EventShopDismissed)return;
 if(s_EventShopActive&&s_EventShop.Token==d.Token&&d.Sequence<s_EventShop.Sequence)return;
 memset(&s_EventShop,0,sizeof(s_EventShop));memcpy(&s_EventShop,bytes,size);s_EventShopPending=false;s_EventShopActive=d.Count>0;
 for(unsigned int i=0;i<s_EventShop.Count;++i){PTEventShopRow& row=s_EventShop.Rows[i];PhongThanQuestItemTuple tuple;
  if(!PhongThanDecodeQuestItemTuple(row.Genre,row.Detail,row.Part,row.Level,row.Series,0,tuple))continue;
  int levels[MAX_ITEM_MAGICLEVEL];memset(levels,0,sizeof(levels));int preview=ItemSet.Add(tuple.genre,tuple.series,tuple.level,tuple.luck,tuple.detail,tuple.particular,levels,g_SubWorldSet.GetGameVersion());
  if(preview>0&&preview<MAX_ITEM){lstrcpynA(row.Image,Item[preview].GetImageName(),sizeof(row.Image));ItemSet.Remove(preview);}
 }
 }CoreDataChanged(PT_GDCNI_EVENT_SHOP,0,0);
}
bool PhongThanEventShopGet(PTEventShopData* out){
 PTEventShopGuard guard;
 int actor=Player[CLIENT_PLAYER_INDEX].m_nIndex;if(!s_EventShopActive||!out||actor<=0||actor>=MAX_NPC)return false;
 int world=Npc[actor].m_SubWorldIndex;if(world<0||world>=MAX_SUBWORLD||s_EventShop.MapId!=(unsigned int)SubWorld[world].m_SubWorldID||s_EventShop.ActorId!=Npc[actor].m_dwID){s_EventShopActive=false;return false;}
 int npc=NpcSet.SearchID(s_EventShop.NpcEntity);if(npc<=0||NpcSet.GetDistance(actor,npc)>384){s_EventShopActive=false;return false;}
 *out=s_EventShop;return true;
}
bool PhongThanEventShopTooltip(unsigned int id,PTEventShopTooltip* out){
 PTEventShopGuard guard;PTEventShopData current;if(!out||!PhongThanEventShopGet(&current))return false;
 for(unsigned int i=0;i<current.Count;++i)if(current.Rows[i].Id==id){const PTEventShopRow& row=current.Rows[i];PhongThanQuestItemTuple tuple;if(!PhongThanDecodeQuestItemTuple(row.Genre,row.Detail,row.Part,row.Level,row.Series,0,tuple))return false;
  int levels[MAX_ITEM_MAGICLEVEL];memset(levels,0,sizeof(levels));int preview=ItemSet.Add(tuple.genre,tuple.series,tuple.level,tuple.luck,tuple.detail,tuple.particular,levels,g_SubWorldSet.GetGameVersion());if(preview<=0||preview>=MAX_ITEM)return false;
  KGameObjDesc description;memset(&description,0,sizeof(description));memset(out,0,sizeof(*out));Item[preview].GetDesc((char*)&description);lstrcpynA(out->Title,description.szTitle,sizeof(out->Title));lstrcpynA(out->Properties,description.szProp,sizeof(out->Properties));lstrcpynA(out->Description,description.szDesc,sizeof(out->Description));out->ImageFlags=Item[preview].GetTooltipImageName(out->Image,sizeof(out->Image));ItemSet.Remove(preview);return true;
 }return false;
}
bool PhongThanEventShopPurchase(unsigned int id,unsigned int lots){
 PTEventShopGuard guard;
 PTEventShopData current;if(s_EventShopPending||!g_pClient||!PhongThanEventShopGet(&current))return false;
 bool found=false;for(unsigned int i=0;i<current.Count;++i)if(current.Rows[i].Id==id&&lots&&lots<=PTEventShopMaxLots(current.Rows[i].Count,current.Rows[i].UnitPrice))found=true;if(!found)return false;
 PTEventShopBuy request;memset(&request,0,sizeof(request));PhongThanInitializeWireHeader(&request.Header,PT_EVENT_SHOP_BUY,sizeof(request),PHONGTHAN_WIRE_FLAG_REQUEST,0);
 request.MapId=current.MapId;request.ActorId=current.ActorId;request.EventId=current.EventId;request.Token=current.Token;request.Sequence=current.Sequence;request.GoodsId=id;request.Lots=lots;s_EventShopPending=true;g_pClient->SendPackToServer(&request,sizeof(request));return true;
}
void PhongThanEventShopDismiss(){PTEventShopGuard guard;s_EventShopDismissed=s_EventShop.Token;s_EventShopActive=false;s_EventShopPending=false;}
#endif
