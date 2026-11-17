#ifndef PHONGTHAN_PLAYER_BODY_REFRESH_H
#define PHONGTHAN_PLAYER_BODY_REFRESH_H
// Player snapshots repeat frequently. Resource identity changes are rare;
// rebuilding unchanged bodies removes their scene sprite and restarts animation.
struct PTPlayerBodyIdentity {
    unsigned long entity;
    int type,series;
    bool valid;
    bool Update(unsigned long nextEntity,int nextType,int nextSeries,bool playerBody) {
        if(!playerBody || !nextEntity){valid=false;return false;}
        bool changed=!valid || entity!=nextEntity || type!=nextType || series!=nextSeries;
        entity=nextEntity;type=nextType;series=nextSeries;valid=true;
        return changed;
    }
};
#endif
