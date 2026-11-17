#ifndef PT_RANDOM_BLUE_NPC_POLICY_H
#define PT_RANDOM_BLUE_NPC_POLICY_H
#include "LegacyDropEngine.h"
namespace RandomBlueNpc {
struct Life {
 unsigned long entity;bool seen,assigned;
 Life():entity(0),seen(false),assigned(false){}
 bool Begin(unsigned long id,int current,bool respawn,int& base){
  if(entity!=id){entity=id;seen=false;assigned=false;}
  if(seen&&!respawn)return false;
  base=(assigned&&current==1)?0:current;
  seen=true;assigned=false;return true;
 }
};
inline bool Selected(int chance,unsigned long draw){return chance>=0&&chance<=1000000&&draw<1000000&&draw<(unsigned long)chance;}
inline bool Eligible(bool normalKind,bool owned,bool scripted,bool scheduledBoss,int base,int treasure,int treasure1,int maxTreasure){
 return normalKind&&!owned&&!scripted&&!scheduledBoss&&base==0&&treasure>=0&&treasure1>=0&&maxTreasure>=0&&treasure<=maxTreasure&&treasure1<=maxTreasure;
}
}
#endif
