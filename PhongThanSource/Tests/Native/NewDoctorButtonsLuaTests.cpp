#include <windows.h>
#include <stdio.h>
#include <vector>
#include "KWin32.h"
#include "KLuaScript.h"
static int buttons,shop,life,mana,task;
static int Noop(Lua_State*){return 0;}static int Buttons(Lua_State*L){buttons=(int)Lua_ValueToNumber(L,2);return 0;}
static int Sale(Lua_State*L){shop=(int)Lua_ValueToNumber(L,1);return 0;}static int Get(Lua_State*L){Lua_PushNumber(L,task);return 1;}
static int Set(Lua_State*L){task=(int)Lua_ValueToNumber(L,2);return 0;}static int Time(Lua_State*L){Lua_PushNumber(L,1800000000);return 1;}
static int Life(Lua_State*){++life;return 0;}static int Mana(Lua_State*){++mana;return 0;}
#define CHECK(x) do{if(!(x)){printf("FAIL line%d file%d\n",__LINE__,count);return 1;}}while(0)
int main(int argc,char**argv){if(argc!=3)return 2;FILE*list=fopen(argv[1],"rb");if(!list)return 3;char path[1024];int count=0,expected=atoi(argv[2]);
 while(fgets(path,sizeof(path),list)){int size=strlen(path);while(size&&(path[size-1]=='\r'||path[size-1]=='\n'))path[--size]=0;FILE*f=fopen(path,"rb");CHECK(f);fseek(f,0,SEEK_END);size=ftell(f);rewind(f);std::vector<unsigned char> bytes(size);CHECK(fread(&bytes[0],1,size,f)==size);fclose(f);
 KLuaScript s;CHECK(s.Init());TLua_Funcs api[]={{"SayButtons",Buttons},{"Sale",Sale},{"GetTask",Get},{"SetTask",Set},{"SystemTime",Time},{"RestoreLife",Life},{"RestoreMana",Mana},{"Talk",Noop},{"Msg2Player",Noop},{"CloseDialog",Noop}};CHECK(s.RegisterFunctions(api,sizeof(api)/sizeof(api[0])));CHECK(s.LoadBuffer(&bytes[0],size)&&s.ExecuteCode());
 buttons=shop=life=mana=task=0;CHECK(s.CallFunction("main",0,"")&&buttons==3);CHECK(s.CallFunction("pt_doctor_shop",0,"")&&shop==expected);CHECK(s.CallFunction("pt_doctor_heal",0,"")&&life==1&&mana==1&&task==1800000000);CHECK(s.CallFunction("pt_doctor_heal",0,"")&&life==1&&mana==1);++count;
 }fclose(list);printf("PASS %d newly added doctor scripts: real Lua4 button menu, isolated shop and unchanged healing cooldown\n",count);return 0;}
