// Le Quan claims must be made beside an actual server NPC in a starter village.
#ifdef _SERVER
static bool PhongThanNearLeQuan(int player) {
    if(player<=0 || player>=MAX_PLAYER)return false;
    int actor=Player[player].m_nIndex;
    if(actor<=0 || actor>=MAX_NPC || !Npc[actor].m_dwID)return false;
    int world=Npc[actor].m_SubWorldIndex;
    if(world<0 || world>=MAX_SUBWORLD)return false;
    int map=SubWorld[world].m_SubWorldID;
    if(map!=1002 && map!=1003 && map!=1004)return false;
    int x,y;Npc[actor].GetMpsPos(&x,&y);
    for(int i=1;i<MAX_NPC;++i) {
        if(!Npc[i].m_dwID || Npc[i].m_Kind!=kind_dialoger ||
           Npc[i].m_SubWorldIndex!=world || Npc[i].m_NpcSettingIdx!=188 ||
           strcmp(Npc[i].Name,"\xC0\xF1\xB9\xD9"))continue;
        int nx,ny;Npc[i].GetMpsPos(&nx,&ny);
        if(abs(nx-x)<=512 && abs(ny-y)<=512)return true;
    }
    return false;
}
int LuaNearLeQuan(Lua_State* L) {
    Lua_PushNumber(L,PhongThanNearLeQuan(GetPlayerIndex(L))?1:0);return 1;
}
int LuaLeQuanStarterXu(Lua_State* L) {
    int p=GetPlayerIndex(L),result=0;
    if(PhongThanNearLeQuan(p) && Player[p].m_cTask.GetSaveVal(4700)!=2) {
        if(Player[p].m_cTask.GetSaveVal(4703)==1)result=1;
        else {
            PhongThanQuestExchangeRow reward;
            if(PhongThanDecodeQuestItemTuple(4,47,0,0,0,0,reward.item)) {
                reward.count=100;
                result=PhongThanQuestExchange(p,4703,0,1,0,0,&reward,1,true);
            }
        }
    }
    Lua_PushNumber(L,result);return 1;
}
#endif
