#include "KWin32.h"
#include <stdio.h>
#include "KLuaScript.h"
static int pk,moved,revived,shops,blocked,fight;
static int GetPk(Lua_State* L){Lua_PushNumber(L,pk);return 1;}
static int Ignore(Lua_State*){return 0;}
static int Refuse(Lua_State*){++blocked;return 0;}
static int Move(Lua_State* L){
 if(Lua_ValueToNumber(L,1)!=1 || Lua_ValueToNumber(L,2)!=1523 || Lua_ValueToNumber(L,3)!=3241)lua_error(L,"unexpected boat destination");
 ++moved;return 0;
}
static int Revive(Lua_State* L){if(Lua_ValueToNumber(L,1)!=1 || Lua_ValueToNumber(L,2)!=1)lua_error(L,"unexpected revive");++revived;return 0;}
static int Fight(Lua_State* L){if(Lua_ValueToNumber(L,1)!=0)lua_error(L,"fight must stop");++fight;return 0;}
static int Sale(Lua_State* L){if(Lua_ValueToNumber(L,1)!=15)lua_error(L,"wrong medicine shop");++shops;return 0;}
static bool Load(KLuaScript& s,const char* path){
 if(!s.Init())return false;
 TLua_Funcs api[]={{"GetPK",GetPk},{"CloseDialog",Ignore},{"Msg2Player",Ignore},{"MsgBox",Refuse},
  {"NewWorld",Move},{"SetRevPos",Revive},{"SetFightState",Fight},{"Sale",Sale},{"SayTask",Ignore}};
 s.RegisterFunctions(api,sizeof(api)/sizeof(api[0]));
 FILE* f=fopen(path,"rb");if(!f)return false;fseek(f,0,SEEK_END);int n=ftell(f);rewind(f);
 char* b=new char[n];bool ok=fread(b,1,n,f)==n;fclose(f);
 ok=ok&&s.LoadBuffer((BYTE*)b,n)&&s.ExecuteCode();delete[] b;return ok;
}
#define CHECK(x) do{if(!(x)){printf("FAIL line %d\n",__LINE__);return 1;}}while(0)
int main(int argc,char** argv){
 CHECK(argc==3);KLuaScript boat,doctor;CHECK(Load(boat,argv[1]));CHECK(Load(doctor,argv[2]));
 pk=1;CHECK(boat.CallFunction("main",0,""));CHECK(boat.CallFunction("likai",0,""));CHECK(!moved&&!revived&&!fight&&blocked==1);
 pk=0;CHECK(boat.CallFunction("likai",0,""));CHECK(moved==1&&revived==1&&fight==1);
 pk=-1;CHECK(boat.CallFunction("likai",0,""));CHECK(moved==2&&revived==2);
 CHECK(doctor.CallFunction("main",0,""));CHECK(doctor.CallFunction("yes",0,""));CHECK(shops==1);
 CHECK(boat.CallFunction("no",0,""));CHECK(doctor.CallFunction("no",0,""));
 puts("PASS Lua4 actual island scripts: PK>0 refused; PK<=0 exits to original destination; original medicine shop 15.");return 0;
}
