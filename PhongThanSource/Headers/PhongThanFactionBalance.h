#ifndef PHONGTHAN_FACTION_BALANCE_H
#define PHONGTHAN_FACTION_BALANCE_H
#include <limits.h>
// Requested Loose PvP policy, not an assertion about original VNG balance.
// Vu Si advances Giap Si. Neutral/world NPCs never enter this policy.
inline int PhongThanCombatFaction(int profession)
{return profession==3?0:profession>=0&&profession<=2?profession:-1;}
inline int PhongThanFactionDamagePercent(int attacker,int victim)
{
    attacker=PhongThanCombatFaction(attacker);victim=PhongThanCombatFaction(victim);
    if(attacker<0||victim<0||attacker==victim)return 100;
    return (attacker+1)%3==victim?200:50;
}
inline int PhongThanScaleCombatDamage(int damage,int percent)
{
    if(damage<=0||percent<=0)return 0;
    __int64 scaled=(__int64)damage*percent/100;
    return scaled>INT_MAX?INT_MAX:(int)scaled;
}
inline int PhongThanFactionDamage(int damage,int attacker,int victim)
{return PhongThanScaleCombatDamage(damage,PhongThanFactionDamagePercent(attacker,victim));}
#endif
