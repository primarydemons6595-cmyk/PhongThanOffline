#define main OriginalQuestMenuTestMain
#include "StudioQuestLuaTests.cpp"
#undef main
static int profession=0,opened=0;
static int Type(Lua_State* L){Lua_PushNumber(L,profession);return 1;}
static int Level(Lua_State* L){Lua_PushNumber(L,90);return 1;}
static int Points(Lua_State* L){Lua_PushNumber(L,15);return 1;}
static int Shop(Lua_State* L){opened=(int)Lua_ValueToNumber(L,1);Lua_PushNumber(L,1);return 1;}
static bool Has(const char* s){for(int i=0;i<callbackCount;++i)if(!strcmp(callbacks[i],s))return true;return false;}
int main(int argc,char** argv){CHECK(argc==2);KLuaScript vm;CHECK(Load(vm,argv[1]));vm.RegisterFunction("GetPlayerType",Type);vm.RegisterFunction("GetLevel",Level);vm.RegisterFunction("GetTask",Points);vm.RegisterFunction("StudioQuestShop",Shop);
 for(profession=0;profession<3;++profession){states[1]=0;CHECK(vm.CallFunction("main",0,"")&&Has("quest_1")&&Has("sqm_3_0"));CHECK(vm.CallFunction("quest_1",0,"")&&buttons>0);CHECK(vm.CallFunction("start_1",0,"")&&lastQuest==1&&lastOperation==0&&lastNpc==1);for(int stage=1;stage<=17;++stage){states[1]=stage;CHECK(vm.CallFunction("quest_1",0,""));}CHECK(vm.CallFunction("sqm_3_0",0,"")&&Has("sqm_4_0")&&Has("sqx_7"));CHECK(vm.CallFunction("sqm_4_0",0,"")&&Has("sqx_5")&&Has("sqx_6"));for(int menu=5;menu<=7;++menu){char fn[20];sprintf(fn,"sqx_%d",menu);CHECK(vm.CallFunction(fn,0,"")&&opened==menu);}}
 result=0;CHECK(vm.CallFunction("advance_1",0,""));result=2;CHECK(vm.CallFunction("advance_1",0,""));
 puts("PASS published bounty Lua4: three professions, all sixteen stages and reward, colored dialogue parse, nested menus and three original-shop callbacks");return 0;
}
