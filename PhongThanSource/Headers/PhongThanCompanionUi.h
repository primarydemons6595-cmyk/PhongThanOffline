#ifndef PHONGTHAN_COMPANION_UI_H
#define PHONGTHAN_COMPANION_UI_H
enum {PT_COMPANION_HUD_GET=0x50544850};
struct PT_COMPANION_HUD {
 unsigned int Id;int Level,Life,MaxLife,Locked;char Name[64],Portrait[128];
};
#endif
