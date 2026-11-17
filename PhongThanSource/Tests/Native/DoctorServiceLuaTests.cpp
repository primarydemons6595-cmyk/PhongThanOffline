#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <vector>
#include "KWin32.h"
#include "KLuaScript.h"
static int healing,mana,task763,shop,options;
static int Noop(Lua_State*){return 0;}
static int Say(Lua_State* L){options=(int)Lua_ValueToNumber(L,2);return 0;}
static int GetTask(Lua_State* L){Lua_PushNumber(L,(int)Lua_ValueToNumber(L,1)==763?task763:0);return 1;}
static int SetTask(Lua_State* L){if((int)Lua_ValueToNumber(L,1)==763)task763=(int)Lua_ValueToNumber(L,2);return 0;}
static int Clock(Lua_State* L){Lua_PushNumber(L,1800000000);return 1;}
static int Heal(Lua_State*){++healing;return 0;}static int Mana(Lua_State*){++mana;return 0;}
static int Sale(Lua_State* L){shop=(int)Lua_ValueToNumber(L,1);return 0;}
#define CHECK(x) do{if(!(x)){printf("FAIL line%d file%d %s\n",__LINE__,count,#x);return 1;}}while(0)
int main(int argc,char** argv){if(argc!=2)return 2;FILE* list=fopen(argv[1],"rb");if(!list)return 3;char line[1200];int count=0;
while(fgets(line,sizeof(line),list)){char path[1024];int expected;if(sscanf(line,"%1023s\t%d",path,&expected)!=2)return 4;FILE* f=fopen(path,"rb");if(!f)return 5;fseek(f,0,SEEK_END);int len=ftell(f);rewind(f);std::vector<char> bytes(len+1);fread(&bytes[0],1,len,f);fclose(f);bytes[len]=0;char* marker=strstr(&bytes[0],"-- PT_DOCTOR_SERVICES_20261102");if(!marker)return 6;
 KLuaScript s;CHECK(s.Init());TLua_Funcs api[]={{"Say",Say},{"GetTask",GetTask},{"SetTask",SetTask},{"SystemTime",Clock},{"RestoreLife",Heal},{"RestoreMana",Mana},{"Sale",Sale},{"CloseDialog",Noop},{"Talk",Noop},{"Msg2Player",Noop}};
 CHECK(s.RegisterFunctions(api,sizeof(api)/sizeof(api[0])));CHECK(s.LoadBuffer((unsigned char*)&bytes[0],marker-&bytes[0])&&s.ExecuteCode());Lua_GetGlobal(s.m_LuaState,"main");s.SetGlobalName("expected_original_main");
 CHECK(s.LoadBuffer((unsigned char*)marker,len-(marker-&bytes[0]))&&s.ExecuteCode());Lua_GetGlobal(s.m_LuaState,"expected_original_main");Lua_GetGlobal(s.m_LuaState,"pt_doctor_quest_main");CHECK(Lua_IsEqual(s.m_LuaState,-1,-2));Lua_Pop(s.m_LuaState,2);
 healing=mana=task763=shop=options=0;CHECK(s.CallFunction("main",0,"")&&options==4);CHECK(s.CallFunction("pt_doctor_shop",0,"")&&shop==expected);CHECK(s.CallFunction("pt_doctor_heal",0,"")&&healing==1&&mana==1&&task763==1800000000);CHECK(s.CallFunction("pt_doctor_heal",0,"")&&healing==1&&mana==1);++count;
}fclose(list);printf("PASS %d actual Lua4 doctor scripts: shop, healing, original cooldown and preserved quest function identity\n",count);return 0;}
