#include "PhongThanThanKyRuntime.h"
#include "PhongThanThanKyExperiment.h"
#ifdef _SERVER
void PhongThanThanKyClearSelection(int player,int skill){
 if(player<=0||player>=MAX_PLAYER)return;
 // The client validates its own left/right selections against the complete
 // skill snapshot. Server builds do not own the client's selected buttons.
 int actor=Player[player].m_nIndex;if(actor>0&&actor<MAX_NPC&&Npc[actor].m_ActiveSkillID==skill)Npc[actor].m_ActiveSkillID=0;
}
void PhongThanThanKySyncRemoveBuff(int actor,int skill,int level){
 if(actor<=0||actor>=MAX_NPC||!Npc[actor].IsPlayer()||!g_pServer)return;
 PHONGTHAN_STATE_EFFECT_HEADER state;ZeroMemory(&state,sizeof(state));
 state.MapId=SubWorld[Npc[actor].m_SubWorldIndex].m_SubWorldID;state.EntityId=Npc[actor].m_dwID;
 state.SkillId=skill;state.Level=level;state.ReplaceExisting=1;
 PhongThanInitializeWireHeader(&state.Header,PHONGTHAN_MSG_GAMEPLAY_STATE_EFFECT,sizeof(state),PHONGTHAN_WIRE_FLAG_RESPONSE,0);
 g_pServer->PackDataToClient(Player[Npc[actor].GetPlayerIdx()].m_nNetConnectIdx,&state,sizeof(state));
}
// Shares the existing morph-state owner/identity checks and visual wire.
int PhongThanThanKyMorphPlayer(int p,int target,int ticks){
 PHONGTHAN_PLAYER_NPC_STATE* state=PhongThanPlayerState(p);if(!state)return 0;
 int actor=Player[p].m_nIndex;
 if(target<0){Npc[actor].m_NpcSettingIdx=state->nOriginalTemplate;state->nMorphTemplate=-1;state->nMorphDeadline=0;Npc[actor].SendSyncData(0,TRUE);return 1;}
 if(target>=g_NpcSetting.GetHeight()-1||ticks<=0||state->nMorphTemplate>=0)return 0;
 Npc[actor].m_NpcSettingIdx=target;state->nMorphTemplate=target;
 // Than Ky expires by game ticks in its own lifecycle, not wall-clock Lua.
 state->nMorphDeadline=0;Npc[actor].SendSyncData(0,TRUE);return 1;
}
int LuaPtThanKyUseBook(Lua_State* L){
 int p=GetPlayerIndex(L);int item=Lua_GetTopIndex(L)>=1?(int)Lua_ValueToNumber(L,1):0;
 Lua_PushNumber(L,PhongThanThanKyUseBook(p,item));return 1;
}
int LuaPtThanKyExchange(Lua_State* L){
 int p=GetPlayerIndex(L),choice=Lua_GetTopIndex(L)>0?(int)Lua_ValueToNumber(L,1):0;
 if(!PhongThanIsLivePlayer(p)||choice<1||choice>29){Lua_PushNumber(L,0);return 1;}
 KPlayer& owner=Player[p];int actor=owner.m_nIndex,npc=owner.m_nLastNpcIndex;
 int result=0;
 if(npc>0&&npc<MAX_NPC&&Npc[npc].m_RegionIndex>=0&&Npc[npc].m_SubWorldIndex==Npc[actor].m_SubWorldIndex&&
    SubWorld[Npc[actor].m_SubWorldIndex].m_SubWorldID==1003&&NpcSet.GetDistance(actor,npc)<=320&&
    !owner.GetLockState()&&!owner.CheckTrading()&&Npc[actor].m_Level>=140){
  int x,y;Npc[npc].GetMpsPos(&x,&y);
  // Exact service location, not any unrelated NPC on the map.
  if(x/256==207&&y/512==207&&Npc[npc].m_ActionScriptID==g_FileName2Id("\\script\\phongthan\\services\\ho_lo_dang.lua")){
   struct Recipe{int output,skill,profession,fragment,fragments,stones,tokens,sourceRank;};
   static const Recipe recipes[]={
    {8885,1556,2,8868,2,1000,1,0},{8886,1556,2,8868,4,1000,1,0},{8887,1556,2,8868,6,1000,1,0},
    {8888,1551,2,8869,2,1000,1,0},{8889,1547,2,8870,2,1000,1,0},{8890,1557,0,8871,2,1000,1,0},
    {8891,1560,0,8872,2,1000,1,0},{8892,1552,0,8873,2,1000,1,0},{8893,1548,1,8874,2,1000,1,0},
    {8894,1554,1,8875,2,1000,1,0},{8895,1553,1,8876,2,1000,1,0},
    {8375,1986,0,8352,2,1000,1,0},{8376,1996,2,8353,2,1000,1,0},{8377,1990,1,8354,2,1000,1,0},
    {8378,1991,1,8355,2,1000,1,0},{8379,1993,1,8356,2,1000,1,0},{8380,1994,1,8357,2,1000,1,0},
    {8621,1986,0,8352,4,2000,2,1},{8622,1996,2,8353,4,2000,2,1},{8623,1990,1,8354,4,2000,2,1},
    {8624,1991,1,8355,4,2000,2,1},{8625,1993,1,8356,4,2000,2,1},{8626,1994,1,8357,4,2000,2,1},
    {8621,1986,0,0,0,4000,4,1},{8622,1996,2,0,0,4000,4,1},{8623,1990,1,0,0,4000,4,1},
    {8624,1991,1,0,0,4000,4,1},{8625,1993,1,0,0,4000,4,1},{8626,1994,1,0,0,4000,4,1}};
   const Recipe& r=recipes[choice-1];
   if(owner.m_cProfession.GetProfession()==r.profession&&PtTkExperiment::Enabled(r.skill)){
    PhongThanQuestExchangeRow needs[4],reward;int count=0;bool valid=true;
    if(r.sourceRank){int source=PhongThanThanKyCarriedBookKey(p,r.skill,r.sourceRank);if(!source)valid=false;
     if(valid){valid=PhongThanDecodeQuestItemTuple(6,1,source,1,0,0,needs[count].item);needs[count++].count=1;}}
    if(r.fragments){valid=valid&&PhongThanDecodeQuestItemTuple(6,1,r.fragment,1,0,0,needs[count].item);needs[count++].count=r.fragments;}
    valid=valid&&PhongThanDecodeQuestItemTuple(3,1527,0,0,0,0,needs[count].item);needs[count++].count=r.tokens;
    valid=valid&&PhongThanDecodeQuestItemTuple(3,1360,0,0,0,0,needs[count].item);needs[count++].count=r.stones;
    valid=valid&&PhongThanDecodeQuestItemTuple(6,1,r.output,1,0,0,reward.item);reward.count=1;
    if(valid){int value=owner.m_cTask.GetSaveVal(TASKVALUE_PT_HANDBOOK_RECEIPT);
     result=PhongThanQuestExchange(p,TASKVALUE_PT_HANDBOOK_RECEIPT,value,value==1?0:1,needs,count,&reward,1,true,0,true,0,true)?1:0;}
   }
  }
 }
 Lua_PushNumber(L,result);return 1;
}
#else
int LuaPtThanKyUseBook(Lua_State* L){Lua_PushNumber(L,0);return 1;}
int LuaPtThanKyExchange(Lua_State* L){Lua_PushNumber(L,0);return 1;}
#endif
