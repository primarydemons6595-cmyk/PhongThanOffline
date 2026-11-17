#ifndef PHONGTHAN_BOT_MARKET_QUALITY_H
#define PHONGTHAN_BOT_MARKET_QUALITY_H
#include "../Sources/Core/Src/KMagicAttrib.h"
// Quality values are KItem::GetPhongThanQuality: 1 blue, 2 gold, 3 orange,
// 4 green set, 5 purple. Count rolled bonuses, never ordinary base damage.
inline int PTBotMarketBonus(int type,int value){
 if(value<=0)return 0;
 switch(type){
 case magic_attackspeed_v:case magic_castspeed_v:return value>=10?2:0;
 case magic_weapondamageenhance_p:case magic_physicsenhance_p:case magic_addphysicsdamage_p:case magic_magicenhance_p:return value>=15?2:0;
 case magic_allskill_v:return 2;
 case magic_fastwalkrun_p:return value>=10?2:0;
 case magic_steallife_p:return value>=5?2:0;
 case magic_lifemax_v:case magic_manamax_v:return value>=150?1:0;
 case magic_lifemax_p:return value>=10?1:0;
 case magic_allres_p:return value>=8?1:0;
 case magic_allresmax_p:return value>=5?1:0;
 case magic_physicsres_p:case magic_coldres_p:case magic_fireres_p:case magic_poisonres_p:case magic_lightingres_p:case magic_earthres_p:return value>=15?1:0;
 case magic_strength_v:case magic_intellect_v:case magic_dexterity_v:case magic_constitution_v:return value>=15?1:0;
 case magic_attackrating_v:return value>=100?1:0;
 case magic_armordefense_v:case magic_adddefense_v:return value>=30?1:0;
 default:return 0;
 }
}
struct PTBotMarketBonuses {
 int good,strong,count,types[20];PTBotMarketBonuses():good(0),strong(0),count(0){}
 void Add(int type,int value){int merit=PTBotMarketBonus(type,value);if(!merit)return;for(int i=0;i<count;++i)if(types[i]==type)return;if(count<20)types[count++]=type;++good;if(merit==2)++strong;}
 bool Eligible(int quality,int requiredLevel)const{return quality>=2&&quality<=5?true:quality==1&&requiredLevel>=30&&(strong>=1||good>=2);}
};
#endif
