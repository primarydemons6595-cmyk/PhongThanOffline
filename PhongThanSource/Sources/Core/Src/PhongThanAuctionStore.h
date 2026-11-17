#ifndef PHONGTHAN_AUCTION_STORE_H
#define PHONGTHAN_AUCTION_STORE_H
#include "../../../Headers/PhongThanAuction.h"
enum {PT_AUCTION_LOTS=256,PT_AUCTION_CLAIMS=1024};
enum {PT_AL_FREE=0,PT_AL_POSTING=1,PT_AL_LIVE=2,PT_AL_BIDDING=3,PT_AL_FINISHED=4,PT_AL_ABORTED=5};
struct PTAuctionIdentity {char Account[32],Role[32];};
inline bool PTAuctionSame(const PTAuctionIdentity& a,const PTAuctionIdentity& b){return !strcmp(a.Account,b.Account)&&!strcmp(a.Role,b.Role);}
struct PTAuctionLot {
 unsigned int Id,Revision,Expires,Receipt;int State,Kind,Price,Buyout,PendingPrice,PendingOperation;
 PTAuctionIdentity Owner,Bidder,Pending;PT_PS_ITEM Item,PendingItem;
};
struct PTAuctionDelivery {
 unsigned int Id,Receipt;int State,Money,HasItem;PTAuctionIdentity Owner;PT_PS_ITEM Item;
};
struct PTAuctionBook {
 unsigned int Magic,Version,NextId;PTAuctionLot Lots[PT_AUCTION_LOTS];PTAuctionDelivery Claims[PT_AUCTION_CLAIMS];
};
class PTAuctionStore {
public:
 PTAuctionBook Book;bool Ready,Failed;
 PTAuctionStore():Ready(false),Failed(false){memset(&Book,0,sizeof(Book));}
 static bool Identity(const PTAuctionIdentity& id){return id.Account[0]&&id.Role[0]&&PtPsText(id.Account,32)&&PtPsText(id.Role,32);}
 bool Valid()const {
  if(Book.Magic!=0x43415450||Book.Version!=1||Book.NextId<1||Book.NextId>2000000000)return false;
  for(int i=0;i<PT_AUCTION_LOTS;++i){const PTAuctionLot& l=Book.Lots[i];if(!l.Id)continue;
   if(l.Id>=Book.NextId||l.State<1||l.State>5||l.Kind<0||l.Kind>1||!Identity(l.Owner)||!memchr(l.Item.Name,0,80)||l.Item.Stack<1||
      l.Price<0||l.Price>1000000000||l.Buyout<0||l.Buyout>1000000000)return false;
   if(l.State==PT_AL_BIDDING&&(!Identity(l.Pending)||!l.Receipt||l.Receipt>=Book.NextId||l.PendingPrice<0||l.PendingPrice>1000000000))return false;
  }
  for(int c=0;c<PT_AUCTION_CLAIMS;++c){const PTAuctionDelivery& d=Book.Claims[c];if(!d.Id)continue;
   if(d.Id>=Book.NextId||d.State<1||d.State>3||!Identity(d.Owner)||d.Money<0||d.Money>1000000000||d.HasItem<0||d.HasItem>1||
      d.HasItem&&(!memchr(d.Item.Name,0,80)||d.Item.Stack<1))return false;}
  return true;
 }
 bool Load(const char* path){
  if(Ready||Failed)return Ready;HANDLE f=CreateFileA(path,GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,0,0);
  if(f==INVALID_HANDLE_VALUE){if(GetLastError()!=ERROR_FILE_NOT_FOUND){Failed=true;return false;}Book.Magic=0x43415450;Book.Version=1;Book.NextId=1;Ready=true;return true;}
  DWORD n=0,hash=0;bool ok=GetFileSize(f,0)==sizeof(Book)+4&&ReadFile(f,&Book,sizeof(Book),&n,0)&&n==sizeof(Book)&&ReadFile(f,&hash,4,&n,0)&&n==4;CloseHandle(f);
  Ready=ok&&hash==PtPsHash(&Book,sizeof(Book))&&Valid();Failed=!Ready;return Ready;
 }
 bool Save(const char* path){
  if(!Ready||Failed||!Valid())return false;char temp[MAX_PATH];if(strlen(path)>MAX_PATH-8)return false;sprintf(temp,"%s.new",path);
  HANDLE f=CreateFileA(temp,GENERIC_WRITE,0,0,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,0);if(f==INVALID_HANDLE_VALUE)return false;
  DWORD n=0,hash=PtPsHash(&Book,sizeof(Book));bool ok=WriteFile(f,&Book,sizeof(Book),&n,0)&&n==sizeof(Book)&&WriteFile(f,&hash,4,&n,0)&&n==4&&FlushFileBuffers(f);CloseHandle(f);
  if(ok)ok=MoveFileExA(temp,path,MOVEFILE_REPLACE_EXISTING|MOVEFILE_WRITE_THROUGH)!=0;if(!ok)DeleteFileA(temp);return ok;
 }
 unsigned int Next(){return Book.NextId>=1999999999?0:Book.NextId++;}
 int Lot(unsigned int id){for(int i=0;i<PT_AUCTION_LOTS;++i)if(id&&Book.Lots[i].Id==id)return i;return -1;}
 int Claim(unsigned int id){for(int i=0;i<PT_AUCTION_CLAIMS;++i)if(id&&Book.Claims[i].Id==id)return i;return -1;}
 int FreeLot(){for(int i=0;i<PT_AUCTION_LOTS;++i)if(!Book.Lots[i].Id||Book.Lots[i].State>=PT_AL_FINISHED)return i;return -1;}
 int FreeClaims(){int count=0;for(int i=0;i<PT_AUCTION_CLAIMS;++i)if(!Book.Claims[i].Id||Book.Claims[i].State==3)++count;return count;}
 bool Deliver(const PTAuctionIdentity& owner,int money,const PT_PS_ITEM* item){
  if(!Identity(owner)||money<0||money>1000000000||Book.NextId>=1999999999)return false;
  for(int i=0;i<PT_AUCTION_CLAIMS;++i){PTAuctionDelivery& d=Book.Claims[i];if(d.Id&&d.State!=3)continue;
   memset(&d,0,sizeof(d));d.Id=Next();d.Owner=owner;d.State=1;d.Money=money;d.HasItem=item?1:0;if(item)d.Item=*item;return true;}return false;
 }
 bool Pending(const PTAuctionIdentity& owner)const{
  for(int i=0;i<PT_AUCTION_LOTS;++i){const PTAuctionLot& l=Book.Lots[i];if(l.State==PT_AL_POSTING&&PTAuctionSame(l.Owner,owner)||l.State==PT_AL_BIDDING&&PTAuctionSame(l.Pending,owner))return true;}
  for(int c=0;c<PT_AUCTION_CLAIMS;++c)if(Book.Claims[c].State==2&&PTAuctionSame(Book.Claims[c].Owner,owner))return true;return false;
 }
};
#endif
