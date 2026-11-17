#include "KWin32.h"
#include "LuaLib.h"
#include "KLuaScript.h"
#include <stdio.h>
#include <string.h>
static int states[65],result=1,menus,talks,buttons,lastQuest,lastOperation,lastNpc,lastOriginal;
static char callbacks[12][64];static int callbackCount;
static int Status(Lua_State* L){int id=(int)Lua_ValueToNumber(L,1);Lua_PushNumber(L,id>0&&id<=64?states[id]:-1);Lua_PushNumber(L,1);Lua_PushString(L,"Quest detail");return 3;}
static int Act(Lua_State* L){lastQuest=(int)Lua_ValueToNumber(L,1);lastOperation=(int)Lua_ValueToNumber(L,2);lastNpc=(int)Lua_ValueToNumber(L,3);if(result&&lastOperation==0)states[lastQuest]=1;Lua_PushNumber(L,result);Lua_PushString(L,"Missing material");return 2;}
static int Original(Lua_State* L){lastOriginal=(int)Lua_ValueToNumber(L,1);return 0;}
static int CanGather(Lua_State* L){Lua_PushNumber(L,states[(int)Lua_ValueToNumber(L,1)]==1?1:0);return 1;}
static int Gather(Lua_State* L){lastQuest=(int)Lua_ValueToNumber(L,1);lastNpc=(int)Lua_ValueToNumber(L,2);lastOperation=(int)Lua_ValueToNumber(L,3);Lua_PushNumber(L,result);Lua_PushString(L,"Gather result");return 2;}
static int SayTask(Lua_State* L){++menus;callbackCount=Lua_GetN(L,2);if(callbackCount>11)return lua_error(L,"Too many menu options"),0;for(int i=1;i<=callbackCount;i++){Lua_RawGetI(L,2,i);Lua_RawGetI(L,-1,2);const char* cb=Lua_ValueToString(L,-1);if(!cb)return lua_error(L,"Missing callback"),0;strncpy(callbacks[i-1],cb,63);Lua_Pop(L,2);}return 0;}
static int SayButtons(Lua_State* L){++buttons;int n=(int)Lua_ValueToNumber(L,2);if(n<1||n>10||Lua_GetTopIndex(L)!=n+2)return lua_error(L,"Invalid counted buttons"),0;return 0;}
static int Talk(Lua_State* L){++talks;if(!Lua_IsString(L,3))return lua_error(L,"Missing dialogue content"),0;return 0;}
static int Close(Lua_State*){return 0;}
static bool Load(KLuaScript& vm,const char* path){FILE* f=fopen(path,"rb");if(!f)return false;char source[100000];int size=fread(source,1,sizeof(source),f);fclose(f);if(!vm.Init())return false;vm.RegisterFunction("StudioQuestStatus",Status);vm.RegisterFunction("StudioQuestAction",Act);vm.RegisterFunction("StudioQuestOriginal",Original);vm.RegisterFunction("StudioQuestCanGather",CanGather);vm.RegisterFunction("StudioQuestGather",Gather);vm.RegisterFunction("SayTask",SayTask);vm.RegisterFunction("SayButtons",SayButtons);vm.RegisterFunction("Talk",Talk);vm.RegisterFunction("CloseDialog",Close);return vm.LoadBuffer((PBYTE)source,size)&&vm.ExecuteCode();}
#define CHECK(c) do{if(!(c)){printf("FAIL quest Lua line=%d %s\n",__LINE__,#c);return 1;}}while(0)
int main(int argc,char**argv){CHECK(argc==2);char path[1024];states[3]=-1;KLuaScript a;sprintf(path,"%s/npc_1.lua",argv[1]);CHECK(Load(a,path));CHECK(a.CallFunction("main",0,""));CHECK(menus==1&&callbackCount<=9);for(int i=0;i<callbackCount;i++)CHECK(strcmp(callbacks[i],"quest_3"));CHECK(a.CallFunction("start_1",0,"")&&lastQuest==1&&lastOperation==0&&lastNpc==1&&buttons>0);CHECK(a.CallFunction("original",0,"")&&lastOriginal==1);
 result=0;int old=talks;CHECK(a.CallFunction("advance_1",0,"")&&talks==old+1);result=2;old=talks;CHECK(a.CallFunction("advance_1",0,"")&&talks==old+1);
 KLuaScript b;sprintf(path,"%s/npc_2.lua",argv[1]);CHECK(Load(b,path));states[1]=0;CHECK(b.CallFunction("main",0,""));for(i=0;i<callbackCount;i++)CHECK(strcmp(callbacks[i],"quest_1"));
 KLuaScript empty;sprintf(path,"%s/npc_3.lua",argv[1]);CHECK(Load(empty,path));old=talks;CHECK(empty.CallFunction("main",0,"")&&talks==old+1);
 KLuaScript pages;sprintf(path,"%s/npc_pages.lua",argv[1]);CHECK(Load(pages,path));CHECK(pages.CallFunction("main",0,"")&&callbackCount<=9);CHECK(pages.CallFunction("menu_1",0,"")&&callbackCount<=9);
 KLuaScript node;sprintf(path,"%s/node_1.lua",argv[1]);CHECK(Load(node,path));states[8]=0;old=talks;CHECK(node.CallFunction("main",0,"")&&talks==old+1);states[8]=1;CHECK(node.CallFunction("main",0,"")&&callbackCount==1&&!strcmp(callbacks[0],"start_8"));CHECK(node.CallFunction("start_8",0,""));result=1;CHECK(node.CallFunction("begin_8",0,"")&&lastQuest==8&&lastNpc==1&&lastOperation==0);CHECK(node.CallFunction("gather_finish",0,"")&&lastQuest==0&&lastNpc==1&&lastOperation==1);
 result=1;CHECK(a.CallFunction("start_9",0,"")&&lastQuest==9&&lastOperation==0);CHECK(a.CallFunction("renew_9",0,"")&&lastQuest==9&&lastOperation==2&&lastNpc==1);
 puts("PASS generated NPC scripts in actual Lua VM: profession/eligibility filtering, correct start NPC, original functions, counted buttons, failure/success content, empty NPC and paginated menus");return 0;}
