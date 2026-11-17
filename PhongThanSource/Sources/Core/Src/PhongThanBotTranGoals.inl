#ifdef _SERVER
#include "../../../Headers/PhongThanBotProgression.h"
namespace PtBotTran {
static PTBProgressItem Tuple(int g,int d,int part,int level,int count){PTBProgressItem t;ZeroMemory(&t,sizeof(t));t.genre=g;t.detail=d;t.part=part;t.level=level;t.series=-1;t.count=count;t.row=-1;return t;}
static bool Spare(int p,int n){if(!PtTran::Owned(p,n,Item[n].GetID()))return false;int power=0;if(!Item[n].GetEquipmentPower(power))return false;for(int slot=0;slot<itempart_num;++slot)if(Player[p].m_ItemList.CanEquip(n,slot)){int old=Player[p].m_ItemList.GetEquipment(slot),score=0;if(old<=0||old>=MAX_ITEM||!Item[old].GetEquipmentPower(score)||power>score)return false;}return true;}
}
int PhongThanBotTranGoal(int p,int ordinal,PTBProgressGoal* out){
 using namespace PtTran;if(!out||ordinal<0||!Live(p)||Npc[Player[p].m_nIndex].m_Level<42||!Tables())return 0;
 ZeroMemory(out,sizeof(*out));out->provider=PTB_TRAN;out->map=1052;out->npc=202;out->script=0x378b6d9aUL;out->sourceRow=out->targetRow=-1;out->priority=420;
 int phase=Get(p,Phase),mask=Get(p,Mask);out->progress=mask;
 if(ordinal==0){if(phase!=0)return 2;out->id=1;out->phase=1;return 1;}
 if(ordinal>=1&&ordinal<=6){if(phase!=1||mask&(1<<(ordinal-1)))return 2;out->id=ordinal;out->phase=1;out->priority=900;out->target=ordinal<=3?PT_TRAN_BOSSES[ordinal-1].tid+1:ordinal==4?10:ordinal==5?9:12;out->targetMap=ordinal<=3?PT_TRAN_BOSSES[ordinal-1].map:ordinal==4?1011:ordinal==5?1008:1016;return 1;}
 if(ordinal==7){if(phase!=1||mask!=63)return 2;out->id=2;out->phase=2;out->priority=950;return 1;}
 if(ordinal>=8&&ordinal<=12){if(phase!=2||Get(p,Slot))return 2;int slot=ordinal-7;out->id=slot;out->phase=3;out->reward=PtBotTran::Tuple(0,slot==1?5:slot==2?7:slot==3?6:slot==4?9:2,-1,1,1);return 1;}
 if(ordinal==13){int slot=Get(p,Slot),credit=Reputation(p);if(phase!=2||slot<1||slot>5||Count(p)>=5||credit>=501)return 2;out->id=slot;out->phase=4;out->priority=400;out->progress=credit;int level=credit<=50?1:credit<=100?2:credit<=150?3:credit<=200?4:credit<=300?5:credit<=400?6:credit<=500?7:8;out->needCount=1;out->needs[0]=PtBotTran::Tuple(0,slot==1?5:slot==2?7:slot==3?6:slot==4?9:2,-1,level,1);out->needs[0].quality=2;return 1;}
 ordinal-=14;if(ordinal>=(int)catalog.size())return 0;const PhongThanComposeRecipe& r=catalog[ordinal];int book=Book(r);
 if(phase!=2||Get(p,Slot)!=r.group-1||(book>=1000&&!Get(p,Tier3)))return 2;
 out->id=r.id;out->phase=Learned(p,book)?7:6;out->progress=Reputation(p);
 if(!Learned(p,book)){
  if(Reputation(p)<Requirement(book))return 2;
  if(!Basic(book)){bool found=false;for(unsigned paper=0;paper<sizeof(PT_TRAN_PAPERS)/sizeof(PT_TRAN_PAPERS[0]);++paper)if(PT_TRAN_PAPERS[paper].book==book){out->needCount=1;out->needs[0]=PtBotTran::Tuple(6,PT_TRAN_PAPERS[paper].detail,0,0,1);found=true;break;}if(!found)return 2;}out->priority=550;return 1;
 }
 int fee=0;if(!PhongThanReadComposeCost(costs,r.id,1,fee))return 2;out->fee=fee;out->priority=650;
 for(int n=0;n<r.inputCount;++n){const PhongThanRecipeItem& in=r.inputs[n];PTBProgressItem item=PtBotTran::Tuple(in.genre,in.detail,in.part,-1,in.quantity);item.native=in.upgradeMask;if(n==0)item.quality=1;out->needs[out->needCount++]=item;}
 out->reward=PtBotTran::Tuple(r.output.genre,r.output.detail,r.output.part,-1,r.output.quantity);out->reward.native=r.output.upgradeMask;return 1;
}
int PhongThanBotTranCommit(int p,const PTBProgressGoal* goal){
 using namespace PtTran;if(!goal||!Player[p].m_bIsBot||!Near(p)||!Tables())return 0;int a=goal->id,b=0;DWORD c=0;
 if(goal->phase==4||goal->phase==7){bool found=false;for(PlayerItem* item=Player[p].m_ItemList.GetFirstItem();item;item=Player[p].m_ItemList.GetNextItem()){int n=item->nIdx;if(n<=0||n>=MAX_ITEM||!Owned(p,n,Item[n].GetID()))continue;
  if(goal->phase==4){int required=goal->needs[0].level;if(EquipSlot(Item[n])!=Get(p,Slot)||Item[n].GetPhongThanQuality()!=1||Item[n].GetLevel()!=required||!PtBotTran::Spare(p,n))continue;a=n;b=0;c=Item[n].GetID();}
  else {PhongThanComposeRecipe* r=Recipe(goal->id);if(!r||!Match(Item[n],r->inputs[0])||Item[n].GetPhongThanQuality()!=0||Item[n].m_CommonAttrib.nUpgradeLvl)continue;a=goal->id;b=n;c=Item[n].GetID();}found=true;break;
 }if(!found)return 0;}
 int npc=Player[p].m_nLastNpcIndex;KLuaScript* script=(KLuaScript*)g_GetScript(Npc[npc].m_ActionScriptID);if(!script||!script->m_LuaState)return 0;lua_State* L=script->m_LuaState;int top=lua_gettop(L),refs[3];const char* names[]={SCRIPT_PLAYERINDEX,SCRIPT_PLAYERID,SCRIPT_SUBWORLDINDEX};double values[]={p,Player[p].m_dwID,Npc[Player[p].m_nIndex].m_SubWorldIndex};
 for(int at=0;at<3;++at){lua_getglobal(L,names[at]);refs[at]=lua_ref(L,1);lua_pushnumber(L,values[at]);lua_setglobal(L,names[at]);}
 int result=script->CallFunction("TranNguyenAction",1,"dddd",goal->phase,a,b,c)&&lua_isnumber(L,-1)?(int)lua_tonumber(L,-1):0;
 for(at=0;at<3;++at){if(refs[at]==LUA_REFNIL)lua_pushnil(L);else lua_getref(L,refs[at]);lua_setglobal(L,names[at]);if(refs[at]>=0)lua_unref(L,refs[at]);}lua_settop(L,top);return result;
}
#endif
