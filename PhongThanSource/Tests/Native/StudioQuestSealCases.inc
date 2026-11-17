static int SealCases(){
 Setup();CHECK(PtStudioQuest::Act(1,9,0,1));PtStudioQuest::Quest* q=PtStudioQuest::Find(9);
 CHECK(PtStudioQuest::SealOps::Has(1,1)&&PtStudioQuest::SealOps::Remaining(1,1)==5);
 CHECK(!PtStudioQuest::Act(1,9,2,1)&&PtStudioQuest::SealOps::Remaining(1,1)==5);
 PhongThanStudioQuestKilled(3,1);CHECK(PtStudioQuest::Owned(1,q->steps[0].needs[0].row)==1);
 questSeconds+=6;CHECK(!PtStudioQuest::SealOps::Has(1,1));PhongThanStudioQuestRevived(3);PhongThanStudioQuestKilled(3,1);CHECK(PtStudioQuest::Owned(1,q->steps[0].needs[0].row)==1);
 CHECK(PtStudioQuest::Act(1,9,2,1)&&PtStudioQuest::SealOps::Remaining(1,1)==5);PhongThanStudioQuestRevived(3);PhongThanStudioQuestKilled(3,1);CHECK(PtStudioQuest::Owned(1,q->steps[0].needs[0].row)==2);
 puts("PASS timed seal: native buff grant, no early refresh, monster loot stops at expiry and resumes after reissue at quest NPC");
 Setup();PHONGTHAN_IBBUFF_STORE* full=GetPlayerIBBuffStore(1);for(int i=0;i<48;++i){full->Entry[i].nId=300+i;full->Entry[i].nTimes=1;full->Entry[i].dwExpireTime=questSeconds+100;}CHECK(!PtStudioQuest::Act(1,9,0,1)&&PtStudioQuest::Raw(1,*PtStudioQuest::Find(9))==0);
 Setup();CHECK(PtStudioQuest::Act(1,9,0,1));q=PtStudioQuest::Find(9);q->steps[0].consumeSeal=1;Player[1].m_ItemList.failSearch=true;PhongThanStudioQuestKilled(3,1);CHECK(PtStudioQuest::SealOps::Has(1,1)&&PtStudioQuest::Owned(1,q->steps[0].needs[0].row)==0);Player[1].m_ItemList.failSearch=false;PhongThanStudioQuestRevived(3);PhongThanStudioQuestKilled(3,1);CHECK(!PtStudioQuest::SealOps::Has(1,1)&&PtStudioQuest::Owned(1,q->steps[0].needs[0].row)==1);
 puts("PASS buff slot-full rejection and consume-only-after-success; full bag retains buff and quest state");
 Setup();Player[1].m_ItemList.capacity=1;CHECK(!PtStudioQuest::Act(1,10,0,1)&&PtStudioQuest::Raw(1,*PtStudioQuest::Find(10))==0&&!PtStudioQuest::SealOps::Has(1,2));
 Setup();CHECK(!PtStudioQuest::SealOps::Has(1,2));CHECK(PtStudioQuest::Act(1,10,0,1));q=PtStudioQuest::Find(10);CHECK(PtStudioQuest::SealOps::Has(1,2)&&PtStudioQuest::SealOps::Remaining(1,2)==20);
 int tagged=0;for(i=1;i<MAX_ITEM;++i)if(Item[i].id&&Item[i].GetParam()==PhongThanQuestSealParam(2))tagged=i;CHECK(tagged>0&&Item[1].GetParam()==0);
 PhongThanQuestExchangeRow cost;cost.item=q->steps[0].needs[0].row.item;CHECK(PhongThanDecodeQuestItemTuple(4,26,0,0,0,0,cost.item));cost.count=2;
 CHECK(!PhongThanQuestExchange(1,5390,0,1,&cost,1,0,0)&&Item[1].id&&Item[tagged].id); // plain + private seal must not satisfy plain quantity two
 int wrongParam[1]={PhongThanQuestSealParam(1)};cost.count=1;CHECK(!PhongThanQuestExchange(1,5390,0,1,&cost,1,0,0,false,0,true,0,true,wrongParam));
 CHECK(PhongThanQuestExchange(1,5390,0,1,&cost,1,0,0)&&!Item[1].id&&Item[tagged].id);
 int expired=tagged;questSeconds+=21;CHECK(!PtStudioQuest::SealOps::Has(1,2));CHECK(PtStudioQuest::Act(1,10,2,1));CHECK(PtStudioQuest::SealOps::Has(1,2)&&Item[expired].id);
 int obj=PtStudioGather::spawned[1].slot;std::string text;CHECK(PtStudioGather::Start(1,10,1,obj,text)==1);g_SubWorldSet.tick=PtStudioGather::sessions[1].deadline;Player[1].m_nPaceBarTime=0;Player[1].m_ItemList.failSearch=true;
 CHECK(!PtStudioGather::Finish(1,1,text)&&PtStudioQuest::SealOps::Has(1,2));Player[1].m_ItemList.failSearch=false;
 CHECK(PtStudioGather::Start(1,10,1,obj,text)==1);g_SubWorldSet.tick=PtStudioGather::sessions[1].deadline;Player[1].m_nPaceBarTime=0;
 CHECK(PtStudioGather::Finish(1,1,text)==1&&!PtStudioQuest::SealOps::Has(1,2)&&Item[expired].id&&PtStudioQuest::Status(1,*q)==2);
 puts("PASS private F4 seal: separate param, exact expiry, base/other-seal exclusion, reissue, full-bag rollback and valid-seal-only atomic consumption");return 0;
}
