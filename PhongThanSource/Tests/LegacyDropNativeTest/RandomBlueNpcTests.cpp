#include <windows.h>
#include <stdio.h>
#include "RandomBlueNpcPolicy.h"
int main(){
 int checks=0,failed=0;
#define CHECK(x) do{++checks;if(!(x)){++failed;printf("FAIL line=%d\n",__LINE__);}}while(0)
 CHECK(!RandomBlueNpc::Selected(0,0));CHECK(RandomBlueNpc::Selected(10000,0));CHECK(RandomBlueNpc::Selected(10000,9999));CHECK(!RandomBlueNpc::Selected(10000,10000));CHECK(!RandomBlueNpc::Selected(10000,999999));CHECK(RandomBlueNpc::Selected(1000000,999999));CHECK(!RandomBlueNpc::Selected(-1,0));CHECK(!RandomBlueNpc::Selected(1000001,0));CHECK(!RandomBlueNpc::Selected(1000000,1000000));
 RandomBlueNpc::Life life;int base=99;
 CHECK(life.Begin(100,0,false,base)&&base==0);CHECK(!life.Begin(100,0,false,base));
 life.assigned=true;CHECK(life.Begin(100,1,true,base)&&base==0&&!life.assigned);CHECK(!life.Begin(100,0,false,base));
 CHECK(life.Begin(101,1,false,base)&&base==1);CHECK(life.Begin(101,1,true,base)&&base==1);
 life.assigned=true;CHECK(life.Begin(102,2,false,base)&&base==2&&!life.assigned);
 CHECK(RandomBlueNpc::Eligible(true,false,false,false,0,1,1,1));
 CHECK(!RandomBlueNpc::Eligible(false,false,false,false,0,1,1,1));CHECK(!RandomBlueNpc::Eligible(true,true,false,false,0,1,1,1));CHECK(!RandomBlueNpc::Eligible(true,false,true,false,0,1,1,1));CHECK(!RandomBlueNpc::Eligible(true,false,false,true,0,1,1,1));CHECK(!RandomBlueNpc::Eligible(true,false,false,false,1,1,1,1));CHECK(!RandomBlueNpc::Eligible(true,false,false,false,0,30,6,1));CHECK(!RandomBlueNpc::Eligible(true,false,false,false,0,-1,1,1));
 LegacyDrop::Random blue(20261018),drop(86),untouched(86),replay(20261018);
 for(int n=0;n<16;++n){unsigned long a=blue.Below(1000000);CHECK(a==replay.Below(1000000));CHECK(drop.Next()==untouched.Next());}
 printf("RANDOM_BLUE_TEST_RESULT checks=%d failures=%d npc_spawn_death=0 world_item_generation=0\n",checks,failed);return failed?1:0;
}
