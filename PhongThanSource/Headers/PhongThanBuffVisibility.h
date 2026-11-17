#ifndef PHONGTHAN_BUFF_VISIBILITY_H
#define PHONGTHAN_BUFF_VISIBILITY_H
// Passive/equipment states are not spells the player has just cast.
inline bool PTShowSkillBuff(int ticks,bool passive,int skill,int activeAura){
 return !passive && (ticks>0 || (ticks<0 && skill>0 && skill==activeAura));
}
#endif
