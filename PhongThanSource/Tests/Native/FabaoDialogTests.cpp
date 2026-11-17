#include <windows.h>
#include <stdio.h>
#include <assert.h>
#include "KWin32.h"
#include "KStrBase.h"
#include "KLuaScript.h"
static int buttons=0,mode=0,operation=-1,pages=0,world=1052;
static int Buttons(Lua_State* L){buttons=(int)lua_tonumber(L,2);assert(buttons>=1&&buttons<=10&&lua_gettop(L)==buttons+2);for(int i=3;i<=buttons+2;++i)assert(strchr(lua_tostring(L,i),'/'));return 0;}
static int Say(Lua_State* L){int n=(int)lua_tonumber(L,2);assert(n>=1&&n<=9&&lua_istable(L,3));return 0;}
static int Talk(Lua_State* L){pages=(int)lua_tonumber(L,1);assert(pages>=1&&pages<=4&&lua_gettop(L)==pages+2);for(int i=3;i<=pages+2;++i)assert(strlen(lua_tostring(L,i))<600);return 0;}
static int Close(Lua_State*){return 0;}
static int World(Lua_State* L){lua_pushnumber(L,world);return 1;}
static int Panel(Lua_State* L){mode=(int)lua_tonumber(L,1);return 0;}
static int Service(Lua_State* L){operation=(int)lua_tonumber(L,1);lua_pushnumber(L,1);return 1;}
static void Load(KLuaScript& s,const char* path){FILE* f=fopen(path,"rb");assert(f);fseek(f,0,SEEK_END);int bytes=ftell(f);rewind(f);char* b=new char[bytes];assert(fread(b,1,bytes,f)==bytes);fclose(f);assert(s.Init());TLua_Funcs funcs[]={{"SayButtons",Buttons},{"Say",Say},{"Talk",Talk},{"CloseDialog",Close},{"GetWorldPos",World},{"EnchaseItem",Panel},{"XichTungTuService",Service},{"Include",Close}};assert(s.RegisterFunctions(funcs,8)&&s.LoadBuffer((BYTE*)b,bytes)&&s.ExecuteCode());delete[] b;}
int main(int argc,char**argv){assert(argc==3);KLuaScript xich,tao;Load(xich,argv[1]);assert(xich.CallFunction("main",0,"")&&buttons==6);assert(xich.CallFunction("pt_fabao_menu",0,"")&&buttons==3);assert(xich.CallFunction("dz",0,"")&&operation==0);for(int i=1;i<=48;++i)assert(xich.CallFunction("pt_fabao_formula",0,"dd",i,1));assert(xich.CallFunction("pt_fabao_book",0,"d",8));
 world=1073;Load(tao,argv[2]);assert(tao.CallFunction("main",0,"")&&buttons==3);assert(tao.CallFunction("pt_tao_fabao",0,"")&&mode==7);for(i=1;i<=66;++i)assert(tao.CallFunction("pt_fabao_formula",0,"dd",i,1));assert(tao.CallFunction("pt_fabao_book",0,"d",7));world=1003;mode=0;assert(tao.CallFunction("pt_tao_fabao",0,"")&&mode==0);puts("PASS actual Lua4: Xich root six buttons, old native service, Tao scoped mode7, 114 formula pages, book navigation and wrong-map guard");return 0;}
