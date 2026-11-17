#ifndef PHONGTHAN_PLAYER_TARGET_H
#define PHONGTHAN_PLAYER_TARGET_H
enum { PT_PLAYER_TARGET_GET=0x50545047, PT_PLAYER_TARGET_SELECT=0x50545053 };
// Local UI bridge; no client-supplied stats are sent to the server.
struct PT_PLAYER_TARGET {
    unsigned int Id;int Index,Life,MaxLife,Level,Head,Sex;
    char Name[32];
};
#endif
