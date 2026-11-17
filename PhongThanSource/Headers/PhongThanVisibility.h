#ifndef PHONGTHAN_VISIBILITY_H
#define PHONGTHAN_VISIBILITY_H
#ifndef _SERVER
bool PhongThanClientVisualHidden(int npc);
#endif
// The four original shield rows are client display preferences only.
inline bool PhongThanHideVisualRule(int mode,bool self,bool player,bool sameTeam,bool pet){
    if(self)return false;
    if(mode==0)return pet;
    if(mode==1)return player&&!sameTeam;
    if(mode==2)return player;
    return false;
}
#endif
