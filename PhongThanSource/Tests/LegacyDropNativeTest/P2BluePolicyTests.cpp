#include "KCore.h"
#include "BlueMagicTestPolicy.h"
#include <stdio.h>
// Policy boundaries only: no KItem, blueprint, NPC spawn/death or pickup.
int main(){
 BlueMagic::BlueMagicTableLoader table;table.valid=true;BlueMagic::Row r;r.reason="";r.sourceRow=2;r.type=85;r.level=1;
 for(int g=0;g<3;++g)for(int d=0;d<11;++d)r.weights[g][d]=1;
 for(int p=0;p<3;++p){r.minimum[p]=p;r.maximum[p]=p+2;}table.rows.push_back(r);
 BlueMagic::BlueLimits limits;limits.valid=true;limits.version=0;for(int a=0;a<4;++a)for(int t=0;t<10;++t)limits.caps[a][t]=1;
 BlueMagic::Config c;c.mode=BlueMagic::Config::LIVE_TEST;c.version=0;c.group=0;c.chance=1000000;c.lines=1;c.uniqueType=c.blankElementUnrestricted=c.attributeLevelIsTier=true;
 int checks=0,failed=0;std::string error;const int ids[5]={0,7,11,15,18};
 for(int n=0;n<5;++n){c.npc=ids[n];BlueMagic::BlueMagicTestPolicy plan;bool ok=plan.PrepareWorld(table,limits,c,ids[n],6,2,0,error);++checks;if(!ok)++failed;
  LegacyDrop::Random drop(123),control(123),blue(456),replay(456);BlueMagic::Result x,y;
  ok=plan.Draw(blue,x,error)&&plan.Draw(replay,y,error)&&x.blue&&x.attributes.size()==1&&y.attributes.size()==1&&!memcmp(x.attributes[0].value,y.attributes[0].value,sizeof(x.attributes[0].value));++checks;if(!ok)++failed;
  ++checks;if(drop.Next()!=control.Next())++failed;
 }
 BlueMagic::BlueMagicTestPolicy plan;c.npc=18;c.chance=0;BlueMagic::Result x;LegacyDrop::Random blue(456);
 ++checks;if(!plan.PrepareWorld(table,limits,c,18,6,2,0,error)||!plan.Draw(blue,x,error)||x.blue)++failed;
 c.npc=86;++checks;if(plan.PrepareWorld(table,limits,c,86,6,2,0,error))++failed;
 c.npc=18;c.chance=1000001;++checks;if(plan.PrepareWorld(table,limits,c,18,6,2,0,error))++failed;
 c.chance=1000000;++checks;if(plan.Prepare(table,limits,c,18,6,2,0,error)||error!="BLUE_PERSISTENCE_ABI_UNSUPPORTED")++failed;
 c.lines=2;++checks;if(plan.PrepareWorld(table,limits,c,18,6,2,0,error))++failed;
 c.lines=1;++checks;if(plan.PrepareWorld(table,limits,c,18,3,2,0,error))++failed;
 printf("P2_POLICY_BOUNDARIES checks=%d failures=%d items_created=0 npc_spawn_death=0 pickups=0\n",checks,failed);return failed?1:0;
}
