#ifndef PT_BOT_WORK_BUDGET_H
#define PT_BOT_WORK_BUDGET_H
// Cooperative scheduling only. A transaction already in progress is atomic;
// unvisited bots and overdue saves remain queued for later world ticks.
inline bool PTBotMayStartDecision(int processed,double elapsed,int milliseconds){
 return processed==0||milliseconds<=0||elapsed<milliseconds;
}
inline bool PTBotMayAutoSave(int attempted,int maximum){return maximum<=0||attempted<maximum;}
inline int PTBotNextDecisionTurn(int turn,int processed,int listed){return listed>0?(turn+processed)%listed:0;}
#endif
