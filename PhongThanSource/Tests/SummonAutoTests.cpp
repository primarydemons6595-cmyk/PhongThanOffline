#include <stdio.h>
#include <string.h>
typedef unsigned long DWORD;
#define CLIENT_PLAYER_INDEX 0
#define MAX_SKILL 1024
#define TRUE 1
#define do_stand 0
#define do_skill 1
struct ISkill {int GetSkillCostType(){return 0;}int GetSkillCost(void*){return 5;}} skill;
struct Manager {ISkill* GetSkill(int,int){return &skill;}} g_SkillManager;
struct Skills {int learned[1024];bool cooldown;int GetCurrentLevel(int id){return learned[id];}bool CanCast(int,DWORD){return cooldown;}};
int lastSkill,lastTarget,requests;
struct NPC {Skills m_SkillList;int m_Doing,m_RegionIndex,m_CurrentLife,m_CurrentLifeMax,m_dwID;bool cost,state;
 bool Cost(int,int,int,int){return cost;}bool IsNpcStateExist(int){return state;}
 void SendCommand(int,int id,int,int target){lastSkill=id;lastTarget=target;}
} Npc[4];
struct P {int m_nIndex;struct {int m_nProfession;} m_cProfession;} Player[1];
struct World {DWORD m_dwCurrentTime;} SubWorld[1];
struct Set {int GetNextIdx(int i){return i<3?i+1:0;}} NpcSet;
int owner[4];int PhongThanClientSummonOwner(int i){return owner[i];}
void SendClientCmdSkill(int,int,int){++requests;}
DWORD now=1000;DWORD IR_GetCurrentTime(){return now;}
#define PHONGTHAN_SUMMON_H
bool PhongThanIsSummon(int id){return id>=450&&id<=458;}
#include "../Sources/Core/Src/PhongThanSummonAuto.inl"
#define CHECK(cond) if(!(cond)){printf("FAIL line %d\n",__LINE__);return 1;}
int main(){
 Player[0].m_nIndex=1;Player[0].m_cProfession.m_nProfession=2;
 for(int i=1;i<4;++i){Npc[i].m_CurrentLife=100;Npc[i].m_CurrentLifeMax=100;Npc[i].m_dwID=100+i;Npc[i].cost=true;Npc[i].m_SkillList.cooldown=true;}
 PhongThanSetSummonAuto(450,60,40);
 CHECK(!PhongThanSummonAutoTick()); // unlearned cannot cast
 Npc[1].m_SkillList.learned[450]=1;
 CHECK(PhongThanSummonAutoTick()&&lastSkill==450&&lastTarget==1&&requests==1);
 CHECK(!PhongThanSummonAutoTick()&&requests==1); // throttle
 now+=1000;owner[2]=1;Npc[2].m_CurrentLife=90;
 CHECK(!PhongThanSummonAutoTick()); // owned living pet prevents re-summoning
 Npc[2].m_CurrentLife=50;Npc[1].m_SkillList.learned[45]=1;
 CHECK(PhongThanSummonAutoTick()&&lastSkill==45&&lastTarget==2);
 now+=1000;Npc[1].m_CurrentLife=30;
 CHECK(PhongThanSummonAutoTick()&&lastTarget==1); // self first
 now+=1000;Npc[1].m_CurrentLife=100;owner[2]=3;
 CHECK(PhongThanSummonAutoTick()&&lastSkill==450); // another player's pet excluded
 now+=1000;Npc[1].cost=false;CHECK(!PhongThanSummonAutoTick());
 Npc[1].cost=true;Npc[1].m_SkillList.cooldown=false;CHECK(!PhongThanSummonAutoTick());
 Npc[1].m_SkillList.cooldown=true;Npc[1].m_Doing=2;CHECK(!PhongThanSummonAutoTick());
 Npc[1].m_Doing=0;Player[0].m_cProfession.m_nProfession=1;CHECK(!PhongThanSummonAutoTick());
 PhongThanSetNativeBuff(0,9);Npc[1].m_SkillList.learned[9]=1;
 CHECK(PhongThanSummonAutoTick()&&lastSkill==9&&lastTarget==1);
 now+=1000;Npc[1].state=true;CHECK(!PhongThanSummonAutoTick());
 PhongThanSetNativeBuff(0,0);Player[0].m_cProfession.m_nProfession=2;owner[2]=1;Npc[2].m_CurrentLife=100;
 PhongThanSetSummonAuto(0,99999,-1);CHECK(!PhongThanSummonAutoTick());
 puts("PASS native summon auto: learned/cost/cooldown, owner isolation, heal priority, throttle, profession buffs");return 0;
}
