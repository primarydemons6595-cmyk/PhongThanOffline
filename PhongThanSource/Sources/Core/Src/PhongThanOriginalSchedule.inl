#include "KTabFile.h"
#include "KIniFile.h"
#include "KPlayerTask.h"
#include "KNpcSet.h"
#include "KNpc.h"
#include "../../../Headers/PhongThanMapIdentity.h"
#include "../../../Headers/PhongThanScheduleClock.h"
#include <vector>

struct PhongThanOriginalScheduleRow
{
    int Hour, Minute;
    char Path[240];
};

static void PhongThanOriginalScheduleTick()
{
    static bool initialized = false;
    static int offset = 420;
    static PhongThanScheduleClock clock;
    static std::vector<PhongThanOriginalScheduleRow> rows;
    if (!initialized)
    {
        initialized = true;
        KIniFile config;
        int enabled = 0;
        if (config.Load("\\settings\\phongthan\\OriginalWorldSchedule.ini"))
        {
            config.GetInteger("Schedule", "Enabled", 0, &enabled);
            config.GetInteger("Schedule", "UtcOffsetMinutes", 420, &offset);
        }
        if (!enabled || offset < -720 || offset > 840) return;
        // These slots are derived from matching original spawn/death scripts.
        // Static bosses already present in a map must not be duplicated.
        KTabFile boot;
        if (boot.Load("\\settings\\phongthan\\OriginalBossState.txt"))
        {
            for (int k = 2; k <= boot.GetHeight(); ++k)
            {
                int slot = -1, tid = -1, content = 0;
                boot.GetInteger(k, "Flag", -1, &slot);
                boot.GetInteger(k, "Template", -1, &tid);
                boot.GetInteger(k, "ContentMap", 0, &content);
                PHONGTHAN_MAP_RESOLUTION resolution;
                if (slot < 0 || slot >= TASKGLOBALVALUENUM || tid < 0 ||
                    !PhongThanResolveContentMap(content, &resolution)) continue;
                bool present = false;
                for (int npc = NpcSet.GetNextIdx(0); npc > 0; npc = NpcSet.GetNextIdx(npc))
                {
                    const int world = Npc[npc].m_SubWorldIndex;
                    if (Npc[npc].m_NpcSettingIdx == tid && Npc[npc].m_CurrentLife>0 && Npc[npc].IsAlive() && !PhongThanRealmIsHuntNpc(npc) && world >= 0 && world < MAX_SUBWORLD &&
                        SubWorld[world].m_SubWorldID == resolution.RuntimeMapId) present = true;
                }
                g_TaskGlobalValue[slot] = present ? 0 : -1;
                g_DebugLog("[OriginalSchedule] boss_flag=%d template=%d present=%d", slot, tid, present);
            }
        }
        KTabFile table;
        if (!table.Load("\\settings\\phongthan\\OriginalWorldSchedule.txt"))
        { g_DebugLog("[OriginalSchedule] missing reviewed schedule table"); return; }
        for (int i = 2; i <= table.GetHeight(); ++i)
        {
            PhongThanOriginalScheduleRow row;
            ZeroMemory(&row, sizeof(row));
            table.GetInteger(i, "Hour", -1, &row.Hour);
            table.GetInteger(i, "Minute", -1, &row.Minute);
            table.GetString(i, "Script", "", row.Path, sizeof(row.Path));
            if (row.Hour < 0 || row.Hour > 23 || row.Minute < 0 || row.Minute > 59 ||
                strnicmp(row.Path, "\\script\\", 8) || strstr(row.Path, ".."))
            { g_DebugLog("[OriginalSchedule] invalid row=%d", i); continue; }
            bool duplicate = false;
            for (unsigned int j = 0; j < rows.size(); ++j)
                if (rows[j].Hour == row.Hour && rows[j].Minute == row.Minute &&
                    !stricmp(rows[j].Path, row.Path)) duplicate = true;
            if (!duplicate) rows.push_back(row);
        }
        g_DebugLog("[OriginalSchedule] loaded=%u utc_offset=%d", rows.size(), offset);
        FILE* status = fopen("original_schedule_status.log", "a");
        if (status) { fprintf(status,"READY utc=%ld rows=%u utc_offset=%d enabled=1\n",(long)time(NULL),rows.size(),offset); fclose(status); }
    }
    if (rows.empty()) return;
    const time_t utc = time(NULL);
    if (utc < 0 || !clock.Advance((long)(utc / 60))) return;
    const time_t local = utc + offset * 60;
    struct tm* calendar = gmtime(&local);
    if (!calendar) return;
    const int hour = calendar->tm_hour, minute = calendar->tm_min;
    for (unsigned int n = 0; n < rows.size(); ++n)
    {
        const PhongThanOriginalScheduleRow& row = rows[n];
        if (row.Hour != hour || row.Minute != minute) continue;
        if (PhongThanEventOwnsSchedule(row.Path)) continue;
        KLuaScript* script = (KLuaScript*)g_GetScript(row.Path);
        if (!script && ReLoadScript(row.Path)) script = (KLuaScript*)g_GetScript(row.Path);
        if (!script)
        { g_DebugLog("[OriginalSchedule] LOAD_FAILED path=%s", row.Path); continue; }
        int top = 0;
        script->SafeCallBegin(&top);
        const BOOL ok = script->CallFunction("main", 0, "");
        script->SafeCallEnd(top);
        g_DebugLog("[OriginalSchedule] hour=%d minute=%d status=%s path=%s",
            hour, minute, ok ? "CALLED" : "FAILED", row.Path);
        FILE* status = fopen("original_schedule_status.log", "a");
        if (status) { fprintf(status,"DISPATCH utc=%ld hour=%d minute=%d status=%s path=%s\n",(long)utc,hour,minute,ok?"CALLED":"FAILED",row.Path); fclose(status); }
    }
}
