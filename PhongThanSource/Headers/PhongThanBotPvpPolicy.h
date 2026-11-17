#ifndef PHONGTHAN_BOT_PVP_POLICY_H
#define PHONGTHAN_BOT_PVP_POLICY_H
inline bool PTBotNeedsAllies(int life,int maximum,int ownLevel,int enemyLevel,int nearbyEnemies,int nearbyAllies,unsigned int choice,int dominationPercent){
 if(life<=0||maximum<=0)return false;
 return (__int64)life*100<(__int64)maximum*70||enemyLevel>ownLevel||nearbyEnemies>nearbyAllies||choice%100<(unsigned int)dominationPercent;
}
inline bool PTBotCanProvoke(int ownLevel,int enemyLevel,int minimum,int gap,bool ownAlive,bool targetAlive,bool sameTeam,bool sameNation,bool targetOptedIn){
 return ownAlive&&targetAlive&&!sameTeam&&!sameNation&&targetOptedIn&&ownLevel>=minimum&&enemyLevel>=minimum&&ownLevel-enemyLevel<=gap&&enemyLevel-ownLevel<=gap;
}
#endif
