#include "../../Headers/PhongThanSkillLearning.h"
#include <assert.h>
#include <stdio.h>
int main()
{
    assert(PhongThanDefaultPhysicalSkill(0)==1);
    assert(PhongThanDefaultPhysicalSkill(1)==2);
    assert(PhongThanDefaultPhysicalSkill(2)==135);
    assert(PhongThanCanLearnBook(0,3,1,4,0));
    assert(!PhongThanCanLearnBook(0,3,1,3,0));
    assert(!PhongThanCanLearnBook(0,3,0,100,0));
    assert(!PhongThanCanLearnBook(0,3,1,100,1));
    assert(!PhongThanCanLearnBook(0,26,1,100,0));
    assert(PhongThanSkillBookLevel(23,26)==90);
    assert(PhongThanCanLearnBook(24,27,0,6,0));
    assert(PhongThanCanLearnBook(49,450,2,5,0));
    assert(!PhongThanCanLearnBook(49,450,2,4,0));
    assert(!PhongThanSkillBookLevel(-1,3));
    assert(!PhongThanSkillBookLevel(58,62));
    puts("PASS native book identity, profession, level and already-learned checks");
    return 0;
}
