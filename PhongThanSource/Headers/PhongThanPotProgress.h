#ifndef PHONGTHAN_POT_PROGRESS_H
#define PHONGTHAN_POT_PROGRESS_H
#include "PhongThanWorldProtocol.h"
enum {PT_POT_PROGRESS=0x30c6};
#pragma pack(push,1)
struct PTPotProgress {
 PHONGTHAN_WIRE_HEADER Header;
 PHONGTHAN_U32 MapId,EntityId,Now,Started,Ready,Burn;
 char Text[256];
};
#pragma pack(pop)
inline bool PTValidatePotProgress(const void* data,unsigned int size){
 if(!PhongThanWorldFixedPacket(data,size,PT_POT_PROGRESS,PHONGTHAN_WIRE_FLAG_RESPONSE,sizeof(PTPotProgress)))return false;
 const PTPotProgress& p=*(const PTPotProgress*)data;
 return p.MapId&&p.EntityId&&p.Started&&p.Started<p.Ready&&p.Ready<p.Burn&&p.Burn-p.Started<=1209600&&p.Text[255]==0;
}
inline int PTPotRemainingPercent(const PTPotProgress& p,unsigned int now){
 if(now>=p.Ready)return 0;
 if(now<=p.Started)return 100;
 return (int)(((double)(p.Ready-now)*100+p.Ready-p.Started-1)/(p.Ready-p.Started));
}
#ifndef _SERVER
void PhongThanReceivePotProgress(const PTPotProgress&);
bool PhongThanGetPotProgress(unsigned int map,unsigned int entity,PTPotProgress&);
#endif
#endif
