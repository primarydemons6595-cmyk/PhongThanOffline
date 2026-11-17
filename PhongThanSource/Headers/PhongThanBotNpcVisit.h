#ifndef PHONGTHAN_BOT_NPC_VISIT_H
#define PHONGTHAN_BOT_NPC_VISIT_H
// Small leases bound simultaneous bot visitors. A departed bot cannot reserve
// an NPC indefinitely; native entity serials protect recycled NPC slots.
class PTBotNpcVisits {
 enum {Slots=4096};unsigned long entity[Slots],until[Slots];
public:
 PTBotNpcVisits(){for(int i=0;i<Slots;++i){entity[i]=until[i]=0;}}
 bool Acquire(int bot,unsigned long npc,unsigned long now){
  if(bot<0||bot>=Slots||!npc)return false;int visitors=0;
  for(int i=0;i<Slots;++i)if(i!=bot&&entity[i]==npc&&(int)(now-until[i])<0)++visitors;
  if(visitors>=2)return false;entity[bot]=npc;until[bot]=now+8000;return true;
 }
 void Release(int bot){if(bot>=0&&bot<Slots){entity[bot]=until[bot]=0;}}
};
#endif
