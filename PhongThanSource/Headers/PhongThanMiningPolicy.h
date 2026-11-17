#ifndef PHONGTHAN_MINING_POLICY_H
#define PHONGTHAN_MINING_POLICY_H
enum {PT_MINING_MAX_SITES=128,PT_MINING_MAX_LOOT=128,PT_MINING_SKILL=62,PT_MINING_PICK=8,PT_MINING_MAX_REACH=512};
struct PTMiningSite {int id,map,x,y,capacity,respawn,reach,table,enabled;char name[128];};
struct PTMiningLoot {int table,genre,detail,part,level,series,weight,minimum,maximum;};
inline bool PtMiningSiteValid(const PTMiningSite& s){return s.id>0&&s.id<=1000000&&s.map>0&&s.map<=1000000&&s.x>=0&&s.y>=0&&s.capacity>0&&s.capacity<=1000000&&s.respawn>0&&s.respawn<=2592000&&s.reach>=64&&s.reach<=PT_MINING_MAX_REACH&&s.table>=0&&s.table<128&&(s.enabled==0||s.enabled==1);}
inline bool PtMiningLootValid(const PTMiningLoot& l){return l.table>=0&&l.table<128&&(l.genre==0||l.genre==1||l.genre==3||l.genre==4||l.genre==6||l.genre==7||l.genre==8)&&l.detail>=0&&l.detail<=99999&&l.part>=0&&l.part<=99999&&l.level>=0&&l.level<=9999&&l.series>=0&&l.series<=5&&l.weight>0&&l.weight<=1000000&&l.minimum>0&&l.minimum<=20&&l.maximum>=l.minimum&&l.maximum<=20;}
inline int PtMiningChance(int base,int bonus,int luck){if(base<0||base>10000||bonus<0||bonus>1000)return 0;if(luck<0)luck=0;if(luck>10000)luck=10000;__int64 n=(__int64)base+(__int64)luck*bonus;return n>10000?10000:(int)n;}
inline bool PtMiningNear(int x,int y,int tx,int ty,int reach){__int64 dx=(__int64)x-tx,dy=(__int64)y-ty;return reach>=0&&dx*dx+dy*dy<=(__int64)reach*reach;}
#endif
