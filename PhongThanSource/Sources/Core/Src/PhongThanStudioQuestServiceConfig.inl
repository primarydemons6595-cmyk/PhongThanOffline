// Included inside PtStudioQuest after the native INI/item helpers.
struct ServicePoints {int id,enabled;std::string name;};
struct ServiceMenu {int id,npc,kind,classes,quest,theme;std::string name;};
struct ServiceOffer {int id,menu,classes,point,price,minLevel,enabled;PhongThanQuestExchangeRow reward,payment;std::string name,paymentName;};
struct ServiceCatalog {std::vector<ServicePoints> points;std::vector<ServiceMenu> menus;std::vector<ServiceOffer> offers;};
static ServiceCatalog services;
static const ServicePoints* ServicePoint(const ServiceCatalog& catalog,int id){for(unsigned i=0;i<catalog.points.size();++i)if(catalog.points[i].id==id&&catalog.points[i].enabled)return &catalog.points[i];return 0;}
static bool LoadServices(KIniFile& ini,ServiceCatalog& next){
 int points=Int(ini,"Services","PointCount"),menus=Int(ini,"Services","MenuCount"),offers=Int(ini,"Services","OfferCount");if(points<0||points>16||menus<0||menus>256||offers<0||offers>512)return false;
 for(int i=1;i<=points;++i){char section[40];sprintf(section,"Point%d",i);ServicePoints p;p.id=Int(ini,section,"Id");p.enabled=Int(ini,section,"Enabled",1);p.name=Hex(ini,section,"NameHex");if(p.id<1||p.id>16||p.name.empty()||p.name.size()>60)return false;for(unsigned old=0;old<next.points.size();++old)if(next.points[old].id==p.id)return false;next.points.push_back(p);}
 for(int m=1;m<=menus;++m){char section[40];sprintf(section,"Menu%d",m);ServiceMenu n;n.id=Int(ini,section,"Id");n.npc=Int(ini,section,"Npc");n.kind=Int(ini,section,"Kind");n.classes=Int(ini,section,"Classes",7);n.quest=Int(ini,section,"Quest");n.theme=Int(ini,section,"Theme");n.name=Hex(ini,section,"NameHex");if(n.id<1||n.id>100000||n.npc<1||n.kind<0||n.kind>2||n.classes<1||n.classes>7||n.theme<0||n.theme>4||n.name.empty()||n.name.size()>60)return false;for(unsigned old=0;old<next.menus.size();++old)if(next.menus[old].id==n.id)return false;next.menus.push_back(n);}
 for(int o=1;o<=offers;++o){char section[40];sprintf(section,"Offer%d",o);ServiceOffer r;memset(&r.payment,0,sizeof(r.payment));r.id=Int(ini,section,"Id");r.menu=Int(ini,section,"Menu");r.classes=Int(ini,section,"Classes",7);r.point=Int(ini,section,"Point");r.price=Int(ini,section,"Price");r.minLevel=Int(ini,section,"MinLevel",1);r.enabled=Int(ini,section,"Enabled",1);r.name=Hex(ini,section,"NameHex");r.paymentName=Hex(ini,section,"PaymentNameHex");
  if(r.id<1||r.id>100000||r.classes<1||r.classes>7||r.price<1||r.price>1000000000||r.minLevel<1||r.minLevel>200||!Item(ini,section,r.reward)||r.reward.count>64)return false;
  for(unsigned old=0;old<next.offers.size();++old)if(next.offers[old].id==r.id)return false;
  if(r.point){if(!ServicePoint(next,r.point))return false;}else{r.payment.count=r.price;if(r.price>10000||!PhongThanDecodeQuestItemTuple(Int(ini,section,"PaymentGenre",-1),Int(ini,section,"PaymentDetail",-1),Int(ini,section,"PaymentParticular"),Int(ini,section,"PaymentLevel"),Int(ini,section,"PaymentSeries"),0,r.payment.item))return false;}
  next.offers.push_back(r);
 }return true;
}
static bool ValidateServices(const ServiceCatalog& catalog,const std::vector<Quest>& qs,const std::vector<Binding>& ns){
 for(unsigned i=0;i<catalog.menus.size();++i){const ServiceMenu& m=catalog.menus[i];bool found=false;for(unsigned n=0;n<ns.size();++n)if(ns[n].id==m.npc)found=true;if(!found)return false;
  if(m.kind==1){found=false;for(unsigned q=0;q<qs.size();++q)if(qs[q].id==m.quest){if(qs[q].start==m.npc||qs[q].end==m.npc)found=true;for(unsigned s=0;s<qs[q].steps.size();++s)if(qs[q].steps[s].npc==m.npc)found=true;}if(!found)return false;}
  int count=0;for(unsigned o=0;o<catalog.offers.size();++o)if(catalog.offers[o].menu==m.id)++count;if(count>19||(count&&m.kind!=2))return false;
 }
 for(unsigned o=0;o<catalog.offers.size();++o){bool found=false;for(unsigned m=0;m<catalog.menus.size();++m)if(catalog.menus[m].id==catalog.offers[o].menu&&catalog.menus[m].kind==2&&(catalog.menus[m].classes&catalog.offers[o].classes))found=true;if(!found)return false;}
 return true;
}
