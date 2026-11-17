// Existing event-shop wire and native UI, with a separate server-owned catalog/session.
#include "../../../Headers/PhongThanEventShop.h"
#ifndef PTSQ_SHOP_SEND
#define PTSQ_SHOP_SEND(p,data,size) g_pServer->PackDataToClient(Player[p].m_nNetConnectIdx,&data,size)
#endif
namespace PtStudioQuest {
static ServiceMenu* ServiceMenuById(int id){for(unsigned i=0;i<services.menus.size();++i)if(services.menus[i].id==id)return &services.menus[i];return 0;}
static bool ServiceAccess(int p,const ServiceMenu& m){Binding* b=Bind(m.npc);return Live(p)&&b&&Near(p,*b)&&(m.classes&Class(p))&&Npc[Player[p].m_nIndex].IsAlive()&&!Player[p].GetLockState()&&!Player[p].CheckTrading();}
static int ServiceTrade(int p,int menu,int npc,int offer,int lots){
 error=PTSQ_TRADE_FAILED;if(!Load()||!Live(p))return -1;Operation operation;ServiceMenu* m=ServiceMenuById(menu);if(!m||m->kind!=2||m->npc!=npc||!ServiceAccess(p,*m))return -1;
 for(unsigned i=0;i<services.offers.size();++i){ServiceOffer& o=services.offers[i];if(o.id!=offer||o.menu!=menu||!o.enabled)continue;
  if(!(o.classes&Class(p))||Npc[Player[p].m_nIndex].m_Level<o.minLevel||lots<1||(unsigned)lots>PTEventShopMaxLots(o.reward.count,o.price))return -2;
  int price=o.price*lots;if(!o.point&&price>10000)return -2;int balance=o.point?Player[p].m_cTask.GetSaveVal(5230+o.point-1):0;
  if(o.point&&(balance<price||!ServicePoint(services,o.point)))return -2;
  int token=Player[p].m_cTask.GetSaveVal(5246);if(token<0||token>=2000000000)return -2;
  int extra=Player[p].m_cTask.GetSaveStr(5246)[0]?0:1;if(!ServiceTaskRoom(p,extra))return -2;
  PhongThanQuestExchangeRow reward=o.reward;reward.count*=lots;PhongThanQuestExchangeRow payment=o.payment;payment.count=price;
  if(!PhongThanQuestExchange(p,5246,token,token+1,o.point?0:&payment,o.point?0:1,&reward,1,true,0,true,0,true))return -2;
  if(o.point)Player[p].m_cTask.SetSaveVal(5230+o.point-1,balance-price,TRUE);Player[p].Save();return 2;
 }return -2;
}
struct ServiceSession {DWORD player,actor,entity,token,sequence,created;int menu,npc,revision;ServiceSession():player(0),actor(0),entity(0),token(0),sequence(1),created(0),menu(0),npc(0),revision(0){}};
static ServiceSession serviceSessions[MAX_PLAYER];static DWORD serviceToken=0;
static bool ServiceSessionAccess(int p){if(!Live(p))return false;ServiceSession& s=serviceSessions[p];ServiceMenu* m=ServiceMenuById(s.menu);int npc=Player[p].m_nLastNpcIndex;return m&&s.token&&s.revision==loadedRevision&&s.player==Player[p].m_dwID&&s.actor==Npc[Player[p].m_nIndex].m_dwID&&npc>0&&npc<MAX_NPC&&s.entity==Npc[npc].m_dwID&&(DWORD)(GetTickCount()-s.created)<=600000&&ServiceAccess(p,*m);}
static void ServiceSnapshot(int p,int result){if(!Live(p))return;ServiceSession& s=serviceSessions[p];PTEventShopData data;memset(&data,0,sizeof(data));ServiceMenu* m=ServiceMenuById(s.menu);int actor=Player[p].m_nIndex;data.MapId=SubWorld[Npc[actor].m_SubWorldIndex].m_SubWorldID;data.ActorId=Npc[actor].m_dwID;data.NpcEntity=s.entity;data.EventId=0x40000000UL|s.menu;data.Token=s.token;data.Sequence=s.sequence;data.Result=result;data.Theme=m?m->theme:0;lstrcpynA(data.Title,m?m->name.c_str():"",sizeof(data.Title));
 if(m&&ServiceSessionAccess(p))for(unsigned i=0;i<services.offers.size()&&data.Count<19;++i){const ServiceOffer& o=services.offers[i];if(!o.enabled||o.menu!=s.menu||!(o.classes&Class(p))||Npc[actor].m_Level<o.minLevel)continue;PTEventShopRow& row=data.Rows[data.Count++];row.Id=o.id;row.Count=o.reward.count;row.Genre=o.reward.item.genre;row.Detail=o.reward.item.detail;row.Part=o.reward.item.particular;row.Level=o.reward.item.level<0?0:o.reward.item.level;row.Series=o.reward.item.series<0?0:o.reward.item.series;row.PaymentKind=2;row.UnitPrice=o.price;lstrcpynA(row.Name,o.name.c_str(),sizeof(row.Name));const ServicePoints* point=o.point?ServicePoint(services,o.point):0;const char* label=point?point->name.c_str():o.paymentName.c_str();lstrcpynA(row.PaymentName,label,sizeof(row.PaymentName));_snprintf(row.Price,sizeof(row.Price)-1,"%d %s",o.price,label);}
 unsigned size=PTEventShopSize(data.Count);PhongThanInitializeWireHeader(&data.Header,PT_EVENT_SHOP,size,PHONGTHAN_WIRE_FLAG_RESPONSE,0);PTSQ_SHOP_SEND(p,data,size);
}
static int ServiceOpen(int p,int menu,int npc){if(!Load()||!Live(p))return -1;ServiceMenu* m=ServiceMenuById(menu);if(!m||m->kind!=2||m->npc!=npc||!ServiceAccess(p,*m))return -1;ServiceSession& s=serviceSessions[p];s=ServiceSession();s.player=Player[p].m_dwID;s.actor=Npc[Player[p].m_nIndex].m_dwID;s.entity=Npc[Player[p].m_nLastNpcIndex].m_dwID;s.menu=menu;s.npc=npc;s.revision=loadedRevision;s.created=GetTickCount();serviceToken=(serviceToken+1)&0x7fffffff;if(!serviceToken)++serviceToken;s.token=serviceToken|0x80000000UL;ServiceSnapshot(p,0);return 1;}
static void ServiceRequest(int p,const PTEventShopBuy& request){if(p<=0||p>=MAX_PLAYER||!Load()||!Live(p))return;ServiceSession& s=serviceSessions[p];if(!s.token||request.EventId!=(0x40000000UL|s.menu)||request.ActorId!=s.actor||request.Token!=s.token||request.MapId!=(unsigned)SubWorld[Npc[Player[p].m_nIndex].m_SubWorldIndex].m_SubWorldID)return;
 if(!ServiceSessionAccess(p)){ServiceSnapshot(p,-1);return;}if(request.Sequence!=s.sequence){ServiceSnapshot(p,0);return;}if(s.sequence==0xffffffff){s.token=0;return;}++s.sequence;s.created=GetTickCount();int result=ServiceTrade(p,s.menu,s.npc,request.GoodsId,request.Lots);ServiceSnapshot(p,result);
}
struct ServiceShopRegistration {ServiceShopRegistration(){PTEventShopExternal()=ServiceRequest;}};static ServiceShopRegistration serviceShopRegistration;
}
int LuaStudioQuestShop(Lua_State* L){int p=GetPlayerIndex(L);int result=PtStudioQuest::ServiceOpen(p,(int)Lua_ValueToNumber(L,1),(int)Lua_ValueToNumber(L,2));Lua_PushNumber(L,result);return 1;}
int LuaStudioQuestExchange(Lua_State* L){int p=GetPlayerIndex(L);int result=PtStudioQuest::ServiceTrade(p,(int)Lua_ValueToNumber(L,1),(int)Lua_ValueToNumber(L,2),(int)Lua_ValueToNumber(L,3),1);Lua_PushNumber(L,result);Lua_PushString(L,result==2?PTSQ_TRADE_OK:PtStudioQuest::error.c_str());return 2;}
