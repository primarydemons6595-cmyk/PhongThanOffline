#ifndef PHONGTHAN_COMBAT_STATE_H
#define PHONGTHAN_COMBAT_STATE_H
#include <string.h>
// Names and operations verified from the original attribute dispatch table at
// game.dat:0x966730. This is runtime state, never a serialized character record.
struct PhongThanCombatState
{
    int PhysicsEnhance;
    int LifeDamageRate, ManaColdDamageRate, ManaEarthDamageRate;
    int OutgoingReduce[5]; // physics, cold, fire, lightning, earth
    int CriticalDamage[2], CriticalDamageReduce[2];
    int AbsorbDamage[5];
    int HurtRate, PlayerDamageReduce, MonsterDamageEnhance;
    int AttackHitRecover, WalkRunEnhance, IgnoreMagicResist;
    int EffectEnhance[2], ConfuseTimeReduce, PlayerDamageEnhance;
    int MonsterDamageReduce; // percent, wild NPC/boss sources only
    int ElementCriticalReduce[4]; // fire, lightning, cold, earth
    int LifePotionEffect,ManaPotionEffect,AdditionalHeal,MonsterHostility;
    int SummonHealPercent; // magic_bless_summon_p, original Lua 408
    int ImmunityPoison,ImmunityCold,ImmunityConfuse,ImmunityStun;
    int LifeDebuff[5];
    void Clear() { memset(this, 0, sizeof(*this)); }
};

// Original x86 physical composition, checked with 2,000 emulator cases.
// NormalHurt/BaseDamageC in the original package is /:1,1,1,1.
inline void PhongThanComposeOriginalPhysics(int baseMin, int baseMax,
    int skillType, int skillMin, int skillMax, int enhanceType, int damageType,
    int addBase, int enhancement, int maxLife, int lifeRate, int reduction,
    int* outputMin, int* outputMax)
{
    int bonus = maxLife * lifeRate / 1000;
    int low = 0, high = 0;
    if (skillType == enhanceType)
    {
        low = baseMin * skillMin / 100;
        high = baseMax * skillMin / 100;
    }
    else if (skillType == damageType)
    {
        low = skillMin + bonus;
        high = skillMax + bonus;
    }
    if (addBase)
    {
        low += baseMin;
        high += baseMax;
        if (enhancement > 0)
        {
            low += baseMin * enhancement / 100;
            high += baseMax * enhancement / 100;
        }
        low += bonus;
        high += bonus;
    }
    if (reduction > 0)
    {
        low -= low * reduction / 100;
        high -= high * reduction / 100;
    }
    *outputMin = low > 0 ? low : 0;
    *outputMax = high > 0 ? high : 0;
}

// Original cold/fire/lightning/earth blocks, before victim mitigation.
// Channel order: cold, fire, lightning, earth. Coefficients are the /:1 rows
// of NormalHurt in the original basenumcalc.ini, not inherited JX modifiers.
inline void PhongThanComposeOriginalElement(int channel, int skillPresent,
    const int* skill, const int* item, int addBase, int enhancement,
    int maxMana, int manaRate, int reduction, int minimumEnhance,
    int maximumEnhance, int* output)
{
    int low=0,high=0,time=0;
    if(skillPresent && skill[0]>0)
    {
        low=skill[0];high=skill[2];
        if(channel==0)
        {
            time=skill[1]+skill[1]*enhancement/100;
            if(time>72)time=72;
        }
        else if(channel==2)
        {
            low+=(high-low)*enhancement/100;
            if(low>high){int temp=low;low=high;high=temp;}
        }
        else high+=high*enhancement/100;
        int bonus=maxMana*manaRate/1000;
        low+=bonus;high+=bonus;
    }
    if(addBase)
    {
        int itemLow=item[0],itemHigh=item[2];
        if(channel==2)itemLow+=(itemHigh-itemLow)*enhancement/100;
        else if(channel!=0)itemHigh+=itemHigh*enhancement/100;
        low+=itemLow;high+=itemHigh;
        if(channel==0)
        {
            int duration=item[1]+item[1]*enhancement/100;
            if(low!=0 && time<duration)time=duration;
            if(time>72)time=72;
        }
        if(channel==2 && low>high){int temp=low;low=high;high=temp;}
    }
    if(reduction>0){low-=low*reduction/100;high-=high*reduction/100;}
    if(channel==0)
    {
        if(minimumEnhance>0)low+=low*minimumEnhance/100;
        if(maximumEnhance>0)high+=high*maximumEnhance/100;
    }
    output[0]=low>0?low:0;output[1]=time;output[2]=high>0?high:0;
}
#endif
