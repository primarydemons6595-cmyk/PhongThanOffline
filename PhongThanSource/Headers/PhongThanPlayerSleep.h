#ifndef PT_PLAYER_SLEEP_H
#define PT_PLAYER_SLEEP_H
inline bool PTPlayerShouldShowSleep(bool serverBot,int fightMode,bool sleeping,int idleTicks){return !serverBot&&fightMode==0&&!sleeping&&idleTicks>3600;}
#endif
