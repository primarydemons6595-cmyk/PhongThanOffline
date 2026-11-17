static void BountySetup(){Setup();SubWorld[0].m_SubWorldID=1020;Npc[1].m_Level=90;Npc[2].m_NpcSettingIdx=854;Npc[1].x=Npc[2].x=45184;Npc[1].y=Npc[2].y=99072;PtStudioQuest::questBossLives.clear();questBossSpawnCount=questBossRemoveCount=0;questRoll=0;}
static void BountyKill(int type,int world=1){Npc[3].m_SubWorldIndex=world;Npc[3].m_NpcSettingIdx=type;Npc[3].m_CurrentLife=0;Npc[3].m_dwID++;Npc[1].m_SubWorldIndex=world;PhongThanStudioQuestRevived(3);PhongThanStudioQuestKilled(3,1);}
static int BountyCases(){
 CHECK(SetCurrentDirectoryA("../../bounty/native-bounty"));g_SetRootPath(NULL);g_SetFilePath("\\");BountySetup();
 CHECK(PtStudioQuest::quests.size()==1&&PtStudioQuest::questBosses.size()==4);PtStudioQuest::Quest* q=PtStudioQuest::Find(1);CHECK(q&&q->dailyLimit==3&&q->randomBranches);
 for(int turn=0;turn<3;++turn){
  questRoll=turn;CHECK(PtStudioQuest::Act(1,1,0,1)==1);int first=turn*4+1;CHECK(PtStudioQuest::Status(1,*q)==first);CHECK(!PtStudioQuest::Act(1,1,1,1));
  PtStudioQuest::BossPulse();CHECK(questBossSpawnCount==turn);int target=q->steps[first-1].target;questBossTestMap=q->steps[first-1].map;SubWorld[1].m_SubWorldID=questBossTestMap;
  BountyKill(q->steps[first+2].target);CHECK(PtStudioQuest::Status(1,*q)==first);for(int k=0;k<8;++k)BountyKill(target);CHECK(PtStudioQuest::Status(1,*q)==first+1);
  Npc[1].m_SubWorldIndex=0;ServiceSeed(12,1);CHECK(!PtStudioQuest::Act(1,1,1,1));PtStudioQuest::BossPulse();CHECK(questBossSpawnCount==turn);
  questRoll=99;BountyKill(target);CHECK(PtStudioQuest::Owned(1,q->steps[first].needs[0].row)==0);questRoll=0;BountyKill(target);CHECK(PtStudioQuest::Owned(1,q->steps[first].needs[0].row)==1);
  Npc[1].m_SubWorldIndex=0;CHECK(PtStudioQuest::Act(1,1,1,1)==1&&PtStudioQuest::Status(1,*q)==first+2);CHECK(PtStudioQuest::Owned(1,q->steps[first].needs[0].row)==0);PtStudioQuest::BossPulse();CHECK(questBossSpawnCount==turn);
  CHECK(PtStudioQuest::Act(1,1,1,1)==1&&PtStudioQuest::Status(1,*q)==first+3);PtStudioQuest::BossPulse();CHECK(questBossSpawnCount==turn+1);int type=q->steps[first+2].target;PtStudioQuest::QuestBossLife& life=PtStudioQuest::questBossLives[type];int slot=life.slot;CHECK(slot>0&&Npc[slot].m_NpcSettingIdx==type);int x=0,y=0;PtStudioQuest::BossLocation(type,x,y);CHECK(x&&y);std::string description=PtStudioQuest::Description(1,*q);char coord[64];sprintf(coord,"(%d,%d)",x/256,y/512);CHECK(description.find(coord)!=std::string::npos);
  // Saving unrelated Studio settings must not despawn a living fight.
  int removed=questBossRemoveCount;++PtStudioQuest::loadedRevision;PtStudioQuest::BossPulse();CHECK(life.slot==slot&&questBossRemoveCount==removed);
  Npc[1].m_SubWorldIndex=1;Npc[slot].m_CurrentLife=0;PhongThanStudioQuestRevived(slot);PhongThanStudioQuestKilled(slot,1);CHECK(PtStudioQuest::Status(1,*q)==17);PtStudioQuest::BossPulse();CHECK(!life.slot&&life.after);
  Npc[1].m_SubWorldIndex=0;CHECK(PtStudioQuest::Act(1,1,1,1)==2);CHECK(Player[1].m_cTask.GetSaveVal(5230)==(turn+1)*5&&PtStudioQuest::DailyUsed(1,*q)==turn+1);CHECK(!PtStudioQuest::Act(1,1,1,1));
 }
 CHECK(!PtStudioQuest::Act(1,1,0,1)&&Player[1].m_cTask.GetSaveVal(5230)==15);Player[1].m_cTask.SetSaveVal(5247,((PtStudioQuest::Day()-1)<<6)|3,TRUE);CHECK(PtStudioQuest::Act(1,1,0,1));CHECK(PtStudioQuest::DailyUsed(1,*q)==0);
 puts("PASS bounty: persisted random route, eight guards, 25% exact clue, delivery + interrogation before boss spawn, live coordinates, three completions/15 points per day, reset and no replay");
 // Final step has a pool of locations and never immediately reuses its last one.
 Player[1].m_cTask.SetSaveVal(PtStudioQuest::FirstTask,PtStudioQuest::Pack(*q,4),TRUE);questBossTestMap=q->steps[3].map;SubWorld[1].m_SubWorldID=questBossTestMap;PtStudioQuest::QuestBossLife& life=PtStudioQuest::questBossLives[q->steps[3].target];int last=life.last;life.after=0;PtStudioQuest::BossPulse();CHECK(life.slot&&life.last!=last);int oldSlot=life.slot;PtStudioQuest::questBosses.clear();PtStudioQuest::BossPulse();CHECK(!life.slot&&!Npc[oldSlot].m_dwID);
 puts("PASS random boss: distinct next location, configured revival delay, no early spawn, orphan cleanup and unrelated configuration saves preserve ongoing combat");return 0;
}
