// Included inside PtBotAIgent. Navigation and actions stay on the world thread.
struct Training {
 int PlanFilter[8];int Target,Map,Level,Next,GoalX,GoalY,AimX,AimY,Approach,RouteMap,Failures,BadMap,QuestTarget,QuestMap,BlockedEdge;
 DWORD NoTargetSince,Identity,Selected,BadUntil,PlanAt,DeadAt,BadIdentity,ConsumedAt,BlockedUntil;bool Resting,SurvivalLoaded,Partial;
 unsigned long AvoidUntil,ConservativeUntil,CooldownUntil;int AvoidMap;
 std::vector<PTRoutePoint> Path;PTRouteProgress Progress;
 Training():Target(0),Map(0),Level(0),Next(0),GoalX(0),GoalY(0),AimX(0),AimY(0),Approach(0),RouteMap(0),Failures(0),BadMap(0),QuestTarget(0),QuestMap(0),BlockedEdge(-1),NoTargetSince(0),Identity(0),Selected(0),BadUntil(0),PlanAt(0),DeadAt(0),BadIdentity(0),ConsumedAt(0),BlockedUntil(0),Resting(false),SurvivalLoaded(false),Partial(false),AvoidUntil(0),ConservativeUntil(0),CooldownUntil(0),AvoidMap(0){ZeroMemory(PlanFilter,sizeof(PlanFilter));}
};
static Training training[Capacity];static int routeBudget=0;
static int BotRoute(Runtime& r,int from,int to,int* first=0){Training& t=training[&r-bots];return t.BlockedEdge>=0&&(int)(GetTickCount()-t.BlockedUntil)<0?PTBotRouteWithout(from,to,t.BlockedEdge,first):PTBotRoute(from,to,first);}

