void PhongThanDailyWineKill(int,int);
// Server-owned daily completion receipts. Linh Nguyen is the original 418
// collectible counter, never a newly invented inventory item.
#include "PhongThanDailyQuest.h"
#include <time.h>
#include "../../../Headers/PhongThanNpcLifeToken.h"
static PhongThanNpcLifeToken g_PtDailyLives[MAX_NPC];
void PhongThanDailyNpcRevived(int npc){PhongThanStudioQuestRevived(npc);if(npc>0&&npc<MAX_NPC)g_PtDailyLives[npc].Revived(Npc[npc].m_dwID);}
namespace PtDaily {
enum {LastReceipt=4350,PendingHope=4360,ScoutDay=4361,ScoutRounds=4362};
static bool Live(int p){return p>0&&p<MAX_PLAYER&&Player[p].m_dwID&&Player[p].m_nIndex>0&&Player[p].m_nIndex<MAX_NPC&&Npc[Player[p].m_nIndex].IsPlayer();}
static bool Queue(int p){int pending=Player[p].m_cTask.GetSaveVal(PendingHope),count=PhongThanPetHopeCount(p);
 if(PhongThanPetHopeEligibility(p)!=1||pending<0||count<0||count>=30||pending>=30-count)return false;
 Player[p].m_cTask.SetSaveVal(PendingHope,pending+1,TRUE);return true;}
static int Flush(int p){int pending=Player[p].m_cTask.GetSaveVal(PendingHope),given=0;
 int eligible=PhongThanPetHopeEligibility(p);
 if(eligible==0||PhongThanPetHopeCount(p)>=30){Player[p].m_cTask.SetSaveVal(PendingHope,0,TRUE);return 0;}
 if(eligible<0)return 0;
 while(pending>0&&given<30&&PhongThanPetHopeCount(p)<30&&PhongThanPetHopeAdjust(p,1)){--pending;++given;Player[p].m_cTask.SetSaveVal(PendingHope,pending,TRUE);}
 if(PhongThanPetHopeCount(p)>=30)Player[p].m_cTask.SetSaveVal(PendingHope,0,TRUE);
 return given;}
}
#include "PhongThanSoulCircle.inl"
int LuaDailyHope(Lua_State* L){
 int p=GetPlayerIndex(L),ok=0,given=0;
 if(PtDaily::Live(p)&&Lua_GetTopIndex(L)==2&&Lua_IsNumber(L,1)&&Lua_IsNumber(L,2)){
  double kindValue=Lua_ValueToNumber(L,1),receiptValue=Lua_ValueToNumber(L,2);int kind=(int)kindValue;
  if(kindValue==kind&&kind>=1&&kind<=10&&receiptValue>=1&&receiptValue<=2000000000&&receiptValue==(int)receiptValue){
   int receipt=(int)receiptValue,slot=PtDaily::LastReceipt+kind-1,old=Player[p].m_cTask.GetSaveVal(slot),pending=Player[p].m_cTask.GetSaveVal(PtDaily::PendingHope);
   if(receipt>old&&pending>=0&&pending<1000000){
    bool award=kind!=7;
    if(kind==7){int day=(int)((time(NULL)+25200)/86400),lastDay=Player[p].m_cTask.GetSaveVal(PtDaily::ScoutDay),rounds=lastDay==day?Player[p].m_cTask.GetSaveVal(PtDaily::ScoutRounds):0;
     if(rounds<0||rounds>10)rounds=0;if(rounds<10){++rounds;award=rounds==10;}Player[p].m_cTask.SetSaveVal(PtDaily::ScoutDay,day,TRUE);Player[p].m_cTask.SetSaveVal(PtDaily::ScoutRounds,rounds,TRUE);
    }
    if(award)PtDaily::Queue(p);
    Player[p].m_cTask.SetSaveVal(slot,receipt,TRUE);ok=1;
   }
   given=PtDaily::Flush(p);
  }
 }
 Lua_PushNumber(L,ok);Lua_PushNumber(L,given);return 2;
}
int LuaDailyHopeFlush(Lua_State* L){int p=GetPlayerIndex(L);Lua_PushNumber(L,PtDaily::Live(p)?PtDaily::Flush(p):0);return 1;}
int LuaDailyExchange(Lua_State* L){
 int p=GetPlayerIndex(L),result=0,values[6];bool valid=PtDaily::Live(p)&&Lua_GetTopIndex(L)==8;
 const int positions[6]={1,2,3,6,7,8};
 for(int i=0;i<6&&valid;++i){double n=Lua_ValueToNumber(L,positions[i]);if(!Lua_IsNumber(L,positions[i])||n<0||n>1000000000||n!=(int)n)valid=false;else values[i]=(int)n;}
 if(valid&&(values[0]<4380||values[0]>4459||values[1]>16||values[2]>16))valid=false;
 PhongThanQuestExchangeRow needs[16],rewards[16];int nn=0,nr=0;
 if(valid&&PhongThanReadQuestExchangeRows(L,4,needs,nn)&&PhongThanReadQuestExchangeRows(L,5,rewards,nr)&&!Player[p].GetLockState()&&!Player[p].CheckTrading()&&Player[p].m_cTask.GetSaveVal(values[0])==values[1]){
  int fee=values[4],money=values[5];bool paid=fee==0||Player[p].Pay(fee);
  if(paid){
   bool earned=money==0||Player[p].Earn(money);
   if(earned){int task=values[0],from=values[1],to=values[2];if(from==to){task=4490;from=Player[p].m_cTask.GetSaveVal(task);to=from<1000000000?from+1:0;}if((task==4490&&from>=0&&to>from)||task==values[0])result=PhongThanQuestExchange(p,task,from,to,needs,nn,rewards,nr,true);}
   if(result==1){if(values[3])Player[p].DirectAddExp(values[3]);}
   else{if(earned&&money)Player[p].Pay(money);if(fee)Player[p].Earn(fee);}
  }
 }
 Lua_PushNumber(L,result);return 1;
}
int LuaDailyJournal(Lua_State* L){
 int p=GetPlayerIndex(L);if(PtDaily::Live(p)&&Lua_GetTopIndex(L)==3&&Lua_IsNumber(L,1)&&Lua_IsNumber(L,2)){
  int kind=(int)Lua_ValueToNumber(L,1),phase=(int)Lua_ValueToNumber(L,2);const char* text=Lua_ValueToString(L,3);
  if(kind>=1&&kind<=10&&phase>=0&&phase<=16&&text&&strlen(text)<480)SendPhongThanTaskRecord(p,3089+kind,phase,phase?1:2,text);
 }return 0;
}
int LuaDailySpawn(Lua_State* L){
 int result=0;const int p=GetPlayerIndex(L);
 if(PtDaily::Live(p)&&Lua_GetTopIndex(L)==7){int tid=(int)Lua_ValueToNumber(L,1),map=(int)Lua_ValueToNumber(L,2),x=(int)Lua_ValueToNumber(L,3),y=(int)Lua_ValueToNumber(L,4),level=(int)Lua_ValueToNumber(L,5),life=(int)Lua_ValueToNumber(L,6);const char* name=Lua_ValueToString(L,7);
  int world=g_SubWorldSet.SearchWorld(map);
  if(world>=0&&((tid>=14&&tid<=42)||tid==571||tid==572)&&level>=1&&level<=200&&life>=1000&&life<=10000000&&name&&strlen(name)<32){
   result=PhongThanSpawnNpc(tid,level,world,x,y,FALSE);
   if(result>0){KNpc& n=Npc[result];n.m_Kind=kind_normal;n.m_Camp=n.m_CurrentCamp=camp_animal;n.m_AiMode=1;n.m_CurrentLife=n.m_CurrentLifeMax=life;n.m_LifeMax=life;n.m_DeathScriptID=0;n.m_Experience=0;g_StrCpyLen(n.Name,name,sizeof(n.Name));}
  }
 }Lua_PushNumber(L,result);Lua_PushNumber(L,result>0?Npc[result].m_dwID:0);return 2;
}
// Called by the authoritative death-punishment pipeline, independently of
// monster-specific OnDeath scripts. Preserve every shared Lua context field.
void PhongThanDailyNpcKilled(int npc,int p){
 if(!PtDaily::Live(p)||npc<=0||npc>=MAX_NPC||Npc[npc].IsPlayer()||Npc[npc].m_Kind!=kind_normal||Npc[npc].m_CurrentLife>0||Npc[npc].m_SubWorldIndex!=Npc[Player[p].m_nIndex].m_SubWorldIndex)return;
 PhongThanDailyWineKill(p,SubWorld[Npc[npc].m_SubWorldIndex].m_SubWorldID);
 const char* path="\\script\\phongthan\\daily\\runtime.lua";KLuaScript* script=(KLuaScript*)g_GetScript((char*)path);
 if(!script||!script->m_LuaState)return;Lua_State* L=script->m_LuaState;int top=Lua_GetTopIndex(L),refs[3];const char* names[3]={SCRIPT_PLAYERINDEX,SCRIPT_PLAYERID,SCRIPT_SUBWORLDINDEX};
 for(int i=0;i<3;++i){lua_getglobal(L,names[i]);refs[i]=lua_ref(L,1);}
 double values[3]={p,Player[p].m_dwID,Npc[npc].m_SubWorldIndex};
 for(i=0;i<3;++i){lua_pushnumber(L,values[i]);lua_setglobal(L,names[i]);}
 script->CallFunction("pt_daily_kill",0,"dddddd",npc,Npc[npc].m_dwID,Npc[npc].m_NpcSettingIdx,SubWorld[Npc[npc].m_SubWorldIndex].m_SubWorldID,PtDailySoul::Contains(p,npc)?1:0,g_PtDailyLives[npc].Get(Npc[npc].m_dwID));
 for(i=0;i<3;++i){if(refs[i]==LUA_REFNIL)lua_pushnil(L);else lua_getref(L,refs[i]);lua_setglobal(L,names[i]);if(refs[i]>=0)lua_unref(L,refs[i]);}Lua_SetTopIndex(L,top);
}
void PhongThanDailyTick(){
 static DWORD tick=0;DWORD now=GetTickCount();if(now-tick<1000)return;tick=now;PtDailySoul::Tick();
 KLuaScript* script=(KLuaScript*)g_GetScript("\\script\\phongthan\\daily\\runtime.lua");if(!script||!script->m_LuaState)return;
 Lua_State* L=script->m_LuaState;int top=Lua_GetTopIndex(L),refs[3];const char* names[3]={SCRIPT_PLAYERINDEX,SCRIPT_PLAYERID,SCRIPT_SUBWORLDINDEX};
 for(int i=0;i<3;++i){lua_getglobal(L,names[i]);refs[i]=lua_ref(L,1);}
 for(int p=1;p<MAX_PLAYER;++p)if(PtDaily::Live(p)){double values[3]={p,Player[p].m_dwID,Npc[Player[p].m_nIndex].m_SubWorldIndex};for(i=0;i<3;++i){lua_pushnumber(L,values[i]);lua_setglobal(L,names[i]);}script->CallFunction("pt_daily_tick",0,"");}
 for(i=0;i<3;++i){if(refs[i]==LUA_REFNIL)lua_pushnil(L);else lua_getref(L,refs[i]);lua_setglobal(L,names[i]);if(refs[i]>=0)lua_unref(L,refs[i]);}Lua_SetTopIndex(L,top);
}

#include "PhongThanTuTuong.inl"
