#ifndef PHONGTHAN_THAN_KY_H
#define PHONGTHAN_THAN_KY_H
#include <limits.h>
// Stable group identity: upgrading a book changes this skill's level, never
// adds a second selectable level-specific skill ID.
inline int PtThanKyGroup(int id){
    static const int ids[15]={1547,1548,1551,1552,1553,1554,1556,1557,1560,1986,1996,1990,1991,1993,1994};
    for(int i=0;i<15;++i)if(ids[i]==id)return i;return -1;
}
inline int PtThanKySkill(int group){
    static const int ids[15]={1547,1548,1551,1552,1553,1554,1556,1557,1560,1986,1996,1990,1991,1993,1994};
    return group>=0&&group<15?ids[group]:0;
}
inline bool PtThanKyAuxiliary(int id){return id==1549||id==1550||id==1555||(id>=1561&&id<=1563);}
inline int PtThanKySaturate(__int64 value){return value>INT_MAX?INT_MAX:value<0?0:(int)value;}
inline int PtThanKyHeal(int maximum,int current,int flat,int missingPercent,int casterPower,int powerPercent){
    if(maximum<=0||current<=0||current>=maximum)return 0;
    __int64 amount=(__int64)flat+(__int64)(maximum-current)*missingPercent/100+(__int64)casterPower*powerPercent/100;
    if(amount>maximum-current)amount=maximum-current;return PtThanKySaturate(amount);
}
inline int PtThanKyShield(int maximum,int base,int percent){return PtThanKySaturate((__int64)base+(__int64)maximum*percent/100);}
inline int PtThanKyRate(int damage,int percent){return PtThanKySaturate((__int64)damage*percent/100);}
inline int PtThanKyAbsorb(int damage,int& shield){if(damage<=0||shield<=0)return damage;int paid=damage<shield?damage:shield;shield-=paid;return damage-paid;}
inline bool PtThanKyLeaseMatches(unsigned serial,int particular,int rank,unsigned currentSerial,int currentParticular,int currentRank,bool carried){
    return carried&&serial&&serial==currentSerial&&particular==currentParticular&&rank==currentRank;
}
#endif
