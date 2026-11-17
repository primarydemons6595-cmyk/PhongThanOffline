#pragma once
class KPlayer;
int PhongThanConsumableInitialCharges(int genre,int detail,int part,int capacity,const char* script);
void PhongThanConsumableApply(KPlayer& player);
void PhongThanConsumableClear(KPlayer& player);
#ifdef _SERVER
int PhongThanUseConsumable(KPlayer& player, int item);
BOOL PhongThanConsumableAutoDose(KPlayer& player,int item);
int PhongThanConsumableRecoveryKind(int genre,int detail,int part,const char* script);
void PhongThanConsumableTick(KPlayer& player);
BOOL PhongThanConsumableChatColor(KPlayer& player);
void PhongThanConsumableSnapshot(KPlayer& player, void* packet);
#else
BOOL PhongThanConsumableReceive(KPlayer& player, const void* packet);
int PhongThanConsumableUi(void* output, int size);
#endif
