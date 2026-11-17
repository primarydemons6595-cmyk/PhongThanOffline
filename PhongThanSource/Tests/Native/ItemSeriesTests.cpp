#include "../../Headers/PhongThanItemSeries.h"
#include <assert.h>
#include <stdio.h>
int main(){
 for(int n=0;n<5;++n){assert(PhongThanResolveItemSeries(1000,n,5,5)==n);assert(PhongThanResolveItemSeries(n,2,5,5)==n);}
 assert(PhongThanResolveItemSeries(1000,-1,5,5)==5);
 assert(PhongThanResolveItemSeries(1000,1000,5,5)==5);
 assert(PhongThanResolveItemSeries(1000,5,5,5)==5);
 assert(PhongThanResolveItemSeries(5,0,5,5)==5);
 for(int n=0;n<4;++n){assert(PhongThanResolveEquipmentSeries(13,4000,n,-1,5,5)==n);assert(PhongThanResolveEquipmentSeries(13,3000,n,3,5,5)==3);}
 assert(PhongThanResolveEquipmentSeries(13,4000,4000,-1,5,5)==5);
 assert(PhongThanResolveEquipmentSeries(13,3000,3000,3,5,5)==3);
 assert(PhongThanResolveEquipmentSeries(0,4000,2,-1,5,5)==4000);
 assert(PhongThanResolveEquipmentSeries(13,2,0,-1,5,5)==2);
 puts("PASS: VNG 1000 marker resolves before wire; explicit series preserved.");return 0;
}
