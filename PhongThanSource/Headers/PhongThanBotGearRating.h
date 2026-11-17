#ifndef PHONGTHAN_BOT_GEAR_RATING_H
#define PHONGTHAN_BOT_GEAR_RATING_H
#include "../Sources/Core/Src/KMagicAttrib.h"
// Bot decision heuristic over actual rolled attributes. This never changes the
// game's canonical equipment power, item attributes, requirements or bonuses.
inline int PTBotGearAttributeWeight(int type,int profession){
 switch(type){
 case magic_weapondamagemin_v:case magic_weapondamagemax_v:case magic_physicsdamage_v:return profession==1?3:10;
 case magic_magicdamage_v:case magic_addphysicsmagic_v:case magic_addcoldmagic_v:case magic_addfiremagic_v:case magic_addlightingmagic_v:case magic_addpoisonmagic_v:case magic_addearthmagic_v:return profession==1?10:4;
 case magic_colddamage_v:case magic_firedamage_v:case magic_lightingdamage_v:case magic_poisondamage_v:case magic_addearthdamage_v:return 5;
 case magic_lifemax_v:case magic_constitution_v:return 3;
 case magic_lifemax_p:case magic_allres_p:case magic_allresmax_p:case magic_reduce_monster_damage_p:return 25;
 case magic_armordefense_v:case magic_adddefense_v:return 4;
 case magic_physicsres_p:case magic_coldres_p:case magic_fireres_p:case magic_poisonres_p:case magic_lightingres_p:case magic_earthres_p:return 12;
 case magic_strength_v:return profession==1?2:12;
 case magic_intellect_v:return profession==1?12:2;
 case magic_dexterity_v:return 7;
 case magic_attackrating_v:return profession==1?1:3;
 case magic_manamax_v:return profession==1?2:1;
 case magic_attackspeed_v:return profession==1?4:25;
 case magic_castspeed_v:return profession==1?25:4;
 case magic_fastwalkrun_p:case magic_steallife_p:case magic_allskill_v:return 30;
 case magic_weapondamageenhance_p:case magic_physicsenhance_p:return profession==1?4:20;
 case magic_magicenhance_p:return profession==1?20:4;
 default:return 0;
 }
}
inline int PTBotGearAttributeScore(int type,const int* values,int profession){
 int weight=PTBotGearAttributeWeight(type,profession);if(!weight||!values)return 0;
 int value=values[0];if(value<0)value=0;
 if(type>=magic_physicsdamage_v&&type<=magic_magicdamage_v&&values[2]>0)value=value/2+values[2]/2;
 if(value>1000000)value=1000000;return value*weight;
}
struct PTBotInventoryCell {int place,x,y;PTBotInventoryCell(int p=0,int a=0,int b=0):place(p),x(a),y(b){}};
// The normal player protocol is click/hand based: pick, equip, then store the
// displaced item. A one-shot bag -> equipment request only picks up the item.
template<class Inventory> bool PTBotEquipTransaction(Inventory& inventory,int candidate,int part,const PTBotInventoryCell& source,const PTBotInventoryCell& equipment,const PTBotInventoryCell& displaced){
 if(inventory.Hand())return false;
 inventory.Click(source);if(inventory.Hand()!=candidate)return false;
 inventory.Click(equipment);bool equipped=inventory.Equipped(part)==candidate;
 if(inventory.Hand())inventory.Click(equipped?displaced:source);
 return equipped&&inventory.Hand()==0;
}
#endif
