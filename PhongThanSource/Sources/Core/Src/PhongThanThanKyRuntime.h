#ifndef PHONGTHAN_THAN_KY_RUNTIME_H
#define PHONGTHAN_THAN_KY_RUNTIME_H
#include "../../../Headers/PhongThanThanKy.h"
class KSkill;
bool PhongThanThanKyCanCast(int actor,int skill,int level);
bool PhongThanThanKyPreserveSkill(int player,int skill);
bool PhongThanThanKyCast(int actor,const KSkill* skill,int x,int y);
void PhongThanThanKyTick(int actor);
void PhongThanThanKyState(int source,int target,int skill,int level,void* attributes,int count);
bool PhongThanThanKyImmediate(int source,int target,void* attribute);
int PhongThanThanKyUseBook(int player,int item);
// Exact configured activation-book identity, not every magic-script item.
bool PhongThanThanKyIsActivationBook(int item);
void PhongThanThanKyHitBegin(int source,int target,int skill,int level,int& useAttackRate);
void PhongThanThanKyHitEnd();
int PhongThanThanKyDamage(int source,int target,int damage,int type,bool reflected);
bool PhongThanThanKyNoDebuff();
bool PhongThanThanKySkipDebuff(int skill,int level);
void PhongThanThanKyAfterHit(int source,int target);
int PhongThanThanKyMorphPlayer(int player,int npcTemplate,int ticks);
void PhongThanThanKyRestore(int player);
void PhongThanThanKyClearSelection(int player,int skill);
void PhongThanThanKySyncRemoveBuff(int actor,int skill,int level);
int PhongThanThanKyCarriedBookKey(int player,int skill,int rank);
#endif
