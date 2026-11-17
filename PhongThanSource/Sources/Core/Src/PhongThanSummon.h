#ifndef PHONGTHAN_SUMMON_H
#define PHONGTHAN_SUMMON_H
#include "../../../Headers/PhongThanSummonRules.h"

// Ordinary summon registry recovered from PT vng00.pak / 27955344.
inline bool PhongThanIsSummon(int id) { return id >= 450 && id <= 460; }
inline int PhongThanSummonTemplate(int id)
{
    static const int templates[11] = {359,360,361,362,403,404,405,406,407,1345,1346};
    return PhongThanIsSummon(id) ? templates[id - 450] : -1;
}
inline int PhongThanSummonCost(int base, int param, int level)
{
    if (base < 0 || base > 100000 || param < 0 || param > 1048576 ||
        level < 1 || level >= 64) return -1;
    return base + ((param * level) >> 10);
}
int PhongThanSummonRelation(int first, int second);
#ifndef _SERVER
int PhongThanClientSummonOwner(int pet);
int PhongThanClientSummonRank(int pet);
int PhongThanClientSummonLocked(int pet);
 bool PhongThanIsClientSummon(int pet);
void PhongThanSummonBindClientOwner(DWORD petId, DWORD ownerId,int rank,int locked);
#endif
#ifdef _SERVER
bool PhongThanCastSummon(int owner, int skill);
bool PhongThanSummonTracked(int npc);
bool PhongThanSummonEscortPaused(int npc);
void PhongThanSummonEscortPause(int owner,BOOL paused);
void PhongThanSummonRemoving(int npc);
// -1: not tracked, 0: handled/following/removed, >0: validated monster target.
int PhongThanSummonThink(int npc);
int PhongThanSummonRelation(int first, int second);
void PhongThanSummonOwnerAttack(int owner, int target);
void PhongThanSummonOwnerTick(int owner);
void PhongThanSummonOwnerAttacked(int owner, int attacker);
void PhongThanSummonTakingHit(int victim, int attacker);
void PhongThanSummonSyncOwner(int pet, int client, BOOL broadcast);
int PhongThanSummonTargetRange(int owner);
int PhongThanSummonTrainingOwner(int pet, int& skill);
bool PhongThanSummonInfo(int owner,int& skill,int& type);
bool PhongThanSummonSetLock(int owner,int locked);
int PhongThanSummonGetLock(int owner);
bool PhongThanSummonMorph(int owner,int skill,int type);
#endif
#endif
