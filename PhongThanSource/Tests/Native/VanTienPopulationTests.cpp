#include <windows.h>
#include <stdio.h>
#include <string>
#include "KWin32.h"
#include "KLuaScript.h"
static int arena,phase[5],values[100],timers[3010],spawnCount,failed,normal,guards,bosses,dialogs,task[5000],map;
static KLuaScript* mission;
static int Num(Lua_State* L,int n){return (int)Lua_ValueToNumber(L,n);}
static int Ret(Lua_State* L,int n){Lua_PushNumber(L,n);return 1;}
static int Noop(Lua_State*){return 0;}
static int Zero(Lua_State* L){return Ret(L,0);}
static int One(Lua_State* L){return Ret(L,1);}
static int Time(Lua_State* L){return Ret(L,1800000000);}
static int State(Lua_State* L){return Ret(L,phase[Num(L,1)]);}
static int SetState(Lua_State* L){phase[Num(L,1)]=Num(L,2);return 0;}
static int GetValue(Lua_State* L){return Ret(L,values[Num(L,1)]);}
static int SetValue(Lua_State* L){values[Num(L,1)]=Num(L,2);return 0;}
static int AddNpc(Lua_State* L){if(failed){--failed;return Ret(L,0);}int tid=Num(L,1);++spawnCount;if(tid==51+arena*2||tid==52+arena*2)++normal;else if(tid==128+5*arena)++bosses;else ++guards;return Ret(L,spawnCount);}
static int Bind(Lua_State* L){return Ret(L,Num(L,2));}
static int Start(Lua_State* L){if(Num(L,1)!=arena){lua_error(L,"wrong mission timer owner");return 0;}timers[Num(L,2)]=Num(L,3);return 0;}
static int Stop(Lua_State* L){if(Num(L,1)!=arena){lua_error(L,"wrong mission timer owner");return 0;}timers[Num(L,2)]=0;return 0;}
static int Run(Lua_State* L){if(Num(L,1)!=arena){lua_error(L,"cross-arena RunMission");return 0;}if(!mission->CallFunction("RunMission",0,""))lua_error(L,"RunMission failed");return 0;}
static int Close(Lua_State* L){if(Num(L,1)!=arena){lua_error(L,"cross-arena CloseMission");return 0;}if(!mission->CallFunction("EndMission",0,""))lua_error(L,"EndMission failed");return 0;}
static int Next(Lua_State* L){Lua_PushNumber(L,0);Lua_PushNumber(L,0);return 2;}
static int GetTask(Lua_State* L){return Ret(L,task[Num(L,1)]);}
static int SetTask(Lua_State* L){task[Num(L,1)]=Num(L,2);return 0;}
static int World(Lua_State* L){return Ret(L,map);}
static int Content(Lua_State* L){return Ret(L,map-1000);}
static int Level(Lua_State* L){return Ret(L,40);}
static int Dialog(Lua_State*){++dialogs;return 0;}
static bool Load(KLuaScript& s,const std::string& path){
 FILE* f=fopen(path.c_str(),"rb");if(!f){printf("MISSING %s\n",path.c_str());return false;}std::string raw;char b[4096];int n;while((n=fread(b,1,sizeof(b),f))>0)raw.append(b,n);fclose(f);
 if(!s.Init())return false;
 TLua_Funcs api[]={{"SystemTime",Time},{"GetGlobalValue",State},{"SetGlobalValue",SetState},{"GetMissionV",GetValue},{"SetMissionV",SetValue},{"AddNpc",AddNpc},{"AddMSNpc",Bind},{"SetNpcDeathScript",One},{"SetNpcName",Noop},{"DelNpc",Noop},{"StartMissionTimer",Start},{"StopMissionTimer",Stop},{"RunMission",Run},{"CloseMission",Close},{"GetNextPlayer",Next},{"SetFightState",Noop},{"ShowMissionCountdown",Noop},{"EventVttRefresh",Noop},{"EventServiceOwns",Zero},{"EventServiceValue",Zero},{"Msg2MSAll",Noop},{"GetTask",GetTask},{"SetTask",SetTask},{"GetLevel",Level},{"GetWorldPos",World},{"GetContentWorldPos",Content},{"SubWorldIdx2ContentID",Content},{"CloseDialog",Noop},{"SayTask",Dialog},{"MsgBox",Dialog},{"Say",Dialog}};
 if(!s.RegisterFunctions(api,sizeof(api)/sizeof(api[0]))||Lua_ExecuteBuffer(s.m_LuaState,raw.c_str(),raw.size(),path.c_str()))return false;
 Lua_PushNumber(s.m_LuaState,1);s.SetGlobalName("PlayerIndex");Lua_PushNumber(s.m_LuaState,arena);s.SetGlobalName("SubWorld");return true;
}
#define CHECK(x) do{if(!(x)){printf("FAIL arena=%d line=%d: %s\n",arena,__LINE__,#x);return 1;}}while(0)
int main(int argc,char** argv){
 CHECK(argc==3);std::string staged=argv[1],live=argv[2];
 for(arena=1;arena<=4;++arena){
  memset(values,0,sizeof(values));memset(timers,0,sizeof(timers));spawnCount=normal=guards=bosses=0;failed=3;map=1066+arena;
  char rel[160];KLuaScript m,registration,combat,npc;mission=&m;
  sprintf(rel,"/script/phongthan/van_tien/mission%02d.lua",arena);CHECK(Load(m,staged+rel));
  sprintf(rel,"/script/timertask/task%d.lua",2999+arena*2);CHECK(Load(registration,live+rel));
  sprintf(rel,"/script/timertask/task%d.lua",3000+arena*2);CHECK(Load(combat,live+rel));
  CHECK(m.CallFunction("InitMission",0,""));CHECK(phase[arena]==1&&timers[2999+arena*2]==5400&&normal==247);
  CHECK(registration.CallFunction("OnMissionTimer",0,""));CHECK(phase[arena]==2&&normal==250&&guards==4&&bosses==0&&timers[3000+arena*2]==32400);
  int before=spawnCount;CHECK(m.CallFunction("pt_tick",0,""));CHECK(spawnCount==before);
  for(int i=11;i<=14;++i)values[i]=1;
  CHECK(m.CallFunction("pt_progress",0,""));CHECK(bosses==0);values[34]=1;
  CHECK(m.CallFunction("pt_progress",0,""));CHECK(bosses==1&&values[30]>0);
  CHECK(m.CallFunction("pt_tick",0,""));CHECK(bosses==1&&normal==250);
  CHECK(combat.CallFunction("OnMissionTimer",0,""));CHECK(phase[arena]==0&&!timers[2999+arena*2]&&!timers[3000+arena*2]&&!timers[3009]);
  before=normal;CHECK(m.CallFunction("InitMission",0,""));CHECK(normal-before==250&&phase[arena]==1);
  CHECK(Load(npc,staged+"/script/phongthan/van_tien/thien_hung.lua"));int old=dialogs;
  CHECK(npc.CallFunction("main",0,""));CHECK(dialogs==old+1);
  map=1021;CHECK(npc.CallFunction("main",0,""));CHECK(dialogs==old+2);map=1020;CHECK(npc.CallFunction("main",0,""));CHECK(dialogs==old+2);
  printf("PASS arena=%d: 250 mobs, failed-spawn retry, correct registration/combat timers, 4 guardians, gated single boss, cleanup/new round, Thien Hung dialog/map guard\n",arena);
 }
 return 0;
}
