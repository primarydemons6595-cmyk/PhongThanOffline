#ifndef PHONGTHAN_ESCORT_H
#define PHONGTHAN_ESCORT_H
void PhongThanEscortTick();
int PhongThanEscortDrivenCart(int player);
int PhongThanEscortCombatTarget(int npc);
int PhongThanEscortCargoKind(int npc,int* owner=0);
int PhongThanEscortSnapshotTemplate(int player,int ordinary);
void PhongThanEscortTrapScope(int player,int delta);
bool PhongThanEscortMayChangeMap(int player);
bool PhongThanEscortMounted(int player);
bool PhongThanEscortPkLocked(int player);
bool PhongThanEscortCartKilled(int npc,int robber);
int PhongThanEscortRelation(int first,int second);
#endif
