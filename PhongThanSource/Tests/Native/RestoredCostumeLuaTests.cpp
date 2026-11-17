#include "KWin32.h"
#include "LuaLib.h"
#include "KLuaScript.h"
#include <stdio.h>
#include <string.h>
static int sex,templateId,detail,part,valid=1,prompts,notice,morphed,consumed;
static int World(Lua_State* L){Lua_PushNumber(L,1020);Lua_PushNumber(L,0);Lua_PushNumber(L,0);return 3;}
static int Gen(Lua_State* L){Lua_PushNumber(L,1000000000);return 1;}
static int Now(Lua_State* L){Lua_PushNumber(L,1000000010);return 1;}
static int Sex(Lua_State* L){Lua_PushNumber(L,sex);return 1;}
static int Zero(Lua_State* L){Lua_PushNumber(L,0);return 1;}
static int Date(Lua_State* L){Lua_PushNumber(L,2026);Lua_PushNumber(L,11);Lua_PushNumber(L,4);return 3;}
static int Prompt(Lua_State*){++prompts;return 0;}
static int Notice(Lua_State*){++notice;return 0;}
static int Cost(Lua_State*){++consumed;return 0;}
static int Find(Lua_State* L){if((int)Lua_ValueToNumber(L,1)!=8||(int)Lua_ValueToNumber(L,2)!=detail||(int)Lua_ValueToNumber(L,3)!=part)valid=0;Lua_PushNumber(L,valid?123:0);return 1;}
static int Morph(Lua_State* L){morphed=(int)Lua_ValueToNumber(L,1);return 0;}
#define CHECK(c) do{if(!(c)){printf("FAIL line %d %s\n",__LINE__,#c);return 1;}}while(0)
int main(int argc,char**argv){CHECK(argc==6);detail=atoi(argv[2]);part=atoi(argv[3]);sex=atoi(argv[4]);templateId=atoi(argv[5]);FILE*f=fopen(argv[1],"rb");CHECK(f);char code[24000];int n=fread(code,1,sizeof(code),f);fclose(f);KLuaScript s;CHECK(s.Init());
 s.RegisterFunction("GetWorldPos",World);s.RegisterFunction("GetIBItemGenTime",Gen);s.RegisterFunction("SystemTime",Now);s.RegisterFunction("GetSex",Sex);s.RegisterFunction("Time2LocalYMD",Date);s.RegisterFunction("GetYMD",Date);s.RegisterFunction("GetMorphType",Zero);s.RegisterFunction("IsPlayerInsideWeapon",Zero);s.RegisterFunction("MsgBox",Prompt);s.RegisterFunction("Talk",Notice);s.RegisterFunction("Msg2Player",Notice);s.RegisterFunction("SetExeState",Zero);s.RegisterFunction("FindAValidIBItem",Find);s.RegisterFunction("PolyMorph",Morph);s.RegisterFunction("CloseDialog",Zero);s.RegisterFunction("CostIBItem",Cost);
 CHECK(s.LoadBuffer((PBYTE)code,n)&&s.ExecuteCode());CHECK(s.CallFunction("main",0,"d",123));CHECK(prompts==1&&notice==0);CHECK(s.CallFunction("yes2",0,""));CHECK(valid&&morphed==templateId&&consumed==0);CHECK(s.CallFunction("yes1",0,""));CHECK(morphed==-1);sex=1-sex;CHECK(s.CallFunction("main",0,"d",123));CHECK(notice==1&&consumed==0);
 printf("PASS costume detail=%d resource=%d confirm/restore/sex/ownership tuple no premature consumption\n",detail,templateId);return 0;}
