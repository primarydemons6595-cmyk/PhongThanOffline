// Authoritative activity adapters. Eligibility is read from the live mission,
// authored NPC callbacks and owned tickets; no task/reward/entry state is forged.
struct BotActivityProgress {DWORD next,last,progress;int map,target,life;BotActivityProgress():next(0),last(0),progress(0),map(0),target(0),life(0){}};
static BotActivityProgress botVttProgress[Capacity],botBossProgress[Capacity];
static KMission* BotVttMission(int id,int* world=0){int w=g_SubWorldSet.SearchWorld(1066+id);if(world)*world=w;return w>=0&&w<MAX_SUBWORLD?SubWorld[w].m_MissionArray.FindByMissionId(id):0;}
static bool BotVttMember(KPlayer& p,KMission* m){return m&&m->GetMissionPlayer_DataIndex(p.Name,p.GetPlayerIndex(),p.m_dwID)>0;}
static bool BotVttTicket(Runtime& r,int id,KMission* m){
 KPlayer& p=Player[r.Player];if(m&&m->GetMissionValue(1)>0&&p.m_cTask.GetSaveVal(4789+id)==m->GetMissionValue(1))return true;
 int genre=3,detail=61+id,particular=0,level=0,series=0,count=1;
 if(PhongThanEventOwns(id)){genre=PhongThanEventValue(id,3);detail=PhongThanEventValue(id,4);particular=PhongThanEventValue(id,5);level=PhongThanEventValue(id,6);series=PhongThanEventValue(id,7);count=PhongThanEventValue(id,8);}
 PhongThanQuestItemTuple ticket;if(count>0&&PhongThanDecodeQuestItemTuple(genre,detail,particular,level,series,0,ticket)&&BotStoryItemCount(p,ticket)>=count)return true;
 for(PlayerItem* owned=p.m_ItemList.GetFirstItem();owned;owned=p.m_ItemList.GetNextItem())if(BotBag(owned->nPlace)&&owned->nIdx>0&&owned->nIdx<MAX_ITEM){KItem& item=Item[owned->nIdx];if(PhongThanIsVttStone(item.GetGenre(),item.GetDetailType(),item.GetParticular())&&item.GetDetailType()==143+id&&!item.GetLock()->IsLock()&&PhongThanVttStoneValid(item.GetParam(),(int)time(0))&&(!item.m_CommonAttrib.nExpireTime||item.m_CommonAttrib.nExpireTime>KSG_GetCurSec()))return true;}
 return false;
}
static bool BotActivityWants(Runtime& r,KItem& item){
 if(!r.Config.Auto)return false;KPlayer& p=Player[r.Player];int id=PTBotVttBand(Npc[p.m_nIndex].m_Level);if(!id||(g_TaskGlobalValue[id]!=1&&g_TaskGlobalValue[id]!=2)||BotVttTicket(r,id,BotVttMission(id)))return false;
 int genre=3,detail=61+id,particular=0,level=0,series=0;if(PhongThanEventOwns(id)){genre=PhongThanEventValue(id,3);detail=PhongThanEventValue(id,4);particular=PhongThanEventValue(id,5);level=PhongThanEventValue(id,6);series=PhongThanEventValue(id,7);}
 PhongThanQuestItemTuple ticket;return PhongThanDecodeQuestItemTuple(genre,detail,particular,level,series,0,ticket)&&PhongThanQuestItemMatches(item,ticket);
}
static int BotActivityNeed(Runtime& r,KItem& item){if(!BotActivityWants(r,item))return 0;int id=PTBotVttBand(Npc[Player[r.Player].m_nIndex].m_Level),count=PhongThanEventOwns(id)?PhongThanEventValue(id,8):1;return count>0&&count<65?count:1;}
static bool BotVisitActivity(Runtime& r,Training& route,int npc,const char* primary,const char* secondary,DWORD now,BotActivityProgress& progress){
 if(npc<=0||npc>=MAX_NPC||Npc[npc].m_Kind!=kind_dialoger||!Npc[npc].IsAlive()||Npc[npc].m_SubWorldIndex<0||Npc[npc].m_SubWorldIndex>=MAX_SUBWORLD)return false;
 KPlayer& p=Player[r.Player];KNpc& actor=Npc[p.m_nIndex];int map=SubWorld[actor.m_SubWorldIndex].m_SubWorldID,dest=SubWorld[Npc[npc].m_SubWorldIndex].m_SubWorldID,x=0,y=0;Npc[npc].GetMpsPos(&x,&y);
 if(map!=dest){int edge=-1;if(BotRoute(r,map,dest,&edge)<1||edge<0)return false;lstrcpyA(r.State,"event_travel");MoveTraining(r,route,PTBotEdges[edge].X,PTBotEdges[edge].Y,now);return true;}
 if(NpcSet.GetDistance(p.m_nIndex,npc)>80){lstrcpyA(r.State,"event_travel");MoveTraining(r,route,x,y,now,64);return true;}
 route.Path.clear();actor.SendCommand(do_stand);lstrcpyA(r.State,"event_dialog");if(now-progress.last<2000)return true;progress.last=now;
 if(p.m_UiDialogToken&&p.m_nLastNpcIndex==npc){int choice=primary?QuestChoice(p,primary):-1;if(choice<0&&secondary)choice=QuestChoice(p,secondary);/* A previous result page may not offer this action: reopen the NPC menu below. */if(choice>=0){PHONGTHAN_UI_CHOICE_REQUEST pick;ZeroMemory(&pick,sizeof(pick));PhongThanInitializeWireHeader(&pick.Header,PHONGTHAN_MSG_UI_CHOICE_REQUEST,sizeof(pick),PHONGTHAN_WIRE_FLAG_REQUEST,0);pick.MapId=map;pick.DialogToken=p.m_UiDialogToken;pick.View=p.m_UiDialogView;pick.Selection=choice;p.ProcessPlayerSelectFromUI((BYTE*)&pick);return true;}}
 PHONGTHAN_ENTITY_REFERENCE interact;ZeroMemory(&interact,sizeof(interact));PhongThanInitializeWireHeader(&interact.Header,PHONGTHAN_MSG_NPC_INTERACT_REQUEST,sizeof(interact),PHONGTHAN_WIRE_FLAG_REQUEST,0);interact.MapId=map;interact.EntityId=Npc[npc].m_dwID;p.DialogNpc((BYTE*)&interact);return true;
}
static bool BotVtt(Runtime& r,Training& route,DWORD now){
 if(!r.Config.Auto)return false;KPlayer& p=Player[r.Player];KNpc& actor=Npc[p.m_nIndex];int current=SubWorld[actor.m_SubWorldIndex].m_SubWorldID,id=current>=1067&&current<=1070?current-1066:PTBotVttBand(actor.m_Level);if(!id)return false;
 BotActivityProgress& progress=botVttProgress[&r-bots];int world=-1;KMission* mission=BotVttMission(id,&world);if(!mission)return false;
 bool inside=current==1066+id&&BotVttMember(p,mission);if(!inside&&!botPolicy.vtt)return false;int phase=g_TaskGlobalValue[id];if(!inside&&(phase!=1&&phase!=2))return false;
 if(!inside){
  if((int)(now-progress.next)<0)return false;if(!BotVttTicket(r,id,mission)){lstrcpyA(r.Error,"vtt_needs_owned_ticket_or_valid_stone");return false;}
  PtVttGates::Gate& gate=PtVttGates::gates[id];if(!gate.known||gate.index<=0||gate.index>=MAX_NPC||Npc[gate.index].m_dwID!=gate.entity)return false;
  int map=SubWorld[actor.m_SubWorldIndex].m_SubWorldID;if(progress.map!=map){progress.map=map;progress.progress=now;}if(!progress.progress)progress.progress=now;
  if(now-progress.progress>180000){progress.next=now+60000;progress.progress=0;route.Path.clear();Log("PLAN",r.Config.Id,"vtt_entry_no_progress_trying_other_work");return false;}
  bool handled=BotVisitActivity(r,route,gate.index,"pt_enter","renwu3",now,progress);if(!handled)progress.next=now+30000;return handled;
 }
 route.QuestTarget=route.QuestMap=0;route.Map=current;route.Level=actor.m_Level;
 if(phase!=2){route.Path.clear();route.Target=0;actor.SendCommand(do_stand);lstrcpyA(r.State,phase==1?"vtt_waiting_start":"vtt_finished");BotUseRecovery(r,now,actor.m_CurrentLife<actor.m_CurrentLifeMax/2,actor.m_CurrentManaMax>0&&actor.m_CurrentMana<actor.m_CurrentManaMax/3);return true;}
 BotUseRecovery(r,now,(__int64)actor.m_CurrentLife*100<(__int64)actor.m_CurrentLifeMax*65,actor.m_CurrentManaMax>0&&(__int64)actor.m_CurrentMana*100<(__int64)actor.m_CurrentManaMax*30);
 int target=0,best=2147483647,chest=0,gateNpc=0;DWORD chestScript=g_FileName2Id("\\script\\phongthan\\van_tien\\pearl_chest.lua"),gateScript=g_FileName2Id("\\script\\phongthan\\van_tien\\inner_gate.lua");
 for(int n=NpcSet.GetNextIdx(0);n;n=NpcSet.GetNextIdx(n))if(Npc[n].m_SubWorldIndex==world&&Npc[n].IsAlive()&&(Npc[n].m_Kind==kind_dialoger||Npc[n].m_CurrentLife>0)&&Npc[n].m_RegionIndex>=0){
  if(Npc[n].m_Kind==kind_dialoger){if(Npc[n].m_ActionScriptID==chestScript&&!chest)chest=n;if(Npc[n].m_ActionScriptID==gateScript)gateNpc=n;continue;}
  if(Npc[n].m_Kind!=kind_normal||Npc[n].Owner[0]||!mission->GetMissionNpc_DataIndex(n,Npc[n].m_dwID)||NpcSet.GetRelation(p.m_nIndex,n)!=relation_enemy)continue;
  if(route.BadIdentity==Npc[n].m_dwID&&(int)(now-route.BadUntil)<0)continue;int distance=NpcSet.GetDistance(p.m_nIndex,n);if(distance>=0&&distance<best){best=distance;target=n;}
 }
 if((__int64)actor.m_CurrentLife*100<(__int64)actor.m_CurrentLifeMax*25&&target){int x=0,y=0,tx=0,ty=0;actor.GetMpsPos(&x,&y);Npc[target].GetMpsPos(&tx,&ty);lstrcpyA(r.State,"retreating");MoveTraining(r,route,x+(x>=tx?192:-192),y+(y>=ty?192:-192),now,32);return true;}
 if(target){
  if(progress.target!=target||progress.life!=Npc[target].m_CurrentLife){progress.target=target;progress.life=Npc[target].m_CurrentLife;progress.progress=now;}
  if(now-progress.progress>90000||route.Failures>=3){route.BadIdentity=Npc[target].m_dwID;route.BadUntil=now+30000;route.Path.clear();route.Target=0;progress.target=0;return true;}
  route.Target=target;route.Identity=Npc[target].m_dwID;r.GoalMap=current;r.GoalLevel=Npc[target].m_Level;r.GoalEntity=route.Identity;BotFightTarget(r,route,now);return true;
 }
 CompanionTrainScope scope(r);if(BotLoot(r,route,now,false,true))return true;
 if(chest&&BotVisitActivity(r,route,chest,0,0,now,progress))return true;
 if(gateNpc&&BotVisitActivity(r,route,gateNpc,"pt_open","pt_enter",now,progress))return true;
 route.Path.clear();actor.SendCommand(do_stand);lstrcpyA(r.State,"vtt_seeking_objective");return true;
}
static bool BotNeedsBossGear(Runtime& r){KPlayer& p=Player[r.Player];int level=Npc[p.m_nIndex].m_Level;if(level<30)return false;int weak=0;for(int part=0;part<6&&part<itempart_num;++part){int index=p.m_ItemList.GetEquipment(part);if(index<=0||index>=MAX_ITEM){++weak;continue;}KItem& item=Item[index];int required=0;for(int q=0;q<6;++q)if(item.TooltipRequirements()[q].nAttribType==magic_requirelevel)required=item.TooltipRequirements()[q].nValue[0];if(item.GetPhongThanQuality()<1||required+20<level)++weak;}return weak>=2;}
struct BotBossJob {int target,leader,templateId,map,fought;DWORD entity,until;BotBossJob():target(0),leader(-1),templateId(0),map(0),fought(0),entity(0),until(0){}};
static BotBossJob botBossJobs[Capacity];
static int BotBossGear(Runtime& r){KPlayer& p=Player[r.Player];__int64 sum=0;for(int part=0;part<6&&part<itempart_num;++part){int item=p.m_ItemList.GetEquipment(part);if(item>0&&item<MAX_ITEM)sum+=BotGearScore(p,Item[item]);}return sum>2147480000?2147480000:(int)sum;}
static bool BotBossAvoids(Runtime& r,int target){
 char path[180],key[64];sprintf(path,"BotAIgent\\agenda\\%s.ini",r.Config.Id);sprintf(key,"Boss_%d_%d",SubWorld[Npc[target].m_SubWorldIndex].m_SubWorldID,Npc[target].m_NpcSettingIdx);
 int oldLevel=GetPrivateProfileIntA(key,"FailedLevel",0,path),oldGear=GetPrivateProfileIntA(key,"FailedGear",0,path);return oldLevel>0&&!PTBotBossStronger(Npc[Player[r.Player].m_nIndex].m_Level,BotBossGear(r),oldLevel,oldGear);
}
static void BotBossFailed(Runtime& r,DWORD now){
 BotBossJob failed=botBossJobs[&r-bots];if(!failed.entity)return;
 for(int i=0;i<count;++i)if(botBossJobs[i].entity==failed.entity){Runtime& member=bots[i];if(member.Player&&companions[i].Mode==0){char path[180],key[64],value[32];sprintf(path,"BotAIgent\\agenda\\%s.ini",member.Config.Id);sprintf(key,"Boss_%d_%d",failed.map,failed.templateId);sprintf(value,"%d",Npc[Player[member.Player].m_nIndex].m_Level);WritePrivateProfileStringA(key,"FailedLevel",value,path);sprintf(value,"%d",BotBossGear(member));WritePrivateProfileStringA(key,"FailedGear",value,path);training[i].Target=0;training[i].Identity=0;training[i].Path.clear();botBossProgress[i].next=now+300000;Log("PLAN",member.Config.Id,"autonomous_boss_failed_avoid_until_level_or_gear_improves");}botBossJobs[i]=BotBossJob();}
}
static bool BotBossFriendAvailable(Runtime& r,Runtime& other,int destination){
 if(&r==&other||!other.Config.Auto||!other.Config.Enabled||!other.Player||!SocialAlive(other.Player)||companions[&other-bots].Mode||botBossJobs[&other-bots].entity||BotOwnedRecoveryCount(Player[other.Player],1)<5)return false;
 KNpc& a=Npc[Player[r.Player].m_nIndex];KNpc& b=Npc[Player[other.Player].m_nIndex];if(abs(a.m_Level-b.m_Level)>5||(__int64)b.m_CurrentLife*100<(__int64)b.m_CurrentLifeMax*70)return false;
 if(strcmp(other.State,"training")&&strcmp(other.State,"travel_training")&&strcmp(other.State,"seeking_training")&&strcmp(other.State,"idle"))return false;
 KPlayer& p=Player[other.Player];if(p.m_cTeam.m_nFlag){int team=p.m_cTeam.m_nID;if(team<0||team>=MAX_TEAM||g_Team[team].m_nCaptain<=0||!Player[g_Team[team].m_nCaptain].m_bIsBot)return false;}
 return BotRoute(other,SubWorld[b.m_SubWorldIndex].m_SubWorldID,destination)>=0;
}
static bool BotCompanionVttAllowed(Runtime& r,char* answer,int size){
 KPlayer& p=Player[r.Player];int id=PTBotVttBand(Npc[p.m_nIndex].m_Level);
 if(!id){lstrcpynA(answer,"\103\312\160\040\164\165\151\040\143\150\255\141\040\164\150\165\351\143\040\153\150\157\266\156\147\040\156\150\313\156\040\143\361\141\040\126\271\156\040\124\151\252\156\040\124\162\313\156\040\256\141\156\147\040\150\347\040\164\162\356\054\040\143\150\255\141\040\256\151\040\256\255\356\143\040\253\156\147\056",size);return false;}
 KMission* mission=BotVttMission(id);if(BotVttMember(p,mission))return true;
 if(!mission||(g_TaskGlobalValue[id]!=1&&g_TaskGlobalValue[id]!=2)){lstrcpynA(answer,"\124\162\313\156\040\256\363\156\147\040\143\312\160\040\164\165\151\040\143\150\255\141\040\155\353\040\156\150\313\156\040\156\147\255\352\151\056\040\115\327\156\150\040\143\150\352\040\154\255\356\164\040\155\353\040\156\150\320\056",size);return false;}
 if(!BotVttTicket(r,id,mission)){lstrcpynA(answer,"\124\165\151\040\256\361\040\143\312\160\040\156\150\255\156\147\040\164\150\151\325\165\040\166\320\040\150\157\306\143\040\256\270\040\126\271\156\040\124\151\252\156\040\124\162\313\156\040\150\356\160\040\154\326\056\040\244\156\147\040\147\151\141\157\040\144\336\143\150\040\256\255\141\040\166\320\040\256\363\156\147\040\164\162\313\156\040\147\151\363\160\040\156\150\320\056",size);return false;}return true;
}
static bool BotCompanionVtt(Runtime& r,Training& t,DWORD now){
 CompanionState& c=companions[&r-bots];char reason[256];lstrcpynA(reason,"\110\325\164\040\147\151\352\040\256\151\040\143\357\156\147\040\162\345\151\054\040\143\307\156\040\164\150\327\040\253\156\147\040\156\150\276\156\040\154\271\151\040\156\150\320\056",sizeof(reason));if((int)(now-c.Until)>0||!BotCompanionVttAllowed(r,reason,sizeof(reason))){CompanionTell(r,c.Owner,reason);ResetCompanion(r);return false;}
 int automatic=r.Config.Auto;r.Config.Auto=1;bool handled=BotVtt(r,t,now);r.Config.Auto=automatic;
 int map=SubWorld[Npc[Player[r.Player].m_nIndex].m_SubWorldIndex].m_SubWorldID;if(map>=1067&&map<=1070&&!botMarket[&r-bots].arrived){botMarket[&r-bots].arrived=1;CompanionTell(r,c.Owner,"\124\165\151\040\256\267\040\166\265\157\040\126\271\156\040\124\151\252\156\040\124\162\313\156\040\256\363\156\147\040\143\312\160\040\162\345\151\056\040\124\165\151\040\154\265\155\040\155\364\143\040\164\151\252\165\040\164\162\157\156\147\040\164\162\313\156\040\156\150\320\056");CompanionEvent(r,"vtt_entered","native mission membership and map confirmed");}
 if(!handled){t.Path.clear();Npc[Player[r.Player].m_nIndex].SendCommand(do_stand);lstrcpyA(r.State,"vtt_entry_blocked");CompanionNotice(r,"vtt_entry_blocked","\124\165\151\040\143\150\255\141\040\166\265\157\040\256\255\356\143\040\143\346\156\147\040\164\162\313\156\054\040\256\141\156\147\040\143\150\352\040\256\151\322\165\040\153\151\326\156\040\150\157\306\143\040\256\255\352\156\147\040\164\355\151\040\143\346\156\147\056",now);}return true;
}
static void BotBossFormTeam(Runtime& r){
 int slot=&r-bots;BotBossJob& job=botBossJobs[slot];if(!job.entity||job.leader<0||job.leader>=count)return;
 Runtime& leader=bots[job.leader];if(!leader.Player||!SocialAlive(leader.Player))return;KPlayer& captain=Player[leader.Player];
 if(captain.m_cTeam.m_nFlag&&captain.m_cTeam.m_nFigure!=TEAM_CAPTAIN)return;
 if(!captain.m_cTeam.m_nFlag){captain.m_cTeam.SetCreatTeamFlag(leader.Player,TRUE);PLAYER_APPLY_CREATE_TEAM create;ZeroMemory(&create,sizeof(create));create.ProtocolType=c2s_teamapplycreate;if(!captain.m_cTeam.CreateTeam(leader.Player,&create))return;}
 int team=captain.m_cTeam.m_nID;if(team<0||team>=MAX_TEAM||g_Team[team].CheckFull())return;g_Team[team].SetTeamOpen();
 static DWORD invited[Capacity];DWORD now=GetTickCount();if(now-invited[job.leader]<3000)return;invited[job.leader]=now;
 for(int k=0;k<count&&!g_Team[team].CheckFull()&&g_Team[team].m_nMemNum<3;++k)if(k!=job.leader&&bots[k].Player&&botBossJobs[k].entity==job.entity&&SocialAlive(bots[k].Player)){
  int member=bots[k].Player;if(Player[member].m_cTeam.m_nFlag||Npc[Player[member].m_nIndex].m_SubWorldIndex!=Npc[captain.m_nIndex].m_SubWorldIndex||NpcSet.GetDistance(captain.m_nIndex,Player[member].m_nIndex)>256)continue;
  Player[member].m_cTeam.SetCreatTeamFlag(member,TRUE);TEAM_INVITE_ADD_COMMAND invite;ZeroMemory(&invite,sizeof(invite));invite.m_dwNpcID=Npc[Player[member].m_nIndex].m_dwID;captain.m_cTeam.InviteAdd(leader.Player,&invite,member);
  if(Player[member].m_cTeam.m_nFlag&&Player[member].m_cTeam.m_nID==team)Log("PARTY",bots[k].Config.Id,"boss_rally_real_team_join_confirmed");
 }
}
static bool BotHuntBoss(Runtime& r,Training& route,DWORD now){
 if(!botPolicy.boss||!r.Config.Auto||companions[&r-bots].Mode||(!BotNeedsBossGear(r)&&!botBossJobs[&r-bots].entity))return false;BotActivityProgress& progress=botBossProgress[&r-bots];if((int)(now-progress.next)<0)return false;
 KPlayer& p=Player[r.Player];KNpc& actor=Npc[p.m_nIndex];if(BotOwnedRecoveryCount(p,1)<5)return false;int map=SubWorld[actor.m_SubWorldIndex].m_SubWorldID,target=0,best=2147483647;
 static DWORD refreshed=0;static std::vector<int> bosses;static std::vector<DWORD> identities;
 if(!refreshed||now-refreshed>=3000){refreshed=now;bosses.clear();identities.clear();for(int npc=NpcSet.GetNextIdx(0);npc;npc=NpcSet.GetNextIdx(npc))if(Npc[npc].m_CurrentLife>0&&Npc[npc].m_RegionIndex>=0&&CompanionBoss(npc)){bosses.push_back(npc);identities.push_back(Npc[npc].m_dwID);}}
 for(int entry=0;entry<(int)bosses.size();++entry){int n=bosses[entry];if(Npc[n].m_dwID!=identities[entry]||!CompanionBoss(n)||BotBossAvoids(r,n))continue;if(botBossJobs[&r-bots].entity&&botBossJobs[&r-bots].entity!=Npc[n].m_dwID)continue;int destination=SubWorld[Npc[n].m_SubWorldIndex].m_SubWorldID;if(!n||!Monster(p.m_nIndex,n)||Npc[n].m_Level>actor.m_Level+5||Npc[n].m_Level+15<actor.m_Level)continue;int hops=BotRoute(r,map,destination);if(hops<0)continue;
  int company=1;for(int k=0;k<count;++k)if(&bots[k]!=&r&&bots[k].Player&&SocialAlive(bots[k].Player)&&Npc[Player[bots[k].Player].m_nIndex].m_Level>=actor.m_Level-5&&Npc[Player[bots[k].Player].m_nIndex].m_Level<=actor.m_Level+5&&SubWorld[Npc[Player[bots[k].Player].m_nIndex].m_SubWorldIndex].m_SubWorldID==destination)++company;
  int captainRoom=PlayerSet.m_cLeadExp.GetMemNumFromLevel(p.m_nLeadLevel)+1;for(int friendSlot=0;friendSlot<count;++friendSlot)if(BotBossFriendAvailable(r,bots[friendSlot],destination)&&!BotBossAvoids(bots[friendSlot],n)){if(company<3)++company;int room=PlayerSet.m_cLeadExp.GetMemNumFromLevel(Player[bots[friendSlot].Player].m_nLeadLevel)+1;if(room>captainRoom)captainRoom=room;}if(company>captainRoom)company=captainRoom;if((__int64)Npc[n].m_CurrentLifeMax>(__int64)actor.m_CurrentLifeMax*(company>=3?150:20))continue;
  int score=PTBotObjectiveScore(PTBOT_BOSS,hops,Npc[n].m_Level>actor.m_Level?(Npc[n].m_Level-actor.m_Level)*5:0,0,Hash(r.Config.Id,32)^Npc[n].m_dwID,true);if(score>=0&&score<best){best=score;target=n;}
 }
 if(!target){botBossJobs[&r-bots]=BotBossJob();return false;}
 BotBossJob& job=botBossJobs[&r-bots];if(!job.entity){job.target=target;job.entity=Npc[target].m_dwID;job.templateId=Npc[target].m_NpcSettingIdx;job.map=SubWorld[Npc[target].m_SubWorldIndex].m_SubWorldID;job.leader=&r-bots;int captainRoom=PlayerSet.m_cLeadExp.GetMemNumFromLevel(p.m_nLeadLevel);for(int candidate=0;candidate<count;++candidate)if(BotBossFriendAvailable(r,bots[candidate],job.map)&&!BotBossAvoids(bots[candidate],target)){int room=PlayerSet.m_cLeadExp.GetMemNumFromLevel(Player[bots[candidate].Player].m_nLeadLevel);if(room>captainRoom){captainRoom=room;job.leader=candidate;}}job.until=now+90000;int invited=0;if(job.leader!=(int)(&r-bots)){botBossJobs[job.leader]=job;training[job.leader].Target=0;training[job.leader].Path.clear();++invited;}for(int k=0;k<count&&invited<2&&invited<captainRoom;++k)if(BotBossFriendAvailable(r,bots[k],job.map)&&!BotBossAvoids(bots[k],target)){botBossJobs[k]=job;training[k].Target=0;training[k].Path.clear();++invited;}Log("PLAN",r.Config.Id,invited?"autonomous_boss_group_rally_planned":"autonomous_boss_solo_risk_accepted");}
 if(progress.target!=target||progress.life!=Npc[target].m_CurrentLife){progress.target=target;progress.life=Npc[target].m_CurrentLife;progress.progress=now;}
 if(now-progress.progress>180000||route.Failures>=3){if(job.fought)BotBossFailed(r,now);else{job=BotBossJob();progress.next=now+120000;}route.Target=0;route.Path.clear();return false;}
 route.Target=target;route.Identity=Npc[target].m_dwID;route.Map=SubWorld[Npc[target].m_SubWorldIndex].m_SubWorldID;route.Level=actor.m_Level;route.QuestTarget=route.QuestMap=0;r.GoalMap=route.Map;r.GoalLevel=Npc[target].m_Level;r.GoalEntity=route.Identity;
 if(map!=route.Map){int edge=-1;if(BotRoute(r,map,route.Map,&edge)<1||edge<0)return false;lstrcpyA(r.State,"boss_travel");MoveTraining(r,route,PTBotEdges[edge].X,PTBotEdges[edge].Y,now);return true;}
 BotBossFormTeam(r);int members=1;KPlayer& self=Player[r.Player];for(int k=0;k<count;++k)if(&bots[k]!=&r&&bots[k].Player&&botBossJobs[k].entity==job.entity&&SocialAlive(bots[k].Player)&&self.m_cTeam.m_nFlag&&Player[bots[k].Player].m_cTeam.m_nFlag&&self.m_cTeam.m_nID==Player[bots[k].Player].m_cTeam.m_nID&&Npc[Player[bots[k].Player].m_nIndex].m_SubWorldIndex==actor.m_SubWorldIndex&&NpcSet.GetDistance(p.m_nIndex,Player[bots[k].Player].m_nIndex)<=1024)++members;
 if((__int64)Npc[target].m_CurrentLifeMax>(__int64)actor.m_CurrentLifeMax*20&&members<3){if((int)(now-job.until)>0){job=BotBossJob();progress.next=now+120000;route.Target=0;route.Path.clear();return false;}int tx,ty;Npc[target].GetMpsPos(&tx,&ty);lstrcpyA(r.State,"boss_group_rally");MoveTraining(r,route,tx-384,ty-384,now,128);return true;}
 if((__int64)actor.m_CurrentLife*100<(__int64)actor.m_CurrentLifeMax*35&&BotOwnedRecoveryCount(p,1)<2){BotBossFailed(r,now);lstrcpyA(r.State,"retreating");return false;}
 job.fought=1;BotFightTarget(r,route,now);return true;
}
