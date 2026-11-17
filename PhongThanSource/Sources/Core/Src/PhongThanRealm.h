#ifndef PHONGTHAN_REALM_RUNTIME_H
#define PHONGTHAN_REALM_RUNTIME_H
#include "../../../Headers/PhongThanCharacter.h"
#ifdef _SERVER
bool PhongThanRealmDestination(int player,unsigned long map);
bool PhongThanRealmStoredDestination(const PHONGTHAN_CHARACTER_STATE_HEADER*,int map);
int PhongThanRealmUseItem(int player,int item);
void PhongThanRealmQuestKill(int,int);
void PhongThanRealmQuestTick();
bool PhongThanRealmIsHuntNpc(int);
#endif
#endif
