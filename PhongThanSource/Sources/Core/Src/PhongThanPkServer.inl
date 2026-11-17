#include "../../../Headers/PhongThanPkContract.h"
#include "../../../Headers/PhongThanPkText.h"
#include "KPlayerChat.h"
static void PhongThanPkFeedback(int player,const char* text)
{
    char message[192],sender[]="PK";
    strncpy(message,text,sizeof(message)-1);message[sizeof(message)-1]=0;
    KPlayerChat::SendSystemInfo(1,player,sender,message,strlen(message),-1);
}
// RECONSTRUCTED_FROM_ORIGINAL_CLIENT. Original server policy is unavailable.
// Preserve current locks/rate/high-PK restrictions; do not mutate FightMode.
static void PhongThanProcessPkRequest(int player, const BYTE* data, unsigned int size)
{
    if (!PtPkValidPacket(data,size) || player <= 0 || player >= MAX_PLAYER) return;
    int npc = Player[player].m_nIndex;
    if (npc <= 0 || npc >= MAX_NPC) return;
    const int camp = data[1];
    if(PhongThanEscortPkLocked(player) && camp!=2) {
        PhongThanPkFeedback(player,"V\313n L\255\254ng: kh\253ng th\323 \256\346i phe PK khi \256ang gi\367 xe.");
        return;
    }
    const char* reason = NULL;
    if (!g_pServer || Npc[npc].m_Kind != kind_player || !Npc[npc].IsAlive() ||
        Npc[npc].m_SubWorldIndex < 0 || Npc[npc].m_RegionIndex < 0) reason = "WORLD_INVALID";
    else if (!Npc[npc].m_Camp) reason = "NOVICE";
    else if (!PtPkSelectable(Npc[npc].m_CurrentCamp)) reason = "SPECIAL_CAMP_UNSUPPORTED";
    else if (Player[player].m_bForbidCamp) reason = "CAMP_LOCKED";
    else if (Player[player].CheckTrading()) reason = "TRADING";
    else if (Player[player].m_cTeam.m_nFlag) reason = "TEAM_POLICY_UNRESOLVED";
    else if (Player[player].m_nNextLevelExp <= 0) reason = "EXP_STATE_INVALID";
    if (reason) {
        g_DebugLog("[PT_PK_REJECT] player=%d camp=%d reason=%s",player,camp,reason);
        if (g_pServer) PhongThanPkFeedback(player,!Npc[npc].m_Camp ? PT_PK_NOVICE : PT_PK_BLOCKED);
        return;
    }
    if (Npc[npc].m_CurrentCamp == camp) { PhongThanPkFeedback(player,PtPkStateText(camp)); return; }
    // These flags retain existing server guards only, not PT faction identity.
    BYTE guard = camp == 7 ? enumPKNormal : camp == 8 ? enumPKMurder : enumPKWar;
    if (!Player[player].m_cPK.CheckSwitchPK(guard,FALSE)) {
        g_DebugLog("[PT_PK_REJECT] player=%d camp=%d reason=EXISTING_PK_GUARD",player,camp);
        PhongThanPkFeedback(player,PT_PK_BLOCKED);
        return;
    }
    int oldCamp = Npc[npc].m_CurrentCamp;
    Player[player].m_cPK.SetLockPKState(guard,Player[player].m_cPK.GetLockPKState());
    Npc[npc].SetCamp(camp);
    Npc[npc].SetCurrentCamp(camp);
    PhongThanPkFeedback(player,PtPkStateText(Npc[npc].m_CurrentCamp));
    g_DebugLog("[PT_PK_ACCEPT] player=%d old=%d camp=%d fight=%d",player,oldCamp,
        Npc[npc].m_CurrentCamp,Npc[npc].m_FightMode);
}
