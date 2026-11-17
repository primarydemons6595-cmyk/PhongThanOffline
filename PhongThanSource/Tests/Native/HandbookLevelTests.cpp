#include <windows.h>
#include <assert.h>
#include <stdio.h>
struct KNpc{int m_Level,m_SubWorldIndex,m_dwID,m_LifeMax,m_StaminaMax,m_ManaMax;}Npc[2];
bool allowed=true,blocked=false;int growth,syncs,packets;
struct KPlayer{int m_nIndex,m_nAttributePoint,m_nSkillPoint,m_nNetConnectIdx;
 void LevelUp(){if(blocked)return;++growth;++Npc[m_nIndex].m_Level;m_nAttributePoint+=5;++m_nSkillPoint;Npc[m_nIndex].m_LifeMax+=10;}
 void SendCurNormalSyncData(){++syncs;}
}Player[2];
struct World{int m_SubWorldID;}SubWorld[1];
struct PHONGTHAN_LEVEL_EVENT{int Header,MapId,EntityId,Level,Experience,AttributePoints,SkillPoints,BaseMaxLife,BaseMaxStamina,BaseMaxMana,ResetAttributes;};
enum{PHONGTHAN_MSG_GAMEPLAY_LEVEL=1,PHONGTHAN_WIRE_FLAG_RESPONSE=1};
void PhongThanInitializeWireHeader(int*,int,int,int,int){}
struct Server{void PackDataToClient(int,BYTE* data,int){++packets;PHONGTHAN_LEVEL_EVENT* e=(PHONGTHAN_LEVEL_EVENT*)data;assert(!e->ResetAttributes&&e->Level==Npc[1].m_Level&&e->AttributePoints==Player[1].m_nAttributePoint&&e->BaseMaxLife==Npc[1].m_LifeMax);}}server;
Server* g_pServer=&server;
bool PtHandbookAllowed(int p){return p==1&&allowed;}
#include "HandbookLevelActual.inl"
void Reset(int level){ZeroMemory(Player,sizeof(Player));ZeroMemory(Npc,sizeof(Npc));Player[1].m_nIndex=1;Npc[1].m_Level=level;Npc[1].m_LifeMax=100;growth=syncs=packets=0;allowed=true;blocked=false;}
int main(){
 const int levels[]={50,70,90,150,200};
 for(int i=0;i<5;++i){int target=levels[i];Reset(1);assert(PtHandbookLevel(1,target)==1);assert(Npc[1].m_Level==target&&growth==target-1&&Player[1].m_nAttributePoint==5*(target-1)&&packets==1&&syncs==1);assert(PtHandbookLevel(1,target)==1&&growth==target-1);Reset(200);assert(PtHandbookLevel(1,target)==1&&Npc[1].m_Level==200&&!growth);}
 const int invalid[]={-1,0,1,49,51,100,199,201,999};
 for(i=0;i<sizeof(invalid)/sizeof(invalid[0]);++i){Reset(1);assert(!PtHandbookLevel(1,invalid[i])&&Npc[1].m_Level==1&&!growth);}
 Reset(1);allowed=false;assert(!PtHandbookLevel(1,50)&&!growth);Reset(1);blocked=true;assert(!PtHandbookLevel(1,50)&&!growth&&!packets);
 puts("PASS actual native level grant: 5 milestones, normal stat/point growth and sync, repeat/no downgrade, invalid levels, book guard, stalled LevelUp");return 0;
}
