#include "../../../Headers/PhongThanRemotePeople.h"
#ifndef _SERVER
static PT_PEOPLE_COMMAND s_PtPeopleLast;
static PT_PEOPLE_RESULT s_PtPeopleResult;
static DWORD s_PtPeopleSequence=0,s_PtPeopleReceived=0;
int PhongThanRemotePeopleAction(const PT_PEOPLE_COMMAND* source){
 int a=Player[CLIENT_PLAYER_INDEX].m_nIndex;
 if(!source||!g_pClient||a<=0||a>=MAX_NPC||!PtPeopleActionAllowed(source->Action)||!PtPeopleNameValid(source->Name))return 0;
 int w=Npc[a].m_SubWorldIndex;if(w<0||w>=MAX_SUBWORLD)return 0;
 PT_PEOPLE_COMMAND q=*source;q.ActorId=Npc[a].m_dwID;q.MapId=SubWorld[w].m_SubWorldID;
 PhongThanInitializeWireHeader(&q.Header,PT_PEOPLE_QUERY,sizeof(q),PHONGTHAN_WIRE_FLAG_REQUEST,++s_PtPeopleSequence);
 s_PtPeopleLast=q;s_PtPeopleReceived=0;g_pClient->SendPackToServer(&q,sizeof(q));return 1;
}
int PhongThanRemotePeopleGet(PT_PEOPLE_RESULT* out){
 int a=Player[CLIENT_PLAYER_INDEX].m_nIndex;if(!out||!s_PtPeopleReceived||a<=0||a>=MAX_NPC)return 0;
 int w=Npc[a].m_SubWorldIndex;if(w<0||w>=MAX_SUBWORLD||s_PtPeopleLast.ActorId!=Npc[a].m_dwID||s_PtPeopleLast.MapId!=(DWORD)SubWorld[w].m_SubWorldID)return 0;
 *out=s_PtPeopleResult;return 1;
}
static void PhongThanRemotePeopleReceive(const PT_PEOPLE_RESULT& r){
 if(r.Header.Sequence!=s_PtPeopleSequence||r.ActorId!=s_PtPeopleLast.ActorId||r.MapId!=s_PtPeopleLast.MapId||
    r.Action!=s_PtPeopleLast.Action||!PtPeopleNameValid(r.Name)||strcmp(r.Name,s_PtPeopleLast.Name)||r.Result < -4||r.Result>2)return;
 s_PtPeopleResult=r;s_PtPeopleReceived=GetTickCount();
}
#else
static bool PtPeopleOnline(int p,bool allowServerBot=false){
 if(p<=0||p>=MAX_PLAYER)return false;
 int n=Player[p].m_nIndex;
 return Player[p].m_PlayerDBLoad&&Player[p].m_dwID&&n>0&&n<MAX_NPC&&(Player[p].m_nNetConnectIdx>=0||(allowServerBot&&Player[p].m_bIsBot))&&
  !Player[p].m_bExchangeServer&&Npc[n].m_dwID&&Npc[n].m_Kind==kind_player&&Npc[n].m_RegionIndex>=0&&
  Npc[n].m_SubWorldIndex>=0&&Npc[n].m_SubWorldIndex<MAX_SUBWORLD;
}
static void PhongThanRemotePeopleRequest(int p,const PT_PEOPLE_COMMAND& q){
 if(!PtPeopleOnline(p)||!PtPeopleActionAllowed(q.Action)||!PtPeopleNameValid(q.Name))return;
 int a=Player[p].m_nIndex,w=Npc[a].m_SubWorldIndex;
 if(q.ActorId!=Npc[a].m_dwID||q.MapId!=(DWORD)SubWorld[w].m_SubWorldID)return;
 static DWORD last[MAX_PLAYER],owner[MAX_PLAYER];DWORD now=GetTickCount();
 if(owner[p]==Player[p].m_dwID&&now-last[p]<200)return;owner[p]=Player[p].m_dwID;last[p]=now;
 PT_PEOPLE_RESULT r;ZeroMemory(&r,sizeof(r));r.ActorId=q.ActorId;r.MapId=q.MapId;r.Action=q.Action;r.TeamId=-1;lstrcpyn(r.Name,q.Name,32);
 PhongThanInitializeWireHeader(&r.Header,PT_PEOPLE_REPLY,sizeof(r),PHONGTHAN_WIRE_FLAG_RESPONSE,q.Header.Sequence);
 int target=0;for(int i=1;i<MAX_PLAYER;++i)if(PtPeopleOnline(i,true)&&!stricmp(Npc[Player[i].m_nIndex].Name,q.Name)){target=i;break;}
 if(target&&q.TargetId&&Npc[Player[target].m_nIndex].m_dwID!=q.TargetId)target=0;
 if(!target)r.Result=0;
 else if(target==p)r.Result=-1;
 else{
  r.TargetId=Npc[Player[target].m_nIndex].m_dwID;r.MenuState=Player[target].m_cMenuState.m_nState;
  if(Player[target].m_cTeam.m_nFlag)r.TeamId=Player[target].m_cTeam.m_nID;
  r.Result=1;
  if(q.Action==3||q.Action==4||q.Action==6){
   if(Player[p].CheckTrading()||Player[p].GetLockState())r.Result=-2;
   else if(q.Action==6){Player[target].SendEquipItemInfo(p);r.Result=2;}
   else if(q.Action==3){
    int t=Player[target].m_cTeam.m_nID;
    if(!Player[target].m_cTeam.m_nFlag||Player[target].m_cTeam.m_nFigure!=TEAM_CAPTAIN||t<0||t>=MAX_TEAM||!g_Team[t].IsOpen())r.Result=-4;
    else{PLAYER_APPLY_ADD_TEAM add;ZeroMemory(&add,sizeof(add));add.m_dwTarNpcID=r.TargetId;r.Result=Player[p].S2CSendAddTeamInfo((BYTE*)&add,target)?2:-4;}
   }else{
    int t=Player[p].m_cTeam.m_nID;
    if(!Player[p].m_cTeam.m_nFlag||Player[p].m_cTeam.m_nFigure!=TEAM_CAPTAIN||t<0||t>=MAX_TEAM||!g_Team[t].IsOpen()||!Player[target].m_cTeam.GetCreatTeamFlag())r.Result=-4;
    else{TEAM_INVITE_ADD_COMMAND add;ZeroMemory(&add,sizeof(add));add.m_dwNpcID=r.TargetId;Player[p].m_cTeam.InviteAdd(p,&add,target);r.Result=2;}
   }
  }
 }
 g_pServer->PackDataToClient(Player[p].m_nNetConnectIdx,&r,sizeof(r));
}
#endif
