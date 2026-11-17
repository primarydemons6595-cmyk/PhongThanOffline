#include "PhongThanQuickQuestCatalog.h"
static bool PtQuickClass(int p,const PtQuickArc& arc){int f=Player[p].m_cProfession.GetProfession();if(f==3)f=0;return arc.profession<0||arc.profession==f;}
static bool PtQuickDone(int p,int id){
 const PtQuickArc& a=kPtQuickArcs[id];KPlayerTask& task=Player[p].m_cTask;
 if((task.GetSaveVal(5332+id/31)&(1<<(id%31)))!=0)return true;
 for(int i=0;i<a.count;++i){const PtQuickStep& s=kPtQuickSteps[a.offset+i];if(!s.finish)continue;
  int value=task.GetSaveVal(s.task);
  if(s.to>0&&(value==s.to||(s.to>s.from&&value>s.to)))return true;
 }return false;
}
void PhongThanQuickQuestTransition(int p,int task,int before,int after){
 if(p<=0||p>=MAX_PLAYER||!Player[p].m_PlayerDBLoad||before==after||(task>=5332&&task<=5335))return;
 for(int id=0;id<sizeof(kPtQuickArcs)/sizeof(kPtQuickArcs[0]);++id){
  const PtQuickArc& a=kPtQuickArcs[id];if(a.task!=task||!PtQuickClass(p,a))continue;
  for(int i=0;i<a.count;++i){const PtQuickStep& s=kPtQuickSteps[a.offset+i];
   if(s.finish&&s.task==task&&s.from==before&&s.to==after){int key=5332+id/31;Player[p].m_cTask.SetSaveVal(key,Player[p].m_cTask.GetSaveVal(key)|(1<<(id%31)),TRUE);}
  }
 }
}
static int PtQuickQuest(int p,int op,int number){
 int id=number-1;if(!PtHandbookAllowed(p)||id<0||id>=sizeof(kPtQuickArcs)/sizeof(kPtQuickArcs[0]))return 0;
 const PtQuickArc& a=kPtQuickArcs[id];if(!PtQuickClass(p,a))return 0;
 bool done=PtQuickDone(p,id);if(op==8)return done?2:1;
 if(Npc[Player[p].m_nIndex].m_Level<a.level||done)return 0;
 KPlayerTask& task=Player[p].m_cTask;
 if(op==9){
  // Admin completion records progress only; it never repeats XP/items/money.
  for(int j=0;j<a.count;++j){const PtQuickStep& s=kPtQuickSteps[a.offset+j];
   if(s.task!=a.task&&task.GetSaveVal(s.task)<s.to)task.SetSaveVal(s.task,s.to,TRUE);
   if(s.camp>=0)Npc[Player[p].m_nIndex].SetCamp(s.camp);
  }
  int value=task.GetSaveVal(a.task);
  if(value<=max(a.from,a.to))task.SetSaveVal(a.task,a.to,TRUE);
  int key=5332+id/31;task.SetSaveVal(key,task.GetSaveVal(key)|(1<<(id%31)),TRUE);
  Player[p].m_uMustSave=SAVE_REQUEST;return 1;
 }
 if(op!=10)return 0;
 const PtQuickStep* step=0;
 for(int i=0;i<a.count;++i){const PtQuickStep& s=kPtQuickSteps[a.offset+i];
  if(s.needCount&&task.GetSaveVal(s.task)==s.from&&(s.mainPhase<0||task.GetSaveVal(a.profession==0?3:a.profession)==s.mainPhase)){step=&s;break;}
 }
 if(!step||step->needCount>16)return 0;
 PhongThanQuestExchangeRow rewards[16];int nr=0;
 for(int n=0;n<step->needCount;++n){const int* v=kPtQuickNeeds[step->needOffset+n];PhongThanQuestExchangeRow r;
  if(!PhongThanDecodeQuestItemTuple(v[0],v[1],v[2],v[3],v[4],v[5],r.item))return 0;
  int have=0;for(PlayerItem* b=Player[p].m_ItemList.GetFirstItem();b;b=Player[p].m_ItemList.GetNextItem()){
   int ix=b->nIdx;if(b->nPlace==pos_equiproom&&ix>0&&ix<MAX_ITEM&&Item[ix].GetID()&&PhongThanQuestItemMatches(Item[ix],r.item)&&!Item[ix].GetLock()->IsLock())have+=Item[ix].GetStackNum();
  }
  r.count=v[6]-have;if(r.count>0)rewards[nr++]=r;
 }
 if(!nr)return 1;int receipt=TASKVALUE_PT_HANDBOOK_RECEIPT,value=task.GetSaveVal(receipt);
 return PhongThanQuestExchange(p,receipt,value,value==1?0:1,0,0,rewards,nr,true);
}
