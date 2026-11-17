#ifdef _SERVER
#include "PhongThanCarry.h"
enum {PT_MASTER_CREDIT=5350};
static int PtCarryTier(int capacity){return capacity<400?0:capacity<600?1:capacity<700?2:capacity<800?3:capacity<850?4:capacity<900?5:capacity<1000?6:capacity<1100?7:8;}
int LuaGetNativeWeightMax(Lua_State* L){Lua_PushNumber(L,PhongThanCarryCapacity(GetPlayerIndex(L),false));return 1;}
int LuaMasterCredit(Lua_State* L){
 int p=GetPlayerIndex(L),result=0;if((p>0&&p<MAX_PLAYER&&Player[p].m_dwID&&Player[p].m_nIndex>0&&Player[p].m_nIndex<MAX_NPC)){
  int amount=Lua_GetTopIndex(L)?(int)Lua_ValueToNumber(L,1):0,value=Player[p].m_cTask.GetSaveVal(PT_MASTER_CREDIT);
  if(!amount)result=value;
  else if(amount>0&&value<=2147483647-amount){Player[p].m_cTask.SetSaveVal(PT_MASTER_CREDIT,value+amount,TRUE);result=1;}
  else if(amount<0&&amount>=-value){Player[p].m_cTask.SetSaveVal(PT_MASTER_CREDIT,value+amount,TRUE);result=1;}
 }Lua_PushNumber(L,result);return 1;
}
int LuaDecMasterCredit(Lua_State* L){int amount=Lua_GetTopIndex(L)?(int)Lua_ValueToNumber(L,1):0;if(amount<=0){Lua_PushNumber(L,0);return 1;}Lua_SetTopIndex(L,0);Lua_PushNumber(L,-amount);return LuaMasterCredit(L);}
int LuaEventCarryExchange(Lua_State* L){
 int p=GetPlayerIndex(L),result=0;
 if((p>0&&p<MAX_PLAYER&&Player[p].m_dwID&&Player[p].m_nIndex>0&&Player[p].m_nIndex<MAX_NPC)&&!Player[p].GetLockState()&&!Player[p].CheckTrading()){
  int n=Player[p].m_nLastNpcIndex,actor=Player[p].m_nIndex;
  if(n>0&&n<MAX_NPC&&Npc[n].m_SubWorldIndex==Npc[actor].m_SubWorldIndex&&Npc[n].m_Kind==kind_dialoger){
   const DWORD legacy=g_FileName2Id("\\script\\\xCE\xF7\xE1\xAA\\\xC4\xC4\xDF\xB8.lua");
   const DWORD restored=g_FileName2Id("\\script\\phongthan\\npc_recovered_batch\\1020_0cf87e2d.lua");
   int x,y,nx,ny;Npc[n].GetMpsPos(&nx,&ny);Npc[actor].GetMpsPos(&x,&y);
   if((Npc[n].m_ActionScriptID==legacy||Npc[n].m_ActionScriptID==restored||Npc[n].m_NpcSettingIdx==300)&&abs(nx-x)<=512&&abs(ny-y)<=512){
    int capacity=PhongThanCarryCapacity(p,false),limit=min(1800,Npc[actor].m_Level*10+100);
    static const int credit[9]={5,10,20,40,40,80,100,100,150};
    static const int master[9]={0,0,0,0,1,1,1,1,2};
    static const int money[9]={0,0,0,0,10,10,20,50,100};int tier=PtCarryTier(capacity);
    int cash=money[tier]*capacity,rep=Player[p].m_cTask.GetSaveVal(TASKVALUE_PT_CREDIT),pr=Player[p].m_cTask.GetSaveVal(PT_MASTER_CREDIT);
    if(Npc[actor].m_Level<=20||capacity>=limit)result=-2;
    else if(rep<credit[tier])result=-3;
    else if(pr<master[tier])result=-4;
    else if(cash&&Player[p].m_ItemList.GetMoney(room_equipment)<cash)result=-5;
    else if(!cash||Player[p].Pay(cash)){
     Player[p].m_cTask.SetSaveVal(TASKVALUE_PT_CREDIT,rep-credit[tier],TRUE);
     Player[p].m_cTask.SetSaveVal(PT_MASTER_CREDIT,pr-master[tier],TRUE);
     Player[p].m_cTask.SetSaveVal(TASKVALUE_PT_CARRY_BONUS,capacity-200+1,TRUE);result=1;
    }
   }
  }
 }Lua_PushNumber(L,result);return 1;
}

#endif
