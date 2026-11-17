#include "../../../Headers/PhongThanIslandContract.h"
namespace PtIslandFame {
static int Rate(){KIniFile ini;int rate=1;if(ini.Load("\\settings\\phongthan\\IslandReputation.ini"))ini.GetInteger("PhuongTruong","FamePerItem",1,&rate);return rate>=1&&rate<=100?rate:1;}
static int Apply(int p,int op,int count){int role=PtRealmQuest::Role(p);if(role!=11||p<=0||p>=MAX_PLAYER||!Player[p].m_PlayerDBLoad||Player[p].CheckTrading()||Player[p].GetLockState())return 0;
 int actor=Player[p].m_nIndex;if(actor<=0||actor>=MAX_NPC||Npc[actor].m_CurrentLife<=0)return 0;
 int task=PtIslandTask::QuynhFame,old=Player[p].m_cTask.GetSaveVal(task);if(old<0||old>1000000000)return 0;
 if(op==0)return 1;
 if(op==1){int rate=Rate();if(count<1||count>1000||old>1000000000-count*rate)return 0;
  PhongThanQuestExchangeRow need;ZeroMemory(&need,sizeof(need));PhongThanDecodeQuestItemTuple(3,178,0,0,0,0,need.item);need.count=count;
  if(PhongThanQuestExchange(p,task,old,old+count*rate,&need,1,0,0,true,0,true)!=1)return 0;
 }else if(op==2||op==3){int cost=op==2?1000:3000,profession=Player[p].m_cProfession.GetProfession();if(old<cost||profession<0||profession>2)return 0;
  PhongThanQuestExchangeRow gift;ZeroMemory(&gift,sizeof(gift));if(!PhongThanDecodeQuestItemTuple(op==2?6:0,op==2?1:9,op==2?287+profession:6+profession,op==2?0:10,0,0,gift.item))return 0;gift.count=1;
  if(PhongThanQuestExchange(p,task,old,old-cost,0,0,&gift,1,true)!=1)return 0;
 }else return 0;
 Player[p].m_uMustSave=SAVE_REQUEST;return 1;
}}
int LuaPtIslandFame(Lua_State* L){int p=GetPlayerIndex(L),argc=Lua_GetTopIndex(L);int op=argc>=1&&Lua_IsNumber(L,1)?(int)Lua_ValueToNumber(L,1):-1;
 int count=argc>=2&&Lua_IsNumber(L,2)?(int)Lua_ValueToNumber(L,2):1;int result=PtIslandFame::Apply(p,op,count);
 Lua_PushNumber(L,result);Lua_PushNumber(L,result?Player[p].m_cTask.GetSaveVal(PtIslandTask::QuynhFame):0);Lua_PushNumber(L,PtIslandFame::Rate());return 3;}
