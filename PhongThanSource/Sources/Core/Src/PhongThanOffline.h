#ifndef PHONGTHAN_OFFLINE_H
#define PHONGTHAN_OFFLINE_H
void PhongThanOfflineLeaving(int player);
void PhongThanOfflineLogin(int player);
void PhongThanOfflineTick(int player);
int PhongThanUseOfflinePill(int player,int item);
bool PhongThanProcessOffline(int player,const char* bytes,int size);
#endif
