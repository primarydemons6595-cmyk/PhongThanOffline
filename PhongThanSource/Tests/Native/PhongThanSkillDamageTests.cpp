#include <assert.h>
#include <stdio.h>
#include <string.h>
struct KMagicAttrib { int nAttribType; int nValue[3]; };
#include "../../Headers/PhongThanSkillDamage.h"
#include "../../Headers/PhongThanFatalStrike.h"
int main()
{
    KMagicAttrib input[17], output[17];
    memset(input, 0, sizeof(input));
    input[11].nAttribType=30; input[11].nValue[0]=330; input[11].nValue[2]=440;
    PhongThanPrepareSkillDamage(output,input,17,1,10,11);
    assert(output[9].nAttribType==10 && output[9].nValue[0]==0);
    assert(output[11].nValue[0]==330 && output[11].nValue[2]==440);
    assert(input[9].nAttribType==0); // Cached skill remains immutable.
    assert(output[8].nValue[0]==0 && output[16].nValue[2]==0);
    input[9].nAttribType=10; input[9].nValue[0]=35;
    PhongThanPrepareSkillDamage(output,input,17,1,10,11);
    assert(output[9].nValue[0]==35); // Preserve authored physical enhancement.
    input[9].nAttribType=11; input[9].nValue[0]=100; input[9].nValue[2]=200;
    PhongThanPrepareSkillDamage(output,input,17,1,10,11);
    assert(output[9].nValue[0]==100 && output[9].nValue[2]==200);
    memset(input,0,sizeof(input));
    PhongThanPrepareSkillDamage(output,input,17,0,10,11);
    assert(output[9].nAttribType==0); // Buffs/standalone fatal strike get no base attack.
    assert(PhongThanFatalStrikeDamage(1000,18,0,0)==250);
    assert(PhongThanFatalStrikeDamage(1000,18,0,17)==250);
    assert(PhongThanFatalStrikeDamage(1000,18,0,18)==0);
    assert(PhongThanFatalStrikeDamage(1000,18,18,0)==0);
    assert(PhongThanFatalStrikeDamage(3,100,0,99)==1);
    assert(PhongThanFatalStrikeDamage(0,100,0,0)==0);
    puts("PASS: sparse damage slots, authoritative base-damage flag, cached-data immutability, explicit damage preservation and quarter-life strike boundaries");
    return 0;
}
