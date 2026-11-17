#ifndef PHONGTHAN_AUCTION_H
#define PHONGTHAN_AUCTION_H
#include "PhongThanPlayerServices.h"
enum {PT_AUCTION_QUERY=0x4123,PT_AUCTION_REPLY=0x4023,PT_AUCTION_ACTION=0x50544141,PT_AUCTION_GET=0x50544147,PT_AUCTION_ROWS=7};
enum {PT_AUCTION_BROWSE=0,PT_AUCTION_MY_BIDS=1,PT_AUCTION_OWN=2,PT_AUCTION_ORDERS=3,PT_AUCTION_MY_ORDERS=4,PT_AUCTION_DELIVERIES=5};
enum {PT_AUCTION_READ=0,PT_AUCTION_POST=1,PT_AUCTION_BID=2,PT_AUCTION_BUY=3,PT_AUCTION_CANCEL=4,PT_AUCTION_CLAIM=5,PT_AUCTION_ORDER=6,PT_AUCTION_SUPPLY=7};
#pragma pack(push,1)
struct PTAuctionCommand {
 PHONGTHAN_WIRE_HEADER Header;unsigned int ActorId,MapId,Id,Serial,Revision;
 int Operation,View,Category,Page,Price,Buyout,Hours,Count;char Search[80];
};
struct PTAuctionRow {
 unsigned int Id,Revision,Expires;int Kind,State,Price,Buyout,Money;char Owner[32],Bidder[32];PT_PS_ITEM Item;
};
struct PTAuctionPage {
 PHONGTHAN_WIRE_HEADER Header;unsigned int ActorId,MapId;
 int Operation,View,Category,Page,Count,Total,Result,Pending,Deliveries;PTAuctionRow Rows[PT_AUCTION_ROWS];
};
#pragma pack(pop)
inline bool PTAuctionValidCommand(const PTAuctionCommand& q){
 return q.Operation>=0&&q.Operation<=7&&q.View>=0&&q.View<=5&&q.Category>=0&&q.Category<=10&&q.Page>=0&&q.Page<300&&
  q.Price>=0&&q.Price<=1000000000&&q.Buyout>=0&&q.Buyout<=1000000000&&q.Count>=0&&q.Count<=9999&&PtPsText(q.Search,80);
}
inline bool PTAuctionValidPage(const PTAuctionPage& p){
 if(p.View<0||p.View>5||p.Count<0||p.Count>PT_AUCTION_ROWS||p.Total<0||p.Total>2048||p.Page<0||p.Page>=300)return false;
 for(int i=0;i<p.Count;++i){const PTAuctionRow& r=p.Rows[i];if(!r.Id||r.Price<0||r.Money<0||!memchr(r.Owner,0,32)||!memchr(r.Bidder,0,32)||!memchr(r.Item.Name,0,80))return false;}
 return true;
}
#ifndef _SERVER
int PhongThanAuctionAction(const PTAuctionCommand* q);
int PhongThanAuctionGet(PTAuctionPage* page);
#endif
#endif
