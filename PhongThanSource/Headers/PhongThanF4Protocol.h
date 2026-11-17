#ifndef PHONGTHAN_F4_PROTOCOL_H
#define PHONGTHAN_F4_PROTOCOL_H
#include "PhongThanWorldProtocol.h"
#include "PhongThanBankSecurity.h"
enum { PT_F4_COMMAND=0x3110, PT_F4_LAYOUT=0x3010, PT_GOI_F4=0x50544634,
    PT_GDCNI_F4_LAYOUT=0x50544635, PT_F4_SORT=1, PT_F4_UNLOCK=2, PT_F4_REALM=3, PT_F4_BANK_MENU=4 };
#pragma pack(push,1)
struct PTF4Command {
    PHONGTHAN_WIRE_HEADER Header;
    PHONGTHAN_U32 MapId,EntityId;
    PHONGTHAN_U8 Action,Page,ExpectedRows,Reserved;
};
struct PTF4Cell { PHONGTHAN_U32 ItemId; PHONGTHAN_U8 X,Y; };
struct PTF4Layout {
    PHONGTHAN_WIRE_HEADER Header;
    PHONGTHAN_U32 MapId,EntityId;
    PHONGTHAN_U8 Page,Count,Rows[3],Reserved;
    PTF4Cell Items[60];
};
#pragma pack(pop)
inline int PTValidateF4Command(const void* data,unsigned int size) {
    if(!PhongThanWorldFixedPacket(data,size,PT_F4_COMMAND,PHONGTHAN_WIRE_FLAG_REQUEST,sizeof(PTF4Command)))return 0;
    const PTF4Command& c=*(const PTF4Command*)data;
    return c.MapId && c.EntityId && c.Page<9 && c.ExpectedRows<=7 && !c.Reserved &&
        ((c.Action==PT_F4_REALM && c.Page<2 && c.ExpectedRows==0) ||
         (c.Action==PT_F4_SORT && (c.Page<4 || c.ExpectedRows==0)) ||
         (c.Action==PT_F4_BANK_MENU && c.Page>=5 && c.Page<=8 && c.ExpectedRows==0) ||
         (c.Action==PT_F4_UNLOCK && c.Page>0 && c.Page<4 && c.ExpectedRows<7));
}
#endif
