// Shared compatibility choices; none assert original Kingsoft semantics.
#ifndef PT_NATIVE_DROP_COMPATIBILITY_H
#define PT_NATIVE_DROP_COMPATIBILITY_H
#include "LegacyDropExecutionPolicy.h"
namespace LegacyDrop { namespace Execution {
inline Profile RuntimeProfile(const std::string& rule,int special,const std::string& normal,const std::string& blue){
 std::string selected=rule;
 if(rule=="NPC_STATE_EXPERIMENTAL")selected=special==0?normal:special==1?blue:"UNBOUND";
 if(rule=="NPC_STATE")selected=special==1?blue:normal;
 return selected=="A"?PROFILE_A:selected=="B"?PROFILE_B:PROFILE_UNBOUND;
}
inline int ExperimentalMagicChance(int base,int raw){
 if(base<0||base>1000000||raw<0)return -1;
 double chance=(double)base*raw;return chance>1000000?1000000:(int)chance;
}
inline bool CompletionNpc(int id){return id==0||id==7||id==11||id==15||id==18||id==86;}
}}
#endif
