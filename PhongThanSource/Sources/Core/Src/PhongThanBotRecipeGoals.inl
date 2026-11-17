// Included after the authoritative compose implementation, server only.
#include "../../../Headers/PhongThanBotProgression.h"
namespace PtBotRecipes {
static KTabFile rules,costs;static bool loaded=false;
static bool Tables(){if(!loaded)loaded=rules.Load("\\settings\\item\\001\\zhuang_bei_he_cheng_gui_ze_biao.txt")&&costs.Load("\\settings\\item\\001\\zhuang_bei_he_cheng_jin_qian_xu_qiu.txt");return loaded;}
static PTBProgressItem Tuple(const PtWorkshop::Input& t){PTBProgressItem v;ZeroMemory(&v,sizeof(v));v.genre=t.genre;v.detail=t.detail;v.part=t.part;v.level=t.level;v.series=t.series;v.count=t.count;v.row=-1;return v;}
static PTBProgressItem Tuple(const PhongThanRecipeItem& t,unsigned native){PTBProgressItem v;ZeroMemory(&v,sizeof(v));v.genre=t.genre;v.detail=t.detail;v.part=t.part;v.level=-1;v.series=-1;v.count=t.quantity;v.row=-1;v.stars=t.upgradeMask;v.native=native;return v;}
}
int PhongThanBotRecipeGoal(int ordinal,PTBProgressGoal* g){
 if(!g||ordinal<0)return 0;PtWorkshop::Load();ZeroMemory(g,sizeof(*g));g->sourceRow=g->targetRow=-1;
 if(ordinal<(int)PtWorkshop::recipes.size()){
  if(!PtWorkshop::valid)return 0;const PtWorkshop::Recipe& r=PtWorkshop::recipes[ordinal];if(r.output.genre!=0)return 2;
  g->provider=PTB_WORKSHOP;g->id=r.id;g->fee=r.fee;g->chance=r.base;g->phase=r.step;g->sourceRow=r.source;g->targetRow=r.target;g->reward=PtBotRecipes::Tuple(r.output);g->reward.row=r.target;
  for(unsigned s=0;s<PtWorkshop::stations.size();++s)if(PtWorkshop::stations[s].id==r.station){const PtWorkshop::Station& st=PtWorkshop::stations[s];g->npc=st.npc;g->map=st.map;g->script=st.script;g->alias=st.alias;g->point=st.id;}
  for(int n=0;n<r.count;++n)g->needs[g->needCount++]=PtBotRecipes::Tuple(r.inputs[n]);if(r.source>=0)g->needs[0].row=r.source;
  return 1;
 }
 ordinal-=PtWorkshop::recipes.size();if(!PtBotRecipes::Tables()||ordinal>=PtBotRecipes::rules.GetHeight()-1)return 0;
 PhongThanEquipmentRecipe r;if(!PhongThanReadEquipmentRecipe(PtBotRecipes::rules,ordinal+2,r)||r.type!=1||r.inputs[0].genre!=0||r.output.genre!=0)return 2;
 g->provider=PTB_UPGRADE;g->id=r.id;g->map=1052;g->npc=206;g->script=g_FileName2Id(PT_XICH_TUNG_TU_SCRIPT);g->alias=0xD02B148DUL;g->chance=r.chance;
 g->reward=PtBotRecipes::Tuple(r.output,r.native[10]);
 for(int i=0;i<r.inputCount;++i)g->needs[g->needCount++]=PtBotRecipes::Tuple(r.inputs[i],r.native[i]);return 1;
}
int PhongThanBotRecipeQuote(int p,const PTBProgressGoal* g,const int* indexes,const int* counts,int* fee){
 if(!g||!indexes||!counts||!fee||p<=0||p>=MAX_PLAYER||!Player[p].m_bIsBot)return PT_COMPOSE_INVALID_ITEMS;
 *fee=0;int recipe=0,result=PT_COMPOSE_NO_RECIPE;
 if(g->provider==PTB_WORKSHOP){PtWorkshop::Plan plan;result=PtWorkshop::Quote(p,indexes,counts,g->point,plan);if(result==PT_COMPOSE_OK){if(plan.recipe.id!=g->id)return PT_COMPOSE_STALE;*fee=plan.recipe.fee;}}
 else if(g->provider==PTB_UPGRADE&&PtBotRecipes::Tables()){result=PhongThanEquipmentCompose(p,indexes,PtBotRecipes::rules,PtBotRecipes::costs,recipe,true,fee,counts);if(result==PT_COMPOSE_OK&&recipe!=g->id)return PT_COMPOSE_STALE;}
 return result;
}
