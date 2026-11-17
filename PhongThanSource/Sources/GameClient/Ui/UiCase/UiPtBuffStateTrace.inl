// Event-only probe: state identities/config matches, never each timer/frame.
#include <stdio.h>
static void TracePtBuffState(const KStateControl* states, int count, int matched, int configured)
{
    static int lastCount = -1, lastMatched = -1, lastConfigured = -1;
    static int lastIds[MAX_BUFF_ROLE];
    int ids[MAX_BUFF_ROLE];
    ZeroMemory(ids, sizeof(ids));
    int shown = count < MAX_BUFF_ROLE ? count : MAX_BUFF_ROLE;
    if (states) for (int i = 0; i < shown; ++i) ids[i] = states[i].nSkillId;
    if (lastCount == count && lastMatched == matched && lastConfigured == configured &&
        memcmp(lastIds, ids, sizeof(ids)) == 0) return;
    lastCount = count; lastMatched = matched; lastConfigured = configured;
    memcpy(lastIds, ids, sizeof(ids));
    FILE* f = fopen("ui_buff_state_trace.log", "ab");
    if (!f) return;
    SYSTEMTIME t; GetLocalTime(&t);
    fprintf(f, "%04u-%02u-%02u %02u:%02u:%02u states=%d matched=%d configured=%d skill_ids=",
        t.wYear,t.wMonth,t.wDay,t.wHour,t.wMinute,t.wSecond,count,matched,configured);
    for (int j = 0; j < shown; ++j) fprintf(f, "%s%d",j ? "," : "",ids[j]);
    fprintf(f, "\r\n"); fclose(f);
}
