#ifndef PHONGTHAN_MINING_RUNTIME_H
#define PHONGTHAN_MINING_RUNTIME_H
void PhongThanMiningTick();
void PhongThanMiningDamaged(int actor);
bool PhongThanMiningClick(int player,int object);
bool PhongThanMiningCast(int actor,int targetX,int targetY);
bool PhongThanMiningCanCast(int actor,int targetX,int targetY);
#endif
