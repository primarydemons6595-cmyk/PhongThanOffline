#ifndef PHONGTHAN_COMBAT_LOG_TEXT_H

#define PHONGTHAN_COMBAT_LOG_TEXT_H

#include "PhongThanCombatLog.h"

#include "PhongThanNpcNameColor.h"

inline bool PTCombatHistoryAllows(const PTCombatLogEvent& e,int tab,int direction,int type){return (tab==2?e.Kind==PT_COMBAT_DAMAGE:e.Kind!=PT_COMBAT_DAMAGE)&&(direction==0||direction==e.Direction)&&(tab!=2||type<0||type==e.DamageType);}

inline void PTCombatHistoryText(const PTCombatLogEvent& e,char* out,int capacity){

 char attacker[32],target[32];unsigned int color=0;PhongThanParseNpcName(e.AttackerName,attacker,sizeof(attacker),color);PhongThanParseNpcName(e.TargetName,target,sizeof(target),color);

 static const char* types[]={"V\313t L\375","H\341a","B\250ng","L\253i","Th\346","\247\351c","Ph\320p"};

 bool outgoing=e.Direction==1;if(!out||capacity<1)return;out[0]=0;

 if(e.Kind==PT_COMBAT_DAMAGE){if(outgoing)_snprintf(out,capacity,"\247\267 g\251y <c=r>%d<c> s\270t th\255\254ng <c=cyan>%s<c> l\252n <c=g>%s<c>.",e.Amount,types[e.DamageType],target);else _snprintf(out,capacity,"<c=g>%s<c> \256\267 g\251y <c=r>%d<c> s\270t th\255\254ng <c=cyan>%s<c> l\252n b\271n.",attacker,e.Amount,types[e.DamageType]);}

 else {const char* format=0;

 if(e.Kind==PT_COMBAT_HIT)format=outgoing?"B\271n \256\267 \256\270nh tr\363ng <c=g>%s<c>.":"<c=g>%s<c> \256\267 \256\270nh tr\363ng b\271n.";

 if(e.Kind==PT_COMBAT_DODGE)format=outgoing?"<c=y>%s<c> \256\267 n\320 tr\270nh \256\337n t\312n c\253ng c\361a b\271n.":"B\271n \256\267 n\320 tr\270nh \256\337n t\312n c\253ng c\361a <c=y>%s<c>.";

 if(e.Kind==PT_COMBAT_DEFEAT)format=outgoing?"B\271n \256\267 \256\270nh b\271i <c=y>%s<c>.":"B\271n \256\267 b\336 <c=r>%s<c> \256\270nh b\271i.";

 if(format)_snprintf(out,capacity,format,outgoing?target:attacker);}

 out[capacity-1]=0;

}

inline const char* PTCombatMenuLabel(int tab,int row){

 static const char* battle[]={"T\312t c\266","G\251y s\270t","Ch\336u s\270t","X\343a nh\313t k\375"};

 static const char* damage[]={"T\312t c\266 s\270t th\255\254ng","V\313t L\375","Ph\320p","Th\346","L\253i","B\250ng","H\341a","\247\351c","X\343a nh\313t k\375"};

 return tab==1?(row>=0&&row<4?battle[row]:""):(row>=0&&row<9?damage[row]:"");

}

#endif

