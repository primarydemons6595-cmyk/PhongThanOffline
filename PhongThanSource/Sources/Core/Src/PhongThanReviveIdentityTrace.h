#ifndef PHONGTHAN_REVIVE_IDENTITY_TRACE_H
#define PHONGTHAN_REVIVE_IDENTITY_TRACE_H
#include <stdio.h>
// Event-only trace: no account credentials, inventory, skills or task payloads.
// PID separates multiple clients writing to the same runtime directory.
inline void PhongThanReviveIdentityTrace(const char* event, int playerIndex,
    unsigned int incomingEntity = 0, const char* incomingName = "",
    int incomingTemplate = 0, int incomingSeries = -1)
{
    if (playerIndex <= 0 || playerIndex >= MAX_PLAYER) return;
    const int actor = Player[playerIndex].m_nIndex;
    FILE* log = fopen("revive_identity_diag.log", "a");
    if (!log) return;
#ifdef _SERVER
    const char* side = "server";
#else
    const char* side = "client";
#endif
    fprintf(log, "pid=%lu tick=%lu side=%s event=%s player=%d actor=%d player_id=%lu",
        (unsigned long)GetCurrentProcessId(), (unsigned long)GetTickCount(),
        side, event, playerIndex, actor, (unsigned long)Player[playerIndex].m_dwID);
    if (actor > 0 && actor < MAX_NPC) {
        const KNpc& npc = Npc[actor];
        fprintf(log, " entity=%lu name=%.31s template=%d series=%d kind=%d doing=%d region=%d",
            (unsigned long)npc.m_dwID, npc.Name, npc.m_NpcSettingIdx,
            npc.m_Series, npc.m_Kind, npc.m_Doing, npc.m_RegionIndex);
#ifndef _SERVER
        fprintf(log, " helm=%d armor=%d weapon=%d mask=%d mark_mask=%d",
            npc.m_Appearance.Helm.nResourceId, npc.m_Appearance.Armor.nResourceId,
            npc.m_Appearance.Weapon.nResourceId, npc.m_MaskType, npc.m_MarkMask);
#endif
    }
    fprintf(log, " incoming_entity=%u incoming_name=%.31s incoming_template=%d incoming_series=%d\n",
        incomingEntity, incomingName ? incomingName : "", incomingTemplate, incomingSeries);
    fclose(log);
}
#endif
