#ifndef PT_VTT_BARRIER_PROTOCOL_H
#define PT_VTT_BARRIER_PROTOCOL_H
#include "PhongThanProtocol.h"
enum {PT_VTT_BARRIER_STATE=0x2016,PT_VTT_GATE_TEMPLATE=2704,PT_VTT_FENCE_DOWN=2705,PT_VTT_FENCE_UP=2706};
typedef unsigned __int64 PT_VTT_MASK;
#pragma pack(push,1)
struct PT_VTT_BARRIERS {PHONGTHAN_WIRE_HEADER Header;PHONGTHAN_U32 MapId,Session;PT_VTT_MASK ClosedWalls;PHONGTHAN_U32 GateClosed,Phase,Sequence;};
#pragma pack(pop)
#endif
