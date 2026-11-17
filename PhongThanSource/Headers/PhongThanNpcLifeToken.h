#ifndef PHONGTHAN_NPC_LIFE_TOKEN_H
#define PHONGTHAN_NPC_LIFE_TOKEN_H
// Entity IDs stay stable when ordinary monsters revive. A quest death receipt
// therefore also needs a life number, without changing the network entity ID.
struct PhongThanNpcLifeToken {
 unsigned long entity,life;
 PhongThanNpcLifeToken():entity(0),life(0){}
 unsigned long Get(unsigned long current){if(entity!=current){entity=current;life=1;}return life;}
 void Revived(unsigned long current){Get(current);if(++life==0)life=1;}
};
#endif
