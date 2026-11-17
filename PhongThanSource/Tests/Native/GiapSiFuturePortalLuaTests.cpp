#include <windows.h>
#include <stdio.h>
#include <vector>
#include "KWin32.h"
#include "KLuaScript.h"
#include "../../Headers/PhongThanMapIdentity.h"
static int phase=71,level=80,type=0,map=52,trade=0,locked=0,world=0,x=0,y=0,offered=0,checks=0;
static int fight=0;static bool moveOk=true;
static int Fight(Lua_State*L){fight=(int)Lua_ValueToNumber(L,1);return 0;}
static int R(Lua_State*L,int n){Lua_PushNumber(L,n);return 1;}
static int Task(Lua_State*L){return R(L,(int)Lua_ValueToNumber(L,1)==3?phase:0);}
static int Level(Lua_State*L){return R(L,level);}static int Type(Lua_State*L){return R(L,type);}
static int Map(Lua_State*L){return R(L,map);}static int Trade(Lua_State*L){return R(L,trade);}
static int Lock(Lua_State*L){return R(L,locked);}static int Noop(Lua_State*){return 0;}
static int Cash(Lua_State*L){return R(L,1000000);}static int Have(Lua_State*L){return R(L,100);}
static int Exchange(Lua_State*L){if((int)Lua_ValueToNumber(L,1)!=3||(int)Lua_ValueToNumber(L,2)!=phase)return R(L,0);phase=(int)Lua_ValueToNumber(L,3);return R(L,1);}
static int Move(Lua_State*L){if(!moveOk)return R(L,0);PHONGTHAN_MAP_RESOLUTION target;if(!PhongThanResolveRuntimeMap((int)Lua_ValueToNumber(L,1),&target)){Lua_PushNumber(L,0);return 1;}world=target.RuntimeMapId;x=(int)Lua_ValueToNumber(L,2);y=(int)Lua_ValueToNumber(L,3);Lua_PushNumber(L,1);return 1;}
static int ContentMove(Lua_State*L){PHONGTHAN_MAP_RESOLUTION target;if(!PhongThanResolveContentMap((int)Lua_ValueToNumber(L,1),&target)){Lua_PushNumber(L,0);return 1;}world=target.RuntimeMapId;Lua_PushNumber(L,1);return 1;}
#define CHECK(c) do{++checks;if(!(c)){printf("FAIL line %d: %s\n",__LINE__,#c);return 1;}}while(0)
int main(int argc,char**argv){if(argc!=3)return 2;
 for(int file=1;file<=2;++file){KLuaScript s;CHECK(s.Init());
 TLua_Funcs api[]={{"GetTask",Task},{"GetLevel",Level},{"GetPlayerType",Type},{"GetContentWorldPos",Map},{"GetTrade",Trade},{"GetLockState",Lock},{"NewWorld",ContentMove},{"NewRuntimeWorld",Move},{"CloseDialog",Noop},{"Include",Noop},{"GetCash",Cash},{"HaveNormalItem",Have},{"HaveEventItem",Have},{"QuestExchange",Exchange},{"AddOwnExp",Noop},{"AddCredit",Noop},{"Msg2Player",Noop},{"SetCamp",Noop},{"SetTask",Noop},{"SetFightState",Fight}};
 CHECK(s.RegisterFunctions(api,sizeof(api)/sizeof(api[0])));FILE*f=fopen(argv[file],"rb");CHECK(f!=0);fseek(f,0,SEEK_END);long size=ftell(f);rewind(f);std::vector<unsigned char> bytes(size);CHECK(fread(&bytes[0],1,size,f)==size);fclose(f);CHECK(s.LoadBuffer(&bytes[0],size));CHECK(s.ExecuteCode());
 // Keep actual production menus/callbacks and suppress unrelated journal I/O.
 char code[]="function PTGJournal() end function PTGPreview(i) previewed=i end function PTGNotice(t) end function SayButtons(t,n,o) if type(o)=='table' then menu=o end end PTSAddChoices=nil";
 CHECK(s.LoadBuffer((unsigned char*)code,sizeof(code)-1)&&s.ExecuteCode());
 map=file==1?52:63;phase=file==1?71:72;level=80;type=trade=locked=world=0;
 CHECK(s.CallFunction("main",0,""));
 char verify[]="assert(menu and menu[1] and strfind(menu[1],'/ptg_future_travel',1,1))";
 CHECK(s.LoadBuffer((unsigned char*)verify,sizeof(verify)-1)&&s.ExecuteCode());
 fight=0;CHECK(s.CallFunction("ptg_future_travel",0,"")&&world==(file==1?1063:1062)&&phase==(file==1?71:72)&&fight==1);
 CHECK(x==(file==1?198:203)*8+4&&y==(file==1?200:198)*16+8);
 for(int p=72;p<=74;++p){phase=p;world=0;CHECK(s.CallFunction("ptg_future_travel",0,"")&&world==(file==1?1063:1062)&&phase==p);}
 phase=file==1?71:72;world=0;fight=0;moveOk=false;CHECK(s.CallFunction("ptg_future_travel",0,"")&&world==0&&fight==0);moveOk=true;
 phase=71;world=0;level=79;CHECK(s.CallFunction("ptg_future_travel",0,"")&&world==0);level=80;
 type=1;CHECK(s.CallFunction("ptg_future_travel",0,"")&&world==0);type=0;
 phase=69;CHECK(s.CallFunction("ptg_future_travel",0,"")&&world==0);phase=80;CHECK(s.CallFunction("ptg_future_travel",0,"")&&world==0);
 phase=file==1?71:72;map=1;CHECK(s.CallFunction("ptg_future_travel",0,"")&&world==0);map=file==1?52:63;
 trade=1;CHECK(s.CallFunction("ptg_future_travel",0,"")&&world==0);trade=0;locked=1;CHECK(s.CallFunction("ptg_future_travel",0,"")&&world==0);locked=0;
 phase=70;CHECK(s.CallFunction("ptg_future_travel",0,"")&&world==0&&phase==70);
 if(file==1){char v[]="assert(previewed and PTG_STEPS[previewed].from==70 and PTG_STEPS[previewed].to==71)";CHECK(s.LoadBuffer((unsigned char*)v,sizeof(v)-1)&&s.ExecuteCode());}
 if(file==1){world=0;char initial[]="for i=1,getn(PTG_STEPS) do if PTG_STEPS[i].role=='tay' and PTG_STEPS[i].task==3 and PTG_STEPS[i].from==70 then PTGApply(i) end end";CHECK(s.LoadBuffer((unsigned char*)initial,sizeof(initial)-1)&&s.ExecuteCode());CHECK(phase==71&&world==1063&&fight==1);}
 }
 printf("PASS %d actual Lua4 future-portal checks, both NPC wrappers and guarded quest acceptance\n",checks);return 0;
}
