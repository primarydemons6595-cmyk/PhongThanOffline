// Rental tasks survive logout; the transient morph extension does not.
// This runs after task restoration and is also safe for older active rentals.
void PhongThanRestoreCommerceCamel(int player)
{
    if(player<=0 || player>=MAX_PLAYER)return;
    const int origin=Player[player].m_cTask.GetSaveVal(60);
    const int kind=Player[player].m_cTask.GetSaveVal(332);
    if(origin<1 || origin>9 || kind<1 || kind>5)return;
    PHONGTHAN_PLAYER_NPC_STATE* state=PhongThanPlayerState(player);
    if(!state || state->nMorphTemplate>=0)return;
    const int actor=Player[player].m_nIndex;
    state->nMorphTemplate=364;
    state->nMorphDeadline=0;
    Npc[actor].m_NpcSettingIdx=364;
    PHONGTHAN_IBBUFF_STORE* buffs=GetPlayerIBBuffStore(player);
    if(buffs && FindIBBuffSlot(buffs,160)<0 && AddIBBuffToStore(buffs,actor,160,0,0))
        PersistPlayerIBBuffStore(player);
    Npc[actor].SendSyncData(0,TRUE);
}
