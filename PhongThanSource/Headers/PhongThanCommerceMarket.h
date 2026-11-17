#ifndef PHONGTHAN_COMMERCE_MARKET_H
#define PHONGTHAN_COMMERCE_MARKET_H
#include <string.h>
#include "PhongThanCommerceData.h"

// One server-thread market. Cargo/cash belong to existing player task storage.
class PhongThanCommerceMarket {
    unsigned long epoch[9],seed;
    int initialized[9],stock[9][27],purchase[9][27],sale[9][27];
    unsigned long Random(unsigned long limit) {
        seed=seed*1664525UL+1013904223UL;
        return limit ? ((seed>>8)%limit) : 0;
    }
    int Draw(int reference,const int *ranges) {
        // Original shop 9/goods 25 has weights totaling 105. Treat them as
        // relative weights rather than silently dropping the last five.
        int draw=(int)Random(ranges[2]+ranges[5]+ranges[8]),offset=0;
        for(int i=0;i<9;i+=3){
            if(draw<ranges[i+2]){offset=ranges[i]+(int)Random(ranges[i+1]-ranges[i]+1);break;}
            draw-=ranges[i+2];
        }
        const int price=reference+offset;
        return price>0 ? price : 1;
    }
    static int Valid(int shop,int good){return shop>=1 && shop<=9 && good>=1 && good<=27;}
    void Refresh(int shop,unsigned long now) {
        const int s=shop-1;
        if(initialized[s] && now>=epoch[s] && now-epoch[s]<pt_shop_refresh_seconds[s])return;
        initialized[s]=1;epoch[s]=now;
        for(int g=0;g<27;++g){
            const PhongThanPriceRule &r=pt_price_rules[s][g];
            const int offered=r.purchase[2]+r.purchase[5]+r.purchase[8]>0;
            stock[s][g]=offered?pt_goods_rules[g].restock:0;
            purchase[s][g]=offered?Draw(r.reference,r.purchase):0;
            sale[s][g]=Draw(r.reference,r.sale);
        }
    }
public:
    PhongThanCommerceMarket(unsigned long randomSeed=1):seed(randomSeed){memset(initialized,0,sizeof(initialized));memset(epoch,0,sizeof(epoch));}
    const char *Name(int good)const{return good>=1 && good<=27?pt_goods_rules[good-1].name:"";}
    int Weight(int good)const{return good>=1 && good<=27?pt_goods_rules[good-1].weight:0;}
    int Stock(int shop,int good,unsigned long now){if(!Valid(shop,good))return 0;Refresh(shop,now);return stock[shop-1][good-1];}
    int Purchase(int shop,int good,unsigned long now){if(!Valid(shop,good))return 0;Refresh(shop,now);return purchase[shop-1][good-1];}
    int Sale(int shop,int good,unsigned long now){if(!Valid(shop,good))return 0;Refresh(shop,now);return sale[shop-1][good-1];}
    int ChangeStock(int shop,int good,int delta,unsigned long now){
        if(!Valid(shop,good) || delta < -100000 || delta > 100000)return 0;
        Refresh(shop,now);const int before=stock[shop-1][good-1];
        if(delta < -before || delta > 100000-before)return 0;
        stock[shop-1][good-1]=before+delta;return 1;
    }
};
#endif
