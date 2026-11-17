#include <windows.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "KWin32.h"
#include "KStrBase.h"
#include "KLuaScript.h"
static int tasks[5000],type,level,map,money,xp,credit,camp,transactions,grants,failTransaction,notes,buttons;
static int N(Lua_State* L,int i){return (int)lua_tonumber(L,i);}
static int R(Lua_State* L,int n){lua_pushnumber(L,n);return 1;}
static int Task(Lua_State* L){return R(L,tasks[N(L,1)]);}
static int SetTask(Lua_State* L){tasks[N(L,1)]=N(L,2);return 0;}
static int Level(Lua_State* L){return R(L,level);}
static int Type(Lua_State* L){return R(L,type);}
static int Map(Lua_State* L){return R(L,map-1000);}
static int Cash(Lua_State* L){return R(L,money);}
static int Earn(Lua_State* L){money+=N(L,1);return R(L,1);}
static int Pay(Lua_State* L){money-=N(L,1);return R(L,1);}
static int Exp(Lua_State* L){xp+=N(L,1);return 0;}
static int Credit(Lua_State* L){credit+=N(L,1);return 0;}
static int Camp(Lua_State* L){camp=N(L,1);return 0;}
static int Have(Lua_State* L){return R(L,100);}
static int EventHave(Lua_State* L){return R(L,N(L,1)==9?3:100);}
static int Zero(Lua_State* L){return R(L,0);}
static int Stub(Lua_State*){return 0;}
static int Random(Lua_State* L){return R(L,N(L,1));}
static int Clock(Lua_State* L){return R(L,1793070000);}
static int Note(Lua_State* L){int id=N(L,1);assert(id>=3040&&id<=3053);const char* text=lua_tostring(L,3);assert(text&&strlen(text)<=480);++notes;return 0;}
static int Buttons(Lua_State* L){++buttons;int count=N(L,2);assert(count>=1&&count<=6);assert(lua_gettop(L)==count+2||(lua_gettop(L)==3&&lua_istable(L,3)&&lua_getn(L,3)==count));return 0;}
static int Reset(Lua_State* L){ZeroMemory(tasks,sizeof(tasks));type=N(L,1);int task=N(L,2);tasks[task]=N(L,3);level=N(L,4);map=N(L,5);if(N(L,6))tasks[type==1?1:2]=N(L,6);money=1000;xp=credit=camp=transactions=grants=failTransaction=notes=buttons=0;return 0;}
static int Exchange(Lua_State* L){if(failTransaction||tasks[N(L,1)]!=N(L,2))return R(L,0);assert(lua_istable(L,4)&&lua_istable(L,5));tasks[N(L,1)]=N(L,3);++transactions;grants+=lua_getn(L,5);return R(L,1);}
static int Failure(Lua_State*){failTransaction=1;return 0;}
static int Check(Lua_State* L){int expected=N(L,1);assert(tasks[N(L,2)]==expected);assert(transactions==N(L,3)&&xp==N(L,4)&&money==N(L,5));return 0;}
static const char* journey=
"function PT2_TEST_ALL()\n"
" for f=1,2 do for task=1,4772 do if task==1 or task==2 or task==10 or task==11 or task==14 or task==15 or task==30 or task==31 or task==34 or task==35 or task==4550 or task==4770 then for state=0,100 do TestReset(f,task,state,200,1003,0);PT2Journal() end end end end\n"
" for i=1,getn(PT2_STEPS) do local a=PT2_STEPS[i]\n"
"  PT2_ROLE=a.role;PT2_MAP=1003\n"
"  TestReset(a.f,a.task,a.from,a.level,1003,a.mainPhase or 0)\n"
"  assert(PT2Ready(a));PT2Menu();PT2Preview(i);TestCheck(a.from,a.task,0,0,1000)\n"
"  TestFailure();PT2Apply(i);TestCheck(a.from,a.task,0,0,1000)\n"
"  TestReset(a.f,a.task,a.from,a.level,1003,a.mainPhase or 0);PT2Apply(i)\n"
"  TestCheck(a.to,a.task,1,a.xp,1000+a.money);PT2Apply(i);TestCheck(a.to,a.task,1,a.xp,1000+a.money)\n"
"  TestReset(a.f==1 and 2 or 1,a.task,a.from,a.level,1003,a.mainPhase or 0);assert(not PT2Ready(a));PT2Apply(i);TestCheck(a.from,a.task,0,0,1000)\n"
"  TestReset(0,a.task,a.from,a.level,1003,a.mainPhase or 0);assert(not PT2Ready(a));PT2Apply(i);TestCheck(a.from,a.task,0,0,1000)\n"
"  TestReset(a.f,a.task,a.from,a.level-1,1003,a.mainPhase or 0);assert(not PT2Ready(a))\n"
"  TestReset(a.f,a.task,a.from,a.level,1004,a.mainPhase or 0);assert(not PT2Ready(a))\n"
" end\n"
" for f=1,2 do local role=f==1 and 'giao_dao' or 'giao_di';local world=f==1 and 1003 or 1004\n"
"  PT2_ROLE=role;PT2_MAP=world;TestReset(f,4554,0,20,world,0)\n"
"  for run=1,3 do assert(PT2GiaoReady()==1);pt2_giao_take();assert(GetTask(4554)>0 and GetTask(4557)==run);SetTask(4555,40);pt2_giao_claim();TestCheck(0,4554,run*2,run*4600,1000+run*3000) end\n"
"  assert(PT2GiaoReady()==0);pt2_giao_take();TestCheck(0,4554,6,13800,10000)\n"
"  TestReset(f,4554,0,29,world,0);pt2_giao_take();assert(GetTask(4558)==1);SetTask(4555,40);pt2_giao_claim();assert(PT2GiaoReady()==0)\n"
"  PT2_ROLE=f==1 and 'giao_di' or 'giao_dao';TestReset(f,4554,0,20,world,0);pt2_giao_take();TestCheck(0,4554,0,0,1000)\n"
" end\n"
" TestReset(1,1,0,200,1003,0);local label=PT2_REPLACED_TITLES[1][1];local choices={{label,'yes';show=1},{'Transport','warp';show=1}};SayTask('test',choices);assert(choices[1].show==0 and choices[2].show==1)\n"
" TestReset(0,1,0,200,1003,0);choices={{label,'yes';show=1},{'Transport','warp';show=1}};SayTask('test',choices);assert(choices[1].show==1 and choices[2].show==1)\n"
"end\n";
int main(int argc,char** argv){assert(argc>1);SetErrorMode(SEM_FAILCRITICALERRORS|SEM_NOGPFAULTERRORBOX);
 TLua_Funcs api[]={{"GetTask",Task},{"SetTask",SetTask},{"GetLevel",Level},{"GetPlayerType",Type},{"GetContentWorldPos",Map},{"GetCash",Cash},{"Earn",Earn},{"Pay",Pay},{"AddOwnExp",Exp},{"AddCredit",Credit},{"SetCamp",Camp},{"HaveNormalItem",Have},{"HaveEventItem",EventHave},{"GetTrade",Zero},{"GetLockState",Zero},{"Include",Stub},{"CloseDialog",Stub},{"AddNote",Note},{"SayButtons",Buttons},{"RANDOM",Random},{"NewWorld",Stub},{"QuestExchange",Exchange},{"TestReset",Reset},{"TestFailure",Failure},{"TestCheck",Check}};
 for(int i=1;i<argc;++i){FILE* f=fopen(argv[i],"rb");assert(f);fseek(f,0,SEEK_END);int bytes=ftell(f);rewind(f);char* data=new char[bytes];assert(fread(data,1,bytes,f)==bytes);fclose(f);KLuaScript script;assert(script.Init()&&script.RegisterFunctions(api,sizeof(api)/sizeof(api[0])));TLua_Funcs extra[]={{"SystemTime",Clock},{"SayTask",Stub},{"PtTwoFactionNote",Note}};assert(script.RegisterFunctions(extra,3));assert(script.LoadBuffer((BYTE*)data,bytes)&&script.ExecuteCode());delete[]data;
  if(i==1){assert(script.LoadBuffer((BYTE*)journey,strlen(journey))&&script.ExecuteCode()&&script.CallFunction("PT2_TEST_ALL",0,""));}
 }
 printf("PASS Lua4 two-faction branches: %d actual NPC scripts load; every staged transition checks class/level/map, preview, transaction rollback, exact XP/coin reward and duplicate claim\n",argc-1);return 0;
}
