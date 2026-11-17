#include <windows.h>
#include <stdio.h>
#include <vector>
#include "KWin32.h"
#include "KLuaScript.h"
static int menu,shop,closed,opened;
static int World(Lua_State* L){Lua_PushNumber(L,1002);return 1;}
static int Content(Lua_State* L){Lua_PushNumber(L,2);return 1;}
static int Say(Lua_State* L){menu=(int)Lua_ValueToNumber(L,2);return 0;}
static int Sale(Lua_State* L){shop=(int)Lua_ValueToNumber(L,1);return 0;}
static int Close(Lua_State*){++closed;return 0;}
static int Open(Lua_State* L){++opened;Lua_PushNumber(L,1);return 1;}
#define CHECK(x) do{if(!(x)){printf("FAIL %d\n",__LINE__);return 1;}}while(0)
int main(int argc,char**argv){if(argc!=2)return 2;FILE*f=fopen(argv[1],"rb");CHECK(f);fseek(f,0,SEEK_END);int size=ftell(f);rewind(f);std::vector<unsigned char> bytes(size);CHECK(fread(&bytes[0],1,size,f)==(unsigned)size);fclose(f);
 KLuaScript s;CHECK(s.Init());TLua_Funcs funcs[]={{"GetWorldPos",World},{"GetContentWorldPos",Content},{"SubWorldIdx2ContentID",Content},{"SayButtons",Say},{"Sale",Sale},{"CloseDialog",Close},{"SmithPropertyOpen",Open}};
 CHECK(s.RegisterFunctions(funcs,7));CHECK(s.LoadBuffer(&bytes[0],size));CHECK(s.ExecuteCode());
 CHECK(s.CallFunction("main",0,"")&&menu==3);CHECK(s.CallFunction("pt_smith_trade",0,"")&&shop==2);
 CHECK(s.CallFunction("pt_smith_property_info",0,"")&&menu==2);
 CHECK(s.CallFunction("pt_smith_property_open",0,"")&&opened==1&&closed==2);
 puts("PASS Sung Thanh Doanh smith: native property menu and original shop2 preserved");return 0;}
