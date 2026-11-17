#include <windows.h>
#include <assert.h>
#include <stdio.h>
#include "KWin32.h"
#include "KLuaScript.h"
static int tasks[5000],bot=1,nearNpc=1,trading=0,locked=0,weight=30,capacity=200,gear=0,red=0,mana=0,xu=0,sales=0,level=13;
static char first[256];
static int N(Lua_State* L,int i){return (int)lua_tonumber(L,i);}
static int R(Lua_State* L,int n){lua_pushnumber(L,n);return 1;}
static int Task(Lua_State* L){return R(L,tasks[N(L,1)]);}
static int SetTask(Lua_State* L){tasks[N(L,1)]=N(L,2);return 0;}
static int Bot(Lua_State* L){return R(L,bot);}
static int Near(Lua_State* L){return R(L,nearNpc);}
static int Trade(Lua_State* L){return R(L,trading);}
static int Lock(Lua_State* L){return R(L,locked);}
static int Level(Lua_State* L){return R(L,level);}
static int Zero(Lua_State* L){return R(L,0);}
static int Stub(Lua_State*){return 0;}
static int CanCarry(Lua_State* L){int units=lua_gettop(L)>=7?N(L,7):1;return R(L,weight+(N(L,1)==0?5:N(L,1)==8?10:1)*units<=capacity*80/100);}
static int Grant(Lua_State* L){if(N(L,1)==0){++gear;weight+=5;}else if(N(L,2)==0){++red;++weight;}else{++mana;++weight;}assert(!bot||weight<=capacity);return R(L,1);}
static int Currency(Lua_State* L){if(tasks[4703]!=1){++xu;tasks[4703]=1;weight+=100;}return R(L,1);}
static int Exchange(Lua_State* L){assert(N(L,1)==4704&&N(L,2)==0&&N(L,3)==1);if(tasks[4704])return R(L,0);tasks[4704]=1;++xu;weight+=10;return R(L,1);}
static int Include(Lua_State* L){const char* relative=lua_tostring(L,1);assert(relative);char path[1024];sprintf(path,"E:/ProjectAI/PhongThan/PT/PhongThanRuntime-Loose/Server%s",relative);for(int i=0;path[i];++i)if(path[i]=='\\')path[i]='/';FILE* f=fopen(path,"rb");assert(f);fseek(f,0,SEEK_END);int size=ftell(f);rewind(f);char* text=new char[size];assert(fread(text,1,size,f)==size);fclose(f);int result=Lua_ExecuteBuffer(L,text,size,path);delete[] text;assert(!result);return 0;}
static int Sale(Lua_State* L){assert(N(L,1)==24&&bot&&nearNpc&&!trading&&!locked);++sales;return 0;}
static int Say(Lua_State* L){const char* s=lua_tostring(L,3);strncpy(first,s?s:"",sizeof(first)-1);return 0;}
static void Eval(KLuaScript& s,const char* text){assert(!Lua_ExecuteBuffer(s.m_LuaState,text,strlen(text),"starter carry regression"));}
int main(int argc,char** argv){assert(argc==2);SetErrorMode(SEM_FAILCRITICALERRORS|SEM_NOGPFAULTERRORBOX);KLuaScript s;assert(s.Init());
 TLua_Funcs api[]={{"GetTask",Task},{"SetTask",SetTask},{"GetSeries",Zero},{"GetLevel",Level},{"IsServerBot",Bot},{"NearLeQuan",Near},{"GetTrade",Trade},{"GetLockState",Lock},{"CanCarryNormalItem",CanCarry},{"AddNormalItem",Grant},{"AddNormalItemPile",Grant},{"LeQuanStarterXu",Currency},{"GetItemCount",Zero},{"DelItem",Stub},{"Msg2Player",Stub},{"Say",Say},{"Sale",Sale},{"Include",Include},{"QuestExchange",Exchange}};
 assert(s.RegisterFunctions(api,sizeof(api)/sizeof(api[0]))&&s.Load(argv[1]));
 Eval(s,"main()");assert(strstr(first,"/PTLeQuanSellSurplus"));Eval(s,"PTLeQuanSellSurplus()");assert(sales==1);
 nearNpc=0;Eval(s,"PTLeQuanSellSurplus(); PhongThanClaimStarter()");assert(sales==1&&gear==0);nearNpc=1;
 trading=1;Eval(s,"PTLeQuanSellSurplus(); PhongThanClaimStarter()");assert(sales==1&&gear==0);trading=0;
 locked=1;Eval(s,"PTLeQuanSellSurplus(); PhongThanClaimStarter()");assert(sales==1&&gear==0);locked=0;
 Eval(s,"PhongThanClaimStarter()");assert(weight==160&&gear==8&&red==90&&mana==0&&tasks[4702]==98&&tasks[4700]==1);
 Eval(s,"PhongThanClaimStarter()");assert(weight==160&&gear==8&&red==90&&tasks[4702]==98);
 weight=0;Eval(s,"PhongThanClaimStarter()");assert(gear==8&&red==100&&mana==100&&xu==1&&tasks[4702]==209&&tasks[4700]==2);
 Eval(s,"PhongThanClaimStarter()");assert(xu==1&&gear==8&&red==100&&mana==100);
 tasks[4700]=1;tasks[4702]=208;weight=190;Eval(s,"PhongThanClaimStarter()");assert(tasks[4700]==2&&xu==1&&weight==190); // Existing card receipt must not require room again.
 tasks[4700]=1;tasks[4704]=0;xu=0;weight=155;Eval(s,"PhongThanClaimStarter()");assert(tasks[4700]==1&&tasks[4704]==0&&xu==0&&gear==8&&red==100&&mana==100);
 weight=140;Eval(s,"PhongThanClaimStarter()");assert(tasks[4700]==2&&tasks[4704]==1&&xu==1&&weight==150);
 bot=0;Eval(s,"main(); PTLeQuanSellSurplus()");assert(strstr(first,"/PhongThanClaimStarter")&&sales==1);
 printf("PASS real Le Quan Lua: bounded gift batches, exact partial resume/no duplicates, actual level-card carry/receipt, nearby/trade/lock guards, human menu unchanged.\n");return 0;
}
