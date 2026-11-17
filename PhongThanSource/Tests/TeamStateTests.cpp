#include <windows.h>
#include <stdio.h>
#include <string.h>
#define _SERVER
#define MAX_PLAYER 5
#define MAX_NPC 5
#define MAX_TEAM 3
#define MAX_TEAM_MEMBER 7
#define MAX_SUBWORLD 2
#include "../Headers/PhongThanTeamProtocol.h"
struct ProfessionMock {int value;int GetProfession(){return value;}};
struct PlayerMock {int m_nIndex,m_nNetConnectIdx;ProfessionMock m_cProfession;struct {int m_nID,m_nFlag;}m_cTeam;}Player[MAX_PLAYER];
struct NpcMock {DWORD m_dwID;int m_CurrentLife,m_CurrentLifeMax,m_Level,m_SubWorldIndex,m_nProfession;char Name[32];}Npc[MAX_NPC];
struct World {int m_SubWorldID;}SubWorld[MAX_SUBWORLD];
struct KTeam {int m_nCaptain,m_nMember[7];DWORD generation;DWORD GetChatGeneration(){return generation;}}g_Team[MAX_TEAM];
struct Server {PT_TEAM_STATE packet;int count;void PackDataToClient(int,BYTE* data,int){memcpy(&packet,data,sizeof(packet));++count;}}server,*g_pServer=&server;
#include "../Sources/Core/Src/PhongThanTeamState.inl"
#define CHECK(c) if(!(c)){printf("FAIL line %d\n",__LINE__);return 1;}
int main(){
 CHECK(sizeof(PT_TEAM_QUERY)==60);CHECK(sizeof(PT_TEAM_STATE)==524);
 CHECK(PhongThanValidTeamName("Phong Than"));CHECK(!PhongThanValidTeamName("   "));CHECK(!PhongThanValidTeamName("<red>"));CHECK(!PhongThanValidTeamName("x\ny"));
 char bad[32];memset(bad,'x',32);CHECK(!PhongThanValidTeamName(bad));
 for(int i=1;i<3;++i){Player[i].m_nIndex=i;Player[i].m_cTeam.m_nFlag=1;Player[i].m_cTeam.m_nID=1;Npc[i].m_dwID=100+i;Npc[i].m_CurrentLife=80-i;Npc[i].m_CurrentLifeMax=100;Npc[i].m_Level=10;Player[i].m_cProfession.value=i-1;strcpy(Npc[i].Name,i==1?"Leader":"Member");}
 g_Team[1].m_nCaptain=1;g_Team[1].m_nMember[0]=2;g_Team[1].generation=7;SubWorld[0].m_SubWorldID=10;
 PT_TEAM_QUERY q;ZeroMemory(&q,sizeof(q));q.ActorId=101;q.Rename=1;strcpy(q.Name,"Team A");
 PhongThanSendTeamState(1,&q);CHECK(server.count==1&&server.packet.Count==2&&server.packet.Result==2);CHECK(!strcmp(server.packet.Name,"Team A"));CHECK(server.packet.Members[1].Life==78);CHECK(server.packet.Members[0].Profession==0&&server.packet.Members[1].Profession==1);
 q.ActorId=102;strcpy(q.Name,"Hijack");PhongThanSendTeamState(2,&q);CHECK(server.count==2&&server.packet.Result==1&&!strcmp(server.packet.Name,"Team A"));
 q.ActorId=101;strcpy(q.Name,"<tag>");PhongThanSendTeamState(1,&q);CHECK(server.count==3&&server.packet.Result==1&&!strcmp(server.packet.Name,"Team A"));
 q.ActorId=999;PhongThanSendTeamState(1,&q);CHECK(server.count==3);PhongThanSendTeamState(0,&q);CHECK(server.count==3);
 q.ActorId=101;q.Rename=0;s_PtTeamRequestTime[1]=GetTickCount()-1000;g_Team[1].generation=8;
 PhongThanSendTeamState(1,&q);CHECK(server.count==4&&server.packet.Name[0]==0); // reused team slot cannot inherit name
 Player[1].m_cTeam.m_nFlag=0;s_PtTeamRequestTime[1]=GetTickCount()-1000;PhongThanSendTeamState(1,&q);CHECK(server.count==5&&server.packet.Count==0); // disband clears HUD
 Player[1].m_cTeam.m_nFlag=1;Player[1].m_cTeam.m_nID=2;g_Team[2].m_nCaptain=2;s_PtTeamRequestTime[1]=GetTickCount()-1000;PhongThanSendTeamState(1,&q);CHECK(server.count==5); // membership checked
 Player[1].m_cTeam.m_nID=1;g_Team[1].generation=9;q.ActorId=101;q.Rename=1;strcpy(q.Name,"Visible team");PhongThanSendTeamState(1,&q);CHECK(server.packet.ShowName==1);
 char overhead[32];PhongThanCopyTeamName(1,overhead,sizeof(overhead));CHECK(!strcmp(overhead,"Visible team"));
 q.Rename=3;PhongThanSendTeamState(1,&q);CHECK(server.packet.ShowName==0&&!strcmp(server.packet.Name,"Visible team"));PhongThanCopyTeamName(1,overhead,sizeof(overhead));CHECK(!overhead[0]);
 q.ActorId=102;q.Rename=2;PhongThanSendTeamState(2,&q);CHECK(server.packet.Result==1&&server.packet.ShowName==0);
 q.ActorId=101;q.Rename=2;PhongThanSendTeamState(1,&q);CHECK(server.packet.ShowName==1);PhongThanCopyTeamName(1,overhead,sizeof(overhead));CHECK(!strcmp(overhead,"Visible team"));
 g_Team[1].generation=10;q.Rename=0;s_PtTeamRequestTime[1]=GetTickCount()-1000;PhongThanSendTeamState(1,&q);CHECK(server.packet.ShowName==1&&!server.packet.Name[0]);
 puts("PASS team state: packet sizes, name validation, leader authority, exact HP, actor/membership validation, generation isolation, disband");return 0;
}
