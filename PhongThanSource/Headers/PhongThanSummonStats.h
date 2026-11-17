#ifndef PHONGTHAN_SUMMON_STATS_H
#define PHONGTHAN_SUMMON_STATS_H
#include "PhongThanSummonRules.h"
// Loose reconstruction: combat level follows the owner, while NPC attack
// skills retain the summon rank used at allocation (not owner level 90/200).
// HP/AR/defense retain the NPC level-script contract. Damage growth and pace
// are the explicit authored Loose policy in PhongThanSummonRules.h.
template<class Actor,class Reader>
bool PhongThanApplySummonCombatLevel(Actor& pet,int ownerLevel,Reader& reader,int rank) {
    if(ownerLevel<1 || ownerLevel>65535||rank<1||rank>=64)return false;
    int hp=reader.Read(ownerLevel,"Life","LifeParam");
    int ar=reader.Read(ownerLevel,"AR","ARParam");
    int df=reader.Read(ownerLevel,"Defense","DefenseParam");
    int ex=reader.Read(ownerLevel,"ExDefense","ExDefenseParam");
    int low=PhongThanSummonDamage(reader.Read(ownerLevel,"MinDamage","MinDamageParam"),ownerLevel,rank);
    int high=PhongThanSummonDamage(reader.Read(ownerLevel,"MaxDamage","MaxDamageParam"),ownerLevel,rank);
    if(hp<=0 || ar<0 || df<0 || ex<0 || low<0 || high<low)return false;
    pet.m_Level=ownerLevel;pet.m_LifeMax=hp;pet.m_AttackRating=ar;
    pet.m_Defend=df;pet.m_ExDefend=ex;
    pet.m_PhysicsDamage.nValue[0]=low;pet.m_PhysicsDamage.nValue[2]=high;
    pet.RestoreNpcBaseInfo();
    pet.m_CurrentAttackSpeed+=PhongThanSummonSpeedBonus(rank);
    pet.m_CurrentCastSpeed+=PhongThanSummonSpeedBonus(rank);
    return true;
}
#endif
