#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../../Headers/PhongThanRealmWorld.h"
using namespace PtRealmWorld;
static void Day(State& s,int day){assert(Dragon(s,day,0,false));assert(!Dragon(s,day,0,false));assert(Dragon(s,day,1,false));assert(Dragon(s,day,2,false));}
int main(){
 State s;memset(&s,0,sizeof(s));assert(Valid(s));Day(s,100);assert(s.Streak==1&&s.Phase==0);Day(s,101);assert(s.Streak==2);assert(AdvanceDay(s,103)&&s.Streak==0);Day(s,103);assert(s.Streak==1);Day(s,104);Day(s,105);assert(s.Phase==1&&s.Streak==3);
 assert(!Dragon(s,106,0,true));for(int i=0;i<1499;++i)Reputation(s);assert(s.Phase==1&&s.Reputation==1499);Reputation(s);assert(s.Phase==2&&s.Reputation==1500);Reputation(s);assert(s.Reputation==1500);
 assert(!Dragon(s,110,0,false));assert(Dragon(s,110,0,true));assert(Dragon(s,110,1,true));assert(s.FinalMask==3);assert(AdvanceDay(s,111)&&s.FinalMask==0);assert(Dragon(s,111,2,true));assert(s.Phase==2);assert(Dragon(s,111,0,true));assert(Dragon(s,111,1,true)&&s.Phase==3);assert(!Dragon(s,112,0,true));
 for(int map=1073;map<=1077;++map){assert(!CanEnter(s,map,10,0,0));assert(CanEnter(s,map,13,1,1));assert(CanEnter(s,map,13,2,1));assert(!CanEnter(s,map,13,0,1));}
 assert(CanEnter(s,1073,11,0,0));assert(!CanEnter(s,1074,11,0,0));assert(!CanEnter(s,60015,11,0,0));assert(CanEnter(s,60015,13,1,1));assert(CanEnter(s,1055,0,0,0));
 State closed=s;closed.Phase=2;closed.FinalMask=0;assert(!CanEnter(closed,1073,13,1,80));assert(!Dragon(closed,100,0,true));
 for(int order=0;order<6;++order){const int permutations[6][3]={{0,1,2},{0,2,1},{1,0,2},{1,2,0},{2,0,1},{2,1,0}};State t;memset(&t,0,sizeof(t));
  for(int day=20;day<23;++day)for(int boss=0;boss<3;++boss)assert(Dragon(t,day,permutations[order][boss],false));assert(t.Phase==1&&t.Streak==3);
 }
 puts("PASS realm world: three consecutive days, missed-day reset, 1500 threshold, registered same-day final trio, all six kill orders and realm entry guards");return 0;
}
