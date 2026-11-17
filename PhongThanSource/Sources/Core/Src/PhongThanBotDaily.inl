// Daily adapters only accept work they know how to finish. Original Lua owns
// counters, day limits, payments and rewards; never set them from the planner.
struct BotDailyState {DWORD next,last,progress;int kills;BotDailyState():next(0),last(0),progress(0),kills(-1){}};
static BotDailyState botDaily[Capacity];
static int BotDaily(Runtime& r,Training& route,DWORD now){
 if(!r.Config.Auto||r.Config.Profession<0||r.Config.Profession>2)return 0;KPlayer& p=Player[r.Player];KNpc& actor=Npc[p.m_nIndex];if(actor.m_Level<20||p.GetLockState()||p.CheckTrading())return 0;
 BotDailyState& state=botDaily[&r-bots];int target=p.m_cTask.GetSaveVal(4554),kills=p.m_cTask.GetSaveVal(4555);unsigned long day=((unsigned long)time(0)+25200)/86400;
 if(!target){bool ready=actor.m_Level>=29?p.m_cTask.GetSaveVal(4558)==0:p.m_cTask.GetSaveVal(4556)!=(int)day||p.m_cTask.GetSaveVal(4557)<3;if(!ready)return 0;
  if(!state.next){state.next=now+5000+(Hash(r.Config.Id,32)^now)%175000;return 0;}if((int)(now-state.next)<0)return 0;
 }
 if(target&&(int)(now-state.next)<0)return 0;
 if(target&&kills<40){int map=p.m_cTask.GetSaveVal(4559);if(map<1000||map>=1256||((unsigned long)time(0)<route.AvoidUntil&&map==route.AvoidMap))return 0;
  if(state.kills!=kills){state.kills=kills;state.progress=now;}if(now-state.progress>120000){route.QuestTarget=route.QuestMap=0;route.Target=0;state.next=now+60000+(Hash(r.Config.Id,32)^now)%90000;state.progress=now;Log("QUEST",r.Config.Id,"daily_no_combat_progress_reconsidering");return 0;}
  int best=0,score=2147483647,current=SubWorld[actor.m_SubWorldIndex].m_SubWorldID;int hops=BotRoute(r,current,map);std::vector<int>& candidates=trainingMaps[map-1000];for(int entry=0;entry<(int)candidates.size();++entry){int n=candidates[entry];if(Monster(p.m_nIndex,n)&&SubWorld[Npc[n].m_SubWorldIndex].m_SubWorldID==map&&(Npc[n].m_NpcSettingIdx==target||(r.Config.Profession==0&&target==3&&Npc[n].m_NpcSettingIdx==0))){int value=PTBotTrainScore(actor.m_Level,Npc[n].m_Level,hops,0,0,true);if(value>=0&&value<score){score=value;best=Npc[n].m_NpcSettingIdx+1;}}}
  if(!best)return 0;if(route.QuestTarget!=best||route.QuestMap!=map){route.Target=0;route.Path.clear();}route.QuestTarget=best;route.QuestMap=map;return 2;
 }
 int npc=botGiaoNpcs[r.Config.Profession];if(npc<=0||npc>=MAX_NPC||Npc[npc].m_Kind!=kind_dialoger||Npc[npc].m_RegionIndex<0)return 0;int current=SubWorld[actor.m_SubWorldIndex].m_SubWorldID,dest=SubWorld[Npc[npc].m_SubWorldIndex].m_SubWorldID,x=0,y=0;Npc[npc].GetMpsPos(&x,&y);
 route.Target=0;route.QuestTarget=route.QuestMap=0;
 if(current!=dest){int edge=-1;if(BotRoute(r,current,dest,&edge)<1||edge<0)return 0;lstrcpyA(r.State,"quest_travel");MoveTraining(r,route,PTBotEdges[edge].X,PTBotEdges[edge].Y,now);return 1;}
 if(NpcSet.GetDistance(p.m_nIndex,npc)>80){lstrcpyA(r.State,"quest_travel");MoveTraining(r,route,x,y,now,64);return 1;}
 lstrcpyA(r.State,"quest_dialog");route.Path.clear();actor.SendCommand(do_stand);if(now-state.last<2000)return 1;state.last=now;
 if(p.m_UiDialogToken&&p.m_nLastNpcIndex==npc){const char* claim=r.Config.Profession==0?"ptg_giao_claim":"pt2_giao_claim";const char* take=r.Config.Profession==0?"ptg_giao_take":"pt2_giao_take";const char* open=r.Config.Profession==0?"ptg_giao":"pt2_giao_preview";int choice=QuestChoice(p,target?claim:take);if(choice<0)choice=QuestChoice(p,open);if(choice<0)choice=BotServiceChoice(p);
  if(choice<0){state.next=now+60000;return 0;}PHONGTHAN_UI_CHOICE_REQUEST request;ZeroMemory(&request,sizeof(request));PhongThanInitializeWireHeader(&request.Header,PHONGTHAN_MSG_UI_CHOICE_REQUEST,sizeof(request),PHONGTHAN_WIRE_FLAG_REQUEST,0);request.MapId=current;request.DialogToken=p.m_UiDialogToken;request.View=p.m_UiDialogView;request.Selection=choice;p.ProcessPlayerSelectFromUI((BYTE*)&request);
  int after=p.m_cTask.GetSaveVal(4554);if(after!=target){p.Save();state.progress=now;state.kills=-1;if(!after)state.next=now+60000+(Hash(r.Config.Id,32)^now)%240000;Log("QUEST",r.Config.Id,after?"daily_giao_huan_accepted_via_npc":"daily_giao_huan_reward_confirmed_via_npc");}return 1;
 }
 PHONGTHAN_ENTITY_REFERENCE request;ZeroMemory(&request,sizeof(request));PhongThanInitializeWireHeader(&request.Header,PHONGTHAN_MSG_NPC_INTERACT_REQUEST,sizeof(request),PHONGTHAN_WIRE_FLAG_REQUEST,0);request.MapId=current;request.EntityId=Npc[npc].m_dwID;p.DialogNpc((BYTE*)&request);return 1;
}
