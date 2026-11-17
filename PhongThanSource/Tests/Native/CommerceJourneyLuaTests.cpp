#include <windows.h>
#include <stdio.h>
#include <string>
#include "KWin32.h"
#include "KLuaScript.h"
static std::string base,fallback;
static bool read(const std::string& path,std::string& text){FILE* f=fopen(path.c_str(),"rb");if(!f)return false;char b[4096];int n;while((n=fread(b,1,sizeof(b),f))>0)text.append(b,n);fclose(f);return true;}
static int include(Lua_State* L){
 const char* arg=Lua_ValueToString(L,1);if(!arg)return 0;std::string rel=arg;
 for(int i=0;i<(int)rel.size();++i)if(rel[i]=='\\')rel[i]='/';if(rel[0]!='/')rel='/'+rel;
 std::string data,path=base+rel;if(!read(path,data)){data="";path=fallback+rel;if(!read(path,data)){printf("Missing include %s\n",path.c_str());lua_error(L,"missing include");}}
 if(Lua_ExecuteBuffer(L,data.c_str(),data.size(),path.c_str())){printf("Include error %s: %s\n",path.c_str(),lua_tostring(L,-1));lua_error(L,"include error");}return 0;
}
int main(int argc,char** argv){
 if(argc!=4)return 2;base=argv[1];fallback=argv[2];std::string text;if(!read(argv[3],text))return 3;
 KLuaScript script;if(!script.Init())return 4;script.RegisterFunction("Include",(void*)include);
 if(Lua_ExecuteBuffer(script.m_LuaState,text.c_str(),text.size(),argv[3])){printf("FAIL %s\n",lua_tostring(script.m_LuaState,-1));return 5;}return 0;
}