static void SurvivalState(Runtime& r,Training& t,bool save){
 char path[160],value[32];CreateDirectoryA("BotAIgent\\agenda",0);sprintf(path,"BotAIgent\\agenda\\%s.ini",r.Config.Id);
 if(!save){t.AvoidMap=GetPrivateProfileIntA("Survival","AvoidMap",0,path);t.AvoidUntil=GetPrivateProfileIntA("Survival","AvoidUntil",0,path);t.ConservativeUntil=GetPrivateProfileIntA("Survival","ConservativeUntil",0,path);t.CooldownUntil=GetPrivateProfileIntA("Survival","CooldownUntil",0,path);t.SurvivalLoaded=true;return;}
 sprintf(value,"%d",t.AvoidMap);WritePrivateProfileStringA("Survival","AvoidMap",value,path);
 sprintf(value,"%lu",t.AvoidUntil);WritePrivateProfileStringA("Survival","AvoidUntil",value,path);
 sprintf(value,"%lu",t.ConservativeUntil);WritePrivateProfileStringA("Survival","ConservativeUntil",value,path);
 sprintf(value,"%lu",t.CooldownUntil);WritePrivateProfileStringA("Survival","CooldownUntil",value,path);
}
class BotGrid {
 int world,startX,startY;
public:
 BotGrid(int w,int x,int y):world(w),startX(x/32),startY(y/32){}
 bool Free(int x,int y){if(x==startX&&y==startY)return true;return x>=0&&y>=0&&x<32767&&y<32767&&SubWorld[world].GetBarrier(x*32+16,y*32+16)==0;}
 DWORD Trap(int x,int y){return x>=0&&y>=0&&x<32767&&y<32767?SubWorld[world].GetTrap(x*32+16,y*32+16):0;}
};
static bool CompanionBoss(int npc);
static bool Monster(int actor,int enemy){
 if(enemy<=0||enemy>=MAX_NPC)return false;KNpc& a=Npc[actor];KNpc& b=Npc[enemy];
 return b.m_Kind==kind_normal&&b.m_nMissionGroup<0&&!b.Owner[0]&&b.m_RegionIndex>=0&&b.m_SubWorldIndex>=0&&b.m_SubWorldIndex<MAX_SUBWORLD&&b.m_CurrentLife>0&&b.m_Doing!=do_death&&b.m_Doing!=do_revive&&NpcSet.GetRelation(actor,enemy)==relation_enemy;
}
// One shared snapshot per decision tick, indexed by real monster level/map.
// Every use revalidates identity/life/relation; no stale entry can be attacked.
static std::vector<int> trainingLevels[256],trainingMaps[256];
static void RefreshTrainingIndex(){
 for(int i=0;i<256;++i){trainingLevels[i].clear();trainingMaps[i].clear();}
 for(int n=NpcSet.GetNextIdx(0);n;n=NpcSet.GetNextIdx(n)){KNpc& npc=Npc[n];if(npc.m_Kind!=kind_normal||npc.m_CurrentLife<=0||npc.m_nMissionGroup>=0||npc.Owner[0]||npc.m_RegionIndex<0||npc.m_SubWorldIndex<0||npc.m_SubWorldIndex>=MAX_SUBWORLD)continue;
  if(npc.m_Level>0&&npc.m_Level<256)trainingLevels[npc.m_Level].push_back(n);int map=SubWorld[npc.m_SubWorldIndex].m_SubWorldID;if(map>=1000&&map<1256)trainingMaps[map-1000].push_back(n);
 }
}
static void SelectTraining(Runtime& r,Training& t,DWORD now){
 PTPerfScope profile(PTPERF_TARGET);
 KNpc& actor=Npc[Player[r.Player].m_nIndex];int map=SubWorld[actor.m_SubWorldIndex].m_SubWorldID,px=0,py=0;actor.GetMpsPos(&px,&py);
 int best=-1,score=2147483647,hopCache[256];for(int i=0;i<256;++i)hopCache[i]=-2;
 int crowded[256];ZeroMemory(crowded,sizeof(crowded));std::map<DWORD,bool> reserved;
 for(int other=0;other<count;++other)if(&bots[other]!=&r&&bots[other].Player){
  if(training[other].Map>=1000&&training[other].Map<1256)++crowded[training[other].Map-1000];
  if(training[other].Identity&&training[other].Target>0)reserved[training[other].Identity]=true;
 }
 ZeroMemory(t.PlanFilter,sizeof(t.PlanFilter));
 int fallback=botPolicy.training&&t.NoTargetSince?(now-t.NoTargetSince>=45000?2:now-t.NoTargetSince>=15000?1:0):0;
 int low=t.QuestTarget>0?1:PTBotTrainingFloor(actor.m_Level,fallback),high=actor.m_Level<=5?actor.m_Level+4:actor.m_Level+2;
 for(int level=low;level<=high&&level<256;++level)for(int entry=0;entry<(int)trainingLevels[level].size();++entry){int n=trainingLevels[level][entry];++t.PlanFilter[0];if(!Monster(Player[r.Player].m_nIndex,n)){++t.PlanFilter[1];continue;}if(!t.QuestTarget&&CompanionBoss(n)){++t.PlanFilter[2];continue;}
  if(t.QuestTarget>0&&Npc[n].m_NpcSettingIdx+1!=t.QuestTarget)continue;
  if(t.QuestTarget>0&&t.QuestMap>0&&SubWorld[Npc[n].m_SubWorldIndex].m_SubWorldID!=t.QuestMap){++t.PlanFilter[3];continue;}
  KNpc& enemy=Npc[n];int targetMap=SubWorld[enemy.m_SubWorldIndex].m_SubWorldID;if(targetMap<1000||targetMap>=1256)continue;
  if(!t.QuestTarget&&!PTBotAutomaticAreaAllowed(r.Config.Profession,actor.m_Level,targetMap))continue;
  if((unsigned long)time(0)<t.AvoidUntil&&targetMap==t.AvoidMap)continue;
  if((unsigned long)time(0)<t.ConservativeUntil&&enemy.m_Level>PTBotConservativeCeiling(actor.m_Level))continue;
  if(now<t.BadUntil&&enemy.m_dwID==t.BadIdentity)continue;
  int& hops=hopCache[targetMap-1000];if(hops==-2)hops=BotRoute(r,map,targetMap);
  int x=0,y=0;enemy.GetMpsPos(&x,&y);int distance=map==targetMap?abs(x-px)+abs(y-py):0;
  if(hops<0){++t.PlanFilter[5];continue;}bool claimed=reserved.find(enemy.m_dwID)!=reserved.end();if(claimed){++t.PlanFilter[6];if(!fallback)continue;}int congestion=crowded[targetMap-1000];
  unsigned int variation=Hash(r.Config.Id,32)^enemy.m_dwID;
  __int64 healthCost=(__int64)enemy.m_CurrentLifeMax*10/(actor.m_Level>0?actor.m_Level:1);if(healthCost>50000)healthCost=50000;
  int value=PTBotTrainScore(actor.m_Level,enemy.m_Level,hops,distance,(now<t.BadUntil&&targetMap==t.BadMap?2000:0)+congestion*100+(variation%120)+(int)healthCost+(claimed?20000:0),t.QuestTarget>0||fallback>0);
  if(value>=0)++t.PlanFilter[7];else ++t.PlanFilter[4];if(value>=0&&value<score){score=value;best=n;}
 }
 t.Selected=now;t.Level=actor.m_Level;t.Path.clear();t.Next=0;t.Progress.Reset();t.Target=best;t.Failures=0;
 if(best>0){t.NoTargetSince=0;r.Error[0]=0;if(fallback)Log("RECOVERY",r.Config.Id,"training_fallback_safe_lower_level_target_confirmed");t.Identity=Npc[best].m_dwID;t.Map=SubWorld[Npc[best].m_SubWorldIndex].m_SubWorldID;
  r.GoalMap=t.Map;r.GoalLevel=Npc[best].m_Level;r.GoalEntity=t.Identity;
  char message[180];sprintf(message,"train_target map=%d bot_level=%d monster_level=%d entity=%lu route_hops=%d",t.Map,actor.m_Level,Npc[best].m_Level,t.Identity,BotRoute(r,map,t.Map));Log("PLAN",r.Config.Id,message);
 }else{if(!t.NoTargetSince)t.NoTargetSince=now;t.Identity=0;t.Map=0;r.GoalMap=r.GoalLevel=0;r.GoalEntity=0;lstrcpyA(r.Error,"no_reachable_level_appropriate_monsters");}
}
// Mount state is authoritative and uses the same native cooldown/attributes
// and observer event as a human mount request; never set the flag directly.
static bool BotTravelMount(Runtime& r,bool mounted){
 KPlayer& player=Player[r.Player];KNpc& actor=Npc[player.m_nIndex];
 if((actor.m_bRideHorse!=FALSE)==mounted)return true;
 if(actor.m_CurrentLife<=0||actor.m_Doing==do_death||actor.m_Doing==do_revive||player.CheckTrading()||player.GetLockState()||player.m_cMenuState.m_nState!=PLAYER_MENU_STATE_NORMAL)return false;
 if(player.m_ItemList.GetEquipment(itempart_horse)<=0)return false;
 if(actor.m_dwNextSwitchHorseTime>g_SubWorldSet.GetGameTime())return false;
 if(actor.m_Doing==do_sit){actor.SendCommand(do_stand);return false;}
 if(actor.m_Doing!=do_stand&&actor.m_Doing!=do_run&&actor.m_Doing!=do_walk)return false;
 if(!actor.CanSwitchRideHorse())return false;
 actor.SwitchRideHorse(mounted?TRUE:FALSE);
 return (actor.m_bRideHorse!=FALSE)==mounted;
}
static bool AdvanceMovement(Runtime& r,Training& t,bool force){
 KNpc& npc=Npc[Player[r.Player].m_nIndex];int px=0,py=0;npc.GetMpsPos(&px,&py);BotGrid grid(npc.m_SubWorldIndex,px,py);int x=t.GoalX,y=t.GoalY;
 if(!npc.m_bRideHorse&&!t.Resting&&abs(px-x)+abs(py-y)>=640)BotTravelMount(r,true);
 // Keep the current long segment running. Reissuing RunTo on every decision
 // restarts its animation and broadcasts redundant commands to nearby clients.
 if(!force&&(npc.m_Doing==do_run||npc.m_Doing==do_walk)&&abs(px-npc.m_DesX)+abs(py-npc.m_DesY)>96)return true;
 PTRoutePoint target;bool walk=false;
 // Commands run continuously between decisions; skip only line-clear cells so
 // the one-second planner cadence does not limit movement to one 32px cell/sec.
 int lookLimit=t.Next+12;for(int ahead=t.Next+1;ahead<(int)t.Path.size()&&ahead<=lookLimit;++ahead){
  int ax=t.Path[ahead].x*32+16,ay=t.Path[ahead].y*32+16;
  if(abs(ax-px)+abs(ay-py)>512||!PTRouteSegmentClear(grid,px,py,ax,ay))break;t.Next=ahead;
 }
 if(PTRouteWaypoint(grid,t.Path,t.Next,px,py,npc.m_CurrentRunSpeed,npc.m_CurrentWalkSpeed,target,walk)){
  NPCCMD command=walk?do_walk:do_run;if(npc.m_Doing!=command||npc.m_DesX!=target.x||npc.m_DesY!=target.y)npc.SendCommand(command,target.x,target.y);return true;
 }
 if(t.Partial){t.Path.clear();return false;}
 // The final few pixels matter for portal activation. Never teleport to another map.
 if(abs(px-x)>8||abs(py-y)>8){npc.SendCommand(do_walk,x,y);return true;}
 return false;
}
static bool MoveTraining(Runtime& r,Training& t,int x,int y,DWORD now,int approach=0){
 PTPerfScope profile(PTPERF_ROUTE);
 KNpc& npc=Npc[Player[r.Player].m_nIndex];int px=0,py=0;npc.GetMpsPos(&px,&py);BotGrid grid(npc.m_SubWorldIndex,px,py);
 bool changed=t.RouteMap!=npc.m_SubWorldIndex||abs(t.AimX-x)>96||abs(t.AimY-y)>96||t.Approach!=approach;
 bool stalled=t.Progress.Stalled(px,py,now);
 if(changed||t.Path.empty()||stalled){
  if(routeBudget<=0||(!changed&&now-t.PlanAt<2000)){if(changed){t.Path.clear();if(npc.m_Doing==do_run||npc.m_Doing==do_walk)npc.SendCommand(do_stand);}return false;}
  --routeBudget;t.PlanAt=now;t.RouteMap=npc.m_SubWorldIndex;t.AimX=x;t.AimY=y;t.Approach=approach;t.GoalX=x;t.GoalY=y;t.Next=0;t.Progress.Reset();
  if(!approach){int gx=0,gy=0;if(PTBotPortalCell(grid,x/32,y/32,px/32,py/32,gx,gy)){t.GoalX=gx*32+16;t.GoalY=gy*32+16;if(changed){char message[200];sprintf(message,"portal_trigger_resolved map=%d marker=%d,%d actual=%d,%d trap=%lu",SubWorld[npc.m_SubWorldIndex].m_SubWorldID,x,y,t.GoalX,t.GoalY,grid.Trap(gx,gy));Log("ROUTE",r.Config.Id,message);}}}
  if(approach>0){
   std::vector< std::pair<int,PTRoutePoint> > candidates;int radius=approach/32+1;
   for(int dy=-radius;dy<=radius;++dy)for(int dx=-radius;dx<=radius;++dx){int gx=(x/32+dx)*32+16,gy=(y/32+dy)*32+16;
    if(abs(gx-x)+abs(gy-y)>approach||!grid.Free(gx/32,gy/32))continue;
    int cost=(abs(gx-px)+abs(gy-py))*2+abs(gx-x)+abs(gy-y);candidates.push_back(std::make_pair(cost,PTRoutePoint(gx,gy)));
   }
   // Choose the closest reachable-side approach; don't path onto an occupied NPC cell.
   for(int a=0;a<(int)candidates.size();++a)for(int b=a+1;b<(int)candidates.size();++b)if(candidates[b].first<candidates[a].first)std::swap(candidates[a],candidates[b]);
   if(!candidates.empty()){int pick=t.Failures%(int)candidates.size();t.GoalX=candidates[pick].second.x;t.GoalY=candidates[pick].second.y;}
  }
  // Portal centers can temporarily contain another NPC. Approach a nearby free
  // cell in the trigger area rather than treating the occupied center as a wall.
  if(!approach&&!grid.Free(t.GoalX/32,t.GoalY/32)){int best=2147483647,gx=t.GoalX,gy=t.GoalY;for(int oy=-3;oy<=3;++oy)for(int ox=-3;ox<=3;++ox){int cx=(x/32+ox)*32+16,cy=(y/32+oy)*32+16;if(!grid.Free(cx/32,cy/32))continue;int cost=abs(cx-x)+abs(cy-y);if(cost<best){best=cost;gx=cx;gy=cy;}}t.GoalX=gx;t.GoalY=gy;}
  if(!PTRouteFind(grid,px/32,py/32,t.GoalX/32,t.GoalY/32,t.Path,8000,&t.Partial)){
   if(t.Failures==0){char message[220];sprintf(message,"route_failed map=%d from=%d,%d to=%d,%d goal_free=%d limit=8000",SubWorld[npc.m_SubWorldIndex].m_SubWorldID,px,py,x,y,grid.Free(x/32,y/32)?1:0);Log("WARN",r.Config.Id,message);}
   if(++t.Failures==3){t.BadIdentity=t.Identity;t.BadMap=t.Map;t.BadUntil=now+60000;t.Target=0;int map=SubWorld[npc.m_SubWorldIndex].m_SubWorldID;for(int e=0;e<sizeof(PTBotEdges)/sizeof(PTBotEdges[0]);++e)if(PTBotEdges[e].From==map&&abs(PTBotEdges[e].X-x)<=96&&abs(PTBotEdges[e].Y-y)<=96){t.BlockedEdge=e;t.BlockedUntil=now+180000;char message[180];sprintf(message,"route_edge_unreachable_try_alternate from=%d to=%d marker=%d,%d",map,PTBotEdges[e].To,x,y);Log("ROUTE",r.Config.Id,message);break;}Log("WARN",r.Config.Id,"train_path_blocked_reselect");}
   npc.SendCommand(do_stand);lstrcpyA(r.State,"path_blocked");return false;
  }
 }
 return AdvanceMovement(r,t,changed||stalled);
}
static void MovementTick(){
 if(expires<(unsigned long)time(0))return;
 for(int i=0;i<count;++i){Runtime& r=bots[i];Training& t=training[i];if(!r.Player||!r.Config.Enabled||t.Path.empty())continue;
  if(strcmp(r.State,"npc_goal_travel")&&strcmp(r.State,"bank_materials_travel")&&strcmp(r.State,"travel_training")&&strcmp(r.State,"party_follow")&&strcmp(r.State,"quest_travel")&&strcmp(r.State,"retreating")&&strcmp(r.State,"shopping_travel")&&strcmp(r.State,"bank_travel")&&strcmp(r.State,"guild_travel")&&strcmp(r.State,"pvp_support_travel")&&strcmp(r.State,"pvp_retreating_waiting_allies")&&strcmp(r.State,"looting")&&strcmp(r.State,"event_travel")&&strcmp(r.State,"boss_travel")&&strcmp(r.State,"thuong_chu_registering")&&strcmp(r.State,"thuong_chu_entering")&&strcmp(r.State,"thuong_chu_returning_flag")&&strcmp(r.State,"thuong_chu_retreating")&&strcmp(r.State,"thuong_chu_fighting"))continue;
  int actor=Player[r.Player].m_nIndex;if(actor<=0||actor>=MAX_NPC)continue;KNpc& npc=Npc[actor];
  if(npc.m_SubWorldIndex!=t.RouteMap||npc.m_CurrentLife<=0||(npc.m_Doing!=do_run&&npc.m_Doing!=do_walk&&npc.m_Doing!=do_stand))continue;
  AdvanceMovement(r,t,false);
 }
}
static bool FollowParty(Runtime& r,Training& t,DWORD now){
 KPlayer& player=Player[r.Player];if(!player.m_cTeam.m_nFlag||player.m_cTeam.m_nFigure!=TEAM_MEMBER||player.m_cTeam.m_nID<0||player.m_cTeam.m_nID>=MAX_TEAM)return false;
 int captain=g_Team[player.m_cTeam.m_nID].m_nCaptain;
 if(captain<=0||captain>=MAX_PLAYER||!Player[captain].m_PlayerDBLoad)return false;
 int leader=Player[captain].m_nIndex;if(leader<=0||leader>=MAX_NPC||Npc[leader].m_SubWorldIndex<0||Npc[leader].m_SubWorldIndex>=MAX_SUBWORLD)return false;
 KNpc& npc=Npc[player.m_nIndex];int map=SubWorld[npc.m_SubWorldIndex].m_SubWorldID,dest=SubWorld[Npc[leader].m_SubWorldIndex].m_SubWorldID;
 if((unsigned long)time(0)<t.AvoidUntil&&dest==t.AvoidMap)return false;
 if(Player[captain].m_bIsBot&&!PTBotAutomaticAreaAllowed(r.Config.Profession,npc.m_Level,dest))return false;
 int px=0,py=0,x=0,y=0;npc.GetMpsPos(&px,&py);Npc[leader].GetMpsPos(&x,&y);
 if(map!=dest){int edge=-1;if(BotRoute(r,map,dest,&edge)>0&&edge>=0){lstrcpyA(r.State,"party_follow");MoveTraining(r,t,PTBotEdges[edge].X,PTBotEdges[edge].Y,now);return true;}return false;}
 // Nearby members fight their own target instead of being yanked back to the
 // captain every decision. Regroup only outside the combat formation.
 if(abs(px-x)+abs(py-y)>960){lstrcpyA(r.State,"party_follow");MoveTraining(r,t,x,y,now,192);return true;}
 return false;
}
static void MeetParty(Runtime& r,Training& t,DWORD now){
 static DWORD last[Capacity];int slot=&r-bots;if(now-last[slot]<15000)return;last[slot]=now;
 KPlayer& self=Player[r.Player];if(self.m_cTeam.m_nFlag&&self.m_cTeam.m_nFigure!=TEAM_CAPTAIN)return;
 if(t.Map<=0)return;
 for(int other=0;other<count;++other)if(&bots[other]!=&r&&bots[other].Player&&strcmp(r.Config.Id,bots[other].Config.Id)<0&&training[other].Map==t.Map){
  int p=bots[other].Player;if(Player[p].m_cTeam.m_nFlag||abs(Npc[Player[p].m_nIndex].m_Level-Npc[self.m_nIndex].m_Level)>5||NpcSet.GetDistance(self.m_nIndex,Player[p].m_nIndex)>256||Npc[Player[p].m_nIndex].m_SubWorldIndex!=Npc[self.m_nIndex].m_SubWorldIndex)continue;
  if(!self.m_cTeam.m_nFlag){PLAYER_APPLY_CREATE_TEAM create;ZeroMemory(&create,sizeof(create));create.ProtocolType=c2s_teamapplycreate;if(!self.m_cTeam.CreateTeam(r.Player,&create))return;}
  if(self.m_cTeam.m_nID<0||self.m_cTeam.m_nID>=MAX_TEAM||g_Team[self.m_cTeam.m_nID].m_nMemNum>=3)return;
  TEAM_INVITE_ADD_COMMAND invite;ZeroMemory(&invite,sizeof(invite));invite.m_dwNpcID=Npc[Player[p].m_nIndex].m_dwID;self.m_cTeam.InviteAdd(r.Player,&invite,p);
  if(Player[p].m_cTeam.m_nFlag)Log("PARTY",r.Config.Id,"met_bot_same_destination_formed_party");return;
 }
}
static bool BotActivityWants(Runtime& r,KItem& item);
static int BotActivityNeed(Runtime& r,KItem& item);
#include "PhongThanBotResources.inl"
#include "PhongThanBotSkills.inl"
static bool BotRecoveryShop(Runtime& r,Training& t,DWORD now,bool life,bool mana);
static bool BotFirstSkillShop(Runtime& r,Training& t,DWORD now);
static bool Recover(Runtime& r,Training& t,DWORD now){
 KPlayer& player=Player[r.Player];KNpc& npc=Npc[player.m_nIndex];
 bool life=npc.m_CurrentLifeMax>0&&(__int64)npc.m_CurrentLife*100<(__int64)npc.m_CurrentLifeMax*(t.Resting?85:50);
 bool mana=npc.m_CurrentManaMax>0&&(__int64)npc.m_CurrentMana*100<(__int64)npc.m_CurrentManaMax*(t.Resting?75:20);
 bool consumed=BotUseRecovery(r,now,life,mana);
 if((life||mana)&&!consumed&&BotLoot(r,t,now,true)){t.Resting=false;return true;}
 if(!life&&!mana&&!t.Resting)return false;
 int threat=0;
 int mapIndex=SubWorld[npc.m_SubWorldIndex].m_SubWorldID-1000;if(mapIndex>=0&&mapIndex<256)for(int nearby=0;nearby<(int)trainingMaps[mapIndex].size();++nearby){int n=trainingMaps[mapIndex][nearby];if(Npc[n].m_SubWorldIndex==npc.m_SubWorldIndex&&Monster(npc.m_Index,n)&&NpcSet.GetDistance(npc.m_Index,n)<=384){threat=n;break;}}
 if(threat){t.Resting=false;if((__int64)npc.m_CurrentLife*100>=(__int64)npc.m_CurrentLifeMax*25)return false;
  int dest=0,x=0,y=0;player.GetDeathRevivalPos(&dest,&x,&y);int map=SubWorld[npc.m_SubWorldIndex].m_SubWorldID;
  if(dest!=map){int edge=-1;if(BotRoute(r,map,dest,&edge)>0&&edge>=0){x=PTBotEdges[edge].X;y=PTBotEdges[edge].Y;}else return false;}
  lstrcpyA(r.State,"retreating");MoveTraining(r,t,x,y,now,dest==map?96:0);return true;
 }
 if((__int64)npc.m_CurrentLife*100>=(__int64)npc.m_CurrentLifeMax*85&&(npc.m_CurrentManaMax<=0||(__int64)npc.m_CurrentMana*100>=(__int64)npc.m_CurrentManaMax*75)){t.Resting=false;if(npc.m_Doing==do_sit)npc.SendCommand(do_stand);return false;}
 if(BotRecoveryShop(r,t,now,life,mana)){t.Resting=false;return true;}
 t.Path.clear();t.Resting=true;lstrcpyA(r.State,"recovering");if(!PTBotSitForRecovery(npc,do_stand,do_sit))lstrcpyA(r.State,"recovery_dismount_pending");return true;
}
#include "PhongThanBotStarterQuest.inl"
static bool CompanionWants(Runtime& r,KItem& item);
static bool CompanionShopping(Runtime& r);
#include "PhongThanBotShop.inl"
#include "PhongThanBotStory.inl"
#include "PhongThanBotDaily.inl"
static void BotFightTarget(Runtime& r,Training& t,DWORD now){
 int actor=Player[r.Player].m_nIndex;if(t.Target<=0||t.Target>=MAX_NPC||Npc[t.Target].m_dwID!=t.Identity||Npc[t.Target].m_CurrentLife<=0)return;KNpc& npc=Npc[actor];if(npc.m_FightMode!=enumFightActive)npc.SetFightMode(enumFightActive);
 int tx=0,ty=0,px=0,py=0;Npc[t.Target].GetMpsPos(&tx,&ty);npc.GetMpsPos(&px,&py);
 // Foot-only skills must remain usable after mounted travel. The travel
 // threshold is larger than this approach band, so we do not toggle repeatedly.
 if(npc.m_bRideHorse&&abs(px-tx)+abs(py-ty)<=512&&!BotTravelMount(r,false)){lstrcpyA(r.State,"combat_dismount_pending");return;}
 int skill=BotAttackSkill(r,t.Target,now),slot=npc.m_SkillList.FindSame(skill);
 if(!slot||!npc.SetActiveSkill(slot)){lstrcpyA(r.State,"combat_blocked");lstrcpyA(r.Error,"default_attack_not_learned");return;}
 ISkill* active=npc.GetActiveSkill();if(!active){lstrcpyA(r.State,"combat_blocked");lstrcpyA(r.Error,"default_attack_data_unavailable");return;}
 int p1=-1,p2=t.Target,cast=active->CanCastSkill(actor,p1,p2),distance=NpcSet.GetDistance(actor,t.Target);
 int radius=npc.m_CurrentAttackRadius;if(radius<32)radius=32;
 if(cast==-1||distance>radius){int approach=radius-8;if(approach<24)approach=24;if(approach>160)approach=160;lstrcpyA(r.State,"travel_training");MoveTraining(r,t,tx,ty,now,approach);return;}
 t.Path.clear();if(cast!=1){lstrcpyA(r.State,"combat_blocked");lstrcpyA(r.Error,"attack_rejected_by_skill_rules");return;}
 lstrcpyA(r.State,"training");npc.SendCommand(do_skill,skill,-1,t.Target);BotSkillCastLog(r,skill,now);
}
#include "../../../Headers/PhongThanTravelDestinations.h"
#include "PhongThanBotTravel.inl"
#include "PhongThanBotCompanion.inl"
#include "../../../Headers/PhongThanVttStone.h"
#include "PhongThanBotActivities.inl"
#include "PhongThanBotThuongChu.inl"
#include "PhongThanBotGuild.inl"
#include "PhongThanBotProgression.inl"
#include "PhongThanBotInventoryPlan.inl"
#include "PhongThanBotProgress.inl"
#include "PhongThanBotPvp.inl"
#include "PhongThanBotStall.inl"
static bool BotReturnForTraining(Runtime& r,Training& t,DWORD now);
static void Train(Runtime& r,DWORD now){
 int actor=Player[r.Player].m_nIndex;if(actor<=0||actor>=MAX_NPC)return;KNpc& npc=Npc[actor];Training& t=training[&r-bots];
 if(!t.SurvivalLoaded)SurvivalState(r,t,false);
 if(npc.m_SubWorldIndex<0||npc.m_SubWorldIndex>=MAX_SUBWORLD)return;
 if(npc.m_CurrentLife<=0||npc.m_Doing==do_death){
  if(PtThuongChu::Own(r.Player)&&PtThuongChu::match.phase==2&&PtThuongChu::World(r.Player)==PtThuongChu::Map){
   if(!t.DeadAt)t.DeadAt=now;lstrcpyA(r.State,"thuong_chu_dead");if(now-t.DeadAt>=8000){Player[r.Player].Revive(REMOTE_REVIVE_TYPE);if(npc.m_CurrentLife>0){t.DeadAt=0;t.Path.clear();t.Target=0;t.Identity=0;t.CooldownUntil=0;Player[r.Player].Save();}}return;
  }
  lstrcpyA(r.State,"dead");if(!t.DeadAt){if(!companions[&r-bots].Mode)BotBossFailed(r,now);t.DeadAt=now;t.BadMap=SubWorld[npc.m_SubWorldIndex].m_SubWorldID;t.BadIdentity=t.Identity;t.BadUntil=now+300000;t.AvoidMap=t.BadMap;t.AvoidUntil=(unsigned long)time(0)+900;t.ConservativeUntil=(unsigned long)time(0)+1800;t.Target=0;t.Identity=0;t.Path.clear();r.GoalEntity=0;r.GoalMap=r.GoalLevel=0;SurvivalState(r,t,true);Log("PLAN",r.Config.Id,"death_marked_training_area_risky");}
  if(now-t.DeadAt>=8000&&PTBotAwaitingTownRevival(npc.m_CurrentLife,npc.m_Doing==do_death,npc.m_Doing==do_revive)){
   CompanionState& command=companions[&r-bots];if(PTBotCombatCommand(command.Mode)){
    bool local=PhongThanTryRevive(r.Player,2),revived=local||PhongThanTryRevive(r.Player,0);if(revived&&npc.m_CurrentLife>0){t.DeadAt=0;t.Resting=false;t.CooldownUntil=(unsigned long)time(0)+(local?2:5);t.Path.clear();command.LastLife=npc.m_CurrentLife;r.Error[0]=0;lstrcpyA(r.State,"returning_to_target");Player[r.Player].Save();Log("RECOVERY",r.Config.Id,local?"paid_local_revival_confirmed":"town_revival_return_to_command_target");}else{t.DeadAt=now;lstrcpyA(r.Error,"revival_not_confirmed");}return;
   }
   int map=0,x=0,y=0;if(!PhongThanStarterSpawn(r.Config.Profession,map,x,y)){lstrcpyA(r.Error,"invalid_profession_revival");return;}
   Player[r.Player].SetRevivalPos(map,0);Player[r.Player].Revive(REMOTE_REVIVE_TYPE);
   int px=0,py=0;npc.GetMpsPos(&px,&py);int actual=npc.m_SubWorldIndex>=0&&npc.m_SubWorldIndex<MAX_SUBWORLD?SubWorld[npc.m_SubWorldIndex].m_SubWorldID:0;
   if(PTBotTownRevivalConfirmed(npc.m_CurrentLife,actual,px,py,map,x,y)){t.DeadAt=0;t.Resting=false;t.CooldownUntil=(unsigned long)time(0)+25+(Hash(r.Config.Id,32)^now)%36;SurvivalState(r,t,true);r.Error[0]=0;lstrcpyA(r.State,"post_death_rest");Player[r.Player].Save();Log("INFO",r.Config.Id,"revived_confirmed_at_starter_town");}
   else{t.DeadAt=now;lstrcpyA(r.Error,"town_revival_not_confirmed_retry_later");Log("ERROR",r.Config.Id,r.Error);}
  }
  return;
 }
 t.DeadAt=0;if(npc.m_Doing==do_revive)return;
 if((unsigned long)time(0)<t.CooldownUntil){lstrcpyA(r.State,"post_death_rest");PTBotSitForRecovery(npc,do_stand,do_sit);return;}
 if(BotStallActive(r,t,now))return;
 if(Player[r.Player].CheckTrading()||Player[r.Player].GetLockState()){lstrcpyA(r.State,"busy_transaction");return;}
 if(BotPvpTick(r,t,now))return;
 if(botPolicy.bag&&BotInventoryPlan(r,t,now))return;
 if(CompanionTick(r,t,now))return;
 if(BotThuongChu(r,t,now))return;
 if(BotVtt(r,t,now))return;
 if(BotCarryMerchant(r,now)){t.Path.clear();if(npc.m_Doing!=do_stand)npc.SendCommand(do_stand);return;}
 if(BotRelieveCarry(r,now)){t.Path.clear();if(npc.m_Doing!=do_stand)npc.SendCommand(do_stand);return;}
 if(Recover(r,t,now))return;
 if(BotFirstSkillShop(r,t,now))return;
 if(BotGuildTick(r,t,now))return;
 if(BotEconomyPlan(r,t,now))return;
 if(BotVtt(r,t,now))return;
 if(r.Config.Train&&PTBotCarryNeedsShop(PhongThanCarryWeight(r.Player),PhongThanCarryCapacity(r.Player))&&BotShopPlan(r,t,now))return;

 if(!r.Config.Train){if(strcmp(r.State,"idle"))npc.SendCommand(do_stand);lstrcpyA(r.State,"idle");t.Target=0;t.Identity=0;t.Map=0;t.Path.clear();r.GoalMap=r.GoalLevel=0;r.GoalEntity=0;return;}
 int story=npc.m_Level<20?BotStory(r,t,now):0;if(story==1)return;
 int progression=story?0:BotProgression(r,t,now);if(progression==1)return;
 if(!story)story=progression==2?2:(npc.m_Level>=20?BotStory(r,t,now):0);if(story==1)return;
 if(!story&&BotHuntBoss(r,t,now))return;
 int daily=story?0:BotDaily(r,t,now);if(daily==1)return;
 if(!story&&!daily&&r.Config.Auto&&npc.m_Level<=20&&StarterQuest(r,t,now))return;
 if(!t.QuestTarget&&BotShopPlan(r,t,now))return;
 if(!story&&!daily&&BotLoot(r,t,now))return;
 if(npc.m_FightMode!=enumFightActive)npc.SetFightMode(enumFightActive);
 if(!Player[r.Player].m_cTeam.GetCreatTeamFlag())Player[r.Player].m_cTeam.SetCreatTeamFlag(r.Player,TRUE);
 if(!t.QuestTarget&&FollowParty(r,t,now))return;
 if(t.Target<=0||!Monster(actor,t.Target)||(!t.QuestTarget&&CompanionBoss(t.Target))||Npc[t.Target].m_dwID!=t.Identity||t.Level!=npc.m_Level){
  if(now-t.Selected>=3000)SelectTraining(r,t,now);
  if(t.Target<=0){if(BotReturnForTraining(r,t,now))return;lstrcpyA(r.State,"seeking_training");return;}
 }
 if(t.Target<=0)return;BotReturnForTraining(r,t,now);
 MeetParty(r,t,now);
 if(BotSummonSkill(r,now))return;
 r.Error[0]=0;int map=SubWorld[npc.m_SubWorldIndex].m_SubWorldID;
 if(map!=t.Map){int edge=-1;if(BotRoute(r,map,t.Map,&edge)<1||edge<0){t.Target=0;lstrcpyA(r.State,"path_blocked");return;}
  lstrcpyA(r.State,"travel_training");MoveTraining(r,t,PTBotEdges[edge].X,PTBotEdges[edge].Y,now);return;
 }
 BotFightTarget(r,t,now);
}
