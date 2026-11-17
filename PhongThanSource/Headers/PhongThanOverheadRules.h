#ifndef PHONGTHAN_OVERHEAD_RULES_H
#define PHONGTHAN_OVERHEAD_RULES_H
// The requested boundary includes exactly twenty levels. Original Game.exe
// 006F7832 uses a strict >20 comparison; this project intentionally uses >=20.
inline bool PhongThanShowSkull(int level, int observerLevel)
{
    return level > 0 && observerLevel > 0 && level >= observerLevel &&
        level - observerLevel >= 20;
}
inline int PhongThanLevelDigits(int level)
{
    int count = 1;
    while (level >= 10) { level /= 10; ++count; }
    return count;
}
inline int PhongThanLevelLeft(int badgeLeft, int level)
{
    return badgeLeft + 1 + (20 - 7 * PhongThanLevelDigits(level)) / 2;
}
#endif
