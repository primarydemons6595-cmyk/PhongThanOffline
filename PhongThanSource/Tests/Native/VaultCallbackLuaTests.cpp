#include "KWin32.h"
#include "LuaLib.h"
#include "KLuaScript.h"
#include <stdio.h>
#include <string.h>
static int rawPage,lastPage,notices,confirmations,paid,result=1;
static int SetTask(Lua_State* L){const char* s=Lua_ValueToString(L,2);rawPage=s?s[0]:0;return 0;}
static int GetTask(Lua_State* L){Lua_PushNumber(L,rawPage);return 1;}
static int BankInfo(Lua_State* L){lastPage=(int)Lua_ValueToNumber(L,1);Lua_PushNumber(L,lastPage>=2&&lastPage<=5?63:0);Lua_PushNumber(L,lastPage>=2&&lastPage<=5?45:0);return 2;}
static int Buy(Lua_State* L){lastPage=(int)Lua_ValueToNumber(L,1);++paid;Lua_PushNumber(L,result);return 1;}
static int Notice(Lua_State*){++notices;return 0;}
static int Confirm(Lua_State*){++confirmations;return 0;}
static int Noop(Lua_State*){return 0;}
#define CHECK(c) do{if(!(c)){printf("FAIL line %d %s\n",__LINE__,#c);return 1;}}while(0)
int main(int argc,char**argv){CHECK(argc==2);FILE* f=fopen(argv[1],"rb");CHECK(f);char code[24000];int n=fread(code,1,sizeof(code),f);fclose(f);
 KLuaScript s;CHECK(s.Init());s.RegisterFunction("SetTask",SetTask);s.RegisterFunction("GetTask",GetTask);s.RegisterFunction("BankInfo",BankInfo);s.RegisterFunction("BankPurchase",Buy);s.RegisterFunction("Talk",Notice);s.RegisterFunction("SayButtons",Noop);s.RegisterFunction("CloseDialog",Noop);s.RegisterFunction("MsgBox",Confirm);
 CHECK(s.LoadBuffer((PBYTE)code,n)&&s.ExecuteCode());
 for(int page=2;page<=5;++page){CHECK(s.CallFunction("main",0,"d",page));CHECK(rawPage==page+'0');CHECK(s.CallFunction("buy",0,""));CHECK(lastPage==page);CHECK(s.CallFunction("rent",0,""));CHECK(lastPage==page);CHECK(s.CallFunction("confirm_buy",0,""));CHECK(lastPage==page);CHECK(s.CallFunction("confirm_rent",0,""));CHECK(lastPage==page);}
 CHECK(confirmations==8&&paid==8&&notices==8);result=0;CHECK(s.CallFunction("confirm_rent",0,""));CHECK(notices==9);rawPage=0;CHECK(s.CallFunction("buy",0,""));CHECK(notices==10);
 puts("PASS real Lua VM bank callbacks pages2-5: ASCII SetTask roundtrip, both confirmations, purchase and success/failure notices");return 0;}
