#include <windows.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "PhongThanRoute.h"
enum {MAX_PLAYER=4};
struct Tasks{int v[400];int GetSaveVal(int k){return v[k];}};
struct P{Tasks m_cTask;int m_nIndex;}Player[MAX_PLAYER];
struct N{int m_NpcSettingIdx,syncs;void SendSyncData(int,BOOL){++syncs;}}Npc[4];
struct PHONGTHAN_PLAYER_NPC_STATE{int nMorphTemplate,nMorphDeadline;}states[4];
struct PHONGTHAN_IBBUFF_STORE{int has;}buffs[4];
bool live=true;int writes=0,adds=0;
PHONGTHAN_PLAYER_NPC_STATE* PhongThanPlayerState(int p){return live?&states[p]:0;}
PHONGTHAN_IBBUFF_STORE* GetPlayerIBBuffStore(int p){return &buffs[p];}
int FindIBBuffSlot(PHONGTHAN_IBBUFF_STORE* s,int id){assert(id==160);return s->has?0:-1;}
int AddIBBuffToStore(PHONGTHAN_IBBUFF_STORE* s,int actor,int id,int sec,int level){assert(actor==1&&id==160&&sec==0&&level==0);++adds;s->has=1;return 1;}
void PersistPlayerIBBuffStore(int p){assert(p==1);++writes;}
#include "../../Sources/Core/Src/PhongThanCommerceRestore.inl"
int main(){
 Player[1].m_nIndex=1;
 for(int origin=1;origin<=9;++origin)for(int kind=1;kind<=5;++kind){
  memset(states,0,sizeof(states));states[1].nMorphTemplate=-1;Npc[1].m_NpcSettingIdx=-2;buffs[1].has=0;Player[1].m_cTask.v[60]=origin;Player[1].m_cTask.v[332]=kind;Player[1].m_cTask.v[333]=origin;Player[1].m_cTask.v[334]=12;Player[1].m_cTask.v[335]=1;Player[1].m_cTask.v[110]=12;Player[1].m_cTask.v[111]=1;
  int oldWrites=writes;PhongThanRestoreCommerceCamel(1);assert(Npc[1].m_NpcSettingIdx==364&&states[1].nMorphTemplate==364&&states[1].nMorphDeadline==0&&buffs[1].has&&writes==oldWrites+1);
  PhongThanRestoreCommerceCamel(1);assert(writes==oldWrites+1);assert(Player[1].m_cTask.v[110]==12&&Player[1].m_cTask.v[111]==1&&Player[1].m_cTask.v[333]==origin);
 }
 Player[1].m_cTask.v[60]=0;states[1].nMorphTemplate=-1;Npc[1].m_NpcSettingIdx=-1;PhongThanRestoreCommerceCamel(1);assert(Npc[1].m_NpcSettingIdx==-1);
 Player[1].m_cTask.v[60]=4;Player[1].m_cTask.v[332]=0;PhongThanRestoreCommerceCamel(1);assert(Npc[1].m_NpcSettingIdx==-1);
 Player[1].m_cTask.v[332]=1;states[1].nMorphTemplate=455;Npc[1].m_NpcSettingIdx=455;PhongThanRestoreCommerceCamel(1);assert(Npc[1].m_NpcSettingIdx==455);
 states[1].nMorphTemplate=-1;live=false;PhongThanRestoreCommerceCamel(1);assert(Npc[1].m_NpcSettingIdx==455);live=true;
 // Native GetDir stops within one movement step. Portal finishing must switch
 // mode even though its final target equals the last running waypoint.
 for(int speed=10;speed<=31;++speed)for(int axis=0;axis<2;++axis){
  int x=axis?16:34,y=axis?34:16,tx=16,ty=16;PTRouteCommandCadence cadence;
  assert(cadence.ShouldSend(1000,tx,ty,false));bool walk=PTRouteCommandWalk(false,false,8);assert(walk&&cadence.ShouldSend(1001,tx,ty,walk));
  for(int tick=0;tick<10&&!PTRouteNear(x,y,tx,ty,8);++tick){int dx=tx-x,dy=ty-y;double dist=sqrt((double)dx*dx+dy*dy);int step=walk?6:speed;assert(dist>=step);x+=(int)(dx*step/dist);y+=(int)(dy*step/dist);}
  assert(PTRouteNear(x,y,tx,ty,8));
 }
 assert(!PTRouteCommandWalk(false,false,0)&&!PTRouteCommandWalk(true,false,8)&&PTRouteCommandWalk(true,true,0));
 puts("PASS production camel restore: 45 saved rental variants, cargo/quest preserved, idempotence, returned/invalid/rival morph guards; portal final walking and immediate cadence mode switch");return 0;
}
