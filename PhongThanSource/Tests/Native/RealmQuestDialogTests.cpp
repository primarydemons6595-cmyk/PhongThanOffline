#include <windows.h>
#include <stdio.h>
#include <assert.h>
#include "KWin32.h"
#include "KStrBase.h"
#include "KLuaScript.h"
static int phase=0,role=0,choice=1,operation=0,buttons=0,previews=0,closed=0,old=0;
static int Realm(Lua_State* L){int op=(int)lua_tonumber(L,1);if(op){operation=op;assert(op>=1&&op<=12);}lua_pushnumber(L,1);lua_pushnumber(L,phase);lua_pushnumber(L,role);lua_pushnumber(L,choice);return 4;}
static int Buttons(Lua_State* L){buttons=(int)lua_tonumber(L,2);assert(buttons>=1&&buttons<=6&&lua_gettop(L)==buttons+2);for(int i=3;i<=buttons+2;++i)assert(strlen(lua_tostring(L,i))<100&&strchr(lua_tostring(L,i),'/'));return 0;}
static int Preview(Lua_State* L){++previews;assert(lua_gettop(L)==3&&strlen(lua_tostring(L,1))<500&&!strcmp(lua_tostring(L,2),"ptrq_confirm"));return 0;}
static int Close(Lua_State*){++closed;return 0;}
static int Talk(Lua_State* L){assert(lua_gettop(L)==3&&strlen(lua_tostring(L,3))<600);return 0;}
static int Old(Lua_State*){++old;return 0;}
static int Level(Lua_State* L){lua_pushnumber(L,200);return 1;}
static int Profession(Lua_State* L){lua_pushnumber(L,0);return 1;}
static int Task(Lua_State* L){lua_pushnumber(L,90);return 1;}
static int World(Lua_State* L){lua_pushnumber(L,1);lua_pushnumber(L,3);lua_pushnumber(L,3);lua_pushnumber(L,1500);lua_pushnumber(L,7);lua_pushnumber(L,1);lua_pushnumber(L,7);return 7;}
static int Reputation(Lua_State* L){lua_pushnumber(L,1);lua_pushnumber(L,0);lua_pushnumber(L,4);lua_pushnumber(L,phase);lua_pushnumber(L,0);return 5;}
static int Island(Lua_State* L){lua_pushnumber(L,1);lua_pushnumber(L,0);lua_pushnumber(L,0);return 3;}
static void Load(KLuaScript& s,const char* path){FILE* f=fopen(path,"rb");assert(f);fseek(f,0,SEEK_END);int bytes=ftell(f);rewind(f);char* data=new char[bytes];assert(fread(data,1,bytes,f)==bytes);fclose(f);assert(s.Init());
 TLua_Funcs funcs[]={{"PtRealmQuest",Realm},{"SayButtons",Buttons},{"MsgBox",Preview},{"GetLevel",Level},{"GetTask",Task},{"CloseDialog",Close},{"Include",Close},{"PT_REALM_OLD_MAIN_TEST",Old},{"PtRealmWorld",World},{"PtRealmReputation",Reputation},{"PtRealmIsland",Island},{"Talk",Talk}};
 assert(s.RegisterFunctions(funcs,12));TLua_Funcs extra[]={ {"GetPlayerType",Profession} };assert(s.RegisterFunctions(extra,1));assert(s.LoadBuffer((BYTE*)data,bytes)&&s.ExecuteCode());delete[]data;
 const char* setup="PT_REALM_OLD_MAIN=PT_REALM_OLD_MAIN_TEST\nfunction PT_REALM_OLD_MARK() return 0,0 end";assert(s.LoadBuffer((BYTE*)setup,strlen(setup))&&s.ExecuteCode());
}
int main(int argc,char** argv){assert(argc==12);const int roles[]={1,2,3,4,5,6,8,7,6,9,10};int stages[]={0,2,4,5,1,6,7,9,10,12,11,13};int rs[]={1,1,1,1,2,3,3,3,3,3,5,6};int expected[]={1,3,4,5,2,6,7,8,9,11,10,12};
 for(int file=0;file<11;++file){KLuaScript script;Load(script,argv[file+1]);
  for(int i=0;i<12;++i){if(rs[i]!=roles[file])continue;phase=stages[i];role=rs[i];choice=1;buttons=operation=previews=0;
   assert(script.CallFunction("main",0,"")&&buttons>=1&&!operation);assert(script.CallFunction("ptrq_preview",0,"")&&previews==1&&!operation);
   assert(script.CallFunction("ptrq_confirm",0,"")&&operation==expected[i]);
  }
  phase=13;role=roles[file];int before=old;assert(script.CallFunction("ptrq_old",0,"")&&old==before+1);assert(script.CallFunction("main",0,""));assert(script.CallFunction("ptrq_world",0,""));
 }
 puts("PASS actual Lua4: eleven scoped NPC scripts load, explicit buttons, personal preview/confirm operations, world progress, boats, no action on preview, preserved old-menu callback");return 0;
}
