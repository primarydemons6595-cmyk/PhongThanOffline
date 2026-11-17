#ifndef PHONGTHAN_BOT_BOSS_POLICY_H
#define PHONGTHAN_BOT_BOSS_POLICY_H
inline bool PTBotBossStronger(int level,int gear,int oldLevel,int oldGear){return level>oldLevel||(gear>oldGear&&(__int64)gear*100>=(__int64)oldGear*125);}
#endif
