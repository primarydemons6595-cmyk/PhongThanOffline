#ifndef PT_BOT_RECOVERY_H
#define PT_BOT_RECOVERY_H
template<class Actor,class Command> inline bool PTBotSitForRecovery(Actor& actor,Command stand,Command sit){
 if(actor.m_bRideHorse){
  if(actor.m_Doing==sit)actor.SendCommand(stand);
  if(!actor.CanSwitchRideHorse())return false;
  actor.SwitchRideHorse(0);
  if(actor.m_bRideHorse)return false;
 }
 if(actor.m_Doing!=sit)actor.SendCommand(sit);
 return true;
}
inline bool PTBotMissingRecovery(bool life,bool mana,int hpStock,int mpStock){return (life&&hpStock<1)||(mana&&mpStock<1);}
// Keep a working supply; sell only excess under actual carry pressure.
inline int PTBotRecoverySurplus(int total,int kind,bool pressure){
 if(!pressure||total<0||(kind!=1&&kind!=2))return 0;
 int keep=kind==1?10:5;return total>keep?total-keep:0;
}
inline bool PTBotRecoverySalePlan(int total,int kind,int stack,bool pressure,int& sell,int& reserve){
 sell=reserve=0;if(stack<1||stack>total)return false;
 int excess=PTBotRecoverySurplus(total,kind,pressure);if(excess<1)return false;
 sell=stack<excess?stack:excess;reserve=stack-sell;return true;
}
inline int PTBotCarryEscapeScrolls(int weight,int capacity,int unitWeight,int owned){
 if(weight<=capacity||capacity<1||unitWeight<1||owned<2)return 0;
 __int64 count=((__int64)weight-capacity+unitWeight-1)/unitWeight;
 return count<=3&&count<owned?(int)count:0;
}
struct PTBotProgressBudget {
 unsigned long since,blockedUntil;int signature;bool active;
 PTBotProgressBudget():since(0),blockedUntil(0),signature(0),active(false){}
 bool Observe(unsigned long now,int value,unsigned long budget,unsigned long backoff){
  if(blockedUntil&&(long)(now-blockedUntil)<0)return false;
  blockedUntil=0;
  if(!active||signature!=value){active=true;signature=value;since=now;return true;}
  if(now-since<budget)return true;
  active=false;blockedUntil=now+backoff;return false;
 }
};
#endif
