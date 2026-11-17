#include "../../../Headers/PhongThanPkContract.h"
static int PhongThanPlayerRelation(int a, int b)
{
    if (Npc[a].m_Kind != kind_player || Npc[b].m_Kind != kind_player) return -1;
    int relation = PtPkBaseRelation(Npc[a].m_CurrentCamp,Npc[b].m_CurrentCamp);
    if (relation < 0) return -1; // Leave unrelated/special legacy camps alone.
    if (!Npc[a].IsAlive() || !Npc[b].IsAlive()) return relation_none;
#ifdef _SERVER
    int pa = Npc[a].GetPlayerIdx(), pb = Npc[b].GetPlayerIdx();
    if (pa <= 0 || pa >= MAX_PLAYER || pb <= 0 || pb >= MAX_PLAYER) return relation_none;
    if (Player[pa].m_cPK.GetExercisePKAim() == pb ||
        (Player[pa].m_cPK.GetEnmityPKState() == enumPK_ENMITY_STATE_PKING &&
         Player[pa].m_cPK.GetEnmityPKAim() == pb)) return relation_enemy;
    if (Player[pa].m_cTeam.m_nFlag && Player[pb].m_cTeam.m_nFlag &&
        Player[pa].m_cTeam.m_nID >= 0 &&
        Player[pa].m_cTeam.m_nID == Player[pb].m_cTeam.m_nID) return relation_ally;
#else
    int local = Player[CLIENT_PLAYER_INDEX].m_nIndex;
    int other = local == a ? b : local == b ? a : 0;
    if (other && (Player[CLIENT_PLAYER_INDEX].m_cPK.GetExercisePKAim() == Npc[other].m_dwID ||
        (Player[CLIENT_PLAYER_INDEX].m_cPK.GetEnmityPKState() == enumPK_ENMITY_STATE_PKING &&
         Player[CLIENT_PLAYER_INDEX].m_cPK.GetEnmityPKAimNpcID() == Npc[other].m_dwID)))
        return relation_enemy;
    if (Npc[a].m_nTeamServerID >= 0 && Npc[a].m_nTeamServerID == Npc[b].m_nTeamServerID)
        return relation_ally;
#endif
    if (relation == relation_enemy && (!Npc[a].m_FightMode || !Npc[b].m_FightMode))
        return relation_none;
    return relation;
}
