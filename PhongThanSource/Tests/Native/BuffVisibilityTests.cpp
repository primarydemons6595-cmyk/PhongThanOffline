#include "../../Headers/PhongThanBuffVisibility.h"
#include <assert.h>
#include <stdio.h>
int main(){
 assert(!PTShowSkillBuff(-1,true,80,0));
 assert(!PTShowSkillBuff(-1,false,80,0));
 assert(!PTShowSkillBuff(0,false,80,80));
 assert(PTShowSkillBuff(180,false,80,0));
 assert(!PTShowSkillBuff(180,true,80,0));
 assert(PTShowSkillBuff(-1,false,80,80));
 assert(!PTShowSkillBuff(-1,false,80,81));
 puts("PASS skill icons: timed active states and selected aura only, no innate/equipment permanent states");return 0;
}
