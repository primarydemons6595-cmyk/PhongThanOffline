#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include "KWin32.h"
#include "KLuaScript.h"
static bool Load(KLuaScript& script,const char* file) {
    FILE* f=fopen(file,"rb");if(!f)return false;
    fseek(f,0,SEEK_END);long size=ftell(f);rewind(f);
    unsigned char* data=(unsigned char*)malloc(size+1);
    fread(data,1,size,f);fclose(f);data[size]=0;
    bool ok=script.LoadBuffer(data,size)!=FALSE;free(data);
    return ok && script.ExecuteCode()!=FALSE;
}
int main(int argc,char**argv) {
    if(argc!=2)return 2;
    SetErrorMode(SEM_FAILCRITICALERRORS|SEM_NOGPFAULTERRORBOX);
    SetCurrentDirectoryA(argv[1]);
    KLuaScript script;if(!script.Init())return 3;
    if(!Load(script,"Tests/fixtures/beginner-gift.lua"))return 4;
    if(!Load(script,"Deploy/ProjectContent/script/player/playerlogin.lua"))return 5;
    if(!script.CallFunction("TestBeginnerLogin",0,""))return 6;
    if(!Load(script,"Deploy/ProjectContent/script/item/camnangtanthu.lua"))return 7;
    if(!script.CallFunction("TestBeginnerGift",0,""))return 8;
    puts("PASS BEGINNER_GIFT_LUA real_Lua4 level_guards careers callbacks repeat partial_resume wallet_retry consume_retry bound_equipment");
    return 0;
}
