#ifndef PHONGTHAN_ATTACK_RATING_H
#define PHONGTHAN_ATTACK_RATING_H
#include <limits.h>
// Original basenumcalc.ini + game.dat:0x703110 / 0x703190.
// Giap Si / Dao Si / Di Nhan have separate accuracy and evasion coefficients.
inline int PhongThanBaseAttackRating(int dexterity, int profession = 0)
{
    static const int factor[4]={5,6,4,6};
    if (dexterity <= 0 || profession < 0 || profession > 3) return 0;
    if (dexterity > INT_MAX / factor[profession]) return INT_MAX;
    return dexterity * factor[profession];
}
inline int PhongThanBaseDefence(int dexterity, int profession)
{
    static const int factor[4]={3,2,4,2};
    if (dexterity <= 0 || profession < 0 || profession > 3) return 0;
    if (dexterity > INT_MAX / factor[profession]) return INT_MAX;
    return dexterity * factor[profession];
}
#endif
