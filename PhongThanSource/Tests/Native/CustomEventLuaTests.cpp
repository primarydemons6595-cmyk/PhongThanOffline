#include <windows.h>
#include <stdio.h>
#include <vector>
#include "KWin32.h"
#include "KLuaScript.h"
static int calls=0,lastOp=0,lastNpc=0,lastQuantity=0,phase=0,dialogCalls=0,activeRecipe=1;
static int Custom(Lua_State*L){++calls;lastNpc=(int)Lua_ValueToNumber(L,2);lastOp=(int)Lua_ValueToNumber(L,3);lastQuantity=(int)Lua_ValueToNumber(L,5);if(lastOp==6){Lua_PushNumber(L,phase?activeRecipe:0);Lua_PushNumber(L,phase);Lua_PushNumber(L,10);return 3;}Lua_PushNumber(L,1);return 1;}
static int Consume(Lua_State*L){Lua_PushNumber(L,1);return 1;}
static int Noop(Lua_State*){++dialogCalls;return 0;}
#define CHECK(c) do{if(!(c)){printf("FAIL line %d %s\n",__LINE__,#c);return 1;}++checks;}while(0)
int main(int argc,char**argv){if(argc<2||argc>3)return 2;if(argc==3)activeRecipe=atoi(argv[2]);int checks=0;KLuaScript s;CHECK(s.Init());TLua_Funcs api[]={{"CustomEvent",Custom},{"CustomEventConsume",Consume},{"Talk",Noop},{"CloseDialog",Noop}};CHECK(s.RegisterFunctions(api,4));
 FILE*f=fopen(argv[1],"rb");CHECK(f!=0);fseek(f,0,SEEK_END);int size=ftell(f);rewind(f);std::vector<unsigned char>b(size);CHECK(fread(&b[0],1,size,f)==size);fclose(f);CHECK(s.LoadBuffer(&b[0],size)&&s.ExecuteCode());
 char capture[]="function SayButtons(t,n,...) assert(n==getn(arg)); for i=1,n do local a,b=strfind(arg[i],'/',1,1); assert(a and a==b); local cb=strsub(arg[i],b+1); assert(type(getglobal(cb))=='function'); assert(not strfind(cb,'/',1,1)); end; menu=arg; prompt=t end";CHECK(s.LoadBuffer((unsigned char*)capture,sizeof(capture)-1)&&s.ExecuteCode());CHECK(s.CallFunction("main",0,"")&&lastNpc==0&&lastOp==6);
 char callback[60];CHECK(s.CallFunction("ce_recipes",0,""));char verify[256];sprintf(verify,"local found=0;for i=1,getn(menu) do if strfind(menu[i],'/ce_make%d',1,1) then found=1 end end;assert(found==1)",activeRecipe);CHECK(s.LoadBuffer((unsigned char*)verify,strlen(verify))&&s.ExecuteCode());sprintf(callback,"ce_make%d",activeRecipe);CHECK(s.CallFunction(callback,0,""));sprintf(callback,"ce_start%d",activeRecipe);CHECK(s.CallFunction(callback,0,"")&&lastNpc==0&&lastOp==2);sprintf(callback,"ce_status%d",activeRecipe);CHECK(s.CallFunction(callback,0,"")&&lastOp==5);sprintf(callback,"ce_claim%d",activeRecipe);CHECK(s.CallFunction(callback,0,"")&&lastOp==4);
 sprintf(callback,"ce_start%d_q100",activeRecipe);CHECK(s.CallFunction(callback,0,"")&&lastQuantity==100&&lastOp==2);
 phase=1;int before=dialogCalls;CHECK(s.CallFunction("main",0,"")&&dialogCalls==before+1);
 phase=2;CHECK(s.CallFunction("main",0,""));char ready[160];sprintf(ready,"assert(getn(menu)==1 and strfind(menu[1],'/ce_claim%d',1,1))",activeRecipe);CHECK(s.LoadBuffer((unsigned char*)ready,strlen(ready))&&s.ExecuteCode());
 printf("PASS CUSTOM_EVENT_LUA checks=%d actual_lua4=1\n",checks);return 0;}
