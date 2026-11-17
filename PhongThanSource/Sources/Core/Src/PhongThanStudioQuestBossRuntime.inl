namespace PtStudioQuest {
struct QuestBossLife {int slot,last,revision,map;DWORD entity,after;QuestBossLife():slot(0),last(-1),revision(-1),map(0),entity(0),after(0){}};
static std::map<int,QuestBossLife> questBossLives;
static bool BossNeeded(const QuestBoss& boss){for(int p=1;p<MAX_PLAYER;++p)if(Live(p))for(unsigned q=0;q<quests.size();++q){const Quest& quest=quests[q];if(!quest.enabled)continue;int stage=Status(p,quest);if(stage>0&&stage<=(int)quest.steps.size()){const Step& s=quest.steps[stage-1];if(s.kind==1&&s.target==boss.type&&s.map==boss.map)return true;}}return false;}
static void BossLocation(int type,int& x,int& y){std::map<int,QuestBossLife>::iterator found=questBossLives.find(type);if(found==questBossLives.end())return;const QuestBossLife& life=found->second;if(life.slot>0&&life.slot<MAX_NPC&&Npc[life.slot].m_dwID==life.entity&&Npc[life.slot].IsAlive()){x=Npc[life.slot].m_OriginX;y=Npc[life.slot].m_OriginY;}}
static void BossPulse(){
 DWORD now=GetTickCount();
 for(std::map<int,QuestBossLife>::iterator it=questBossLives.begin();it!=questBossLives.end();++it){bool found=false;for(unsigned b=0;b<questBosses.size();++b)if(questBosses[b].type==it->first){found=true;break;}QuestBossLife& old=it->second;if(!found&&old.slot>0){if(old.slot<MAX_NPC&&Npc[old.slot].m_dwID==old.entity&&Npc[old.slot].m_NpcSettingIdx==it->first)PhongThanRemoveNpc(old.slot);old.slot=0;old.entity=0;}}
 for(unsigned i=0;i<questBosses.size();++i){const QuestBoss& boss=questBosses[i];QuestBossLife& life=questBossLives[boss.type];bool live=life.slot>0&&life.slot<MAX_NPC&&Npc[life.slot].m_dwID==life.entity&&Npc[life.slot].m_NpcSettingIdx==boss.type;
  if(life.slot&&(!live||!Npc[life.slot].IsAlive())){if(live)PhongThanRemoveNpc(life.slot);live=false;life.slot=0;life.entity=0;life.after=now+boss.respawn*1000;}
  if(live&&life.map!=boss.map){PhongThanRemoveNpc(life.slot);live=false;life.slot=0;life.entity=0;}
  if(!boss.enabled||!BossNeeded(boss)){if(live)PhongThanRemoveNpc(life.slot);life.slot=0;life.entity=0;continue;}
  if(live&&Npc[life.slot].IsAlive())continue;
  if(live){PhongThanRemoveNpc(life.slot);life.slot=0;life.entity=0;life.after=now+boss.respawn*1000;}
  if(life.after&&(long)(now-life.after)<0)continue;int world=g_SubWorldSet.SearchWorld(boss.map);if(world<0)continue;
  int count=boss.spots.size(),at=g_Random(count);if(count>1&&at==life.last)at=(at+1+g_Random(count-1))%count;const QuestBossSpot& spot=boss.spots[at];int npc=PhongThanSpawnNpc(boss.type,boss.level,world,spot.x,spot.y,FALSE);if(npc<=0)continue;
  life.slot=npc;life.entity=Npc[npc].m_dwID;life.last=at;life.revision=loadedRevision;life.map=boss.map;life.after=0;
  for(int p=1;p<MAX_PLAYER;++p)if(Live(p))for(unsigned q=0;q<quests.size();++q){Quest& quest=quests[q];int stage=Status(p,quest);if(stage>0&&stage<=(int)quest.steps.size()&&quest.steps[stage-1].kind==1&&quest.steps[stage-1].target==boss.type)Journal(p,quest);}
 }
}
}
