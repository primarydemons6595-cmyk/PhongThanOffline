#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <vector>
#include "KWin32.h"
#include "KLuaScript.h"
#include "../../Headers/PhongThanVanTienMission.h"
static int checks, arena, state[5], value[5][100], tasks[3][5000], playerMap[3], member[3], fight[3], locked[3], logoutRv[3];
static int doctorShop;
static int Sale(Lua_State* L){doctorShop=(int)Lua_ValueToNumber(L,1);return 0;}
static int npcs,templates[2000],owned[2000],spawnFailure,bindFailure,room=1,awards,moves,moveFailure,phaseTimers[3010];
static KLuaScript *missions[5];
static int Num(Lua_State* L,int n){return (int)Lua_ValueToNumber(L,n);}
static int Ret(Lua_State* L,int n){Lua_PushNumber(L,n);return 1;}
static int Player(Lua_State* L){Lua_GetGlobal(L,"PlayerIndex");int p=Num(L,-1);Lua_Pop(L,1);return p;}
static int Noop(Lua_State*){return 0;}
static int One(Lua_State* L){return Ret(L,1);}
static int Zero(Lua_State* L){return Ret(L,0);}
static int GetState(Lua_State* L){return Ret(L,state[Num(L,1)]);}
static int SetState(Lua_State* L){state[Num(L,1)]=Num(L,2);return 0;}
static int GetValue(Lua_State* L){return Ret(L,value[arena][Num(L,1)]);}
static int SetValue(Lua_State* L){value[arena][Num(L,1)]=Num(L,2);return 0;}
static int GetTask(Lua_State* L){return Ret(L,tasks[Player(L)][Num(L,1)]);}
static int SetTask(Lua_State* L){tasks[Player(L)][Num(L,1)]=Num(L,2);return 0;}
static int Time(Lua_State* L){return Ret(L,1800000000);}
static int Sub(Lua_State* L){return Ret(L,Num(L,1)-66);}
static int ContentID(Lua_State* L){return Ret(L,66+arena);}
static int Level(Lua_State* L){return Ret(L,20*arena+20);}
static int World(Lua_State* L){Lua_PushNumber(L,66+arena);Lua_PushNumber(L,100);Lua_PushNumber(L,100);return 3;}
static int AddNpc(Lua_State* L){if(spawnFailure)return Ret(L,0);++npcs;templates[npcs]=Num(L,1);return Ret(L,npcs);}
static int AddMSNpc(Lua_State* L){owned[Num(L,2)]=arena;return Ret(L,Num(L,2));}
static int Bind(Lua_State* L){return Ret(L,bindFailure?0:1);}
static int DelNpc(Lua_State* L){owned[Num(L,1)]=0;return 0;}
static int Template(Lua_State* L){return Ret(L,templates[Num(L,1)]);}
static int Start(Lua_State* L){phaseTimers[Num(L,2)]=Num(L,3);return 0;}
static int Stop(Lua_State* L){phaseTimers[Num(L,2)]=0;return 0;}
static int IsMission(Lua_State* L){return Ret(L,state[Num(L,1)]?1:0);}
static int Open(Lua_State* L){missions[arena]->CallFunction("InitMission",0,"");return 0;}
static int Run(Lua_State* L){missions[arena]->CallFunction("RunMission",0,"");return 0;}
static int Close(Lua_State* L){missions[arena]->CallFunction("EndMission",0,"");for(int n=1;n<=npcs;++n)if(owned[n]==arena)owned[n]=0;return 0;}
static int Next(Lua_State* L){for(int p=Num(L,2)+1;p<=2;++p)if(member[p]==arena && playerMap[p]==arena){Lua_PushNumber(L,p);Lua_PushNumber(L,p);return 2;}Lua_PushNumber(L,0);Lua_PushNumber(L,0);return 2;}
static int AddPlayer(Lua_State* L){member[Player(L)]=arena;return Ret(L,Player(L));}
static int Move(Lua_State* L){if(moveFailure)return Ret(L,0);playerMap[Player(L)]=Num(L,1)>=67&&Num(L,1)<=70?Num(L,1)-66:0;++moves;return Ret(L,1);}
static int EventEnter(Lua_State* L){
 int p=Player(L),id=Num(L,1),generation=value[arena][1];
 if(id!=arena || state[id]==0 || (state[id]==2 && tasks[p][4789+id]!=generation))return Ret(L,0);
 if(moveFailure)return Ret(L,0);playerMap[p]=arena;member[p]=arena;tasks[p][4789+id]=generation;++moves;return Ret(L,1);
}
static int Fight(Lua_State* L){fight[Player(L)]=Num(L,1);return 0;}
static int Lock(Lua_State* L){locked[Player(L)]=Num(L,1);return 0;}
static int RV(Lua_State* L){logoutRv[Player(L)]=Num(L,1);return 0;}
static int Exchange(Lua_State* L){int p=Player(L),t=Num(L,1);if(!room || tasks[p][t]!=Num(L,2) || Num(L,2)==Num(L,3))return Ret(L,0);tasks[p][t]=Num(L,3);awards+=2;return Ret(L,1);}
#define CHECK(x) do{++checks;if(!(x)){printf("FAIL line=%d arena=%d %s\n",__LINE__,arena,#x);return 1;}}while(0)
static bool Load(KLuaScript& s,const char* path){
 FILE* f=fopen(path,"rb");if(!f){printf("MISSING %s\n",path);return false;}fseek(f,0,SEEK_END);long size=ftell(f);rewind(f);std::vector<unsigned char> b(size);fread(&b[0],1,size,f);fclose(f);
 if(!s.Init())return false;
 TLua_Funcs api[]={{"EventServiceOwns",Zero},{"EventServiceValue",Zero},{"EventEnterVanTien",EventEnter},{"SetNpcName",Noop},{"SetNpcScript",Noop},{"Msg2SubWorld",Noop},{"AddGlobalCountNews",Noop},{"GetTeam",Zero},{"GetGlobalValue",GetState},{"SetGlobalValue",SetState},{"GetMissionV",GetValue},{"SetMissionV",SetValue},{"GetTask",GetTask},{"SetTask",SetTask},{"SystemTime",Time},{"SubWorldID2Idx",Sub},{"SubWorldIdx2ContentID",ContentID},{"GetLevel",Level},{"GetWorldPos",World},{"GetContentWorldPos",World},{"AddNpc",AddNpc},{"AddMSNpc",AddMSNpc},{"SetNpcDeathScript",Bind},{"DelNpc",DelNpc},{"GetNpcTemplateID",Template},{"StartMissionTimer",Start},{"StopMissionTimer",Stop},{"IsMission",IsMission},{"OpenMission",Open},{"RunMission",Run},{"CloseMission",Close},{"GetNextPlayer",Next},{"AddMSPlayer",AddPlayer},{"NewWorld",Move},{"SetFightState",Fight},{"LockCamp",Lock},{"SetLogoutRV",RV},{"QuestExchange",Exchange},{"ShowMissionCountdown",Noop},{"AddNews",Noop},{"MsgBox",Noop},{"Sale",Sale},{"GetTeam",Zero},{"AddGlobalCountNews",Noop},{"Msg2MSAll",Noop},{"Msg2Player",Noop},{"TopMessage",Noop},{"CloseDialog",Noop},{"Say",Noop}};
 if(!s.RegisterFunctions(api,sizeof(api)/sizeof(api[0])) || !s.LoadBuffer(&b[0],size) || !s.ExecuteCode())return false;
 Lua_PushNumber(s.m_LuaState,1);s.SetGlobalName("PlayerIndex");Lua_PushNumber(s.m_LuaState,arena);s.SetGlobalName("SubWorld");return true;
}
int main(int argc,char** argv){ int n;
 CHECK(argc>=2);FILE* list=fopen(argv[1],"rb");CHECK(list!=NULL);char root[512],portalPath[512],path[1024];CHECK(fscanf(list,"%511s",root)==1);
 char route[260];PhongThanMissionScriptPath(route,1,1067);CHECK(!strcmp(route,"\\script\\phongthan\\van_tien\\mission01.lua"));PhongThanMissionScriptPath(route,1,1001);CHECK(!strcmp(route,"\\script\\missions\\mission01.lua"));PhongThanMissionScriptPath(route,2,1067);CHECK(!strcmp(route,"\\script\\missions\\mission02.lua"));
 for(arena=1;arena<=4;++arena){
  CHECK(fscanf(list,"%511s",portalPath)==1);KLuaScript mission,portal,open,start,off,guard,boss,heartbeat;missions[arena]=&mission;
  sprintf(path,"%s/script/phongthan/van_tien/mission%02d.lua",root,arena);CHECK(Load(mission,path));
  sprintf(path,"%s/%s",root,portalPath);CHECK(Load(portal,path));
  sprintf(path,"%s/script/phongthan/van_tien/open%d.lua",root,arena);CHECK(Load(open,path));
  sprintf(path,"%s/script/timertask/task%d.lua",root,3000+arena*2-1);CHECK(Load(start,path));
  sprintf(path,"%s/script/timertask/task%d.lua",root,3000+arena*2);CHECK(Load(off,path));
  sprintf(path,"%s/script/phongthan/van_tien/guard%d.lua",root,arena);CHECK(Load(guard,path));
  sprintf(path,"%s/script/phongthan/van_tien/boss%d.lua",root,arena);CHECK(Load(boss,path));
  sprintf(path,"%s/script/timertask/task3009.lua",root);CHECK(Load(heartbeat,path));
  CHECK(portal.CallFunction("pt_enter",0,""));CHECK(playerMap[1]==0);
  CHECK(open.CallFunction("main",0,""));CHECK(state[arena]==1 && phaseTimers[3000+arena*2-1]==18*5*60);
  int generation=value[arena][1];CHECK(generation>0);CHECK(open.CallFunction("main",0,""));CHECK(value[arena][1]==generation);
  moveFailure=1;CHECK(portal.CallFunction("pt_enter",0,""));CHECK(member[1]!=arena && !tasks[1][4789+arena]);moveFailure=0;
  CHECK(portal.CallFunction("pt_enter",0,""));CHECK(playerMap[1]==arena && member[1]==arena && tasks[1][4789+arena]==generation && !fight[1] && locked[1]);
  int beforeStart=npcs;int prepared=value[arena][40]!=0;spawnFailure=arena==2?0:1;CHECK(start.CallFunction("OnMissionTimer",0,""));CHECK(state[arena]==2 && fight[1] && phaseTimers[3000+arena*2]==18*30*60 && !value[arena][30]);if(arena==2)CHECK(npcs-beforeStart==(prepared?4:28));spawnFailure=0;
  CHECK(heartbeat.CallFunction("OnMissionTimer",0,""));for(n=1;n<=4;++n)CHECK(value[arena][20+n]>0);CHECK(!value[arena][30]);
  for(n=1;n<=3;++n){int npc=value[arena][20+n];CHECK(guard.CallFunction("OnDeath",0,"d",npc));CHECK(!value[arena][30]);int before=npcs;CHECK(guard.CallFunction("OnDeath",0,"d",npc));CHECK(npcs==before);}
  spawnFailure=1;CHECK(guard.CallFunction("OnDeath",0,"d",value[arena][24]));CHECK(!value[arena][30]);spawnFailure=0;CHECK(heartbeat.CallFunction("OnMissionTimer",0,""));CHECK(value[arena][30]>0 && templates[value[arena][30]]==128+5*arena);
  CHECK(boss.CallFunction("OnDeath",0,"d",value[arena][30]+1));CHECK(!tasks[1][4929+arena]);int beforeChests=npcs;CHECK(boss.CallFunction("OnDeath",0,"d",value[arena][30]));CHECK(!tasks[1][4929+arena] && npcs==beforeChests+4);CHECK(boss.CallFunction("OnDeath",0,"d",value[arena][30]));CHECK(npcs==beforeChests+4);
  CHECK(off.CallFunction("OnMissionTimer",0,""));CHECK(!state[arena] && !playerMap[1] && !fight[1] && !locked[1] && !logoutRv[1]);for(n=1;n<=npcs;++n)CHECK(owned[n]!=arena);
  int oldAwards=awards;CHECK(portal.CallFunction("pt_claim",0,""));CHECK(awards==oldAwards);tasks[1][4929+arena]=generation;tasks[1][4933+arena]=generation;room=0;CHECK(portal.CallFunction("pt_claim",0,""));CHECK(awards==oldAwards && tasks[1][4929+arena]==generation);room=1;CHECK(portal.CallFunction("pt_claim",0,""));CHECK(awards==oldAwards+2 && !tasks[1][4929+arena]);CHECK(portal.CallFunction("pt_claim",0,""));CHECK(awards==oldAwards+2);
  // Crash-recovery window: committed reward with uncleared pending marker.
  tasks[1][4929+arena]=generation;CHECK(portal.CallFunction("pt_claim",0,""));CHECK(awards==oldAwards+2 && !tasks[1][4929+arena]);
  member[1]=0;
 }
 if(argc>=3){KLuaScript doctor;CHECK(Load(doctor,argv[2]));tasks[1][408]=1;CHECK(doctor.CallFunction("main",0,""));CHECK(tasks[1][408]==100000 && doctorShop==0);CHECK(doctor.CallFunction("no",0,""));CHECK(doctorShop==0);CHECK(doctor.CallFunction("yes",0,""));CHECK(doctorShop==24);tasks[1][408]=0;CHECK(doctor.CallFunction("main",0,""));CHECK(tasks[1][408]==0);
  for(int tid=53;tid<=60;++tid){KLuaScript mob;sprintf(path,"%s/script/phongthan/van_tien/mob%d.lua",root,tid);CHECK(Load(mob,path));tasks[1][408]=tid%2?2:3;tasks[1][407]=2;tasks[1][409]=20833;CHECK(mob.CallFunction("OnDeath",0,"d",1));CHECK(tasks[1][407]==1 && tasks[1][408]==(tid%2?2:3));CHECK(mob.CallFunction("OnDeath",0,"d",1));CHECK(tasks[1][407]==0 && tasks[1][408]==100000);tasks[1][408]=tid%2?2:3;tasks[1][407]=2;tasks[1][409]=0;CHECK(mob.CallFunction("OnDeath",0,"d",1));CHECK(tasks[1][407]==0 && tasks[1][408]==0);}
 }
 fclose(list);printf("PASS VAN_TIEN_LIFECYCLE arenas=4 checks=%d Lua4_actual_payloads=32 staged_portals=4 registration_combat_guard_gate_boss_original_chests_legacy_pending_cleanup=PASS\n",checks);
 puts("SCOPE real staged Lua on explicit inventory/world/timer test hosts; live player acceptance remains separate.");return 0;
}
