#include "../../Headers/PhongThanSkillExperience.h"
#include <assert.h>
#include <stdio.h>
int main()
{
    const char* lightning="12:0,90,330,770,1570,3170,6620,12340,19960,30000,45000,64000";
    assert(PhongThanSkillNextExperience(lightning,1)==90);
    assert(PhongThanSkillNextExperience(lightning,2)==330);
    assert(PhongThanSkillNextExperience(lightning,9)==30000);
    assert(!PhongThanSkillNextExperience(lightning,10));
    assert(!PhongThanSkillNextExperience("0:",1));
    assert(!PhongThanSkillNextExperience("12:0,90,80",2));
    assert(!PhongThanSkillNextExperience("12:0,",1));
    assert(!PhongThanSkillNextExperience("12:0,99999999999999999",1));
    puts("PASS native cumulative training thresholds, level-10 cap and malformed data");
    return 0;
}
