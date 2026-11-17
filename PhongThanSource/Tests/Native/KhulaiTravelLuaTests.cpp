#include <windows.h>
#include <stdio.h>
#include <vector>
#include "KWin32.h"
#include "KLuaScript.h"
#include "../../Headers/PhongThanMapIdentity.h"
static int profession,phase=73,quantity=1,map=1,morph=-1,inside=0,destination,x,y,menus,checks;
static int Number(Lua_State*L,int n){Lua_PushNumber(L,n);return 1;}
static int Type(Lua_State*L){return Number(L,profession);}
static int Task(Lua_State*L){return Number(L,phase);}
static int Map(Lua_State*L){return Number(L,map);}
static int Morph(Lua_State*L){return Number(L,morph);}
static int Inside(Lua_State*L){return Number(L,inside);}
static int Have(Lua_State*L){return Number(L,quantity);}
static int Noop(Lua_State*){return 0;}
static int Menu(Lua_State*){++menus;return 0;}
static int RuntimeMove(Lua_State*L){PHONGTHAN_MAP_RESOLUTION r;
 if(!PhongThanResolveRuntimeMap((int)Lua_ValueToNumber(L,1),&r))return Number(L,0);
 destination=r.RuntimeMapId;x=(int)Lua_ValueToNumber(L,2);y=(int)Lua_ValueToNumber(L,3);return Number(L,1);}
static int ContentMove(Lua_State*L){PHONGTHAN_MAP_RESOLUTION r;
 if(!PhongThanResolveContentMap((int)Lua_ValueToNumber(L,1),&r))return Number(L,0);
 destination=r.RuntimeMapId;return Number(L,1);}
#define CHECK(c) do{++checks;if(!(c)){printf("FAIL line %d: %s\n",__LINE__,#c);return 1;}}while(0)
int main(int argc,char**argv){if(argc!=2)return 2;KLuaScript s;CHECK(s.Init());
 TLua_Funcs api[]={{"Include",Noop},{"GetPlayerType",Type},{"GetTask",Task},{"GetContentWorldPos",Map},{"GetWorldPos",Map},{"GetMorphType",Morph},{"IsPlayerInsideWeapon",Inside},{"HaveNormalItem",Have},{"SayButtons",Menu},{"NewWorld",ContentMove},{"NewRuntimeWorld",RuntimeMove}};
 CHECK(s.RegisterFunctions(api,sizeof(api)/sizeof(api[0])));
 FILE*f=fopen(argv[1],"rb");CHECK(f!=0);fseek(f,0,SEEK_END);long size=ftell(f);rewind(f);std::vector<unsigned char>b(size);CHECK(fread(&b[0],1,size,f)==size);fclose(f);
 CHECK(s.LoadBuffer(&b[0],size)&&s.ExecuteCode());
 // Load complete production wrappers. Exercise the final bound main for all
 // three professions, including GameMa's phase 73, without advancing quests.
 for(profession=0;profession<3;++profession){
  const char*callback=profession?"pt2_time_travel":"ptg_time_travel";
  phase=73;quantity=1;destination=menus=0;map=1;morph=-1;inside=0;
  CHECK(s.CallFunction("main",0,""));CHECK(menus==1);
  CHECK(s.CallFunction((char*)callback,0,""));CHECK(destination==1061&&x==198*8+4&&y==201*16+8&&phase==73&&quantity==1);
  destination=0;quantity=0;CHECK(s.CallFunction((char*)callback,0,""));CHECK(destination==0);quantity=1;
  int blocked[]={64,71,66,72};for(int i=0;i<4;++i){map=blocked[i];CHECK(s.CallFunction((char*)callback,0,""));CHECK(destination==0);}
  map=1;morph=364;CHECK(s.CallFunction((char*)callback,0,""));CHECK(destination==0);morph=-1;
  inside=1;CHECK(s.CallFunction((char*)callback,0,""));CHECK(destination==0);inside=0;
  phase=86;CHECK(s.CallFunction((char*)callback,0,""));CHECK(destination==1061&&quantity==1&&phase==86);
 }
 printf("PASS %d actual Lua4 Khulai checks: bound wrappers/all professions, phase73, correct map namespace/coordinates, retained item/quest, missing item, forbidden maps, morph and vehicle guards.\n",checks);return 0;
}
