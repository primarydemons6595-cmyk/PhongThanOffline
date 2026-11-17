extern void PhongThanPillsTick(KPlayer& player);
static int PTPillPlayer(Lua_State* L) {int player=GetPlayerIndex(L);return PhongThanDieuTriIsLivePlayer(player)?player:0;}
int LuaGetPillsCountVng(Lua_State* L) {int p=PTPillPlayer(L);Lua_PushNumber(L,p?Player[p].m_cTask.GetSaveVal(TASKVALUE_PT_PILLS_COUNT):0);return 1;}
int LuaGetNewPillsVng(Lua_State* L) {int p=PTPillPlayer(L);Lua_PushNumber(L,p?Player[p].m_cTask.GetSaveVal(TASKVALUE_PT_PILLS_NEW):0);return 1;}
int LuaGetCanMakePillsVng(Lua_State* L) {
    int p=PTPillPlayer(L),allowed=0;
    if(p) {
        int day=((int)KSG_GetCurSec()+25200)/86400;
        int made=Player[p].m_cTask.GetSaveVal(TASKVALUE_PT_PILLS_DAY)==day?Player[p].m_cTask.GetSaveVal(TASKVALUE_PT_PILLS_MADE):0;
        allowed=made<12 && Player[p].m_cTask.GetSaveVal(TASKVALUE_PT_PILLS_NEW)<12;
    }
    Lua_PushNumber(L,allowed);return 1;
}
int LuaStartMakePillsVng(Lua_State* L) {
    int p=PTPillPlayer(L);if(!p)return 0;
    int day=((int)KSG_GetCurSec()+25200)/86400;
    if(Player[p].m_cTask.GetSaveVal(TASKVALUE_PT_PILLS_DAY)!=day) {
        Player[p].m_cTask.SetSaveVal(TASKVALUE_PT_PILLS_DAY,day,TRUE);
        Player[p].m_cTask.SetSaveVal(TASKVALUE_PT_PILLS_MADE,0,TRUE);
    }
    if(Player[p].m_cTask.GetSaveVal(TASKVALUE_PT_PILLS_MADE)<12 && Player[p].m_cTask.GetSaveVal(TASKVALUE_PT_PILLS_NEW)<12) {
        Player[p].m_cTask.SetSaveVal(TASKVALUE_PT_PILLS_ARMED,1,TRUE);
        Player[p].m_cTask.SetSaveVal(TASKVALUE_PT_PILLS_LAST,(int)KSG_GetCurSec());
    }
    return 0;
}
int LuaGetAllPillsVng(Lua_State* L) {
    int p=PTPillPlayer(L);if(!p)return 0;
    int count=Player[p].m_cTask.GetSaveVal(TASKVALUE_PT_PILLS_COUNT),fresh=Player[p].m_cTask.GetSaveVal(TASKVALUE_PT_PILLS_NEW);
    if(count<0 || count>84 || fresh<0 || fresh>12)return 0;
    int take=fresh<84-count?fresh:84-count;
    Player[p].m_cTask.SetSaveVal(TASKVALUE_PT_PILLS_COUNT,count+take,TRUE);
    Player[p].m_cTask.SetSaveVal(TASKVALUE_PT_PILLS_NEW,fresh-take,TRUE);return 0;
}
int LuaStartUsePillsVng(Lua_State* L) {
    int p=PTPillPlayer(L);if(!p)return 0;PhongThanPillsTick(Player[p]);
    if(Lua_GetTopIndex(L)<1 || !Lua_IsNumber(L,1))return 0;
    int bonus=(int)Lua_ValueToNumber(L,1);if(bonus!=50 && bonus!=75)return 0;
    if(Player[p].m_cTask.GetSaveVal(TASKVALUE_PT_PILLS_UNTIL)>(int)KSG_GetCurSec())return 0;
    int count=Player[p].m_cTask.GetSaveVal(TASKVALUE_PT_PILLS_COUNT);if(count<1 || count>84)return 0;
    Player[p].m_cTask.SetSaveVal(TASKVALUE_PT_PILLS_COUNT,count-1,TRUE);
    Player[p].m_cTask.SetSaveVal(TASKVALUE_PT_PILLS_BONUS,bonus,TRUE);
    Player[p].m_cTask.SetSaveVal(TASKVALUE_PT_PILLS_UNTIL,(int)KSG_GetCurSec()+600,TRUE);return 0;
}
int LuaGetPillsStateVng(Lua_State* L) {
    int p=PTPillPlayer(L),state=0;
    if(p) {PhongThanPillsTick(Player[p]);state=Player[p].m_cTask.GetSaveVal(TASKVALUE_PT_PILLS_UNTIL)>(int)KSG_GetCurSec()?2:
        Player[p].m_cTask.GetSaveVal(TASKVALUE_PT_PILLS_ARMED)?1:0;}
    Lua_PushNumber(L,state);return 1;
}
