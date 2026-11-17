#ifndef PHONGTHAN_STATUS_COMBAT_H
#define PHONGTHAN_STATUS_COMBAT_H
#include <limits.h>
inline int PhongThanStatusScale(int value,int percent)
{
    if(value<=0||percent<=0)return 0;
    __int64 result=(__int64)value*percent/100;
    return result>INT_MAX?INT_MAX:(int)result;
}
inline int PhongThanStatusFocusedHit(int damage,int bonus,int baseMultiplier)
{
    __int64 multiplier=(__int64)baseMultiplier+bonus;
    if(multiplier<0)multiplier=0;
    if(multiplier>INT_MAX)multiplier=INT_MAX;
    return PhongThanStatusScale(damage,(int)multiplier);
}
inline int PhongThanStatusReduce(int damage,int reduction)
{
    if(reduction<=0)return damage;
    return PhongThanStatusScale(damage,reduction>=100?0:100-reduction);
}
inline int PhongThanStatusAbsorb(int damage,int absorb)
{
    if(absorb<=0)return damage;
    return damage>absorb?damage-absorb:0;
}
#endif
