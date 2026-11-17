#ifndef PHONGTHAN_ESCORT_RIDER_H
#define PHONGTHAN_ESCORT_RIDER_H
#ifndef _SERVER
void PhongThanSetEscortRider(int npc,unsigned long cart);
void PhongThanSetEscortOwner(int npc,unsigned long actor);
int PhongThanEscortRelation(int first,int second);
bool PhongThanIsEscortRider(int npc);
int PhongThanClientEscortCart(int actor);
int PhongThanClientEscortDriver(int cart);
bool PhongThanEscortHiddenEntity(int npc);
#endif
#endif
