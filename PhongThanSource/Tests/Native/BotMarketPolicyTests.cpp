#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../../Headers/PhongThanBotMarketPolicy.h"
#include "../../Headers/PhongThanBotAdPolicy.h"
#include "../../Headers/PhongThanChatMessageLayout.h"
#include "../../Headers/PhongThanItemLink.h"
#include "../../Headers/PhongThanBotMarketQuality.h"
int main(){
 PTBotMarketBonuses bad;bad.Add(magic_lightingres_p,8);assert(!bad.Eligible(1,8)&&!bad.Eligible(1,40));assert(!bad.Eligible(0,100));
 assert(bad.Eligible(2,1)&&bad.Eligible(3,1)&&bad.Eligible(4,8)&&bad.Eligible(5,1));
 PTBotMarketBonuses speed;speed.Add(magic_attackspeed_v,15);assert(!speed.Eligible(1,29)&&speed.Eligible(1,30));
 PTBotMarketBonuses cast;cast.Add(magic_castspeed_v,10);assert(cast.Eligible(1,40));
 PTBotMarketBonuses damage;damage.Add(magic_weapondamageenhance_p,20);assert(damage.Eligible(1,30));
 PTBotMarketBonuses solid;solid.Add(magic_lifemax_v,150);solid.Add(magic_lifemax_v,200);assert(!solid.Eligible(1,40));solid.Add(magic_allres_p,8);assert(solid.Eligible(1,40));
 int x=0,y=0;assert(PTBotMeetingCoordinates("gap o 214:145 Tay Ky",x,y)&&x==214*256&&y==145*512);
 assert(PTBotMeetingCoordinates("214, 145 Tay Ky",x,y));assert(!PTBotMeetingCoordinates("0:145",x,y));assert(!PTBotMeetingCoordinates("999999:145",x,y));assert(!PTBotMeetingCoordinates("Tay Ky",x,y));
 assert(PTBotMeetingCoordinates("221/199 Ngoc Hu Cung",x,y)&&x==56576&&y==101888);
 assert(PTBotMapOnlyRequest("toi ngoc hu cung cho tui","ngoc hu cung"));assert(!PTBotMapOnlyRequest("le quan ngoc hu cung","ngoc hu cung"));assert(!PTBotMapOnlyRequest("dai phu ngoc hu cung","ngoc hu cung"));
 assert(PTBotAskingPrice(0,0,0,1)==100);assert(PTBotAskingPrice(2000000000,2000000000,6,2000000000)==500000000);
 assert(PTBotSalePayment(1000,1000,0));assert(!PTBotSalePayment(1000,999,0));assert(PTBotSalePayment(1000,200,800));assert(!PTBotSalePayment(1000,-1,5000));
 assert(PTBotFairWager(1000,1000));assert(PTBotFairWager(1000,1500));assert(!PTBotFairWager(1000,999));assert(!PTBotFairWager(1000,1501));assert(PTBotFairWager(0,0));
 PTBotAdLedger ledger;assert(ledger.Available(1,123,10000));ledger.Sent(1,123,10000);
 assert(!ledger.Available(1,456,10001));assert(!ledger.Available(2,123,700000));assert(ledger.Available(1,456,610000));assert(ledger.Available(2,123,3610000));
 // Native encoded icon includes a zero high byte. Keep length and clickable
 // label even when strlen would truncate before the actual advertised item.
 char styled[256];const char* body="Ban <Sword> 1000 luong bac.";int size=PTBotStyleAd(styled,sizeof(styled),body,strlen(body),1);
 assert(size>strlen(styled));assert(PTLocateItemLabel(styled,size,"<Sword>",7)>=0);assert(!PTBotStyleAd(styled,4,body,strlen(body),1));
 // Physical mixed-font rows fit in reserved base-line slots, including partial
 // scrolling. Hit boxes use the same slots, avoiding overlap with next message.
 for(int base=12;base<=16;base+=2)for(int font=12;font<=20;font+=2)for(int lines=1;lines<=10;++lines){
  int slots=PTChatLineSlots(lines,font,base);assert(slots*(base+1)>=lines*(font+1));
  for(int skip=0;skip<slots;++skip)for(int available=1;available<=12;++available){PTChatVisibleLines row(slots,lines,skip,available,font,base);assert(row.count>=0&&row.skip+row.count<=lines);if(row.count)assert(row.offset+row.count*(font+1)<=row.slots*(base+1));}
 }
 printf("PASS market pricing/payment, real coordinate scaling, wager parity, shared ad history, binary icon+item link, mixed-font scrolling\n");return 0;
}
