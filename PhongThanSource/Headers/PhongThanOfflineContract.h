#ifndef PHONGTHAN_OFFLINE_CONTRACT_H
#define PHONGTHAN_OFFLINE_CONTRACT_H
// Character-owned persisted slots, explicitly split from island/realm tasks.
namespace PtOfflineTask {enum {Mode=5380,Start=5381,Skill=5382,Day=5383,ExpHours=5384,
 ReservedGemHours=5385,ReservedSkillHours=5386,Receipt=5387,Pending=5388,GemRolls=5389};}
inline int PhongThanOfflineHours(int start,int now){if(start<=0||now<=start)return 0;__int64 hours=((__int64)now-start)/3600;return hours>4?4:(int)hours;}
inline int PhongThanOfflinePending(int packed,int mode){return mode>=0&&mode<3?(packed>>(mode*4))&15:0;}
inline int PhongThanOfflineSetPending(int packed,int mode,int value){return (packed&~(15<<(mode*4)))|(value<<(mode*4));}
#endif
