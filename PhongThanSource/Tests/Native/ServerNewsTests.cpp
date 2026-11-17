#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <vector>
#include "KWin32.h"
#include "KLuaScript.h"
#include "../../Sources/Core/Src/PhongThanScriptWire.h"
static int checks,sent,minute,phase,alive,lastView;
static char message[1024];
static int Clock(Lua_State* L){Lua_PushNumber(L,20000.0*86400+(minute-420)*60);return 1;}
static int Global(Lua_State* L){int slot=(int)Lua_ValueToNumber(L,1);Lua_PushNumber(L,slot<=4?phase:(alive?0:-1));return 1;}
static int News(Lua_State* L){
 KPhongThanScriptAction action,decoded;memset(&action,0,sizeof(action));
 action.View=UI_NEWSINFO;action.Operation=PHONGTHAN_SCRIPT_SHOW;action.ServerOwned=1;
 action.OptionCount=0; // NEWSMESSAGE_NORMAL
 const char* text=Lua_ValueToString(L,1);if(!text || strlen(text)>500)return 0;
 strcpy(action.Content,text);int length=strlen(text),repeat=(int)Lua_ValueToNumber(L,2);
 memcpy(action.Content+length,&repeat,sizeof(repeat));action.ContentLength=length+sizeof(repeat);
 unsigned char wire[4096];unsigned int size=PhongThanBuildScriptPacket(action,wire,sizeof(wire));
 if(!size || !PhongThanReadScriptPacket(wire,size,&decoded) || memcmp(decoded.Content,action.Content,action.ContentLength))return 0;
 lastView=decoded.View;strcpy(message,text);++sent;return 0;
}
#define CHECK(x) do{++checks;if(!(x)){printf("FAIL line=%d minute=%d %s\n",__LINE__,minute,#x);return 1;}}while(0)
int main(int argc,char** argv){
 CHECK(argc==2);FILE* f=fopen(argv[1],"rb");CHECK(f!=NULL);fseek(f,0,SEEK_END);long size=ftell(f);rewind(f);std::vector<unsigned char> bytes(size);fread(&bytes[0],1,size,f);fclose(f);
 KLuaScript script;CHECK(script.Init());TLua_Funcs api[]={{"SystemTime",Clock},{"GetGlobalValue",Global},{"AddNews",News}};
 CHECK(script.RegisterFunctions(api,3));CHECK(script.LoadBuffer(&bytes[0],size));CHECK(script.ExecuteCode());
 minute=130;CHECK(script.CallFunction("main",0,""));CHECK(sent==1 && strstr(message,"10") && lastView==UI_NEWSINFO);
 CHECK(script.CallFunction("main",0,""));CHECK(sent==1);
 minute=135;CHECK(script.CallFunction("main",0,""));CHECK(sent==2 && strstr(message,"5"));
 minute=139;CHECK(script.CallFunction("main",0,""));CHECK(sent==3);
 minute=140;CHECK(script.CallFunction("main",0,""));CHECK(sent==3);
 minute=145;CHECK(script.CallFunction("main",0,""));CHECK(sent==3); // no false combat notice
 minute=240;CHECK(script.CallFunction("main",0,""));CHECK(sent==4); // original TIP
 minute=720;CHECK(script.CallFunction("main",0,""));CHECK(sent==5); // boss 12:01 warning
 minute=719;CHECK(script.CallFunction("main",0,""));CHECK(sent==5); // backward clock
 alive=1;minute=12*60+15;int previous=sent;CHECK(script.CallFunction("main",0,""));CHECK(sent<=previous+1); // boss alive; only possible TIP
 phase=2;minute=1440+145;previous=sent;CHECK(script.CallFunction("main",0,""));CHECK(sent==previous+1);
 printf("PASS SERVER_NEWS checks=%d VTT_10_5_1=PASS original_tip=PASS boss_warning=PASS minute_dedup=PASS backward_clock=PASS phase_guard=PASS native_wire_roundtrip=PASS\n",checks);return 0;
}
