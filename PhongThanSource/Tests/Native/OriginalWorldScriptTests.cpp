#include <windows.h>
#include <stdio.h>
#include <string.h>
#include "KWin32.h"
#include "LuaLib.h"
#include "KLuaScript.h"
#include <vector>
#include "../../Headers/PhongThanMapIdentity.h"
#include "../../Headers/PhongThanScheduleClock.h"

enum { MAX_SUBWORLD=128 };
struct Worlds {
    int SearchWorld(int id) { return id==1036 ? 4 : id==1016 ? 5 : id==1103 ? 6 :
        id>=1001 && id<=1118 && id!=1058 ? id-1000 : -1; }
} g_SubWorldSet;
struct Templates {
    int FindRow(char* name) { return !strcmp(name,"boss") ? 90 : 0; }
    int GetHeight() { return 2704; }
} g_NpcSetting;
struct Npcs {
    int calls, packed, world, x, y, barrier, fail;
    int Add(int p,int w,int xx,int yy,BOOL b) {
        ++calls;packed=p;world=w;x=xx;y=yy;barrier=b;return fail?0:7;
    }
} NpcSet;
#include "../../Sources/Core/Src/PhongThanWorldScriptCompat.inl"
static int LuaGetNewWorldPos(Lua_State* L) {
    Lua_PushNumber(L,1103);Lua_PushNumber(L,123);Lua_PushNumber(L,456);return 3;
}
static int LuaSubWorldIndexToID(Lua_State* L) {Lua_PushNumber(L,1036);return 1;}
#include "../../Sources/Core/Src/PhongThanContentWorldPos.inl"

static int checks;
#define CHECK(x) do { ++checks; if(!(x)){printf("FAIL line=%d %s\n",__LINE__,#x);return 1;} } while(0)
static void Arg(Lua_State* L,int n) { Lua_PushNumber(L,n); }
static void SpawnArgs(Lua_State* L,int id,int level,int world) {
    Lua_SetTopIndex(L,0);Arg(L,id);Arg(L,level);Arg(L,world);Arg(L,41312);Arg(L,104832);
}
static int globalValues[1024];
static int GlobalGet(Lua_State* L) { Arg(L,globalValues[(int)Lua_ValueToNumber(L,1)]);return 1; }
static int GlobalSet(Lua_State* L) { globalValues[(int)Lua_ValueToNumber(L,1)]=(int)Lua_ValueToNumber(L,2);return 0; }
static int Noop(Lua_State*) {return 0;}

