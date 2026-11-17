#ifndef PT_VTT_ARENA_H
#define PT_VTT_ARENA_H
class KRegion;
bool PhongThanVttBarrierAt(const KRegion* region,int x,int y);
void PhongThanVttArenaTick();
void PhongThanVttResetBarrierState();
bool PhongThanVttApplyBarrierState(const void* bytes,unsigned int size);
void PhongThanVttBarrierNotify(int npc);
bool PhongThanVttMonsterCanMove(int npc,int x,int y);
bool PhongThanVttSegmentBlocked(int world,int x0,int y0,int x1,int y1);
bool PhongThanVttCanDamage(int target,int launcher);
#endif
