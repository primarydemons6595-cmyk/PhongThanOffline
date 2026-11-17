#ifndef PT_THUONG_CHU_H
#define PT_THUONG_CHU_H
#ifdef _SERVER
struct lua_State;typedef struct lua_State Lua_State;
bool PhongThanThuongChuOpen(int registration,int duration);
void PhongThanThuongChuClose();
int PhongThanThuongChuPhase();
void PhongThanThuongChuTick();
int PhongThanThuongChuRelation(int a,int b);
bool PhongThanThuongChuDeath(int victim,int killer);
int LuaThuongChu(Lua_State* L);
#endif
#endif
