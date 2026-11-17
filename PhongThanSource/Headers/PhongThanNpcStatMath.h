#ifndef PT_NPC_STAT_MATH_H
#define PT_NPC_STAT_MATH_H
#include <limits.h>
inline int PhongThanNpcStatClamp(__int64 value){return value>INT_MAX?INT_MAX:value<INT_MIN?INT_MIN:(int)value;}
inline int PhongThanNpcScaledStat(int value,int percent){return PhongThanNpcStatClamp((__int64)value*percent/100);}
inline int PhongThanVttCombatMode(int map,int npc,int mode){
    return map>=1067&&map<=1070&&npc>=129+(map-1067)*5&&npc<=133+(map-1067)*5&&mode==500?1:mode;
}
inline int PhongThanPlayerNameClearance(int riding){return riding?32:24;}
#endif
