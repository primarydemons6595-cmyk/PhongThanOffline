// Native inventory deltas bypass the legacy aggregate socket buffer. Round
// fences must use that SAME native path, otherwise RecoverTrade arrives late
// and restores items already removed/transferred by the preceding round.
static void PhongThanSendTradeFence(KPlayer& player,int operation,int decision) {
    int actor=player.m_nIndex;if(actor<=0 || actor>=MAX_NPC)return;
    int world=Npc[actor].m_SubWorldIndex;if(world<0 || world>=MAX_SUBWORLD)return;
    PHONGTHAN_PLAYER_EVENT packet;memset(&packet,0,sizeof(packet));
    PhongThanInitializeWireHeader(&packet.Header,PHONGTHAN_MSG_UI_PLAYER_EVENT,sizeof(packet),PHONGTHAN_WIRE_FLAG_RESPONSE,0);
    packet.MapId=SubWorld[world].m_SubWorldID;packet.EntityId=Npc[actor].m_dwID;
    packet.Operation=(PHONGTHAN_U16)operation;packet.Value=decision;
    g_pServer->PackDataToClient(player.m_nNetConnectIdx,&packet,sizeof(packet));
}

static void PhongThanSendRpsRound(KPlayer& p,int decision) { PhongThanSendTradeFence(p,PHONGTHAN_PLAYER_RPS_ROUND,decision); }
