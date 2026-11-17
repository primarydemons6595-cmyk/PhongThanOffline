#ifdef _SERVER
#include "../../../Headers/PhongThanBotProgression.h"
namespace PtBotPhi {
static PTBProgressItem Tuple(int g,int d,int part,int level,int count){PTBProgressItem t;ZeroMemory(&t,sizeof(t));t.genre=g;t.detail=d;t.part=part;t.level=level;t.series=-1;t.count=count;t.row=-1;return t;}
}
int PhongThanBotPhiGoal(int p,int ordinal,PTBProgressGoal* out){
 if(!out||ordinal<0||ordinal>=30||p<=0||p>=MAX_PLAYER||!Player[p].m_PlayerDBLoad)return 0;int profession=Player[p].m_cProfession.GetProfession(),type=ordinal/10,step=ordinal%10;
 if(type!=profession)return 2;PtPhiTuyet::Recipe recipe=PtPhiTuyet::recipes[step];if(type==2&&step==7){recipe.money=8000000;recipe.bronze=1000;recipe.orders=30;}
 ZeroMemory(out,sizeof(*out));out->provider=PTB_PHI;out->id=ordinal+1;out->phase=recipe.inputLevel;out->priority=750;out->fee=recipe.money;out->map=1020;out->npc=777;out->script=g_FileName2Id("\\script\\phongthan\\npc_recovered_batch\\1020_a0aef599.lua");out->alias=0xa0aef599UL;out->sourceRow=out->targetRow=-1;
 out->needs[out->needCount++]=PtBotPhi::Tuple(0,equip_horse,21+type,recipe.inputLevel,1);
 if(step==0)out->needs[out->needCount++]=PtBotPhi::Tuple(4,39,0,0,1);
 if(recipe.bronze)out->needs[out->needCount++]=PtBotPhi::Tuple(3,6,0,0,recipe.bronze);
 if(recipe.orders)out->needs[out->needCount++]=PtBotPhi::Tuple(3,100,0,0,recipe.orders);
 if(recipe.jade)out->needs[out->needCount++]=PtBotPhi::Tuple(3,41,0,0,recipe.jade);
 if(recipe.eye)out->needs[out->needCount++]=PtBotPhi::Tuple(3,recipe.eye,0,0,1);
 out->reward=PtBotPhi::Tuple(0,equip_horse,24+type,step+1,1);return 1;
}
int PhongThanBotPhiCommit(int p,const PTBProgressGoal* goal){if(!goal||p<=0||p>=MAX_PLAYER||!Player[p].m_bIsBot)return 0;return PtPhiTuyet::Upgrade(p,goal->id);}
#endif
