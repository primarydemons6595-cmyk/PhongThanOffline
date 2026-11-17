#ifndef PT_BANK_SECURITY_H
#define PT_BANK_SECURITY_H
#include "PhongThanWorldProtocol.h"
enum { PT_BANK_SECURITY_REQUEST=0x3146,PT_BANK_SECURITY_RESPONSE=0x3046,PT_GOI_BANK_SECURITY=0x50544250,PT_GDC_BANK_SECURITY=0x50544251 };
extern int g_PtBankLocked;
#pragma pack(push,1)
struct PTBankSecurityRequest {PHONGTHAN_WIRE_HEADER Header;PHONGTHAN_U32 MapId,EntityId;PHONGTHAN_U8 Action,Reserved[3];char Old[16],Password[16],Confirm[16];};
struct PTBankSecurityState {PHONGTHAN_WIRE_HEADER Header;PHONGTHAN_U32 MapId,EntityId;PHONGTHAN_S32 Result;PHONGTHAN_U8 Configured,Locked,Reserved[2];};
#pragma pack(pop)
inline bool PTValidateBankSecurity(const void* data,unsigned int size){
 if(!PhongThanWorldFixedPacket(data,size,PT_BANK_SECURITY_REQUEST,PHONGTHAN_WIRE_FLAG_REQUEST,sizeof(PTBankSecurityRequest)))return false;
 const PTBankSecurityRequest& q=*(const PTBankSecurityRequest*)data;
 return q.MapId&&q.EntityId&&q.Action<=4&&!q.Reserved[0]&&!q.Reserved[1]&&!q.Reserved[2]&&!q.Old[15]&&!q.Password[15]&&!q.Confirm[15];
}
#endif
