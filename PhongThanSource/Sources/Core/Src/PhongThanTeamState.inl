#include "../../../Headers/PhongThanTeamProtocol.h"
#ifndef _SERVER
static PT_TEAM_STATE s_PtTeamState;
static DWORD s_PtTeamQueryTime=0,s_PtTeamReceiveTime=0;
void PhongThanRequestTeamState(const char* name){
    int actor=Player[CLIENT_PLAYER_INDEX].m_nIndex;if(actor<=0||actor>=MAX_NPC||!g_pClient)return;
    PT_TEAM_QUERY q;ZeroMemory(&q,sizeof(q));
    PhongThanInitializeWireHeader(&q.Header,PT_TEAM_REQUEST,sizeof(q),PHONGTHAN_WIRE_FLAG_REQUEST,0);
    q.ActorId=Npc[actor].m_dwID;
    if(name){lstrcpyn(q.Name,name,sizeof(q.Name));if(!PhongThanValidTeamName(q.Name))return;q.Rename=1;}
    g_pClient->SendPackToServer(&q,sizeof(q));s_PtTeamQueryTime=GetTickCount();
}
void PhongThanSetTeamNameVisible(bool visible){
    int actor=Player[CLIENT_PLAYER_INDEX].m_nIndex;if(actor<=0||actor>=MAX_NPC||!g_pClient)return;
    PT_TEAM_QUERY q;ZeroMemory(&q,sizeof(q));PhongThanInitializeWireHeader(&q.Header,PT_TEAM_REQUEST,sizeof(q),PHONGTHAN_WIRE_FLAG_REQUEST,0);
    q.ActorId=Npc[actor].m_dwID;q.Rename=visible?2:3;g_pClient->SendPackToServer(&q,sizeof(q));
}
int PhongThanGetTeamState(PT_TEAM_STATE* state){
    int actor=Player[CLIENT_PLAYER_INDEX].m_nIndex;if(actor<=0||actor>=MAX_NPC)return 0;
    if(GetTickCount()-s_PtTeamQueryTime>=1000)PhongThanRequestTeamState(0);
    if(s_PtTeamState.ActorId!=Npc[actor].m_dwID||GetTickCount()-s_PtTeamReceiveTime>5000)return 0;
    if(state)*state=s_PtTeamState;return s_PtTeamState.Count;
}
#else
struct PT_TEAM_NAME_RECORD {DWORD Generation;char Name[32];bool Hidden;};
static PT_TEAM_NAME_RECORD s_PtTeamNames[MAX_TEAM];
static DWORD s_PtTeamRequestTime[MAX_PLAYER];
void PhongThanCopyTeamName(int player,char* name,int capacity){
    if(!name||capacity<1)return;name[0]=0;
    if(player<=0||player>=MAX_PLAYER||!Player[player].m_cTeam.m_nFlag)return;
    int team=Player[player].m_cTeam.m_nID;if(team<0||team>=MAX_TEAM)return;
    const PT_TEAM_NAME_RECORD& record=s_PtTeamNames[team];
    if(!record.Hidden&&record.Generation==g_Team[team].GetChatGeneration())lstrcpyn(name,record.Name,capacity);
}
static void PhongThanSendTeamState(int player,const PT_TEAM_QUERY* query){
    if(player<=0||player>=MAX_PLAYER)return;
    int actor=Player[player].m_nIndex;if(actor<=0||actor>=MAX_NPC||query->ActorId!=Npc[actor].m_dwID)return;
    DWORD now=GetTickCount();if(!query->Rename && now-s_PtTeamRequestTime[player]<250)return;s_PtTeamRequestTime[player]=now;
    PT_TEAM_STATE state;ZeroMemory(&state,sizeof(state));
    PhongThanInitializeWireHeader(&state.Header,PT_TEAM_SNAPSHOT,sizeof(state),PHONGTHAN_WIRE_FLAG_RESPONSE,query->Header.Sequence);
    state.ActorId=Npc[actor].m_dwID;
    int team=Player[player].m_cTeam.m_nID;
    if(Player[player].m_cTeam.m_nFlag&&team>=0&&team<MAX_TEAM&&g_Team[team].m_nCaptain>0){
        KTeam& t=g_Team[team];int captain=t.m_nCaptain;
        bool belongs=captain==player;for(int j=0;j<MAX_TEAM_MEMBER;++j)if(t.m_nMember[j]==player)belongs=true;
        if(!belongs)return;
        state.Generation=t.GetChatGeneration();
        PT_TEAM_NAME_RECORD& record=s_PtTeamNames[team];
        if(record.Generation!=state.Generation){record.Generation=state.Generation;record.Name[0]=0;record.Hidden=false;}
        if(query->Rename){
            if(captain!=player||query->Rename>3||(query->Rename==1&&!PhongThanValidTeamName(query->Name)))state.Result=1;
            else {if(query->Rename==1)lstrcpyn(record.Name,query->Name,sizeof(record.Name));else record.Hidden=query->Rename==3;state.Result=2;}
        }
        state.ShowName=!record.Hidden;
        lstrcpyn(state.Name,record.Name,sizeof(state.Name));
        for(int slot=-1;slot<MAX_TEAM_MEMBER&&state.Count<8;++slot){
            int p=slot<0?captain:t.m_nMember[slot];if(p<=0||p>=MAX_PLAYER)continue;
            int n=Player[p].m_nIndex;if(n<=0||n>=MAX_NPC||!Npc[n].m_dwID||Player[p].m_cTeam.m_nID!=team||!Player[p].m_cTeam.m_nFlag)continue;
            PT_TEAM_MEMBER& m=state.Members[state.Count++];m.Id=Npc[n].m_dwID;m.Life=Npc[n].m_CurrentLife;m.MaxLife=Npc[n].m_CurrentLifeMax;m.Level=Npc[n].m_Level;m.Profession=Player[p].m_cProfession.GetProfession();
            int w=Npc[n].m_SubWorldIndex;m.MapId=w>=0&&w<MAX_SUBWORLD?SubWorld[w].m_SubWorldID:0;lstrcpyn(m.Name,Npc[n].Name,sizeof(m.Name));
            if(slot<0)state.CaptainId=m.Id;
        }
    }else if(query->Rename)state.Result=1;
    g_pServer->PackDataToClient(Player[player].m_nNetConnectIdx,(BYTE*)&state,sizeof(state));
}
#endif
