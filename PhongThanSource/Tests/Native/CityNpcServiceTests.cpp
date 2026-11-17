#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <vector>
#include "KWin32.h"
#include "KLuaScript.h"
static int world,kind,closed,sale,compose,answers,pages,gems,awards,spent,room=1;
static int Room(Lua_State* L){Lua_PushNumber(L,room);return 1;}
static int World(Lua_State* L){Lua_PushNumber(L,world);return 1;}
static int Content(Lua_State* L){Lua_PushNumber(L,world-1000);return 1;}
static int Zero(Lua_State* L){Lua_PushNumber(L,0);return 1;}
static int Kind(Lua_State* L){Lua_PushNumber(L,kind);return 1;}
static int Random(Lua_State* L){Lua_PushNumber(L,1);return 1;}
static int Close(Lua_State*){++closed;return 0;}
static int Say(Lua_State* L){answers=(int)Lua_ValueToNumber(L,2);return 0;}
static int Noop(Lua_State*){return 0;}
static int Sale(Lua_State* L){sale=(int)Lua_ValueToNumber(L,1);return 0;}
static int Compose(Lua_State*){++compose;return 0;}
static int Have(Lua_State* L){Lua_PushNumber(L,Lua_ValueToNumber(L,1)==8?pages:gems);return 1;}
static int Find(Lua_State* L){Lua_PushNumber(L,pages?1:0);return 1;}
static int Cost(Lua_State*){if(pages){--pages;++spent;}return 0;}
static int Del(Lua_State*){if(gems){--gems;++spent;}return 0;}
static int Add(Lua_State* L){if(Lua_ValueToNumber(L,1)==7)++awards;Lua_PushNumber(L,1);return 1;}
#define CHECK(x) do{if(!(x)){printf("FAIL line=%d %s\n",__LINE__,#x);return 1;}}while(0)
int main(int argc,char** argv){
    if(argc!=2)return 2;FILE* list=fopen(argv[1],"rb");CHECK(list!=NULL);
    char path[512];int expected,total=0,teachers=0,smiths=0;
    while(fscanf(list,"%511s %d",path,&expected)==2){
        FILE* file=fopen(path,"rb");CHECK(file!=NULL);fseek(file,0,SEEK_END);long size=ftell(file);rewind(file);
        std::vector<unsigned char> bytes(size);CHECK(fread(&bytes[0],1,size,file)==(unsigned)size);fclose(file);
        KLuaScript script;CHECK(script.Init());
        TLua_Funcs api[]={{"GetWorldPos",World},{"GetContentWorldPos",Content},{"SubWorldIdx2ContentID",Content},
            {"GetTask",Zero},{"GetByte",Zero},{"GetBit",Zero},{"GetPlayerType",Kind},{"CloseDialog",Close},
            {"Say",Say},{"SayTask",Noop},{"MsgBox",Noop},{"Talk",Noop},{"TopMessage",Noop},
            {"Sale",Sale},{"EnchaseItem",Compose},{"HaveNormalItem",Have},{"FindAValidIBItem",Find},
            {"CostIBItem",Cost},{"DelNormalItem",Del},{"AddNormalItem",Add},{"random",Random},{"CalcFreeItemCellCount",Room}};
        CHECK(script.RegisterFunctions(api,sizeof(api)/sizeof(api[0])));
        CHECK(script.LoadBuffer(&bytes[0],size));CHECK(script.ExecuteCode());world=expected;
        if(strstr(path,"a1175c2f") || strstr(path,"b081bc9e")){
            ++teachers;CHECK(script.CallFunction("main",0,""));sale=0;CHECK(script.CallFunction("yes_2",0,""));CHECK(sale==14);
            const int counts[3]={13,17,14};
            for(kind=0;kind<3;++kind){answers=0;CHECK(script.CallFunction("books",0,""));CHECK(answers==counts[kind]);}
            pages=gems=awards=spent=0;CHECK(script.CallFunction("JCheck0",0,""));CHECK(!awards && !spent);
            pages=gems=1;room=0;CHECK(script.CallFunction("JCheck0",0,""));CHECK(!awards && !spent && pages==1 && gems==1);room=1;
            pages=gems=1;CHECK(script.CallFunction("JCheck0",0,""));CHECK(awards==1 && spent==2 && !pages && !gems);
            CHECK(script.CallFunction("JCheck0",0,""));CHECK(awards==1 && spent==2);
            world=999999;closed=0;CHECK(script.CallFunction("main",0,""));CHECK(closed==1);
        }
        if(strstr(path,"a0aef599") || strstr(path,"b0192132")){
            ++smiths;answers=0;CHECK(script.CallFunction("main",0,""));CHECK(answers==3);
            sale=0;CHECK(script.CallFunction("yes",0,""));CHECK(sale==12);
            compose=0;CHECK(script.CallFunction("pt_smith_compose",0,""));CHECK(compose==1);
            world=999999;closed=0;CHECK(script.CallFunction("pt_smith_compose",0,""));CHECK(compose==1 && closed==1);
        }
        ++total;
    }
    fclose(list);CHECK(total==7 && teachers==2 && smiths==2);
    puts("PASS CITY_NPC_SERVICES scripts=7 teachers=2 books_shop14=PASS book_menus=3_classes craft_missing_materials=PASS craft_consumption=PASS craft_repeat=BLOCKED smith_shop12=PASS smith_compose=PASS map_guards=PASS");
    puts("SCOPE actual restored Lua with explicit inventory/UI test hosts; live GUI acceptance remains separate.");return 0;
}
