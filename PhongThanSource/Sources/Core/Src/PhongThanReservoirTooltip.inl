// A dedicated formatter state avoids the legacy client's five gameplay-script
// slots. No GameScriptFuns are registered; hover receives only a numeric snapshot.
static BOOL PTReservoirDescribe(const char* path,int remaining,char* out,int size)
{
    if(!path || !out || size<1) return FALSE;
    out[0]=0;
    static KLuaScript script;
    static char loaded[128]={0};
    if(strcmp(loaded,path)) {
        script.Exit();loaded[0]=0;
        if(strlen(path)>=sizeof(loaded) || !script.Init() || !script.Load((char*)path)) return FALSE;
        strcpy(loaded,path);
    }
    int top=0;BOOL ok=FALSE;script.SafeCallBegin(&top);
    if(script.CallFunction("GetDesc",1,"d",remaining) &&
       Lua_IsString(script.m_LuaState,Lua_GetTopIndex(script.m_LuaState))) {
        const char* value=Lua_ValueToString(script.m_LuaState,Lua_GetTopIndex(script.m_LuaState));
        if(value) {strncpy(out,value,size-1);out[size-1]=0;ok=TRUE;}
    }
    script.SafeCallEnd(top);return ok;
}
