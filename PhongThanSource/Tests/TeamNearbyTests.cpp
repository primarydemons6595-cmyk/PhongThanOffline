#include <windows.h>
#include <stdio.h>
#include <string.h>
#define CLIENT_PLAYER_INDEX 0
#define MAX_TEAM_MEMBER 7
#define kind_player 1
struct KUiPlayerItem {char Name[32];DWORD uId;int nIndex,nData;};
struct NPC {int m_Kind,m_RegionIndex,m_SubWorldIndex;DWORD m_dwID;char Name[32];}Npc[7];
struct PlayerMock {int m_nIndex;struct {int m_nFlag;}m_cTeam;}Player[1];
struct TeamMock {int m_nCaptain,m_nMember[7];}g_Team[1];
class KNpcSet {struct Link {int GetNext(int i){return i<6?i+1:0;}}m_UseIdx;public:int GetAroundPlayerForTeamInvite(KUiPlayerItem*,int);};
#include "TeamNearbyProduction.inl"
#define CHECK(c) if(!(c)){printf("FAIL line %d\n",__LINE__);return 1;}
int main(){
 KNpcSet set;Player[0].m_nIndex=1;
 for(int i=1;i<=6;++i){Npc[i].m_Kind=kind_player;Npc[i].m_dwID=100+i;Npc[i].m_SubWorldIndex=0;strcpy(Npc[i].Name,"Player");}
 Npc[2].m_Kind=0;Npc[4].m_RegionIndex=-1;Npc[6].m_SubWorldIndex=1;
 g_Team[0].m_nCaptain=103; // stale membership from previous party must not hide a nearby player
 CHECK(set.GetAroundPlayerForTeamInvite(0,0)==2);
 KUiPlayerItem out[3];memset(out,0x7f,sizeof(out));CHECK(set.GetAroundPlayerForTeamInvite(out,1)==1);CHECK(out[0].uId==103&&out[1].uId==0x7f7f7f7f);
 CHECK(set.GetAroundPlayerForTeamInvite(out,3)==2&&out[1].uId==105);
 Player[0].m_cTeam.m_nFlag=1;CHECK(set.GetAroundPlayerForTeamInvite(0,0)==1);CHECK(set.GetAroundPlayerForTeamInvite(out,3)==1&&out[0].uId==105);
 CHECK(set.GetAroundPlayerForTeamInvite(0,1)==0);Player[0].m_nIndex=0;CHECK(set.GetAroundPlayerForTeamInvite(0,0)==0);
 puts("PASS nearby party list: active players, same map, stale/current membership, count/fill agreement and capacity bounds");return 0;
}
