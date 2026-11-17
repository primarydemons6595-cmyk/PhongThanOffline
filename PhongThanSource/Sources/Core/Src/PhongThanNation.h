#ifndef PHONGTHAN_NATION_H
#define PHONGTHAN_NATION_H
#include "../../../Headers/PhongThanNationProtocol.h"
#ifdef _SERVER
void PhongThanNationSyncPlayer(int player);
void PhongThanNationOpenNpc(int player,int operation);
int PhongThanNationDuty(int player);
int PhongThanNationPending(int player);
int PhongThanNationLeave(int player);
int PhongThanNationDissolve(int player);
#endif
#endif
