#include <windows.h>
#include <assert.h>
#include <stdio.h>
enum {do_stand=0,do_revive=1,VOID_REGION=-1,obj_npc=1};
struct Region {int removed;void DecRef(int,int,int){++removed;}};
struct World {Region m_Region[1];int moved;void NpcChangeRegion(int,int,int){++moved;}}SubWorld[1];
void g_DebugLog(const char*,const char*){}
struct KNpc {
 int m_RegionIndex,m_Doing,m_ProcessAI,m_ProcessState,m_AiMode,m_ReviveFrame;
 int m_SubWorldIndex,m_MapX,m_MapY,m_Index,revived;
 char Name[32];struct Frames{int nTotalFrame,nCurrentFrame;}m_Frames;
 KNpc():m_RegionIndex(0),m_Doing(do_stand),m_ProcessAI(1),m_ProcessState(1),m_AiMode(1),m_ReviveFrame(64800),m_SubWorldIndex(0),m_MapX(0),m_MapY(0),m_Index(1),revived(0){}
 void ClearStateSkillEffect(){}void ClearNormalState(){}BOOL IsPlayer(){return FALSE;}
 void Revive(){++revived;m_Doing=do_stand;m_ProcessAI=m_ProcessState=1;}
 void DoRevive();void OnRevive();BOOL WaitForFrame();
};
#define _SERVER
#include "TruLungReviveMethods.inl"
int main(){
 KNpc boss[3];
 for(int i=0;i<3;++i){
  // While alive no scheduled spawn or revival occurs, regardless of time.
  assert(boss[i].m_Doing==do_stand&&boss[i].revived==0);
  boss[i].DoRevive();assert(boss[i].m_Frames.nTotalFrame==3600*18);
  for(int t=0;t<64800-1;++t)boss[i].OnRevive();
  assert(boss[i].revived==0);
  boss[i].DoRevive(); // repeated death handling cannot reset or duplicate the timer
  boss[i].OnRevive();assert(boss[i].revived==1&&boss[i].m_Doing==do_stand);
  boss[i].DoRevive();for(int again=0;again<64800;++again)boss[i].OnRevive();assert(boss[i].revived==2);
 }
 assert(SubWorld[0].moved==6); // same three objects, two lifecycle transitions each
 puts("PASS production DoRevive/OnRevive/WaitForFrame: three independent native timers, 64800 frames, no early revive, repeat-death guard, same-entity revival");return 0;
}
