// Included after the Studio quest/service implementation in ScriptFuns.cpp.
#ifdef _SERVER
#include "../../../Headers/PhongThanBotProgression.h"
namespace PtBotStudio {
static PTBProgressItem Item(const PhongThanQuestExchangeRow& r){PTBProgressItem t;ZeroMemory(&t,sizeof(t));t.genre=r.item.genre;t.detail=r.item.detail;t.part=r.item.particular;t.level=r.item.level;t.series=r.item.series;t.count=r.count;t.row=-1;return t;}
static void Bind(PTBProgressGoal& g,int id){PtStudioQuest::Binding* b=PtStudioQuest::Bind(id);if(!b)return;g.npc=b->type;g.map=b->map;g.x=b->x;g.y=b->y;g.script=g_FileName2Id((char*)("\\"+b->script).c_str());}
}
int PhongThanBotStudioGoal(int p,int ordinal,PTBProgressGoal* out){
 using namespace PtStudioQuest;if(!out||ordinal<0||!Live(p))return 0;bool available=Load();int questCount=available?quests.size():0,offerCount=available?services.offers.size():0;ZeroMemory(out,sizeof(*out));out->sourceRow=out->targetRow=-1;
 if(ordinal<questCount){
  const Quest& q=quests[ordinal];int state=Status(p,q);if(!q.enabled||state==-1||!Eligible(p,q))return 2;
  out->provider=PTB_STUDIO_QUEST;out->id=q.id;out->phase=state;out->progress=Raw(p,q)&CountMask;out->priority=state>0?900:600;out->point=q.point;out->price=q.pointGain;lstrcpynA(out->name,q.name.c_str(),sizeof(out->name));
  if(state<=0){PtBotStudio::Bind(*out,q.start);return 1;}
  if(state>(int)q.steps.size()){PtBotStudio::Bind(*out,q.end);return 1;}
  const Step& s=q.steps[state-1];PtBotStudio::Bind(*out,s.npc?s.npc:q.end);
  if(s.kind==1||(s.kind==2&&s.dropPercent)){out->target=s.target+1;out->targetMap=s.map;out->reward.count=s.count;}
  if(s.kind==4){out->priority=0;return 2;} // Gathering needs its own interaction adapter.
  for(unsigned n=0;n<s.needs.size()&&out->needCount<10;++n)out->needs[out->needCount++]=PtBotStudio::Item(s.needs[n].row);
  return 1;
 }
 ordinal-=questCount;if(ordinal>=offerCount){int extra=ordinal-offerCount;if(extra<256){int result=PhongThanBotTranGoal(p,extra,out);return result?result:2;}return PhongThanBotPhiGoal(p,extra-256,out);}
 const ServiceOffer& o=services.offers[ordinal];ServiceMenu* m=ServiceMenuById(o.menu);
 if(!o.enabled||!m||m->kind!=2||!(o.classes&Class(p))||!(m->classes&Class(p))||Npc[Player[p].m_nIndex].m_Level<o.minLevel)return 2;
 out->provider=PTB_STUDIO_SHOP;out->id=o.id;out->phase=m->id;out->priority=700;out->point=o.point;out->price=o.price;out->reward=PtBotStudio::Item(o.reward);PtBotStudio::Bind(*out,m->npc);lstrcpynA(out->name,o.name.c_str(),sizeof(out->name));
 if(!o.point){out->needCount=1;out->needs[0]=PtBotStudio::Item(o.payment);out->needs[0].count=o.price;}return 1;
}
int PhongThanBotStudioCommit(int p,const PTBProgressGoal* goal){
 using namespace PtStudioQuest;if(!goal||!Live(p)||!Player[p].m_bIsBot)return 0;
 // Re-resolve all rule data; the bot's cached plan is never authority.
 if(goal->provider==PTB_TRAN)return PhongThanBotTranCommit(p,goal);
 if(goal->provider==PTB_PHI)return PhongThanBotPhiCommit(p,goal);
 if(!Load())return 0;
 if(goal->provider==PTB_STUDIO_QUEST){Quest* q=Find(goal->id);if(!q)return 0;int state=Status(p,*q);int binding=state<=0?q->start:state>(int)q->steps.size()?q->end:(q->steps[state-1].npc?q->steps[state-1].npc:q->end);return Act(p,q->id,state<=0?0:1,binding);}
 if(goal->provider==PTB_STUDIO_SHOP){for(unsigned i=0;i<services.offers.size();++i)if(services.offers[i].id==goal->id){ServiceMenu* m=ServiceMenuById(services.offers[i].menu);return m?ServiceTrade(p,m->id,m->npc,goal->id,1):0;}}
 return 0;
}
#endif
