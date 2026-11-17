#include <windows.h>
#include <stdio.h>
#include <assert.h>
#include "KWin32.h"
#include "KStrBase.h"
#include "KLuaScript.h"
#include "Text.h"
#include "../../Headers/PhongThanSocketState.h"
static int menu=0,mode=0,closed=0,pages=0;
static int Buttons(Lua_State* L){menu=(int)lua_tonumber(L,2);assert(lua_gettop(L)==menu+2);for(int n=3;n<=menu+2;++n)assert(strchr(lua_tostring(L,n),'/'));return 0;}
static int Panel(Lua_State* L){mode=(int)lua_tonumber(L,1);return 0;}
static int Close(Lua_State*){++closed;return 0;}
static int Talk(Lua_State*){++pages;return 0;}
struct TestPictures: IInlinePicEngineSink {
 int added;
 TestPictures():added(0){}
 long GetPicSize(unsigned short,int& x,int& y){x=y=15;return 0;}
 long DrawPic(unsigned short,int,int){return 0;}
 long AddCustomInlinePic(unsigned short& id,const char*){id=512+added++;return 0;}
 long RemoveCustomInlinePic(unsigned short){return 0;}
};
static TestPictures pictures;
IInlinePicEngineSink* g_pIInlinePicSink=&pictures;
#include "../../Sources/GameClient/Ui/Elem/SocketTooltipPictures.inl"
int main(int argc,char** argv){
 if(argc!=2)return 2;FILE* f=fopen(argv[1],"rb");assert(f);fseek(f,0,SEEK_END);int bytes=ftell(f);rewind(f);char buffer[4096];assert(bytes>0&&bytes<sizeof(buffer));assert(fread(buffer,1,bytes,f)==(unsigned)bytes);fclose(f);
 KLuaScript script;assert(script.Init());TLua_Funcs funcs[]={{"SayButtons",Buttons},{"EnchaseItem",Panel},{"CloseDialog",Close},{"Talk",Talk}};
 assert(script.RegisterFunctions(funcs,4)&&script.LoadBuffer((unsigned char*)buffer,bytes)&&script.ExecuteCode());
 assert(script.CallFunction("main",0,"")&&menu==3);assert(script.CallFunction("inlay",0,"")&&menu==4);
 assert(script.CallFunction("socket",0,"")&&mode==2&&closed==1);
 for(int n=1;n<=4;++n){char name[16];sprintf(name,"slot%d",n);assert(script.CallFunction(name,0,"")&&mode==n+2);}
 assert(script.CallFunction("help",0,"")&&pages==1);
 PT_SOCKET_VISUALS v;ZeroMemory(&v,sizeof(v));v.Count=1;v.Slot[0]=0;strcpy(v.MenuImage[0],"\\hole.spr");
 char text[256]="name\n{pts0}<color=green>hole<color>\n{pts1}empty";int length=EncodeSocketTooltip(text,strlen(text),v);
 assert(length>0&&pictures.added==1);bool found=false;for(int i=0;i+2<length;++i)if(text[i]==KTC_INLINE_PIC){WORD id=0;memcpy(&id,text+i+1,2);assert(id==512);found=true;}assert(found);
 strcpy(text,"{pts0}again");EncodeSocketTooltip(text,strlen(text),v);assert(pictures.added==1);
 ZeroMemory(&v,sizeof(v));strcpy(text,"a{pts3}b");length=EncodeSocketTooltip(text,strlen(text),v);assert(length==2&&!memcmp(text,"ab",2));
 puts("PASS LEI_SOCKET_UI actual_Lua4 menus3_4 modes2_to6 close_help actual_engine_text binary_custom_pic cached missing_marker_removed");return 0;
}
