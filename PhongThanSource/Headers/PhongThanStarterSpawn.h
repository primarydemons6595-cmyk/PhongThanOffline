#ifndef PHONGTHAN_STARTER_SPAWN_H
#define PHONGTHAN_STARTER_SPAWN_H

// Only character creation uses these positions. Existing saves and revival
// points remain authoritative for characters that have already been created.
inline bool PhongThanStarterSpawn(unsigned int profession, int& map, int& x, int& y)
{
    static const int maps[] = {1002, 1003, 1004};
    static const int gridX[] = {201, 217, 196};
    static const int gridY[] = {199, 201, 203};
    if (profession >= 3) return false;
    map = maps[profession];
    x = gridX[profession] * 256 + 128;
    y = gridY[profession] * 512 + 256;
    return true;
}

#endif
