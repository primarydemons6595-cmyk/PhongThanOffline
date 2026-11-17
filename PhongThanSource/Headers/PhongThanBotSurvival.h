#ifndef PHONGTHAN_BOT_SURVIVAL_H
#define PHONGTHAN_BOT_SURVIVAL_H
// Player corpses advance from death animation to a zero-life revive-wait state.
// Bots must handle both, without treating a living player's animation as death.
inline bool PTBotAwaitingTownRevival(int life,bool deathAnimation,bool reviveWait){
 return life<=0&&(deathAnimation||reviveWait);
}
inline bool PTBotTownRevivalConfirmed(int life,int map,int x,int y,int town,int tx,int ty){
 return life>0&&map==town&&x-tx>=-256&&x-tx<=256&&y-ty>=-256&&y-ty<=256;
}
#endif
