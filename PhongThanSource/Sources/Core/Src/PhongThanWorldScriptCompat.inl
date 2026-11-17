// Shared by production ScriptFuns.cpp and the native Lua contract regression.
int LuaSubWorldIDToIndex(Lua_State * L)
{
    int index = -1;
    if (Lua_GetTopIndex(L) >= 1 && Lua_IsNumber(L, 1))
    {
        const int requested = (int)Lua_ValueToNumber(L, 1);
        // Preserve an explicitly active runtime world. Original PT scripts
        // otherwise use content IDs, including the 60000+ map family.
        index = g_SubWorldSet.SearchWorld(requested);
        if (index < 0 && requested > 0)
        {
            PHONGTHAN_MAP_RESOLUTION resolution;
            if (PhongThanResolveContentMap(requested, &resolution))
                index = g_SubWorldSet.SearchWorld(resolution.RuntimeMapId);
        }
    }
    Lua_PushNumber(L, index);
    return 1;
}

int LuaAddNpc(Lua_State * L)
{
    int result = 0;
    const int count = Lua_GetTopIndex(L);
    if (count >= 5)
    {
        int id = -1;
        if (Lua_IsNumber(L, 1)) id = (int)Lua_ValueToNumber(L, 1);
        else if (Lua_IsString(L, 1))
            id = g_NpcSetting.FindRow((char*)lua_tostring(L, 1)) - 2;
        int level = (int)Lua_ValueToNumber(L, 2);
        const int world = (int)Lua_ValueToNumber(L, 3);
        const int x = (int)Lua_ValueToNumber(L, 4);
        const int y = (int)Lua_ValueToNumber(L, 5);
        if (level <= 0) level = 1;
        if (id >= 0 && id <= g_NpcSetting.GetHeight() - 2 &&
            level <= 65535 && world >= 0 && world < MAX_SUBWORLD && x >= 0 && y >= 0)
        {
            const BOOL barrier = count >= 6 ? (BOOL)Lua_ValueToNumber(L, 6) : FALSE;
            result = NpcSet.Add(MAKELONG(level, id), world, x, y, barrier);
#ifdef _SERVER
            if(result>0) PhongThanEventBindNpc(result);
#endif
        }
    }
    // Original five-argument callers must receive a numeric failure, never nil.
    Lua_PushNumber(L, result);
    return 1;
}
