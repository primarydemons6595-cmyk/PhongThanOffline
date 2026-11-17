#include "../../Headers/PhongThanSkillGeometry.h"
#include <assert.h>
#include <stdio.h>
int main()
{
    assert(PhongThanAnchorMissileAtTarget(true,true,1,true,7));
    assert(!PhongThanAnchorMissileAtTarget(true,false,1,true,7));
    assert(!PhongThanAnchorMissileAtTarget(true,true,1,false,7));
    assert(!PhongThanAnchorMissileAtTarget(true,true,6,true,7));
    assert(!PhongThanAnchorMissileAtTarget(true,true,1,true,0));
    int x, y;
    PhongThanSpreadFactors(0, 0, 0, 1024, 0, x, y);
    assert(x == 1024 && y == 0);
    PhongThanSpreadFactors(0, 0, 0, 0, -1024, x, y);
    assert(x == 0 && y == -1024);
    PhongThanSpreadFactors(30, 40, 50, 0, 0, x, y);
    assert(x == 614 && y == 819);
    PhongThanSpreadFactors(-30, -40, 50, 0, 0, x, y);
    assert(x == -614 && y == -819);
    puts("PASS skill spread: target-at-origin and nonzero distance");
    return 0;
}
