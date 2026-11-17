#ifndef PHONGTHAN_ITEM_LINK_H
#define PHONGTHAN_ITEM_LINK_H
#include "PhongThanProtocol.h"
#include <string.h>
#define PT_ITEM_LINK_REQUEST 0x7B10
#define PT_ITEM_LINK_MESSAGE 0x7B11
#define PT_GDCNI_ITEM_LINK 0x5054494C
// KMagicAttribRegistry.inc: original IDs 0..373, Loose extensions 374..387.
#define PT_ITEM_LINK_ATTRIBUTE_LIMIT 388
inline void PTPlainItemName(char* out,int capacity,const char* source) {
    if(!out || capacity<1)return;int used=0;
    if(!source){out[0]=0;return;}
    while(*source=='#' || *source=='$')++source;
    while(*source && used<capacity-1) {
        if(!strncmp(source,"<c=",3) || !strncmp(source,"<color=",7) || !strncmp(source,"<bclr=",6)) {
            const char* end=strchr(source,'>');if(end){source=end+1;continue;}
        }
        if(!strncmp(source,"<c>",3)){source+=3;continue;}
        if(!strncmp(source,"<color>",7)){source+=7;continue;}
        if(!strncmp(source,"<bclr>",6)){source+=6;continue;}
        out[used++]=*source++;
    }out[used]=0;
}
// Must match GDI_ITEM_NAME's 64-byte buffer and the client chat button label.
inline bool PTFormatItemChatLabel(char* out,int capacity,const char* source) {
    if(!out || capacity<1)return false;out[0]=0;
    char name[64];PTPlainItemName(name,sizeof(name),source);int size=strlen(name);
    if(!size || size+3>capacity)return false;
    out[0]='<';memcpy(out+1,name,size);out[size+1]='>';out[size+2]=0;return true;
}
inline int PTLocateItemLabel(const char* text,int size,const char* label,int length) {
    if(!text || !label || size<0 || length<1 || length>size)return -1;
    for(int i=0;i<=size-length;++i)if(!memcmp(text+i,label,length))return i;
    return -1;
}
#pragma pack(push,1)
struct PTItemLinkRequest {
    PHONGTHAN_WIRE_HEADER Header;
    PHONGTHAN_U32 ItemId,Channel;
    char Target[32];
    PHONGTHAN_U16 TextSize;
    PHONGTHAN_U8 Flags,Reserved;
    char Text[255];
};
struct PTItemLinkAttribute {PHONGTHAN_S32 Type,Value[3];};
struct PTItemLinkMessage {
    PHONGTHAN_WIRE_HEADER Header;
    PHONGTHAN_U32 Token,Channel;
    char Sender[32],Conversation[32],Name[80];
    PHONGTHAN_S32 Genre,Detail,Particular,Row,Series,Level,Luck,Version;
    PHONGTHAN_U32 Seed;
    PHONGTHAN_S32 Stack,Durability,Expire,Param,Fortune,Upgrade,PhysicalPower,MagicPower;
    PHONGTHAN_S32 Generator[40];
    PTItemLinkAttribute Base[20],Require[6],Magic[20];
    PHONGTHAN_U16 TextSize;
    PHONGTHAN_U8 Flags,Reserved;
    char Text[255];
};
#pragma pack(pop)
inline bool PTValidateItemLinkRequest(const void* data,unsigned size) {
    if(!data || size!=sizeof(PTItemLinkRequest))return false;
    const PTItemLinkRequest& r=*(const PTItemLinkRequest*)data;
    return PhongThanValidateWireHeader(&r.Header,size) && r.Header.PacketSize==size &&
        r.Header.MessageType==PT_ITEM_LINK_REQUEST && r.Header.Flags==PHONGTHAN_WIRE_FLAG_REQUEST &&
        r.ItemId && !r.Reserved && r.TextSize>0 && r.TextSize<=255 && r.Target[31]==0 &&
        (r.Channel || r.Target[0]);
}
inline bool PTValidateItemLinkMessage(const void* data,unsigned size) {
    if(!data || size!=sizeof(PTItemLinkMessage))return false;
    const PTItemLinkMessage& r=*(const PTItemLinkMessage*)data;
    if(!PhongThanValidateWireHeader(&r.Header,size) || r.Header.PacketSize!=size ||
        r.Header.MessageType!=PT_ITEM_LINK_MESSAGE || r.Header.Flags!=PHONGTHAN_WIRE_FLAG_RESPONSE ||
        !r.Token || !r.Sender[0] || r.Sender[31] || r.Conversation[31] || r.Name[79] ||
        r.Genre<0 || r.Genre>255 || r.Detail<0 || r.Row < -1 || r.Level<0 || r.Level>255 ||
        r.Series<0 || r.Series>255 || r.Stack<1 || r.TextSize<1 || r.TextSize>255 || r.Reserved)return false;
    const PTItemLinkAttribute* groups[]={r.Base,r.Require,r.Magic};const int count[]={20,6,20};
    for(int g=0;g<3;++g)for(int i=0;i<count[g];++i)if(groups[g][i].Type<0 || groups[g][i].Type>=PT_ITEM_LINK_ATTRIBUTE_LIMIT)return false;
    return true;
}
#ifndef _SERVER
void PTReceiveItemLink(const void*,unsigned);
int PTCreateItemLinkPreview(const char*);
void PTReleaseItemLinkPreview(int);
#endif
#endif
