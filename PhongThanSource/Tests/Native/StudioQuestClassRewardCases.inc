static int ClassRewardCases(){
 for(int c=0;c<3;++c){Setup();PtStudioQuest::Quest* source=PtStudioQuest::Find(6);CHECK(source&&source->classRewards.size()==3);PtStudioQuest::Quest* q=PtStudioQuest::Find(4);q->classRewards=source->classRewards;Npc[1].m_Series=c;
  CHECK(PtStudioQuest::Act(1,4,0,1)==1);CHECK(PtStudioQuest::Act(1,4,1,1)==2);
  CHECK(Player[1].xp==101+c&&Player[1].cash==1001+c);CHECK(Player[1].m_cTask.GetSaveVal(TASKVALUE_PT_BOUND_MONEY)==11+c&&Player[1].m_cTask.GetSaveVal(TASKVALUE_STATTASK_REPUTE)==21+c);
  CHECK((GetTitleQualifyWord(1,0)&(1UL<<(1+c)))!=0);
  for(int other=0;other<3;++other){PhongThanQuestExchangeRow row=source->classRewards[other].items[0];CHECK(PtStudioQuest::Owned(1,row)==(other==c?c+1:0));}
  CHECK(!PtStudioQuest::Act(1,4,1,1)&&Player[1].cash==1001+c);
 }
 puts("PASS same shared quest: each profession receives only its own items, EXP, silver, bound silver, repute and title; duplicate claim rejected");
 Setup();PtStudioQuest::Quest* q=PtStudioQuest::Find(4);q->classRewards=PtStudioQuest::Find(6)->classRewards;q->classRewards.erase(q->classRewards.begin()+1);Npc[1].m_Series=1;
 CHECK(PtStudioQuest::Act(1,4,0,1)&&PtStudioQuest::Act(1,4,1,1)==2&&Player[1].xp==1&&Player[1].cash==0);
 Setup();q=PtStudioQuest::Find(4);q->classRewards=PtStudioQuest::Find(6)->classRewards;CHECK(PtStudioQuest::Act(1,4,0,1));int before=PtStudioQuest::Raw(1,*q);Player[1].m_ItemList.failSearch=true;
 CHECK(!PtStudioQuest::Act(1,4,1,1)&&PtStudioQuest::Raw(1,*q)==before&&Player[1].cash==0&&Player[1].xp==0&&Player[1].m_cTask.GetSaveVal(TASKVALUE_PT_BOUND_MONEY)==0&&GetTitleQualifyWord(1,0)==0);
 Player[1].m_ItemList.failSearch=false;CHECK(PtStudioQuest::Act(1,4,1,1)==2&&Player[1].cash==1001);
 puts("PASS common fallback and per-profession full-bag rollback/retry without duplicate or mixed rewards");return 0;
}
