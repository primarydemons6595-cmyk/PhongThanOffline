#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "KWin32.h"
#include "KStrBase.h"
#include "KLuaScript.h"
static int calls=0;
static BOOL CallNpcLevelString(KLuaScript* script,int level,const char* column,const char* param,int* result) {
    ++calls;int top=0;script->SafeCallBegin(&top);
    BOOL ok=script->CallFunction("GetNpcLevelData",1,"dss",level,(char*)column,(char*)param);
    if(ok && Lua_IsNumber(script->m_LuaState,Lua_GetTopIndex(script->m_LuaState)))
        *result=(int)Lua_ValueToNumber(script->m_LuaState,Lua_GetTopIndex(script->m_LuaState));
    else ok=FALSE;
    script->SafeCallEnd(top);return ok;
}
static int Read(KLuaScript* pScript,int nSkillId,const char* szLevelParam,int level) {
    struct Npc { int m_Level; } npc; npc.m_Level=level;Npc* pNpc=&npc;
    const char* szLevelColumn="Level1";
    int nSkillLevel=atoi(szLevelParam);const char* sep=strchr(szLevelParam,'|');
    if(sep)nSkillLevel+=atoi(sep+1)*level;
#include "NpcSkillLevelDispatch.inl"
    return nSkillLevel;
}
int main(int argc,char** argv) {
    assert(argc==2);FILE* f=fopen(argv[1],"rb");assert(f);fseek(f,0,SEEK_END);int size=ftell(f);rewind(f);
    unsigned char* data=new unsigned char[size+1];assert(fread(data,1,size,f)==size);fclose(f);data[size]=0;
    KLuaScript script;assert(script.Init());assert(script.LoadBuffer(data,size));assert(script.ExecuteCode());delete[] data;
    // The real original Lua function rejects empty parameters. Dispatch must skip them.
    assert(Read(&script,0,"",90)==0 && calls==0);
    assert(Read(&script,123,"",90)==0 && calls==0);
    assert(Read(&script,0,"1|0",90)==1 && calls==0);
    assert(Read(&script,123,"1|0",90)==1 && calls==1);
    assert(Read(&script,123,"2|3",90)==272 && calls==2);
    assert(Read(&script,123,"0|0",200)==0 && calls==3);
    puts("PASS production NPC dispatch with original Lua4: empty and unused slots skipped; valid level formulas preserved");return 0;
}
