#ifndef PHONGTHAN_SUMMON_RULES_H
#define PHONGTHAN_SUMMON_RULES_H
#include <limits.h>
// Authored Loose combat policy. These are not claimed VNG server formulas.
enum { PT_SUMMON_ACQUIRE_RANGE=1024, PT_SUMMON_LEASH_RANGE=1280,
       PT_SUMMON_IDLE_RECALL_RANGE=384, PT_SUMMON_IDLE_FOLLOW_RANGE=144 };
inline int PhongThanSummonDamage(int templateDamage,int ownerLevel,int rank) {
    if(templateDamage<0||ownerLevel<1||ownerLevel>65535||rank<1||rank>=64)return -1;
    __int64 value=(__int64)templateDamage*(100+(__int64)ownerLevel*rank)/100;
    return value>INT_MAX?INT_MAX:(int)value;
}
inline int PhongThanSummonShownLevel(int combatLevel,int rank) {
    return rank>0&&rank<64?rank:combatLevel;
}
inline int PhongThanSummonSpeedBonus(int rank) {
    if(rank<1||rank>=64)return 0;
    int bonus=(rank-1)*10;return bonus>100?100:bonus;
}
#endif
