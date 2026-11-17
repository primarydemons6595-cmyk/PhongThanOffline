#ifndef PHONGTHAN_FATAL_STRIKE_H
#define PHONGTHAN_FATAL_STRIKE_H
// Van Cot Toan Kho: its level Lua supplies the fatal-strike chance; the
// original SkillDesc specifies one quarter of the target's current life.
inline int PhongThanFatalStrikeDamage(int life, int chance, int resist, int roll)
{
    if (life <= 0 || roll < 0 || roll >= 100) return 0;
    if (resist < 0) resist = 0;
    chance -= resist;
    if (chance <= roll) return 0;
    int damage = life / 4;
    return damage > 0 ? damage : 1;
}
#endif
