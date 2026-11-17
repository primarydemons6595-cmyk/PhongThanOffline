#ifndef PHONGTHAN_AUTO_RECOVERY_H
#define PHONGTHAN_AUTO_RECOVERY_H
#define PT_AUTO_RECOVERY_ROW 0x50545243
#define PT_AUTO_COMBAT_MODE 0x5054434d
#define PT_AUTO_NATIVE_BUFF 0x50544255
#ifndef _SERVER
void PhongThanSetNativeBuff(int slot,int skill);
#endif
#define PT_AUTO_SUMMON 0x50545355
#ifndef _SERVER
void PhongThanSetSummonAuto(int skill,int petLife,int selfLife);
#endif
#define PT_AUTO_RANGE_PREVIEW 0x50545250
#ifndef _SERVER
void PhongThanSetRecoveryRow(int slot,int detail,int percent);
void PhongThanSetAutoMode(int bits,int radius,bool anchor);
void PhongThanSetAutoPreview(bool visible);
bool PhongThanGetAutoRange(int& x,int& y,int& radius);
int PhongThanGetMapRoutePoint(int index,int* x,int* y);
#endif
#endif
