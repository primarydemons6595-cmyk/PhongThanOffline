#ifndef PT_REWARD_OWNER_H
#define PT_REWARD_OWNER_H
inline bool PhongThanRewardOwnerAvailable(int connection,bool serverBot,bool loaded){return connection>=0||(serverBot&&loaded);}
#endif
