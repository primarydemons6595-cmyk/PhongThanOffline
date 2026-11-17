#ifndef _SERVER
#include "../../../Headers/PhongThanPotProgress.h"
#include <map>
struct PTPotClientState {PTPotProgress data;DWORD tick;};
static std::map<unsigned int,PTPotClientState> s_PotProgress;
static unsigned int s_PotProgressMap=0;
struct PTPotProgressLock {CRITICAL_SECTION lock;PTPotProgressLock(){InitializeCriticalSection(&lock);}~PTPotProgressLock(){DeleteCriticalSection(&lock);}};
static PTPotProgressLock s_PotProgressLock;
struct PTPotProgressGuard {PTPotProgressGuard(){EnterCriticalSection(&s_PotProgressLock.lock);}~PTPotProgressGuard(){LeaveCriticalSection(&s_PotProgressLock.lock);}};
void PhongThanReceivePotProgress(const PTPotProgress& p){
 PTPotProgressGuard guard;
 if(s_PotProgressMap!=p.MapId){s_PotProgress.clear();s_PotProgressMap=p.MapId;}
 if(s_PotProgress.size()>=1024)s_PotProgress.clear();
 PTPotClientState& state=s_PotProgress[p.EntityId];state.data=p;state.tick=GetTickCount();
}
bool PhongThanGetPotProgress(unsigned int map,unsigned int entity,PTPotProgress& out){
 PTPotProgressGuard guard;
 if(map!=s_PotProgressMap)return false;
 std::map<unsigned int,PTPotClientState>::iterator found=s_PotProgress.find(entity);
 if(found==s_PotProgress.end())return false;
 DWORD elapsed=GetTickCount()-found->second.tick;
 if(elapsed>5000){s_PotProgress.erase(found);return false;}
 out=found->second.data;out.Now+=elapsed/1000;
 return out.Now<out.Burn;
}
#endif
