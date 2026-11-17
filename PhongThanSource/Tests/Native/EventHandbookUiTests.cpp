#include <windows.h>
#include <stdio.h>
#include <assert.h>
#include "KWin32.h"
#include "KStrBase.h"
#include "KLuaScript.h"
static int menus=0,actions=0,lastOp=0,answers=0,lastQuantity=0;
static int Say(Lua_State* L) {
    assert(lua_gettop(L)==5);assert(!strcmp(lua_tostring(L,1),"LINK:\\spr\\pt_test\\book.spr"));
    int count=(int)lua_tonumber(L,4);assert(count>=1 && count<=11);assert(lua_istable(L,5));
    for(int i=1;i<=count;++i){lua_rawgeti(L,5,i);const char* s=lua_tostring(L,-1);assert(s && strlen(s)<512 && strchr(s,'/'));lua_pop(L,1);}
    ++menus;answers=count;return 0;
}
static int Action(Lua_State* L){++actions;lastOp=(int)lua_tonumber(L,1);if(lastOp==4 || lastOp==2)lastQuantity=(int)lua_tonumber(L,2);lua_pushnumber(L,1);return 1;}
static int Input(Lua_State* L){assert(lua_gettop(L)==4);assert(lua_tonumber(L,2)==2);return 0;}
static int GetInput(Lua_State* L){lua_pushstring(L,"37");return 1;}
static int Profession(Lua_State* L){lua_pushnumber(L,0);return 1;}
int main(int argc,char** argv){
    assert(argc==2);FILE* f=fopen(argv[1],"rb");assert(f);fseek(f,0,SEEK_END);int size=ftell(f);rewind(f);
    unsigned char* buffer=new unsigned char[size+1];assert(fread(buffer,1,size,f)==size);fclose(f);buffer[size]=0;
    KLuaScript script;assert(script.Init());TLua_Funcs funcs[]={{"Say",Say},{"EventHandbookAction",Action},{"InputDialog",Input},{"GetInput",GetInput},{"GetPlayerType",Profession}};
    assert(script.RegisterFunctions(funcs,5));assert(script.LoadBuffer(buffer,size));assert(script.ExecuteCode());delete[] buffer;
    assert(script.CallFunction("main",0,"") && menus==1 && answers==10);
    const char* traversal="for id=1,getn(EBNodes) do for page=1,max(1,ceil(getn(EBNodes[id][3])/8)) do EBMenu(id,page) end end";
    assert(lua_dostring(script.m_LuaState,traversal)==0);assert(menus>2000);
    assert(script.CallFunction("EBAction",0,"d",1) && lastOp==1 && actions==1);
    assert(script.CallFunction("EBAction",0,"d",2) && lastOp==2);
    int before=actions;
    assert(script.CallFunction("EBChooseLevel",0,"") && answers==6 && actions==before);
    assert(lua_dostring(script.m_LuaState,"assert(EBNodes[1][3][1][2]=='EBChooseLevel')")==0);
    const int levels[]={50,70,90,150,200};
    for(int j=0;j<5;++j)assert(script.CallFunction("EBReceiveLevel",0,"d",levels[j]) && lastOp==2 && lastQuantity==levels[j]);
    assert(script.CallFunction("EBAction",0,"d",47) && lastOp==47);
    assert(script.CallFunction("EBMoney",0,""));assert(script.CallFunction("EBMoneyDone",0,"d",37) && lastOp==4 && lastQuantity==37);
    printf("PASS handbook actual_Lua4 menus=%d preview_callback_bounds max_skill_action money_input\n",menus);return 0;
}
