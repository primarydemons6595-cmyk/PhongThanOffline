// F3 reads the same active state used by server hit/damage resolution.
static void PhongThanFillStatusOriginal(const KNpc& npc, PTStatusOriginal& out)
{
    memset(&out,0,sizeof(out));
    int* v=out.Value;
    const PhongThanCombatState& p=npc.m_PtCombat;
    v[0]=p.PhysicsEnhance;
    v[1]=npc.m_CurrentAttackRating;v[2]=npc.m_CurrentDefend;
    v[3]=p.AttackHitRecover;
    v[4]=npc.m_CurrentFireEnhance;v[5]=npc.m_CurrentLightEnhance;
    v[6]=npc.m_CurrentColdEnhance;v[7]=npc.m_CurrentPoisonEnhance;
    v[8]=p.EffectEnhance[0];v[9]=p.EffectEnhance[1];
    v[10]=p.IgnoreMagicResist;
    v[11]=npc.m_CurrentIgnoreFireResist;v[12]=npc.m_CurrentIgnoreLightResist;
    v[13]=npc.m_CurrentIgnoreColdResist;v[14]=npc.m_CurrentIgnoreEarthResist;
    v[15]=npc.m_CurrentIgnoreExDefence;
    v[16]=npc.m_CurrentIgnoreDefensePercent;
    v[17]=npc.m_CurrentDeadlyStrikeEnhanceP;
    v[18]=npc.m_CurrentAllMagicDeadlyStrike;
    v[20]=npc.m_CurrentFireDeadlyStrike;v[21]=npc.m_CurrentLightDeadlyStrike;
    v[22]=npc.m_CurrentColdDeadlyStrike;
    v[23]=npc.m_CurrentReducePhysicsDeadlyStrike;v[24]=npc.m_CurrentReduceMagicDeadlyStrike;
    for(int i=0;i<4;++i)v[25+i]=p.ElementCriticalReduce[i];
    v[29]=p.CriticalDamage[0];v[30]=p.CriticalDamage[1];
    v[31]=p.CriticalDamageReduce[0];v[32]=p.CriticalDamageReduce[1];
    v[33]=npc.m_CurrentPhysicsResist;v[34]=npc.m_CurrentHitRecover;
    v[35]=npc.m_CurrentFreezeTimeReducePercent;v[36]=npc.m_CurrentPoisonTimeReducePercent;
    v[37]=npc.m_CurrentStunTimeReducePercent;v[38]=p.ConfuseTimeReduce;
    v[39]=p.AbsorbDamage[0];v[40]=p.AbsorbDamage[2];v[41]=p.AbsorbDamage[3];
    v[42]=p.AbsorbDamage[1];v[43]=p.AbsorbDamage[4];
    v[44]=npc.m_CurrentStaticMagicShieldP+npc.m_ManaShield.nValue[0];
    v[45]=npc.m_CurrentDamage2Mana;v[46]=p.WalkRunEnhance;
    v[47]=npc.m_CurrentLifeReplenish;v[48]=npc.m_CurrentManaReplenish;
    v[49]=p.PlayerDamageEnhance;v[50]=p.MonsterDamageEnhance;
    v[51]=p.PlayerDamageReduce;v[52]=p.MonsterDamageReduce;
}
