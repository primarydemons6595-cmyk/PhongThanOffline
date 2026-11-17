#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include "KWin32.h"
#include "KLuaScript.h"
#include "../../Headers/PhongThanMapIdentity.h"
static int world, dispatched, closed;
static int World(Lua_State* L) {Lua_PushNumber(L,world);Lua_PushNumber(L,10);Lua_PushNumber(L,20);return 3;}
static int Content(Lua_State* L) {
    PHONGTHAN_MAP_RESOLUTION r;
    Lua_PushNumber(L,PhongThanResolveRuntimeMap(world,&r)?r.ContentMapId:world);
    Lua_PushNumber(L,10);Lua_PushNumber(L,20);return 3;
}
static int Close(Lua_State*) {++closed;return 0;}
static int Original(Lua_State*) {++dispatched;return 0;}
#define CHECK(x) do{if(!(x)){printf("FAIL line=%d %s\n",__LINE__,#x);return 1;}}while(0)
int main(int argc,char**argv) {
    if(argc!=2 && argc!=3)return 2;
    const int expectedTotal=argc==3?atoi(argv[2]):42;
    CHECK(expectedTotal>0);
    FILE* list=fopen(argv[1],"rb");CHECK(list!=NULL);
    char line[1024],path[512];int expected,total=0;
    while(fgets(line,sizeof(line),list)) {
        CHECK(sscanf(line,"%s %d",path,&expected)==2);
        FILE* file=fopen(path,"rb");CHECK(file!=NULL);fseek(file,0,SEEK_END);
        long size=ftell(file);rewind(file);std::vector<unsigned char> bytes(size);
        CHECK(fread(&bytes[0],1,size,file)==(unsigned)size);fclose(file);
        KLuaScript script;CHECK(script.Init());
        TLua_Funcs api[]={{"GetWorldPos",World},{"GetContentWorldPos",Content},
            {"SubWorldIdx2ID",World},{"SubWorldIdx2ContentID",Content},{"CloseDialog",Close}};
        CHECK(script.RegisterFunctions(api,sizeof(api)/sizeof(api[0])));
        CHECK(script.LoadBuffer(&bytes[0],size));CHECK(script.ExecuteCode());
        Lua_State* L=script.m_LuaState;
        lua_getglobal(L,"pt_original_main");CHECK(lua_isfunction(L,-1));lua_pop(L,1);
        CHECK(script.RegisterFunction("pt_original_main",Original));
        world=expected;dispatched=closed=0;CHECK(script.CallFunction("main",0,""));
        CHECK(dispatched==1 && closed==0);
        world=999999;CHECK(script.CallFunction("main",0,""));CHECK(dispatched==1 && closed==1);
        ++total;
    }
    fclose(list);CHECK(total==expectedTotal);
    printf("PASS RESTORED_NPC_DISPATCH scripts=%d original_entrypoint=PASS runtime_guards=PASS\n",total);
    puts("SCOPE compile + dispatch guards; original quest transactions tested separately, not all NPC businesses");
    return 0;
}
