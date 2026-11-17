#include "../../Headers/PhongThanAttackRating.h"
#include <assert.h>
#include <stdio.h>
int main()
{
    const int attack[4]={5,6,4,6},defence[4]={3,2,4,2};
    for(int p=0;p<4;++p)for(int dex=0;dex<=1000;++dex)
    {
        assert(PhongThanBaseAttackRating(dex,p)==dex*attack[p]);
        assert(PhongThanBaseDefence(dex,p)==dex*defence[p]);
    }
    assert(PhongThanBaseAttackRating(3)==15);
    assert(PhongThanBaseAttackRating(INT_MAX)==INT_MAX);
    assert(PhongThanBaseAttackRating(INT_MIN)==0);
    assert(PhongThanBaseAttackRating(8)-PhongThanBaseAttackRating(3)==25);
    assert(PhongThanBaseAttackRating(3)-PhongThanBaseAttackRating(8)==-25);
    assert(PhongThanBaseAttackRating(10,-1)==0);
    assert(PhongThanBaseDefence(10,3)==20);
    assert(PhongThanBaseDefence(10,4)==0);
    puts("PASS native profession coefficients, starter accuracy and overflow bounds");
    return 0;
}
