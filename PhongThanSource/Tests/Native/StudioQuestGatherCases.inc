static int GatherCases(){
 std::string text;Setup();questRoll=0;
 CHECK(PtStudioQuest::Act(1,7,0,1));PtStudioQuest::Quest* q=PtStudioQuest::Find(7);
 Npc[3].m_NpcSettingIdx=901;PhongThanStudioQuestKilled(3,1);CHECK(PtStudioQuest::Owned(1,q->steps[0].needs[0].row)==0);Npc[3].m_NpcSettingIdx=900;
 PhongThanStudioQuestRevived(3);PhongThanStudioQuestKilled(3,1);CHECK(PtStudioQuest::Owned(1,q->steps[0].needs[0].row)==1);
 PhongThanStudioQuestKilled(3,1);CHECK(PtStudioQuest::Owned(1,q->steps[0].needs[0].row)==1);
 PhongThanStudioQuestRevived(3);PhongThanStudioQuestKilled(3,1);CHECK(PtStudioQuest::Owned(1,q->steps[0].needs[0].row)==2);
 PhongThanStudioQuestRevived(3);PhongThanStudioQuestKilled(3,1);CHECK(PtStudioQuest::Owned(1,q->steps[0].needs[0].row)==2);
 CHECK(PtStudioQuest::Act(1,7,1,1)==2);
 puts("PASS quest drops: exact monster, accepted stage only, duplicate-life suppression and stop at required quantity");
 Setup();q=PtStudioQuest::Find(8);CHECK(PtStudioQuest::Act(1,8,0,1));int obj=PtStudioGather::spawned[1].slot;CHECK(obj>0);
 CHECK(!PtStudioGather::Start(1,8,1,obj,text)&&Player[1].m_nPaceBarTime==0); // missing seal
 PhongThanStudioQuestKilled(3,1);CHECK(PtStudioQuest::Owned(1,q->steps[0].keys[0].row)==1);CHECK((PtStudioQuest::Raw(1,*q)&65535)==0); // receipt bit must not count as gathering
 CHECK(PtStudioGather::Start(1,8,1,obj,text)==1);CHECK(Player[1].m_nPaceBarTime==3*GAME_FPS);
 CHECK(!PtStudioGather::Finish(1,1,text)&&PtStudioQuest::Owned(1,q->steps[0].keys[0].row)==1); // early completion
 Npc[1].x++;CHECK(!PtStudioGather::Finish(1,1,text)&&!PtStudioGather::sessions[1].role);Npc[1].x--;
 CHECK(PtStudioGather::Start(1,8,1,obj,text)==1);PhongThanStudioQuestGatherDamaged(1);CHECK(!PtStudioGather::sessions[1].role&&Player[1].m_nPaceBarTime==0);
 CHECK(PtStudioGather::Start(1,8,1,obj,text)==1);g_SubWorldSet.tick=PtStudioGather::sessions[1].deadline;Player[1].m_nPaceBarTime=0;Player[1].m_ItemList.failSearch=true;
 int before=PtStudioQuest::Owned(1,q->steps[0].needs[0].row);CHECK(!PtStudioGather::Finish(1,1,text));CHECK(PtStudioQuest::Owned(1,q->steps[0].keys[0].row)==1&&PtStudioQuest::Owned(1,q->steps[0].needs[0].row)==before&&Object[obj].m_nState==0);Player[1].m_ItemList.failSearch=false;
 CHECK(PtStudioGather::Start(1,8,1,obj,text)==1);g_SubWorldSet.tick=PtStudioGather::sessions[1].deadline;Player[1].m_nPaceBarTime=0;
 CHECK(PtStudioGather::Finish(1,1,text)==1);CHECK(PtStudioQuest::Owned(1,q->steps[0].keys[0].row)==0&&PtStudioQuest::Owned(1,q->steps[0].needs[0].row)==before+1&&(PtStudioQuest::Raw(1,*q)&65535)==1&&Object[obj].m_nState==1);
 CHECK(!PtStudioGather::Finish(1,1,text)&&PtStudioQuest::Owned(1,q->steps[0].needs[0].row)==before+1);
 Object[obj].SetState(OBJ_PROP_STATE_DISPLAY);PhongThanStudioQuestRevived(3);PhongThanStudioQuestKilled(3,1);CHECK((PtStudioQuest::Raw(1,*q)&65535)==1&&PtStudioQuest::Status(1,*q)==1);
 CHECK(PtStudioGather::Start(1,8,1,obj,text)==1);g_SubWorldSet.tick=PtStudioGather::sessions[1].deadline;Player[1].m_nPaceBarTime=0;CHECK(PtStudioGather::Finish(1,1,text)==1&&PtStudioQuest::Status(1,*q)==2);
 puts("PASS native collection: seal prerequisites/drop receipt, original pace timer, movement/damage interruption, early/repeated completion, full-bag rollback, seal consumption, object depletion and step progression");
 Setup();q=PtStudioQuest::Find(8);q->steps[0].keys.clear();CHECK(PtStudioQuest::Act(1,8,0,1));obj=PtStudioGather::spawned[1].slot;
 Player[2]=Player[1];Player[2].m_dwID=2002;Player[2].m_nIndex=5;ZeroMemory(&Player[2].m_ItemList,sizeof(Player[2].m_ItemList));Player[2].m_ItemList.capacity=12;Npc[5]=Npc[1];Npc[5].m_dwID=2002;
 CHECK(PtStudioGather::Start(1,8,1,obj,text)==1&&PtStudioGather::Start(2,8,1,obj,text)==1);
 g_SubWorldSet.tick=PtStudioGather::sessions[1].deadline;Player[1].m_nPaceBarTime=Player[2].m_nPaceBarTime=0;
 CHECK(PtStudioGather::Finish(1,1,text)==1);CHECK(!PtStudioGather::Finish(2,1,text)&&PtStudioQuest::Owned(2,q->steps[0].needs[0].row)==0&&(PtStudioQuest::Raw(2,*q)&65535)==0);
 puts("PASS shared collection point: two simultaneous sessions yield one successful claim, no duplicate item or progress");return 0;
}
