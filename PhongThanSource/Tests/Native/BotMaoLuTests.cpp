#include <windows.h>
#include <assert.h>
#include <stdio.h>
#include "KWin32.h"
#include "KLuaScript.h"
static int tasks[5000],level=22,profession=0,world=1002,bot=0,xp=0,exchanges=0;
static int N(Lua_State* L,int i){return (int)lua_tonumber(L,i);}
static int R(Lua_State* L,int n){lua_pushnumber(L,n);return 1;}
static int Task(Lua_State* L){return R(L,tasks[N(L,1)]);}
static int SetTask(Lua_State* L){tasks[N(L,1)]=N(L,2);return 0;}
static int Level(Lua_State* L){return R(L,level);}
static int Type(Lua_State* L){return R(L,profession);}
static int World(Lua_State* L){return R(L,world-1000);}
static int Bot(Lua_State* L){return R(L,bot);}
static int Zero(Lua_State* L){return R(L,0);}
static int Hundred(Lua_State* L){return R(L,100);}
static int Stub(Lua_State*){return 0;}
static char firstOption[256];
static int Buttons(Lua_State* L){const char* value=lua_isstring(L,3)?lua_tostring(L,3):"";strncpy(firstOption,value,sizeof(firstOption)-1);firstOption[sizeof(firstOption)-1]=0;return 0;}
static int LoadInclude(Lua_State* L){const char* relative=lua_tostring(L,1);if(!relative)return 0;char path[1024];sprintf(path,"E:/ProjectAI/PhongThan/PT/PhongThanRuntime-Loose/Server%s",relative);for(int i=0;path[i];++i)if(path[i]=='\\')path[i]='/';FILE* f=fopen(path,"rb");if(!f)return 0;fseek(f,0,SEEK_END);int size=ftell(f);rewind(f);char* text=new char[size];assert(fread(text,1,size,f)==size);fclose(f);int result=Lua_ExecuteBuffer(L,text,size,path);delete[] text;assert(!result);return 0;}
static int Exp(Lua_State* L){xp+=N(L,1);return 0;}
static int Exchange(Lua_State* L){if(tasks[N(L,1)]!=N(L,2))return R(L,0);tasks[N(L,1)]=N(L,3);++exchanges;return R(L,1);}
static void Eval(KLuaScript& s,const char* code){assert(!Lua_ExecuteBuffer(s.m_LuaState,code,strlen(code),"maolu bot regression"));}
int main(int argc,char** argv){assert(argc==2);SetErrorMode(SEM_FAILCRITICALERRORS|SEM_NOGPFAULTERRORBOX);KLuaScript s;assert(s.Init());
 TLua_Funcs api[]={{"GetTask",Task},{"SetTask",SetTask},{"GetLevel",Level},{"GetPlayerType",Type},{"GetContentWorldPos",World},{"IsServerBot",Bot},{"GetTrade",Zero},{"GetLockState",Zero},{"GetExp",Zero},{"GetNextExp",Hundred},{"AddOwnExp",Exp},{"QuestExchange",Exchange},{"AddNote",Stub},{"MsgBox",Stub},{"Msg2Player",Stub},{"SayButtons",Buttons},{"CloseDialog",Stub}};assert(s.RegisterFunctions(api,sizeof(api)/sizeof(api[0]))&&s.Load(argv[1]));
 Eval(s,"pt_mao_take()");assert(tasks[4960]==0&&exchanges==0);
 bot=1;Eval(s,"pt_mao_take()");assert(tasks[4960]==1&&exchanges==1&&xp==0);Eval(s,"pt_mao_take()");assert(exchanges==1);
 Eval(s,"PTMAO_NPC_CLASS=0;PTMAO_NPC_ROLE='wrong';pt_mao_claim()");assert(tasks[4960]==1&&xp==0);
 const char* roles[]={"giao","store","lo","giao"};for(int i=0;i<4;++i){char code[128];sprintf(code,"PTMAO_NPC_ROLE='%s';pt_mao_claim()",roles[i]);Eval(s,code);assert(tasks[4960]==i+2);}
 assert(xp==3600&&exchanges==5);Eval(s,"pt_mao_claim()");assert(xp==3600&&exchanges==5);
 ZeroMemory(tasks,sizeof(tasks));bot=0;level=1;exchanges=0;Eval(s,"pt_mao_take()");assert(tasks[4960]==1&&exchanges==1);
 KLuaScript npc;assert(npc.Init()&&npc.RegisterFunctions(api,sizeof(api)/sizeof(api[0])));TLua_Funcs includes[]={{"Include",LoadInclude}};assert(npc.RegisterFunctions(includes,1));assert(npc.Load("E:/ProjectAI/PhongThan/PT/PhongThanRuntime-Loose/Server/script/phongthan/npc_restore/1004_00.lua"));ZeroMemory(tasks,sizeof(tasks));profession=2;world=1004;level=18;bot=1;tasks[4960]=1;exchanges=0;xp=0;Eval(npc,"assert(PTMaoReady()); main()");assert(strstr(firstOption,"/pt_mao_claim")&&exchanges==0);Eval(npc,"pt_mao_claim()");assert(tasks[4960]==2&&exchanges==1&&xp==100);bot=0;tasks[4960]=1;Eval(npc,"main()");assert(strstr(firstOption,"/pt3_book_view"));
 printf("PASS Mao Lu: late bot catch-up uses real four visits, bounded rewards, wrong-NPC/duplicate guards; human level gate unchanged; real Di Nhan guide main/callback advances phase.\n");return 0;
}
