#include <assert.h>
#include <stdio.h>
#include "../../Headers/PhongThanTopLevelName.h"
int main() {
    assert(!PhongThanHasTopLevelCrown(true,199));
    assert(PhongThanHasTopLevelCrown(true,200));
    assert(PhongThanHasTopLevelCrown(true,201));
    assert(!PhongThanHasTopLevelCrown(false,200));
    for(int camp=0;camp<14;++camp) {
        assert(PhongThanHasTopLevelNameColor(true,200,camp)==(camp==7));
        assert(!PhongThanHasTopLevelNameColor(true,199,camp));
        assert(!PhongThanHasTopLevelNameColor(false,200,camp));
    }
    PhongThanTopLevelPosition p=PhongThanTopLevelCrownPosition(100,50,12,25,18);
    assert(p.x==73 && p.y==47); // Original 25x18 sprite stays left of text.
    p=PhongThanTopLevelCrownPosition(20,50,12,25,18);
    assert(p.x==-7 && p.y==47); // Long nation/duty prefix shares the same rule.
    puts("PASS top-level crown: 199/200/201, player/NPC, 14 camps, original sprite layout");
    return 0;
}
