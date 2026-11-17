#ifndef PHONGTHAN_MAP_SLEEP_H
#define PHONGTHAN_MAP_SLEEP_H
#ifdef _SERVER
bool PhongThanMapSleepEnabled();
int PhongThanMapSleepIdleSeconds();
bool PhongThanMapSleepPinned(int map);
int PhongThanGatherSpawnWorld(int map);
void PhongThanMapSleepStatus();
#endif
#endif
