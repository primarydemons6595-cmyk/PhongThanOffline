#ifndef PHONGTHAN_COMBAT_LOG_H
#define PHONGTHAN_COMBAT_LOG_H
#include "PhongThanWorldProtocol.h"
enum {PT_COMBAT_LOG_EVENT=0x4020,PT_GDC_COMBAT_LOG=0x5054434c};
enum {PT_COMBAT_DAMAGE=1,PT_COMBAT_HIT=2,PT_COMBAT_DODGE=3,PT_COMBAT_DEFEAT=4};
#pragma pack(push,1)
struct PTCombatLogEvent {
 PHONGTHAN_WIRE_HEADER Header;
 PHONGTHAN_U32 MapId,Attacker,Target,Recipient;
 PHONGTHAN_S32 Amount;
 PHONGTHAN_U8 Kind,DamageType,Critical,Direction;
 char AttackerName[32],TargetName[32];
};
#pragma pack(pop)
inline bool PTCombatLogValid(const void* data,unsigned int size){
 if(!PhongThanWorldFixedPacket(data,size,PT_COMBAT_LOG_EVENT,PHONGTHAN_WIRE_FLAG_RESPONSE,sizeof(PTCombatLogEvent)))return false;
 const PTCombatLogEvent& e=*(const PTCombatLogEvent*)data;
 return e.MapId&&e.Attacker&&e.Target&&e.Recipient&&e.Kind>=PT_COMBAT_DAMAGE&&e.Kind<=PT_COMBAT_DEFEAT&&e.DamageType<=6&&e.Critical<=1&&e.Direction>=1&&e.Direction<=2&&e.Amount>=0&&e.AttackerName[31]==0&&e.TargetName[31]==0;
}
#endif
