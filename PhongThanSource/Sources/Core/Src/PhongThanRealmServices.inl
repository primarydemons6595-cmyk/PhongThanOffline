#include "../../../Headers/PhongThanBoatLanding.h"
namespace PtRealmServices {
static int Ensure(int map,int tid,int x,int y,const char* script,const char* name){
 int world=g_SubWorldSet.SearchWorld(map);if(world<0)return 0;DWORD hash=g_FileName2Id((char*)script);
 for(int n=NpcSet.GetNextIdx(0);n;n=NpcSet.GetNextIdx(n))if(Npc[n].m_SubWorldIndex==world&&Npc[n].m_NpcSettingIdx==tid&&Npc[n].m_ActionScriptID==hash){g_StrCpyLen(Npc[n].ActionScript,script,sizeof(Npc[n].ActionScript));return n;}
 if(!g_GetScript((char*)script))return 0;int spawned=PhongThanSpawnNpc(tid,1,world,x,y,FALSE);if(spawned<=0)return 0;
 g_StrCpyLen(Npc[spawned].ActionScript,script,sizeof(Npc[spawned].ActionScript));
 Npc[spawned].m_Kind=kind_dialoger;Npc[spawned].m_AiMode=0;Npc[spawned].m_ActionScriptID=hash;Npc[spawned].m_DeathScriptID=0;Npc[spawned].m_TimerScriptID=0;
 Npc[spawned].m_DialogRadius=256;Npc[spawned].m_nNpcTimeout=0;g_StrCpyLen(Npc[spawned].Name,name,sizeof(Npc[spawned].Name));Npc[spawned].SendSyncData(0,TRUE);return spawned;
}
}
void PhongThanRealmServicesTick(){
 static DWORD last=0;DWORD now=GetTickCount();if(now-last<5000)return;last=now;
 PtRealmServices::Ensure(1065,1453,230*256,174*512,"\\script\\phongthan\\realm\\boat_tran_duong.lua",PT_REALM_BOAT_NAME);
 PtRealmServices::Ensure(1055,1453,223*256,197*512,"\\script\\phongthan\\realm\\boat_dong_doanh.lua",PT_REALM_BOAT_NAME);
 const char* van="\\script\\\xD1\xFE\xB3\xD8\\\xD4\xC6\xCF\xF6\xC4\xEF\xC4\xEF.lua";
 PtRealmServices::Ensure(1052,165,195*256,203*512,van,PT_REALM_VAN_NAME);
 const char* quynh="\\script\\\xD1\xFE\xB3\xD8\\\xC7\xED\xCF\xF6\xC4\xEF\xC4\xEF.lua";
 PtRealmServices::Ensure(1052,165,201*256,191*512,quynh,"\x51\x75\xfa\x6e\x68\x20\x54\x69\xaa\x75");
 if(!PtRealmWorldRuntime::Load())return;
 if(PtRealmWorldRuntime::state.Phase>=1)PtRealmServices::Ensure(1052,757,196*256,203*512,"\\script\\phongthan\\realm\\linh_quang.lua",PT_REALM_LINH_QUANG);
 if(PtRealmWorldRuntime::state.Phase==3){
  int portal=PtRealmServices::Ensure(1052,513,200*256,196*512,"\\script\\phongthan\\realm\\portal.lua",PT_REALM_PORTAL);
  // Replace only the earlier authored placeholder sharing the portal point.
  if(portal){int world=g_SubWorldSet.SearchWorld(1052);
   for(int n=NpcSet.GetNextIdx(0);n;){int next=NpcSet.GetNextIdx(n);
    if(Npc[n].m_SubWorldIndex==world&&Npc[n].m_NpcSettingIdx==780&&Npc[n].m_ActionScriptID==g_FileName2Id("\\script\\phongthan\\npc_restore\\1052_04.lua"))PhongThanRemoveNpc(n);n=next;}
  }
 }
}
int LuaPtRealmWorld(Lua_State* L){
 int p=GetPlayerIndex(L),role=PtRealmQuest::Role(p),op=Lua_GetTopIndex(L)==1&&Lua_IsNumber(L,1)?(int)Lua_ValueToNumber(L,1):-1;
 bool loaded=PtRealmWorldRuntime::Load();int result=0;
 if(role&&loaded&&op==0)result=1;
 if(role==7&&loaded&&op==1&&(PtRealmWorldRuntime::state.Phase==1||PtRealmWorldRuntime::state.Phase==2)&&
  Npc[Player[p].m_nIndex].m_Level>=121&&Player[p].m_cTask.GetSaveVal(4879)==0){Player[p].m_cTask.SetSaveVal(4879,1,TRUE);result=1;}
 Lua_PushNumber(L,result);Lua_PushNumber(L,loaded?PtRealmWorldRuntime::state.Phase:0);Lua_PushNumber(L,loaded?PtRealmWorldRuntime::state.Streak:0);
 Lua_PushNumber(L,loaded?PtRealmWorldRuntime::state.Reputation:0);Lua_PushNumber(L,loaded?PtRealmWorldRuntime::state.FinalMask:0);
 Lua_PushNumber(L,role?Player[p].m_cTask.GetSaveVal(4879):0);Lua_PushNumber(L,loaded?PtRealmWorldRuntime::state.Mask:0);return 7;
}
