#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <vector>
#include "KWin32.h"
#include "KLuaScript.h"
#include "KStrBase.h"
#include "PhongThanNpcDialogRules.h"
#define MAX_ANSWERNUM 20
#define MAX_SCIRPTACTION_BUFFERNUM 4096
static int task[5000],level=40,world=1021,closed,exp,cash,count,layout;
static char offered[20][160];
static int GetPlayerIndex(Lua_State*){return 1;}
static int LuaSelectUIWithPresentation(Lua_State* L,int mode){
    layout=mode;count=(int)Lua_ValueToNumber(L,2);
    for(int i=0;i<count;++i)g_StrCpyLen(offered[i],Lua_ValueToString(L,i+3),160);return 0;
}
#include "../../Sources/Core/Src/PhongThanNpcDialogLua.inl"
static int GetTask(Lua_State* L){Lua_PushNumber(L,task[(int)Lua_ValueToNumber(L,1)]);return 1;}
static int SetTask(Lua_State* L){task[(int)Lua_ValueToNumber(L,1)]=(int)Lua_ValueToNumber(L,2);return 0;}
static int Level(Lua_State* L){Lua_PushNumber(L,level);return 1;}
static int World(Lua_State* L){Lua_PushNumber(L,world);return 1;}
static int Content(Lua_State* L){Lua_PushNumber(L,world-1000);return 1;}
static int Close(Lua_State*){++closed;return 0;}
static int Noop(Lua_State*){return 0;}
static int Exp(Lua_State* L){exp+=(int)Lua_ValueToNumber(L,1);return 0;}
static int Earn(Lua_State* L){cash+=(int)Lua_ValueToNumber(L,1);return 0;}
#define CHECK(x) do{if(!(x)){printf("FAIL line=%d %s\n",__LINE__,#x);return 1;}}while(0)
int main(int argc,char** argv){
    CHECK(argc==3);
    for(int which=0;which<2;++which){
        FILE* file=fopen(argv[which+1],"rb");CHECK(file);fseek(file,0,SEEK_END);long size=ftell(file);rewind(file);
        std::vector<unsigned char> bytes(size);CHECK(fread(&bytes[0],1,size,file)==(unsigned)size);fclose(file);
        KLuaScript script;CHECK(script.Init());
        TLua_Funcs api[]={{"GetTask",GetTask},{"SetTask",SetTask},{"GetLevel",Level},{"GetWorldPos",World},
            {"GetContentWorldPos",Content},{"SubWorldIdx2ContentID",Content},{"CloseDialog",Close},
            {"SayTask",LuaSayTaskCompat},{"MsgBox",LuaMsgBoxCompat},{"Talk",Noop},{"Msg2Player",Noop},{"AddOwnExp",Exp},{"Earn",Earn}};
        CHECK(script.RegisterFunctions(api,sizeof(api)/sizeof(api[0])));
        CHECK(script.LoadBuffer(&bytes[0],size));CHECK(script.ExecuteCode());
        memset(task,0,sizeof(task));level=40;world=1021;count=0;
        CHECK(script.CallFunction("main",0,""));CHECK(PhongThanDialogUsesButtons(layout,0));
        if(which==0){
            CHECK(count==1 && strstr(offered[0],"/renwu1"));
            CHECK(script.CallFunction("renwu1",0,""));CHECK(count==2 && strstr(offered[0],"/yes_1"));
            CHECK(script.CallFunction("yes_1",0,""));CHECK(task[408]==1 && !exp && !cash);
            task[408]=100000;CHECK(script.CallFunction("renwu1",0,""));CHECK(task[405]==1 && task[406]==1 && task[408]==0 && exp==5000);
            CHECK(script.CallFunction("renwu1",0,""));CHECK(exp==5000);
            CHECK(script.CallFunction("main",0,""));CHECK(count==2 && strstr(offered[0],"/renwu2") && strstr(offered[1],"/chaxun"));
            task[408]=100000;CHECK(script.CallFunction("reward_normal",0,""));CHECK(exp==37000 && cash==5000 && task[405]==2 && task[408]==0);
            CHECK(script.CallFunction("reward_normal",0,""));CHECK(exp==37000 && cash==5000);
        }else{
            CHECK(count==4 && strstr(offered[0],"/yes_2") && strstr(offered[1],"/yes_1") && strstr(offered[2],"/zhiren") && strstr(offered[3],"/muren"));
            level=20;CHECK(script.CallFunction("main",0,""));CHECK(count==3 && strstr(offered[0],"/yes_1"));
        }
        world=999999;closed=0;CHECK(script.CallFunction("main",0,""));CHECK(closed==1);
    }
    puts("PASS REQUESTED_NPC_SERVICES: actual Thien Hung Lua initial/repeat quest rewards, astrologer level menu/callback order, map guards. State APIs are test hosts; live acceptance separate.");
    return 0;
}
