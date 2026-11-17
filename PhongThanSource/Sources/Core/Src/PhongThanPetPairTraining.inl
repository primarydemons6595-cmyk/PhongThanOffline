struct PT_PET_PAIR_CONSENT {DWORD Actor,Partner,Tick,Signature[2];};
static PT_PET_PAIR_CONSENT ptPetConsent[MAX_PLAYER];
static DWORD PtPetPairSignature(const PT_PET_DATA& p){
 PT_PET_DATA copy=p;
 copy.AgeSeconds=copy.Strength=copy.Day=copy.FeedDay=copy.FeedCount=0;
 ZeroMemory(copy.Log,sizeof(copy.Log));return PtPetHash(&copy,sizeof(copy));
}
static int PtPetSecondPurification(int player){
 if(!Player[player].m_cTeam.m_nFlag)return -5;
 int team=Player[player].m_cTeam.m_nID;
 if(team<0||team>=MAX_TEAM||g_Team[team].m_nMemNum!=1)return -5;
 int first=g_Team[team].m_nCaptain,second=0;
 for(int member=0;member<MAX_TEAM_MEMBER;++member)if(g_Team[team].m_nMember[member]>0){if(second)return -5;second=g_Team[team].m_nMember[member];}
 int other=first==player?second:second==player?first:0;
 if(other<=0||other>=MAX_PLAYER||other==player||!Player[other].m_cTeam.m_nFlag||Player[other].m_cTeam.m_nID!=team)return -5;
 int actor=Player[player].m_nIndex,partner=Player[other].m_nIndex;
 if(actor<=0||actor>=MAX_NPC||partner<=0||partner>=MAX_NPC||
    Npc[actor].m_SubWorldIndex!=Npc[partner].m_SubWorldIndex||NpcSet.GetDistance(actor,partner)>400||
    Player[other].GetLockState()||Player[other].CheckTrading()||!PtPetAtSage(other))return -3;
 PT_PET_RUNTIME* a=PtPetLoad(player);PT_PET_RUNTIME* b=PtPetLoad(other);
 if(!a||!b||a->Journal||b->Journal)return -2;
 PT_PET_DATA next[2];next[0]=a->Data;next[1]=b->Data;
 for(int i=0;i<2;++i)if(!next[i].Owned||next[i].Strength<=0||(next[i].Flags&PT_PET_REST)||
    next[i].PurifyStage!=1||next[i].FeedValue<300||!next[i].Sex)return -5;
 if(next[0].Type!=next[1].Type||next[0].Sex==next[1].Sex)return -5;
 DWORD signatures[2]={PtPetPairSignature(next[0]),PtPetPairSignature(next[1])};
 PT_PET_PAIR_CONSENT& consent=ptPetConsent[player];
 consent.Actor=Npc[actor].m_dwID;consent.Partner=Npc[partner].m_dwID;consent.Tick=GetTickCount();
 consent.Signature[0]=signatures[0];consent.Signature[1]=signatures[1];
 PT_PET_PAIR_CONSENT& reciprocal=ptPetConsent[other];
 if(reciprocal.Actor!=consent.Partner||reciprocal.Partner!=consent.Actor||
    GetTickCount()-reciprocal.Tick>60000||reciprocal.Signature[0]!=signatures[1]||
    reciprocal.Signature[1]!=signatures[0])return 3; // partner must independently confirm
 ZeroMemory(&consent,sizeof(consent));ZeroMemory(&reciprocal,sizeof(reciprocal));
 for(i=0;i<2;++i){next[i].PurifyStage=2;next[i].FeedValue=0;
  for(int attr=0;attr<4;++attr)next[i].Attributes[attr]=next[i].BaseAttributes[attr];
  PtPetLog(next[i],58,2);
 }
 if(!PtPetCommitPair(player,other,next[0],next[1]))return -2;
 return 1;
}
