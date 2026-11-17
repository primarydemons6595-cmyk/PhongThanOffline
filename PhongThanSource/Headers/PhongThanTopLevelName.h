#ifndef PHONGTHAN_TOP_LEVEL_NAME_H
#define PHONGTHAN_TOP_LEVEL_NAME_H
// Original game.dat: 6DDBF5 gates the crown; 6DD32D applies TopLevel colors
// only in current camp 7. Never apply a player's badge to NPCs or summons.
inline bool PhongThanHasTopLevelCrown(bool player, int level) {
    return player && level >= 200;
}
inline bool PhongThanHasTopLevelNameColor(bool player, int level, int camp) {
    return PhongThanHasTopLevelCrown(player, level) && camp == 7;
}
struct PhongThanTopLevelPosition { int x, y; };
inline PhongThanTopLevelPosition PhongThanTopLevelCrownPosition(
    int nameLeft, int nameTop, int fontHeight, int width, int height) {
    PhongThanTopLevelPosition p;
    p.x = nameLeft - width - 2;
    p.y = nameTop + (fontHeight - height) / 2;
    return p;
}
#endif
