#ifndef PT_BOT_PROGRESSION_H
#define PT_BOT_PROGRESSION_H
// Read-only plans. Execution uses the same guarded NPC transactions as players.
enum {PTB_STUDIO_QUEST=1,PTB_STUDIO_SHOP=2,PTB_WORKSHOP=3,PTB_UPGRADE=4,PTB_LUA=5,PTB_TRAN=6,PTB_PHI=7};
struct PTBProgressItem {int genre,detail,part,level,series,count,row,quality;unsigned stars,native;};
struct PTBProgressGoal {
 int provider,id,phase,progress,npc,map,x,y,target,targetMap,priority,fee,point,price,sourceRow,targetRow,chance;
 int needCount,relevant;PTBProgressItem needs[10],reward;
 unsigned long script,alias;char name[96],choices[4][48];
};
int PhongThanBotStudioGoal(int player,int ordinal,PTBProgressGoal* out);
int PhongThanBotStudioCommit(int player,const PTBProgressGoal* goal);
int PhongThanBotTranGoal(int player,int ordinal,PTBProgressGoal* out);
int PhongThanBotTranCommit(int player,const PTBProgressGoal* goal);
int PhongThanBotPhiGoal(int player,int ordinal,PTBProgressGoal* out);
int PhongThanBotPhiCommit(int player,const PTBProgressGoal* goal);
int PhongThanBotRecipeGoal(int ordinal,PTBProgressGoal* out);
int PhongThanBotRecipeQuote(int player,const PTBProgressGoal* goal,const int* indexes,const int* counts,int* fee);
#endif
