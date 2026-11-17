// The original Phu The script calls AddWeightMax for its permanent 20-point
// endurance reward. Base stamina is serialized as CHARACTER_STATE.MaxStamina.
#ifdef _SERVER
int LuaAddWeightMaxSharedQuest(Lua_State* L)
{
    const int player=GetPlayerIndex(L);int result=0;
    if(player>0 && player<MAX_PLAYER && Lua_GetTopIndex(L)==1 &&
       Lua_IsNumber(L,1) && Lua_ValueToNumber(L,1)==20 &&
       Player[player].m_nIndex>0 && Player[player].m_nIndex<MAX_NPC &&
       Player[player].m_cTask.GetSaveVal(92)==6 &&
       Player[player].m_cTask.GetSaveVal(4572)==0){
        KNpc& npc=Npc[Player[player].m_nIndex];
        if(npc.m_StaminaMax>=0 && npc.m_StaminaMax<2000000000-20){
            Player[player].m_cTask.SetSaveVal(TASKVALUE_PT_CARRY_BONUS,Player[player].m_cTask.GetSaveVal(TASKVALUE_PT_CARRY_BONUS)+20,TRUE);
            Player[player].m_cTask.SetSaveVal(4572,1,TRUE);
            PHONGTHAN_LEVEL_EVENT packet;ZeroMemory(&packet,sizeof(packet));
            PhongThanInitializeWireHeader(&packet.Header,PHONGTHAN_MSG_GAMEPLAY_LEVEL,sizeof(packet),PHONGTHAN_WIRE_FLAG_RESPONSE,0);
            packet.MapId=SubWorld[npc.m_SubWorldIndex].m_SubWorldID;
            packet.EntityId=npc.m_dwID;packet.ResetAttributes=FALSE;
            packet.Level=npc.m_Level;packet.Experience=Player[player].m_nExp;
            packet.AttributePoints=Player[player].m_nAttributePoint;
            packet.SkillPoints=Player[player].m_nSkillPoint;
            packet.BaseMaxLife=npc.m_LifeMax;packet.BaseMaxMana=npc.m_ManaMax;
            packet.BaseMaxStamina=npc.m_StaminaMax;
            if(g_pServer)g_pServer->PackDataToClient(Player[player].m_nNetConnectIdx,(BYTE*)&packet,sizeof(packet));
            result=1;
        }
    }
    Lua_PushNumber(L,result);return 1;
}
#endif
