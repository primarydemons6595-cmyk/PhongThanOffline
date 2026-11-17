// Uses real NPC dialogs for registration, entry and missions. Never teleports
// a bot into the arena or bypasses the same Join/Enter gates as a human.
static BotActivityProgress botTcProgress[Capacity];
static unsigned long botTcSession[Capacity];static int botTcSide[Capacity];
static bool BotThuongChuTraditionalTravel(Runtime& r,Training& route,DWORD now,BotActivityProgress& progress){
 KPlayer& p=Player[r.Player];KNpc& actor=Npc[p.m_nIndex];int map=SubWorld[actor.m_SubWorldIndex].m_SubWorldID;
 int cost=actor.m_Level<=30?200:actor.m_Level<=50?500:actor.m_Level<=70?1000:actor.m_Level<=90?2000:5000;
 if(p.m_ItemList.GetEquipmentMoney()<cost){lstrcpyA(r.Error,"thuong_chu_needs_normal_teleporter_fare");return false;}
 char script[]="\\script\\phongthan\\gate\\supertrap.lua";DWORD binding=g_FileName2Id(script);int best=2147483647,gate=0;
 for(int n=NpcSet.GetNextIdx(0);n;n=NpcSet.GetNextIdx(n))if(Npc[n].m_Kind==kind_dialoger&&Npc[n].IsAlive()&&Npc[n].m_SubWorldIndex>=0&&Npc[n].m_ActionScriptID==binding){
  int dest=SubWorld[Npc[n].m_SubWorldIndex].m_SubWorldID,hops=BotRoute(r,map,dest);if(hops<0)continue;int distance=dest==map?NpcSet.GetDistance(p.m_nIndex,n):0;if(distance<0)continue;int score=hops*100000+distance;if(score<best){best=score;gate=n;}}
 return gate&&BotVisitActivity(r,route,gate,"fst",0,now,progress);
}
static bool BotThuongChu(Runtime& r,Training& route,DWORD now){
 using namespace PtThuongChu;int p=r.Player,slot=&r-bots;if((!botPolicy.tc&&!Own(p))||!r.Config.Auto||companions[slot].Mode||Player[p].CheckTrading()||Player[p].GetLockState())return false;
 KNpc& actor=Npc[Player[p].m_nIndex];
 // Settle the previous real battle at its normal reward NPC before signing
 // up again; pending rewards otherwise keep Join() closed indefinitely.
 if(Player[p].m_cTask.GetSaveVal(TaskReward)>0){
  int side=Player[p].m_cTask.GetSaveVal(TaskSide);const char* script=side==1?"\\script\\phongthan\\npc_restore\\1021_14.lua":"\\script\\phongthan\\npc_restore\\1020_02.lua";DWORD id=g_FileName2Id((char*)script);int reward=0;
  for(int n=NpcSet.GetNextIdx(0);n;n=NpcSet.GetNextIdx(n))if(Npc[n].m_Kind==kind_dialoger&&Npc[n].IsAlive()&&Npc[n].m_ActionScriptID==id){reward=n;break;}
  if(reward&&BotVisitActivity(r,route,reward,"pt_tc_reward",0,now,botTcProgress[slot])){lstrcpyA(r.State,"thuong_chu_claiming_reward");return true;}
  lstrcpyA(r.Error,"thuong_chu_reward_needs_normal_npc_route");return false;
 }
 if(!match.phase||actor.m_Level<minimumLevel)return false;
 bool enrolled=Own(p)&&!members[p].left;int current=SubWorld[actor.m_SubWorldIndex].m_SubWorldID;BotActivityProgress& progress=botTcProgress[slot];
 if(botTcSession[slot]!=generation){botTcSession[slot]=generation;botTcSide[slot]=0;progress=BotActivityProgress();}
 if(!enrolled&&match.phase!=1)return false;
 if(!enrolled){int side=Count(1)<Count(2)?1:Count(2)<Count(1)?2:1+(Hash(r.Config.Id,32)%2);botTcSide[slot]=side;}
 int side=enrolled?members[p].side:botTcSide[slot];
 if(!enrolled||current!=Map){
  if(match.phase==3)return false;int stele=0,best=2147483647,targetWorld=g_SubWorldSet.SearchWorld(1001);
  if(current!=1001){if(BotThuongChuTraditionalTravel(r,route,now,progress)){lstrcpyA(r.State,"thuong_chu_registering");return true;}return false;}
  for(int n=NpcSet.GetNextIdx(0);n;n=NpcSet.GetNextIdx(n))if(Npc[n].m_SubWorldIndex==targetWorld&&Npc[n].m_Kind==kind_dialoger){int x,y;Npc[n].GetMpsPos(&x,&y);int distance=abs(x-(side==1?189:212)*256)+abs(y-(side==1?191:205)*512);if(distance<best&&distance<2048){best=distance;stele=n;}}
  if(!stele){lstrcpyA(r.Error,"thuong_chu_stele_not_loaded");return false;}
  if(enrolled&&match.phase==1){route.Path.clear();actor.SendCommand(do_stand);lstrcpyA(r.State,"thuong_chu_registered");return true;}
  if(BotVisitActivity(r,route,stele,enrolled?"pt_tc_enter":"pt_tc_join",0,now,progress)){lstrcpyA(r.State,enrolled?"thuong_chu_entering":"thuong_chu_registering");return true;}
  lstrcpyA(r.Error,"thuong_chu_no_normal_route_to_stele");return false;
 }
 if(match.phase!=2){route.Path.clear();actor.SendCommand(do_stand);lstrcpyA(r.State,"thuong_chu_closing");return true;}
 BotUseRecovery(r,now,true,true);
 int mission=units[29+side].index;
 if(members[p].carried&&mission){if(BotVisitActivity(r,route,mission,"pt_tc_deliver",0,now,progress)){lstrcpyA(r.State,"thuong_chu_returning_flag");return true;}}
 int quest=ActiveQuest(p),goal=PTThuongChuMatch::QuestGoal(quest);
 if(mission&&quest&&Player[p].m_cTask.GetSaveVal(TaskQuestProgress)>=goal){if(BotVisitActivity(r,route,mission,"pt_tc_finish",0,now,progress))return true;}
 if(mission&&!quest&&Player[p].m_ItemList.GetEquipmentMoney()>=40000&&(Player[p].m_cTask.GetSaveVal(TaskQuestDay)!=Day()||Player[p].m_cTask.GetSaveVal(TaskQuestRuns)<10)){
  const char* callback=slot%3==0?"pt_tc_flagquest":slot%3==1?"pt_tc_scoutquest":"pt_tc_totemquest";if(BotVisitActivity(r,route,mission,callback,0,now,progress))return true;
 }
 int target=0,best=2147483647;
 for(int n=NpcSet.GetNextIdx(0);n;n=NpcSet.GetNextIdx(n)){if(Npc[n].m_SubWorldIndex!=actor.m_SubWorldIndex||!Npc[n].IsAlive()||NpcSet.GetRelation(actor.m_Index,n)!=relation_enemy)continue;
  int score=NpcSet.GetDistance(actor.m_Index,n);if(score<0)continue;
  for(int u=0;u<UnitCount;++u)if(units[u].index==n&&units[u].id==Npc[n].m_dwID){if(units[u].dead){score=2147483647;break;}if(units[u].kind==UnitCommanderFlag||units[u].kind==UnitFlag){if(members[p].carried)score=2147483647;else score/=4;}else if((quest==3&&units[u].kind==UnitScout)||(quest==4&&units[u].kind==UnitTotem))score/=2;}
  if(score<best){best=score;target=n;}
 }
 if(target){if((__int64)actor.m_CurrentLife*100<(__int64)actor.m_CurrentLifeMax*25){int x,y,tx,ty;actor.GetMpsPos(&x,&y);Npc[target].GetMpsPos(&tx,&ty);MoveTraining(r,route,x+(x>=tx?192:-192),y+(y>=ty?192:-192),now,32);lstrcpyA(r.State,"thuong_chu_retreating");return true;}
  route.Target=target;route.Identity=Npc[target].m_dwID;r.GoalMap=Map;r.GoalEntity=route.Identity;BotFightTarget(r,route,now);lstrcpyA(r.State,"thuong_chu_fighting");return true;
 }
 route.Path.clear();actor.SendCommand(do_stand);lstrcpyA(r.State,"thuong_chu_seeking_objective");return true;
}
