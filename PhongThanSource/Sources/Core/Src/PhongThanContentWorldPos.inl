// Explicit content-ID views for recovered PT scripts. Project-authored Lua
// keeps GetWorldPos/SubWorldIdx2ID and therefore retains runtime identities.
int LuaGetContentWorldPos(Lua_State* L)
{
    const int top = Lua_GetTopIndex(L);
    const int count = LuaGetNewWorldPos(L);
    if (count != 3) return count;
    const int runtime = (int)Lua_ValueToNumber(L, -3);
    const double x = Lua_ValueToNumber(L, -2), y = Lua_ValueToNumber(L, -1);
    PHONGTHAN_MAP_RESOLUTION resolution;
    const int content = PhongThanResolveRuntimeMap(runtime, &resolution) ?
        resolution.ContentMapId : runtime;
    Lua_SetTopIndex(L, top);
    Lua_PushNumber(L, content); Lua_PushNumber(L, x); Lua_PushNumber(L, y);
    return 3;
}

int LuaSubWorldIndexToContentID(Lua_State* L)
{
    const int top = Lua_GetTopIndex(L);
    const int count = LuaSubWorldIndexToID(L);
    if (count != 1) return count;
    const int runtime = (int)Lua_ValueToNumber(L, -1);
    PHONGTHAN_MAP_RESOLUTION resolution;
    const int content = PhongThanResolveRuntimeMap(runtime, &resolution) ?
        resolution.ContentMapId : runtime;
    Lua_SetTopIndex(L, top); Lua_PushNumber(L, content);
    return 1;
}
