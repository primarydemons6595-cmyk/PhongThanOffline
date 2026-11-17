#define main OriginalQuestMenuTestMain
#include "StudioQuestLuaTests.cpp"
#undef main
static int profession=0,opened=0,exchanged=0;
static int MenuType(Lua_State*L){Lua_PushNumber(L,profession);return 1;}
static int MenuLevel(Lua_State*L){Lua_PushNumber(L,90);return 1;}
static int MenuPoints(Lua_State*L){Lua_PushNumber(L,77);return 1;}
static int MenuShop(Lua_State*L){opened=(int)Lua_ValueToNumber(L,1);Lua_PushNumber(L,1);return 1;}
static int MenuTrade(Lua_State*L){exchanged=(int)Lua_ValueToNumber(L,3);Lua_PushNumber(L,2);Lua_PushString(L,"Trade OK");return 2;}
static bool HasCallback(const char* s){for(int i=0;i<callbackCount;++i)if(!strcmp(callbacks[i],s))return true;return false;}
int main(int argc,char**argv){CHECK(argc==2);KLuaScript vm;CHECK(Load(vm,argv[1]));vm.RegisterFunction("GetPlayerType",MenuType);vm.RegisterFunction("GetTask",MenuPoints);vm.RegisterFunction("GetLevel",MenuLevel);vm.RegisterFunction("StudioQuestShop",MenuShop);vm.RegisterFunction("StudioQuestExchange",MenuTrade);
 CHECK(vm.CallFunction("main",0,"")&&HasCallback("quest_4")&&HasCallback("quest_2")&&HasCallback("sqm_3_0")&&HasCallback("original"));
 CHECK(vm.CallFunction("sqm_3_0",0,"")&&HasCallback("sqx_4")&&!HasCallback("sqx_5")&&!HasCallback("sqx_6"));
 profession=1;CHECK(vm.CallFunction("sqm_3_0",0,"")&&HasCallback("sqx_5")&&!HasCallback("sqx_6"));
 CHECK(vm.CallFunction("sqx_4",0,"")&&HasCallback("sqb_1")&&HasCallback("sqb_2"));CHECK(vm.CallFunction("sqb_1",0,"")&&buttons>0);CHECK(vm.CallFunction("sqc_1",0,"")&&exchanged==1&&talks>0);
 CHECK(vm.CallFunction("sqx_5",0,"")&&opened==5);profession=2;CHECK(vm.CallFunction("sqm_3_0",0,"")&&!HasCallback("sqx_5")&&HasCallback("sqx_6"));
 profession=0;CHECK(vm.CallFunction("sqx_4",0,"")&&HasCallback("sqb_1")&&!HasCallback("sqb_2"));CHECK(vm.CallFunction("original",0,"")&&lastOriginal==1);
 puts("PASS actual generated Lua4: nested NPC menus, three profession filters, deposit/daily links, list confirmation, shop opening and original NPC callback");return 0;
}
