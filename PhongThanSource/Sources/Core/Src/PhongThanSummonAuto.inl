#include "PhongThanSummon.h"
// Native Di Nhan controls. The server still validates ownership, cast cost,
// learned level and cooldown; client requests never create or heal NPCs locally.
static int s_PtNativeBuff[5];
void PhongThanSetNativeBuff(int slot,int skill){if(slot>=0&&slot<5)s_PtNativeBuff[slot]=skill>0&&skill<MAX_SKILL?skill:0;}
static int s_PtSummonSkill=0,s_PtPetHealLife=0,s_PtSelfHealLife=0;
static DWORD s_PtSummonAutoTime=0;
void PhongThanSetSummonAuto(int skill,int petLife,int selfLife){
    s_PtSummonSkill=PhongThanIsSummon(skill)?skill:0;
    s_PtPetHealLife=petLife>0&&petLife<=9999?petLife:0;
    s_PtSelfHealLife=selfLife>0&&selfLife<=9999?selfLife:0;
}
static bool PhongThanSummonAutoCast(int actor,int skill,int target){
    int level=Npc[actor].m_SkillList.GetCurrentLevel(skill);if(level<=0)return false;
    ISkill* p=g_SkillManager.GetSkill(skill,level);if(!p)return false;
    if(!Npc[actor].m_SkillList.CanCast(skill,SubWorld[0].m_dwCurrentTime)||
       !Npc[actor].Cost(p->GetSkillCostType(),p->GetSkillCost(&Npc[actor]),TRUE,TRUE))return false;
    Npc[actor].SendCommand(do_skill,skill,-1,target);
    SendClientCmdSkill(skill,-1,Npc[target].m_dwID);return true;
}
static bool PhongThanSummonAutoTick(){
    int actor=Player[CLIENT_PLAYER_INDEX].m_nIndex;
    if(actor<=0 || PhongThanIsEscortRider(actor))return false;
    if(Npc[actor].m_Doing!=do_stand)return false;
    DWORD now=IR_GetCurrentTime();if(now-s_PtSummonAutoTime<1000)return false;
    for(int b=0;b<5;++b)if(s_PtNativeBuff[b]&&!Npc[actor].IsNpcStateExist(s_PtNativeBuff[b])&&PhongThanSummonAutoCast(actor,s_PtNativeBuff[b],actor)){s_PtSummonAutoTime=now;return true;}
    if(Player[CLIENT_PLAYER_INDEX].m_cProfession.m_nProfession!=2)return false;
    int pet=0;
    for(int i=NpcSet.GetNextIdx(0);i>0;i=NpcSet.GetNextIdx(i))
        if(PhongThanClientSummonOwner(i)==actor && Npc[i].m_RegionIndex>=0 && Npc[i].m_CurrentLife>0){pet=i;break;}
    bool cast=false;
    if(s_PtSelfHealLife && Npc[actor].m_CurrentLife<s_PtSelfHealLife && Npc[actor].m_CurrentLife<Npc[actor].m_CurrentLifeMax)
        cast=PhongThanSummonAutoCast(actor,45,actor);
    if(!cast&&pet&&s_PtPetHealLife&&Npc[pet].m_CurrentLife<s_PtPetHealLife&&Npc[pet].m_CurrentLife<Npc[pet].m_CurrentLifeMax)
        cast=PhongThanSummonAutoCast(actor,45,pet);
    if(!cast&&!pet&&s_PtSummonSkill)cast=PhongThanSummonAutoCast(actor,s_PtSummonSkill,actor);
    if(cast)s_PtSummonAutoTime=now;
    return cast;
}
