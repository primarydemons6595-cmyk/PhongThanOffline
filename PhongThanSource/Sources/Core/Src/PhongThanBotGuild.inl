struct BotGuildPlan {DWORD next,until;int mode,npc;unsigned int nation;BotActivityProgress progress;BotGuildPlan():next(0),until(0),mode(0),npc(0),nation(0){}};
static BotGuildPlan botGuildPlans[Capacity];
static int BotGuildLeader(const char* name){for(int i=0;i<count;++i)if(!strcmp(bots[i].Config.Name,name))return i;return -1;}
static int BotGuildNpc(const char* script){DWORD id=g_FileName2Id((char*)script);for(int n=NpcSet.GetNextIdx(0);n;n=NpcSet.GetNextIdx(n))if(Npc[n].m_Kind==kind_dialoger&&Npc[n].IsAlive()&&Npc[n].m_SubWorldIndex>=0&&Npc[n].m_SubWorldIndex<MAX_SUBWORLD&&(Npc[n].m_ActionScriptID==id||g_FileName2Id(Npc[n].ActionScript)==id))return n;return 0;}
static bool BotGuildWalk(Runtime& r,Training& route,int map,int x,int y,DWORD now){
 KPlayer& p=Player[r.Player];KNpc& actor=Npc[p.m_nIndex];int current=SubWorld[actor.m_SubWorldIndex].m_SubWorldID;
 if(current!=map){if(BotTryTravel(r,map,now))return true;int edge=-1;if(BotRoute(r,current,map,&edge)<1||edge<0)return false;lstrcpyA(r.State,"guild_travel");MoveTraining(r,route,PTBotEdges[edge].X,PTBotEdges[edge].Y,now);return true;}
 int px,py;actor.GetMpsPos(&px,&py);if(abs(px-x)+abs(py-y)>256){lstrcpyA(r.State,"guild_travel");MoveTraining(r,route,x,y,now,128);return true;}return false;
}
static bool BotGuildTick(Runtime& r,Training& route,DWORD now){
 int slot=&r-bots;BotGuildPlan& plan=botGuildPlans[slot];KPlayer& p=Player[r.Player];KNpc& actor=Npc[p.m_nIndex];
 if(!botPolicy.nationMode||!r.Config.Auto||companions[slot].Mode||p.CheckTrading()||p.GetLockState()||p.m_ItemList.Hand()||actor.m_CurrentLife<actor.m_CurrentLifeMax*4/5)return false;
 if((int)(now-plan.next)<0&&!plan.mode)return false;
 PTBotNationInfo own;if(!PhongThanBotNationInfo(r.Player,&own))return false;
 if(!own.id){
  if(actor.m_Level<PT_NATION_JOIN_LEVEL){plan.next=now+60000;return false;}
  unsigned int selected=0;int best=-2147483647,botNations=0;
  for(int index=0;index<256;++index){PTBotNationInfo n;if(!PhongThanBotNationAt(index,&n))break;int leader=BotGuildLeader(n.leader);if(leader>=0)++botNations;
   int score=(leader>=0?100000:0)+(n.cityLevel?1000:0)-n.members*10+(Hash(r.Config.Id,32)^n.id)%100;
   if(score>best){best=score;selected=n.id;}}
  int fee=GetPrivateProfileIntA("MoneyREQ","Money",500000,".\\settings\\tongsetting\\createtong.ini"),credit=GetPrivateProfileIntA("BaseREQ","CreatorCredit",500,".\\settings\\tongsetting\\createtong.ini");
  bool founder=botPolicy.nationMode==2&&botNations<botPolicy.maxNations&&actor.m_Level>=PT_NATION_CREATE_LEVEL&&fee>=0&&credit>=0&&p.m_ItemList.GetEquipmentMoney()>=fee&&p.m_cTask.GetSaveVal(TASKVALUE_PT_CREDIT)>=credit;
  if(selected&&(botNations||!founder)){plan.mode=0;plan.next=now+60000;if(PhongThanBotNationAction(r.Player,PT_NATION_APPLY,selected,0,0,0)){plan.nation=selected;plan.until=now+1800000;Log("GUILD",r.Config.Id,"real_nation_application_submitted_waiting_for_acceptance");p.Save();}return false;}
  if(botPolicy.nationMode<2||botNations>=botPolicy.maxNations||actor.m_Level<PT_NATION_CREATE_LEVEL){plan.next=now+60000;return false;}
  if(fee<0||credit<0||p.m_ItemList.GetEquipmentMoney()<fee||p.m_cTask.GetSaveVal(TASKVALUE_PT_CREDIT)<credit){plan.next=now+60000;return false;}
  // Elect one qualified founder, never one nation per bot or free funding.
  for(int other=0;other<count;++other)if(&bots[other]!=&r&&bots[other].Player&&bots[other].Config.Auto&&SocialAlive(bots[other].Player)&&strcmp(bots[other].Config.Id,r.Config.Id)<0){KPlayer& candidate=Player[bots[other].Player];PTBotNationInfo info;if(Npc[candidate.m_nIndex].m_Level>=PT_NATION_CREATE_LEVEL&&candidate.m_ItemList.GetEquipmentMoney()>=fee&&candidate.m_cTask.GetSaveVal(TASKVALUE_PT_CREDIT)>=credit&&PhongThanBotNationInfo(bots[other].Player,&info)&&!info.id){plan.next=now+30000;return false;}}
  if(!plan.mode){plan.mode=1;plan.until=now+180000;}if((int)(now-plan.until)>0){plan.mode=0;plan.next=now+300000;return false;}
  if(BotGuildWalk(r,route,1001,52352,102656,now))return true;
  char name[17];sprintf(name,"Hoi%.8s",r.Config.Id+24);int created=PhongThanBotNationAction(r.Player,PT_NATION_CREATE,0,0,0,name);plan.mode=0;plan.next=now+(created?60000:300000);
  if(created){Log("GUILD",r.Config.Id,"nation_created_native_fee_credit_and_wallet_confirmed");p.Save();}return created!=0;
 }
 if(!own.duty){if(plan.nation!=own.id||!plan.until){plan.nation=own.id;plan.until=now+1800000;}else if((int)(now-plan.until)>0){PhongThanBotNationAction(r.Player,PT_NATION_CANCEL,own.id,0,0,0);plan.until=0;Log("GUILD",r.Config.Id,"unanswered_application_cancelled_through_native_service");}plan.mode=0;plan.next=now+60000;lstrcpyA(r.State,"guild_application_pending");return false;}
 if(own.pending){plan.mode=0;plan.next=now+10000;return false;}
 // Only a real leader/right holder may accept a submitted application.
 char pending[32];for(int application=0;application<64&&PhongThanBotNationPendingAt(r.Player,application,pending,sizeof(pending));++application){int target=BotGuildLeader(pending);if(target>=0&&bots[target].Player&&SocialAlive(bots[target].Player)){if(PhongThanBotNationAction(r.Player,PT_NATION_ACCEPT,own.id,0,pending,0))Log("GUILD",r.Config.Id,"bot_member_accepted_through_real_nation_rights");plan.next=now+10000;return false;}}
 int current=SubWorld[actor.m_SubWorldIndex].m_SubWorldID;
 if(own.cityMap&&current==own.cityMap){plan.mode=0;plan.next=now+900000;lstrcpyA(r.State,"guild_city_visit");return false;}
 if(!plan.mode){
  if(own.cityMap){plan.mode=4;plan.nation=own.id;}
  else{
   bool books=true;for(int detail=58;detail<=61;++detail){PhongThanQuestItemTuple book;if(!PhongThanDecodeQuestItemTuple(3,detail,0,0,0,0,book)||BotStoryItemCount(p,book)<1)books=false;}
   int gold=PhongThanBotCityRequirement(botPolicy.land,0),bronze=PhongThanBotCityRequirement(botPolicy.land,1),glory=PhongThanBotCityRequirement(botPolicy.land,2),popularity=PhongThanBotCityRequirement(botPolicy.land,3),fee=PhongThanBotCityRequirement(botPolicy.land,4),cap=PhongThanBotCityRequirement(botPolicy.land,5);
   if(own.duty==PT_NATION_LORD&&books&&gold>=0&&bronze>=0&&glory>=0&&popularity>=0&&fee>=0&&own.money>=gold&&own.bronze>=bronze&&own.popularity>=popularity&&p.m_cTask.GetSaveVal(TASKVALUE_PT_CREDIT)>=glory&&p.m_ItemList.GetEquipmentMoney()>=fee){plan.mode=3;plan.nation=own.id;}
   else if(botPolicy.contribution&&cap>0&&own.today<cap&&own.bronze<bronze){
    int held=0;for(PlayerItem* stock=p.m_ItemList.GetFirstItem();stock;stock=p.m_ItemList.GetNextItem()){int n=stock->nIdx;if(n>0&&n<MAX_ITEM&&BotBag(stock->nPlace)&&Item[n].GetGenre()==item_materials&&(Item[n].GetDetailType()==3||Item[n].GetDetailType()==6)&&!Item[n].GetLock()->IsLock()&&!Item[n].GetLockTrade())held+=Item[n].GetStackNum();}
    if(held>=250*100/botPolicy.contribution){plan.mode=5;plan.nation=own.id;}
   }
   if(!plan.mode&&botPolicy.contribution&&cap>0&&own.today<cap&&p.m_ItemList.GetEquipmentMoney()>=500000*100/botPolicy.contribution){plan.mode=2;plan.nation=own.id;}
  }
  if(!plan.mode){plan.next=now+60000;return false;}plan.until=now+180000;plan.progress=BotActivityProgress();
 }
 if(plan.nation!=own.id||(int)(now-plan.until)>0){plan.mode=0;plan.next=now+300000;route.Path.clear();return false;}
 const char* script=(plan.mode==2||plan.mode==5)?"\\script\\phongthan\\territory\\warehouse.lua":plan.mode==3?"\\script\\phongthan\\territory\\founder.lua":"\\script\\phongthan\\territory\\gate.lua";
 int npc=BotGuildNpc(script);
 if(!npc){plan.mode=0;plan.next=now+60000;return false;}
 char primary[40],secondary[40]={0};
 if(plan.mode==2)strcpy(primary,"pt_money1");
 else if(plan.mode==5)strcpy(primary,"pt_bronze1");
 else if(plan.mode==3){sprintf(primary,"pt_create%d_%d",botPolicy.land,botPolicy.totem);sprintf(secondary,"pt_land%d",botPolicy.land);}
 else{int index=0;for(;index<25;++index){char name[32];int level=0;if(PhongThanTerritoryCityAt(index,name,level)==own.cityMap)break;}if(index>=25){plan.mode=0;return false;}sprintf(primary,"pt_enter%d",index);}
 int oldMoney=p.m_ItemList.GetEquipmentMoney(),oldCity=own.cityLevel;
 bool handled=BotVisitActivity(r,route,npc,primary,secondary[0]?secondary:0,now,plan.progress);
 if(plan.mode==2&&p.m_ItemList.GetEquipmentMoney()<oldMoney){Log("GUILD",r.Config.Id,"real_city_contribution_paid_and_receipted");plan.mode=0;plan.next=now+3600000;return true;}
 PTBotNationInfo after;if(plan.mode==5&&PhongThanBotNationInfo(r.Player,&after)&&after.bronze>own.bronze){Log("GUILD",r.Config.Id,"owned_bronze_contribution_confirmed_native_receipt");plan.mode=0;plan.next=now+3600000;return true;}
 if(PhongThanBotNationInfo(r.Player,&after)&&after.cityLevel>oldCity){Log("GUILD",r.Config.Id,"real_city_creation_confirmed_owned_books_and_resources_consumed");plan.mode=0;plan.next=now+60000;return true;}
 return handled;
}
