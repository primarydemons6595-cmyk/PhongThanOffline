#ifndef PHONGTHAN_PLAYER_DAMAGE_REDUCTION_H
#define PHONGTHAN_PLAYER_DAMAGE_REDUCTION_H
inline int PhongThanPlayerDamageReduction(int damage,int reduction,bool playerSource,bool playerVictim){
 if(damage<=0)return 0;if(!playerSource||!playerVictim||reduction<=0)return damage;
 if(reduction>100)reduction=100;return (int)(((__int64)damage*(100-reduction))/100);
}
inline int PhongThanMonsterDamageReduction(int damage,int reduction,bool wildMonsterSource,bool playerVictim){
 if(damage<=0)return 0;if(!wildMonsterSource||!playerVictim||reduction<=0)return damage;
 if(reduction>100)reduction=100;return (int)(((__int64)damage*(100-reduction))/100);
}
#endif
