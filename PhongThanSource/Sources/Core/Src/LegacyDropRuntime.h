#ifndef PT_LEGACY_DROP_RUNTIME_H
#define PT_LEGACY_DROP_RUNTIME_H
#ifdef _SERVER
class KNpc;
#include "NativeDropRelease.h"
bool LegacyDropOnDeath(KNpc& npc,int owner,int attacker);
void LegacyDropRandomBlue(KNpc& npc,bool respawn);
void LegacyDropNormalEngineDispatch(int npc);
void LegacyDropResetNpc(int index);
void LegacyDropHit(int index,int attacker,int damage);
void LegacyDropTick();
void LegacyDropLua(int npc,unsigned long script,const char* phase,int result);
void LegacyDropLuaItem(const char* api,int item,int object);
void LegacyDropPickup(unsigned long serial,int player,int object,const char* result);
bool LegacyDropMoneyIsTracked(int object);
void LegacyDropMoneyPickup(int object,int player,const char* result);
class LegacyDropMoneyAttempt {
    int object,player;bool native,done;
public:LegacyDropMoneyAttempt(int o,int p):object(o),player(p),native(LegacyDropMoneyIsTracked(o)),done(false){if(native)try{LegacyDropMoneyPickup(object,player,"ATTEMPT");}catch(...){}}
    bool Native()const{return native;}
    void Complete(){done=true;if(native)try{LegacyDropMoneyPickup(object,player,"PICKED_UP");}catch(...){}}
    ~LegacyDropMoneyAttempt(){if(native&&!done)try{LegacyDropMoneyPickup(object,player,"REJECTED_BY_NATIVE_PICKUP");}catch(...){}}
};
class LegacyDropPickupAttempt {
    unsigned long serial;int player,object;bool complete;
public:
    LegacyDropPickupAttempt(unsigned long s,int p,int o):serial(s),player(p),object(o),complete(false){try{LegacyDropPickup(serial,player,object,"ATTEMPT");}catch(...){}}
    void Complete(){complete=true;try{LegacyDropPickup(serial,player,object,"PICKED_UP");}catch(...){}}
    ~LegacyDropPickupAttempt(){if(!complete)try{LegacyDropPickup(serial,player,object,"REJECTED_BY_NATIVE_PICKUP");}catch(...){}}
};
#endif
#endif
