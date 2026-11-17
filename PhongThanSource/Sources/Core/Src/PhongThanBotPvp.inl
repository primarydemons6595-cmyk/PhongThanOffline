struct BotPvpPlan {
 DWORD actor,entity,until,next,lastHit,lastCall,lastFriend,restoreAt;int target,mode,root,previousCamp,hits,kills;
 BotPvpPlan():actor(0),entity(0),until(0),next(0),lastHit(0),lastCall(0),lastFriend(0),restoreAt(0),target(0),mode(0),root(-1),previousCamp(0),hits(0),kills(0){}
};
static BotPvpPlan botPvp[Capacity];
static std::map<std::string,bool> botFriends[Capacity];static bool botFriendsLoaded[Capacity];
static void BotLoadFriends(int slot){
 if(botFriendsLoaded[slot])return;botFriendsLoaded[slot]=true;char path[160],line[80];sprintf(path,"BotAIgent\\friends\\%s.txt",bots[slot].Config.Id);FILE* f=fopen(path,"rb");if(!f)return;
 while(botFriends[slot].size()<128&&fgets(line,sizeof(line),f)){char id[33];if(sscanf(line,"%32s",id)!=1||strlen(id)!=32)continue;bool valid=true;for(int i=0;i<32;++i)if(PTBotNameHex(id[i])<0)valid=false;if(valid)botFriends[slot][id]=true;}fclose(f);
}
static void BotRememberFriends(Runtime& r,DWORD now){
 int slot=&r-bots;BotPvpPlan& plan=botPvp[slot];if(now-plan.lastFriend<30000)return;plan.lastFriend=now;KPlayer& p=Player[r.Player];if(!p.m_cTeam.m_nFlag)return;BotLoadFriends(slot);bool changed=false;
 for(int index=0;index<listedCount;++index){int other=listedSlots[index];if(other==slot||!bots[other].Player||!SocialAlive(bots[other].Player))continue;KPlayer& q=Player[bots[other].Player];if(!q.m_cTeam.m_nFlag||q.m_cTeam.m_nID!=p.m_cTeam.m_nID||botFriends[slot].find(bots[other].Config.Id)!=botFriends[slot].end()||botFriends[slot].size()>=128)continue;botFriends[slot][bots[other].Config.Id]=true;changed=true;}
 if(changed){CreateDirectoryA("BotAIgent\\friends",0);std::string data;for(std::map<std::string,bool>::iterator i=botFriends[slot].begin();i!=botFriends[slot].end();++i)data+=i->first+"\n";char path[160];sprintf(path,"BotAIgent\\friends\\%s.txt",r.Config.Id);if(Atomic(path,data.data(),data.size()))Log("SOCIAL",r.Config.Id,"real_teammates_remembered_as_friends");}
}
static bool BotSameTeam(int a,int b){return Player[a].m_cTeam.m_nFlag&&Player[b].m_cTeam.m_nFlag&&Player[a].m_cTeam.m_nID==Player[b].m_cTeam.m_nID;}
static bool BotSameNation(int a,int b){return Player[a].m_cTong.m_nFlag&&Player[b].m_cTong.m_nFlag&&Player[a].m_cTong.m_dwTongNameID&&Player[a].m_cTong.m_dwTongNameID==Player[b].m_cTong.m_dwTongNameID;}
static bool BotPvpMap(int map){if(botPolicy.pvpMaps.empty())return true;for(unsigned i=0;i<botPolicy.pvpMaps.size();++i)if(botPolicy.pvpMaps[i]==map)return true;return false;}
static void BotEndPvp(Runtime& r,DWORD now,const char* reason){
 BotPvpPlan& job=botPvp[&r-bots];job.mode=0;job.target=0;job.entity=0;job.until=0;job.root=-1;job.next=now+botPolicy.pvpMinutes*60000UL;
 training[&r-bots].Target=0;training[&r-bots].Identity=0;training[&r-bots].Path.clear();r.GoalEntity=0;Log("PVP",r.Config.Id,reason);
 if(job.previousCamp){if(PhongThanBotRestorePk(r.Player,job.previousCamp))job.previousCamp=0;job.restoreAt=now;}
}
static void BotPvpIncident(int victim,int attacker,int damage){
 if(victim<=0||victim>=MAX_NPC||attacker<=0||attacker>=MAX_NPC||damage<=0||!Npc[victim].IsPlayer())return;
 if(!Npc[attacker].IsPlayer()&&Npc[attacker].m_nOwnerIdx>0&&Npc[attacker].m_nOwnerIdx<MAX_NPC)attacker=Npc[attacker].m_nOwnerIdx;
 if(!Npc[attacker].IsPlayer())return;int vp=Npc[victim].GetPlayerIdx(),ap=Npc[attacker].GetPlayerIdx();if(vp<=0||ap<=0||vp>=MAX_PLAYER||ap>=MAX_PLAYER)return;
 DWORD now=GetTickCount();for(int index=0;index<listedCount;++index){int slot=listedSlots[index];Runtime& r=bots[slot];BotPvpPlan& job=botPvp[slot];
  if(r.Player==ap){++job.hits;}
  if(r.Player!=vp||!botPolicy.defend||BotSameTeam(vp,ap)||BotSameNation(vp,ap))continue;
  bool changed=job.entity!=Npc[attacker].m_dwID||!job.mode;job.actor=Npc[victim].m_dwID;job.mode=1;job.root=slot;job.target=attacker;job.entity=Npc[attacker].m_dwID;job.lastHit=now;job.until=now+180000;
  if(changed){training[slot].Path.clear();char message[160];sprintf(message,"actual_player_damage attacker=%lu damage=%d",job.entity,damage);Log("PVP",r.Config.Id,message);}
 }
}
static void BotCallAllies(Runtime& r,DWORD now){
 int slot=&r-bots;BotPvpPlan& job=botPvp[slot];if(!botPolicy.callAllies||!botPolicy.maxAllies||job.mode==2||!job.target||now-job.lastCall<15000)return;
 KPlayer& p=Player[r.Player];KNpc& actor=Npc[p.m_nIndex];KNpc& enemy=Npc[job.target];
 int foes=0,friends=1;for(int q=1;q<MAX_PLAYER;++q)if(q!=r.Player&&SocialAlive(q)&&Npc[Player[q].m_nIndex].m_SubWorldIndex==actor.m_SubWorldIndex&&NpcSet.GetDistance(p.m_nIndex,Player[q].m_nIndex)<=768){if(NpcSet.GetRelation(p.m_nIndex,Player[q].m_nIndex)==relation_enemy)++foes;else if(BotSameTeam(r.Player,q)||BotSameNation(r.Player,q))++friends;}
 if(!PTBotNeedsAllies(actor.m_CurrentLife,actor.m_CurrentLifeMax,actor.m_Level,enemy.m_Level,foes,friends,Hash(r.Config.Id,32)^job.entity^(now/60000),botPolicy.domination))return;
 job.lastCall=now;BotLoadFriends(slot);std::vector<std::pair<int,int> > candidates;int accepted=0;
 for(int active=0;active<listedCount;++active){int k=listedSlots[active];if(k!=slot&&bots[k].Player&&botPvp[k].mode==2&&botPvp[k].root==slot&&botPvp[k].entity==job.entity&&(int)(now-botPvp[k].until)<0)++accepted;}
 for(int index=0;index<listedCount;++index){int other=listedSlots[index];Runtime& helper=bots[other];if(other==slot||!helper.Player||!helper.Config.Auto||!helper.Config.Enabled||!SocialAlive(helper.Player)||companions[other].Mode||Player[helper.Player].CheckTrading()||Player[helper.Player].GetLockState()||Player[helper.Player].m_PTrade.nTrade||botBossJobs[other].entity)continue;
  if(botPvp[other].mode==2&&botPvp[other].root==slot&&botPvp[other].entity==job.entity)continue;
  KNpc& ally=Npc[Player[helper.Player].m_nIndex];int world=SubWorld[ally.m_SubWorldIndex].m_SubWorldID;if(world>=1067&&world<=1071)continue;
  bool team=BotSameTeam(r.Player,helper.Player),nation=BotSameNation(r.Player,helper.Player),known=botFriends[slot].find(helper.Config.Id)!=botFriends[slot].end();if(!team&&!nation&&!known)continue;
  if(BotSameTeam(helper.Player,enemy.GetPlayerIdx())||BotSameNation(helper.Player,enemy.GetPlayerIdx())||abs(ally.m_Level-enemy.m_Level)>botPolicy.pvpGap||ally.m_CurrentLife<ally.m_CurrentLifeMax*7/10)continue;
  int map=SubWorld[actor.m_SubWorldIndex].m_SubWorldID,hops=BotRoute(helper,world,map);if(hops<0||hops>2)continue;int distance=world==map?NpcSet.GetDistance(Player[helper.Player].m_nIndex,p.m_nIndex):0;if(distance<0||(world==map&&distance>3072))continue;
  candidates.push_back(std::make_pair((team?0:nation?1000:2000)+hops*1000+distance/16,other));
 }
 std::sort(candidates.begin(),candidates.end());
 for(unsigned i=0;i<candidates.size()&&accepted<botPolicy.maxAllies;++i){int other=candidates[i].second;BotPvpPlan& support=botPvp[other];if(support.mode&&support.entity!=job.entity)continue;
  support.actor=Npc[Player[bots[other].Player].m_nIndex].m_dwID;support.target=job.target;support.entity=job.entity;support.mode=2;support.root=slot;support.until=now+180000;training[other].Path.clear();++accepted;Log("PVP",bots[other].Config.Id,"friend_team_or_nation_help_accepted_normal_travel_required");}
 char message[180];sprintf(message,"help_requested target=%lu allies=%d reason=%s",job.entity,accepted,actor.m_CurrentLife<actor.m_CurrentLifeMax*7/10?"low_health":enemy.m_Level>actor.m_Level?"stronger_enemy":foes>friends?"outnumbered":"seek_advantage");Log("PVP",r.Config.Id,message);
}
static bool BotPvpTick(Runtime& r,Training& route,DWORD now){
 int slot=&r-bots;BotPvpPlan& job=botPvp[slot];KPlayer& p=Player[r.Player];KNpc& actor=Npc[p.m_nIndex];int map=SubWorld[actor.m_SubWorldIndex].m_SubWorldID;
 if(job.actor&&job.actor!=actor.m_dwID){int hits=job.hits,kills=job.kills;job=BotPvpPlan();job.hits=hits;job.kills=kills;}job.actor=actor.m_dwID;BotRememberFriends(r,now);
 if(job.previousCamp&&!job.mode&&now-job.restoreAt>=5000){job.restoreAt=now;if(PhongThanBotRestorePk(r.Player,job.previousCamp))job.previousCamp=0;}
 if(p.CheckTrading()||p.GetLockState()||p.m_PTrade.nTrade||p.m_ItemList.Hand()||(map>=1067&&map<=1071))return false;
 // Old builds persisted temporary autonomous camp8. With autonomous PvP
 // disabled, neutralize that stale choice only outside explicit player orders
 // and event locks. Preserve PK points, combat mode and equipment.
 if(!job.mode&&!job.previousCamp&&!botPolicy.pvp&&!PTBotCombatCommand(companions[slot].Mode)&&actor.m_CurrentCamp==8){
  if(PhongThanBotRestorePk(r.Player,7))Log("PVP",r.Config.Id,"stale_autonomous_free_pk_restored_neutral_policy_disabled");
 }
 if(!job.mode){
  if(!botPolicy.pvp||!r.Config.Auto||companions[slot].Mode||actor.m_Level<botPolicy.pvpMinLevel||actor.m_CurrentLife<actor.m_CurrentLifeMax*4/5||botPolicy.pvpMaps.empty()||!BotPvpMap(map))return false;
  if(!job.next){job.next=now+60000+(Hash(r.Config.Id,32)%240000);return false;}if((int)(now-job.next)<0)return false;job.next=now+botPolicy.pvpMinutes*60000UL;
  int chosen=0,best=2147483647;for(int target=1;target<MAX_PLAYER;++target)if(target!=r.Player&&SocialAlive(target)&&!Player[target].CheckTrading()&&!Player[target].m_PTrade.nTrade){KNpc& other=Npc[Player[target].m_nIndex];if(other.m_SubWorldIndex!=actor.m_SubWorldIndex||!other.m_FightMode)continue;int distance=NpcSet.GetDistance(p.m_nIndex,Player[target].m_nIndex);if(distance<0||distance>1024)continue;
   bool opted=Player[target].m_bIsBot||other.m_CurrentCamp==8;
   if(!PTBotCanProvoke(actor.m_Level,other.m_Level,botPolicy.pvpMinLevel,botPolicy.pvpGap,true,true,BotSameTeam(r.Player,target),BotSameNation(r.Player,target),opted))continue;
   if(distance<best){best=distance;chosen=Player[target].m_nIndex;}}
  if(!chosen)return false;job.target=chosen;job.entity=Npc[chosen].m_dwID;job.mode=3;job.root=slot;job.until=now+120000;Log("PVP",r.Config.Id,"occasional_pvp_target_selected_requires_native_pk_permission");
 }
 if((job.mode==1&&!botPolicy.defend)||(job.mode==3&&!botPolicy.pvp)||(job.mode==2&&!botPolicy.callAllies)){BotEndPvp(r,now,"pvp_policy_disabled");return false;}
 int target=job.target,other=target>0&&target<MAX_NPC&&Npc[target].IsPlayer()?Npc[target].GetPlayerIdx():0;
 if((int)(now-job.until)>=0||target<=0||target>=MAX_NPC||Npc[target].m_dwID!=job.entity||!SocialAlive(other)||BotSameTeam(r.Player,other)||BotSameNation(r.Player,other)){BotEndPvp(r,now,"pvp_target_finished_expired_or_no_longer_enemy");return false;}
 BotCallAllies(r,now);
 int dest=SubWorld[Npc[target].m_SubWorldIndex].m_SubWorldID;if(!BotPvpMap(dest)){BotEndPvp(r,now,"pvp_target_left_allowed_maps");return false;}
 if(actor.m_CurrentLife<actor.m_CurrentLifeMax*3/10||Npc[target].m_Level>actor.m_Level+botPolicy.pvpGap){int x,y,tx,ty;actor.GetMpsPos(&x,&y);Npc[target].GetMpsPos(&tx,&ty);BotUseRecovery(r,now,true,true);lstrcpyA(r.State,"pvp_retreating_waiting_allies");MoveTraining(r,route,x+(x>=tx?192:-192),y+(y>=ty?192:-192),now,32);return true;}
 if(map!=dest){int edge=-1;if(BotRoute(r,map,dest,&edge)<1||edge<0){BotEndPvp(r,now,"pvp_no_normal_route_to_target");return false;}lstrcpyA(r.State,"pvp_support_travel");MoveTraining(r,route,PTBotEdges[edge].X,PTBotEdges[edge].Y,now);return true;}
 if(NpcSet.GetRelation(p.m_nIndex,target)!=relation_enemy){
  // Self defence and helping allies never grant permission for free PK.
  if(job.mode!=3||!botPolicy.pvp||botPolicy.pvpMaps.empty()||!BotPvpMap(map)){BotEndPvp(r,now,"pvp_no_autonomous_free_pk_permission");return false;}
  int previous=actor.m_CurrentCamp;
  if(!PhongThanBotRequestPk(r.Player,8)){BotEndPvp(r,now,"pvp_native_pk_rules_rejected_no_attack");return false;}
  // Remember the original camp even when native relation checks reject the attack.
  if(!job.previousCamp)job.previousCamp=previous;
  if(NpcSet.GetRelation(p.m_nIndex,target)!=relation_enemy){BotEndPvp(r,now,"pvp_native_relation_rejected_restore_camp");return false;}
 }
 route.Target=target;route.Identity=job.entity;route.Map=dest;route.Level=actor.m_Level;r.GoalMap=dest;r.GoalEntity=job.entity;r.GoalLevel=Npc[target].m_Level;BotUseRecovery(r,now,true,true);BotFightTarget(r,route,now);
 if(!strcmp(r.State,"training"))lstrcpyA(r.State,job.mode==2?"pvp_supporting_friend":job.mode==1?"pvp_self_defence":"pvp_fighting");return true;
}
static void AppendAutonomyStatus(FILE* f,Runtime& r){
 BotPvpPlan& p=botPvp[&r-bots];int index=r.Player>0&&r.Player<MAX_PLAYER?Player[r.Player].m_nIndex:0;
 int camp=index>0&&index<MAX_NPC?Npc[index].m_CurrentCamp:-1;
 int state=r.Player>0&&r.Player<MAX_PLAYER?Player[r.Player].m_cPK.GetNormalPKState():-1;
 fprintf(f,",\"PvpMode\":%d,\"PvpTarget\":%lu,\"PvpHits\":%d,\"PvpKills\":%d,\"GuildPlan\":%d,\"PkCamp\":%d,\"PkState\":%d",p.mode,p.entity,p.hits,p.kills,botGuildPlans[&r-bots].mode,camp,state);
}