int main(int argc,char** argv) {
    Lua_State* L=Lua_Create(0);
    CHECK(L!=NULL);
    Arg(L,36);CHECK(LuaSubWorldIDToIndex(L)==1 && (int)Lua_ValueToNumber(L,-1)==4);
    Lua_SetTopIndex(L,0);Arg(L,1036);CHECK(LuaSubWorldIDToIndex(L)==1 && (int)Lua_ValueToNumber(L,-1)==4);
    Lua_SetTopIndex(L,0);Arg(L,60010);CHECK(LuaSubWorldIDToIndex(L)==1 && (int)Lua_ValueToNumber(L,-1)==6);
    Lua_SetTopIndex(L,0);Arg(L,103);CHECK(LuaSubWorldIDToIndex(L)==1 && (int)Lua_ValueToNumber(L,-1)==-1);
    Lua_SetTopIndex(L,0);Arg(L,58);CHECK(LuaSubWorldIDToIndex(L)==1 && (int)Lua_ValueToNumber(L,-1)==-1);
    SpawnArgs(L,88,60,4);CHECK(LuaAddNpc(L)==1 && (int)Lua_ValueToNumber(L,-1)==7);
    CHECK(NpcSet.calls==1 && HIWORD(NpcSet.packed)==88 && LOWORD(NpcSet.packed)==60 && !NpcSet.barrier);
    SpawnArgs(L,88,60,4);Arg(L,1);CHECK(LuaAddNpc(L)==1 && NpcSet.barrier==1);
    SpawnArgs(L,88,60,-1);CHECK(LuaAddNpc(L)==1 && (int)Lua_ValueToNumber(L,-1)==0 && NpcSet.calls==2);
    SpawnArgs(L,2703,60,4);CHECK(LuaAddNpc(L)==1 && (int)Lua_ValueToNumber(L,-1)==0 && NpcSet.calls==2);
    Lua_SetTopIndex(L,0);CHECK(LuaAddNpc(L)==1 && Lua_GetTopIndex(L)==1 && (int)Lua_ValueToNumber(L,-1)==0);
    SpawnArgs(L,88,65536,4);CHECK(LuaAddNpc(L)==1 && (int)Lua_ValueToNumber(L,-1)==0 && NpcSet.calls==2);
    PhongThanScheduleClock clock;
    CHECK(clock.Advance(100));CHECK(!clock.Advance(100));CHECK(!clock.Advance(99));
    CHECK(clock.Advance(105));CHECK(!clock.Advance(104));CHECK(clock.Advance(106));
    Lua_SetTopIndex(L,0);CHECK(LuaGetContentWorldPos(L)==3 && Lua_GetTopIndex(L)==3);
    CHECK((int)Lua_ValueToNumber(L,1)==60010 && (int)Lua_ValueToNumber(L,2)==123 && (int)Lua_ValueToNumber(L,3)==456);
    Lua_SetTopIndex(L,0);Arg(L,4);CHECK(LuaSubWorldIndexToContentID(L)==1 && (int)Lua_ValueToNumber(L,-1)==36 && Lua_GetTopIndex(L)==2);
    Lua_Release(L);
    int bosses=0;
    if(argc==2) {
        FILE* list=fopen(argv[1],"rb");CHECK(list!=NULL);
        char line[1024],path[512];int tid,level,content,x,y,slot;
        while(fgets(line,sizeof(line),list)) {
            CHECK(sscanf(line,"%s %d %d %d %d %d %d",path,&tid,&level,&content,&x,&y,&slot)==7);
            FILE* file=fopen(path,"rb");CHECK(file!=NULL);
            fseek(file,0,SEEK_END);long size=ftell(file);rewind(file);
            std::vector<unsigned char> bytes(size);CHECK(fread(&bytes[0],1,size,file)==(unsigned)size);fclose(file);
            KLuaScript script;CHECK(script.Init());
            TLua_Funcs api[]={{"SubWorldID2Idx",LuaSubWorldIDToIndex},{"AddNpc",LuaAddNpc},
                {"GetGlobalValue",GlobalGet},{"SetGlobalValue",GlobalSet},
                {"SetNpcName",Noop},{"AddGlobalCountNews",Noop}};
            CHECK(script.RegisterFunctions(api,sizeof(api)/sizeof(api[0])));
            CHECK(script.LoadBuffer(&bytes[0],size));
            CHECK(script.ExecuteCode());
            globalValues[slot]=-1;const int before=NpcSet.calls;
            CHECK(script.CallFunction("main",0,""));CHECK(NpcSet.calls==before+1);
            PHONGTHAN_MAP_RESOLUTION resolution;CHECK(PhongThanResolveContentMap(content,&resolution));
            CHECK(HIWORD(NpcSet.packed)==tid && LOWORD(NpcSet.packed)==level &&
                NpcSet.world==g_SubWorldSet.SearchWorld(resolution.RuntimeMapId) && NpcSet.x==x && NpcSet.y==y);
            CHECK(globalValues[slot]==0);
            CHECK(script.CallFunction("main",0,""));CHECK(NpcSet.calls==before+1);
            globalValues[slot]=-1;NpcSet.fail=1;
            CHECK(script.CallFunction("main",0,""));CHECK(globalValues[slot]==-1);
            NpcSet.fail=0;CHECK(script.CallFunction("main",0,""));CHECK(globalValues[slot]==0);
            ++bosses;
        }
        fclose(list);CHECK(bosses==12);
    }
    printf("PASS ORIGINAL_WORLD_SCRIPT checks=%d original_boss_scripts=%d five_six_arguments=PASS map_domains=PASS minute_dedup=PASS\n",checks,bosses);
    return 0;
}
