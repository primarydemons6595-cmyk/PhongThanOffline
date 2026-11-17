#ifndef PT_PK_PUNISHMENT_RUNTIME_H
#define PT_PK_PUNISHMENT_RUNTIME_H
#ifdef _SERVER
void PhongThanPkValueChanged(int player,int before,int after);
void PhongThanPkPunishmentTick(int player);
bool PhongThanPkDestination(int player,unsigned long* map,int* x,int* y);
void PhongThanPkRevivalPos(int player,int* map,int* x,int* y);
void PhongThanPkDeathItems(int player,int mode);
int PhongThanPkRaw(int player);
void PhongThanPkSetRaw(int player,int minutes);
int PhongThanUsePkWater(int player,int item);
#endif
#endif
