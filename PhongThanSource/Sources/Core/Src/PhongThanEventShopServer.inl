// Included inside PtCustomActions after Buy. The client sends only a goods ID.
struct EventShopSession {DWORD player,actor,token,sequence,created;long revision;int eventId,npc;EventShopSession():player(0),actor(0),token(0),sequence(1),created(0),revision(0),eventId(0),npc(0){}};
static EventShopSession shopSessions[MAX_PLAYER];static DWORD shopToken=0;
static bool ShopAccess(int p){
 if(!Ready(p))return false;EventShopSession& s=shopSessions[p];int actor=Player[p].m_nIndex;const PTCustomEvent* e=PhongThanCustomEvent(s.eventId);if(!e||s.revision!=e->revision)return false;
 return s.token&&s.player==Player[p].m_dwID&&s.actor==Npc[actor].m_dwID&&(DWORD)(GetTickCount()-s.created)<=600000&&PhongThanCustomNpcAccess(p,s.eventId,s.npc,1);
}
static void ShopSnapshot(int p,int result){
 EventShopSession& session=shopSessions[p];const PTCustomEvent* event=PhongThanCustomEvent(session.eventId);if(!event||!Ready(p))return;
 PTEventShopData data;memset(&data,0,sizeof(data));int actor=Player[p].m_nIndex,world=Npc[actor].m_SubWorldIndex;
 if(world<0||world>=MAX_SUBWORLD)return;
 data.MapId=SubWorld[world].m_SubWorldID;data.ActorId=Npc[actor].m_dwID;data.EventId=session.eventId;data.Token=session.token;data.Sequence=session.sequence;data.Result=result;data.Theme=event->shopTheme;
 lstrcpynA(data.Title,event->name.c_str(),sizeof(data.Title));int npc=Player[p].m_nLastNpcIndex;
 if(npc>0&&npc<MAX_NPC)data.NpcEntity=Npc[npc].m_dwID;
 if(ShopAccess(p))for(unsigned int i=0;i<event->goods.size()&&data.Count<19;++i){const PTCustomGoods& goods=event->goods[i];const PTCustomItem& item=goods.item;
  int preview=CreateVngNormalItem(item.genre,item.detail,item.part,item.level,item.series,0);if(preview<=0)continue;
  PTEventShopRow& row=data.Rows[data.Count++];row.Id=goods.id;row.Count=item.count;row.Genre=item.genre;row.Detail=item.detail;row.Part=item.part;row.Level=item.level;row.Series=item.series;
  lstrcpynA(row.Name,goods.name.empty()?Item[preview].GetName():goods.name.c_str(),sizeof(row.Name));ItemSet.Remove(preview);
  row.PaymentKind=goods.barter?(goods.payment.genre==4&&goods.payment.detail==47?1:2):0;row.UnitPrice=goods.barter?goods.payment.count:goods.silver;
  if(goods.barter){const PTCustomItem& payment=goods.payment;
   if(payment.genre==4&&payment.detail==47)sprintf(row.Price,"%d Xu",payment.count);
   else{int price=CreateVngNormalItem(payment.genre,payment.detail,payment.part,payment.level,payment.series,0);char name[80];lstrcpynA(name,price>0?Item[price].GetName():"Vat pham",sizeof(name));lstrcpynA(row.PaymentName,name,sizeof(row.PaymentName));if(price>0)ItemSet.Remove(price);_snprintf(row.Price,sizeof(row.Price)-1,"%s x%d",name,payment.count);}
  }else{char price[64];PTEventShopSilver(goods.silver,price);lstrcpynA(row.Price,price,sizeof(row.Price));}
 }
 unsigned int size=PTEventShopSize(data.Count);PhongThanInitializeWireHeader(&data.Header,PT_EVENT_SHOP,size,PHONGTHAN_WIRE_FLAG_RESPONSE,0);g_pServer->PackDataToClient(Player[p].m_nNetConnectIdx,&data,size);
}
static int OpenShop(int p,const PTCustomEvent& event,int npc){
 if(!PhongThanCustomNpcAccess(p,event.id,npc,1))return -1;
 EventShopSession& s=shopSessions[p];s=EventShopSession();s.player=Player[p].m_dwID;s.actor=Npc[Player[p].m_nIndex].m_dwID;s.eventId=event.id;s.revision=event.revision;s.npc=npc;s.created=GetTickCount();if(!++shopToken)++shopToken;s.token=shopToken;ShopSnapshot(p,0);return 1;
}
static void ShopRequest(int p,const PTEventShopBuy& request){
 if(p<=0||p>=MAX_PLAYER)return;EventShopSession& s=shopSessions[p];
 if(!Ready(p)||s.token!=request.Token||s.eventId!=(int)request.EventId||s.actor!=request.ActorId)return;
 int actor=Player[p].m_nIndex,world=Npc[actor].m_SubWorldIndex;if(world<0||world>=MAX_SUBWORLD||request.MapId!=(unsigned int)SubWorld[world].m_SubWorldID)return;
 if(!ShopAccess(p)){ShopSnapshot(p,-1);return;}
 if(request.Sequence!=s.sequence){ShopSnapshot(p,0);return;}
 if(s.sequence==0xffffffff){s.token=0;return;}++s.sequence;s.created=GetTickCount();int result=-2;
 const PTCustomEvent* e=PhongThanCustomEvent(s.eventId);if(e)for(unsigned int i=0;i<e->goods.size();++i)if(e->goods[i].id==(int)request.GoodsId){PTCustomGoods scaled=e->goods[i];unsigned int price=scaled.barter?scaled.payment.count:scaled.silver;if(request.Lots&&request.Lots<=PTEventShopMaxLots(scaled.item.count,price)){scaled.item.count*=request.Lots;if(scaled.barter)scaled.payment.count*=request.Lots;else scaled.silver*=request.Lots;result=Buy(p,*e,scaled);}break;}
 ShopSnapshot(p,result);
}
