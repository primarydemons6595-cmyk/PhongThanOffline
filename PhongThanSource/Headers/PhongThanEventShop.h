#ifndef PHONGTHAN_EVENT_SHOP_H
#define PHONGTHAN_EVENT_SHOP_H
#include "PhongThanWorldProtocol.h"
#include <stdio.h>
#include <string.h>
enum {PT_EVENT_SHOP=0x30c7,PT_EVENT_SHOP_BUY=0x31c7,PT_EVENT_SHOP_GET=0x50544553,PT_EVENT_SHOP_PURCHASE=0x50544554,PT_GDCNI_EVENT_SHOP=0x50544555,PT_EVENT_SHOP_DISMISS=0x50544556,PT_EVENT_SHOP_TOOLTIP=0x50544557};
struct PTEventShopTooltip {char Title[4096],Properties[1024],Description[1024],Image[256];int ImageFlags;};
inline unsigned int PTEventShopMaxLots(unsigned int count,unsigned int price){if(!count||count>64||price>1000000000)return 0;unsigned int max=10000/count;if(max>1000)max=1000;if(price&&max>1000000000/price)max=1000000000/price;return max;}
#pragma pack(push,1)
struct PTEventShopRow {PHONGTHAN_U32 Id,Count,PaymentKind,UnitPrice;PHONGTHAN_S32 Genre,Detail,Part,Level,Series;char Name[80],Price[128],Image[256],Description[2048],PaymentName[80];};
struct PTEventShopData {PHONGTHAN_WIRE_HEADER Header;PHONGTHAN_U32 MapId,ActorId,NpcEntity,EventId,Token,Sequence,Count,Theme;PHONGTHAN_S32 Result;char Title[80];PTEventShopRow Rows[19];};
struct PTEventShopBuy {PHONGTHAN_WIRE_HEADER Header;PHONGTHAN_U32 MapId,ActorId,EventId,Token,Sequence,GoodsId,Lots;};
#pragma pack(pop)
inline unsigned int PTEventShopSize(unsigned int count){return sizeof(PTEventShopData)-sizeof(PTEventShopRow)*(19-count);}
inline bool PTValidateEventShop(const void* bytes,unsigned int size){
 if(size<PTEventShopSize(0))return false;const PTEventShopData& d=*(const PTEventShopData*)bytes;
 if(d.Theme>4||d.Count>19||!PhongThanWorldFixedPacket(bytes,size,PT_EVENT_SHOP,PHONGTHAN_WIRE_FLAG_RESPONSE,PTEventShopSize(d.Count))||!d.MapId||!d.ActorId||!d.EventId||!d.Token||!d.Sequence||d.Title[79])return false;
 for(unsigned int i=0;i<d.Count;++i){const PTEventShopRow& r=d.Rows[i];if(!r.Id||!r.Count||r.Genre<0||r.Genre>8||r.Detail<0||r.Detail>100000||r.Part<0||r.Part>100000||r.Level<0||r.Level>1000||r.Series<0||r.Series>1000||r.Name[79]||r.Price[127]||r.Image[255]||r.Description[2047])return false;for(unsigned int j=0;j<i;++j)if(d.Rows[j].Id==r.Id)return false;}
 for(unsigned int p=0;p<d.Count;++p)if(d.Rows[p].PaymentKind>2||d.Rows[p].PaymentName[79]||!PTEventShopMaxLots(d.Rows[p].Count,d.Rows[p].UnitPrice))return false;
 return true;
}
inline bool PTValidateEventShopBuy(const void* bytes,unsigned int size){
 if(!PhongThanWorldFixedPacket(bytes,size,PT_EVENT_SHOP_BUY,PHONGTHAN_WIRE_FLAG_REQUEST,sizeof(PTEventShopBuy)))return false;
 const PTEventShopBuy& r=*(const PTEventShopBuy*)bytes;return r.MapId&&r.ActorId&&r.EventId&&r.Token&&r.Sequence&&r.GoodsId&&r.Lots&&r.Lots<=1000;
}
inline void PTEventShopSilver(int amount,char* text){
 if(amount<10000){sprintf(text,"\045\144\040\154\255\356\156\147",amount);return;}
 int whole=amount/10000,fraction=amount%10000;if(!fraction){sprintf(text,"\045\144\040\166\271\156",whole);return;}
 char digits[8];sprintf(digits,"%04d",fraction);int n=strlen(digits);while(n&&digits[n-1]=='0')digits[--n]=0;sprintf(text,"\045\144\054\045\163\040\166\271\156",whole,digits);
}
#ifdef _SERVER
typedef void (*PTEventShopExternalHandler)(int,const PTEventShopBuy&);
inline PTEventShopExternalHandler& PTEventShopExternal(){static PTEventShopExternalHandler handler=0;return handler;}
void PhongThanEventShopBuy(int,const PTEventShopBuy&);
#else
void PhongThanReceiveEventShop(const void*,unsigned int);
bool PhongThanEventShopGet(PTEventShopData*);
bool PhongThanEventShopPurchase(unsigned int,unsigned int);
bool PhongThanEventShopTooltip(unsigned int,PTEventShopTooltip*);
void PhongThanEventShopDismiss();
#endif
#endif
