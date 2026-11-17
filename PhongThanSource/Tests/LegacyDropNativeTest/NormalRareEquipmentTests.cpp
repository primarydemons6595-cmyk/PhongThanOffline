#include <stdio.h>
#include "NormalRareEquipmentPolicy.h"
static int checks=0;
#define CHECK(x) do{++checks;if(!(x)){printf("FAIL line %d\n",__LINE__);return 1;}}while(0)
int main(){
 using namespace NormalRareEquipment;
 CHECK(Eligible(true,false,false,false,0,1,0,false));
 CHECK(!Eligible(false,false,false,false,0,1,0,false));
 CHECK(!Eligible(true,true,false,false,0,1,0,false));
 CHECK(!Eligible(true,false,true,false,0,1,0,false));
 CHECK(!Eligible(true,false,false,true,0,1,0,false));
 CHECK(!Eligible(true,false,false,false,1,1,0,false));
 CHECK(!Eligible(true,false,false,false,0,2,0,false));
 CHECK(!Eligible(true,false,false,false,0,1,2,false));
 CHECK(!Eligible(true,false,false,false,0,1,0,true));
 for(int quality=0;quality<=5;++quality){
  int kept=0;
  for(unsigned long roll=0;roll<10000;++roll){
   if(Keep(true,true,quality,roll))++kept;
   CHECK(Keep(false,true,quality,roll)); // bosses/specials unaffected
   CHECK(Keep(true,false,quality,roll)); // money, materials and quest items
  }
  CHECK(kept==KeepRate(quality));
 }
 CHECK(KeepRate(0)==10000);CHECK(KeepRate(1)==5000);
 CHECK(KeepRate(2)==1000);CHECK(KeepRate(3)==100);
 CHECK(KeepRate(4)==250);CHECK(KeepRate(5)==100);
 printf("Normal rare equipment: %d checks passed\n",checks);return 0;
}
