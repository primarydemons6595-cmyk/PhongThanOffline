#include <windows.h>
#include <stdio.h>
#include <vector>
#include "KWin32.h"
#include "KLuaScript.h"
static int phase=73,items=0,map=1062,profession=0,space=1,checks=0;
static int R(Lua_State*L,int n){Lua_PushNumber(L,n);return 1;}
static int Task(Lua_State*L){return R(L,(int)Lua_ValueToNumber(L,1)==3?phase:0);}
static int SetTask(Lua_State*L){if((int)Lua_ValueToNumber(L,1)==3)phase=(int)Lua_ValueToNumber(L,2);return 0;}
static int Type(Lua_State*L){return R(L,profession);}
static int ContentMap(Lua_State*L){return R(L,map-1000);}
static int NpcMap(Lua_State*L){return R(L,1062);}
static int Have(Lua_State*L){return R(L,(int)Lua_ValueToNumber(L,1)==10?items:0);}
static int Add(Lua_State*L){if((int)Lua_ValueToNumber(L,1)!=10||!space)return R(L,0);++items;return R(L,1);}
static int Noop(Lua_State*){return 0;}
#define CHECK(c) do{++checks;if(!(c)){printf("FAIL line %d: %s\n",__LINE__,#c);return 1;}}while(0)
static bool Eval(KLuaScript&s,const char*c){return s.LoadBuffer((unsigned char*)c,strlen(c))&&s.ExecuteCode();}
int main(int argc,char**argv){if(argc!=2)return 2;KLuaScript s;CHECK(s.Init());
 TLua_Funcs api[]={{"GetTask",Task},{"SetTask",SetTask},{"GetPlayerType",Type},{"GetContentWorldPos",ContentMap},{"GetNpcWorldPos",NpcMap},{"HaveEventItem",Have},{"AddEventItem",Add},{"Include",Noop},{"openfile",Noop}};
 CHECK(s.RegisterFunctions(api,sizeof(api)/sizeof(api[0])));FILE*f=fopen(argv[1],"rb");CHECK(f!=0);fseek(f,0,SEEK_END);long size=ftell(f);rewind(f);std::vector<unsigned char>b(size);CHECK(fread(&b[0],1,size,f)==size);fclose(f);CHECK(s.LoadBuffer(&b[0],size)&&s.ExecuteCode());
 CHECK(Eval(s,"PlayerIndex=1; function PTGJournal() end; function PTGJournalState() return '' end; pt_common_death=nil"));
 CHECK(s.CallFunction("OnDeath",0,"d",100)&&phase==74&&items==1);
 CHECK(s.CallFunction("OnDeath",0,"d",100)&&phase==74&&items==1);
 phase=72;items=0;CHECK(s.CallFunction("OnDeath",0,"d",100)&&phase==72&&items==0);
 phase=73;map=1063;CHECK(s.CallFunction("OnDeath",0,"d",100)&&phase==73&&items==0);
 map=1062;space=0;CHECK(s.CallFunction("OnDeath",0,"d",100)&&phase==73&&items==0);
 space=1;CHECK(s.CallFunction("OnDeath",0,"d",100)&&phase==74&&items==1);
 phase=73;items=0;profession=1;CHECK(Eval(s,"PTG_OLD_DEATH=function() end"));CHECK(s.CallFunction("OnDeath",0,"d",100)&&phase==73&&items==0);
 profession=0;CHECK(Eval(s,"PlayerIndex=0"));CHECK(s.CallFunction("OnDeath",0,"d",100)&&phase==73&&items==0);
 printf("PASS %d production death_116 Lua4 checks: guaranteed quest item, phase/map/profession/owner gates, full bag retry, duplicate kill\n",checks);return 0;
}
