#ifndef PHONGTHAN_OFFLINE_PROTOCOL_H
#define PHONGTHAN_OFFLINE_PROTOCOL_H
enum {PT_OFFLINE_FAMILY=0x71,PT_GDC_OFFLINE_RESULT=0x50544f};
#pragma pack(push,1)
struct PTOfflineRequest {unsigned char Family,Version;unsigned short Skill;unsigned int Sequence;unsigned char Mode,Action;unsigned short Reserved;};
struct PTOfflineResponse {unsigned char Family,Version;unsigned short Result;unsigned int Sequence;};
#pragma pack(pop)
#endif
