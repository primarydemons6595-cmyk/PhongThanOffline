#ifndef PHONGTHAN_DROP_RULES_H
#define PHONGTHAN_DROP_RULES_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
// NEW_PROJECT_RULES_V1: not an implementation of the original PT drop scales.
// One cumulative roll per Draw. Unassigned range means no drop. Zero weight
// never wins. Repeated wins are allowed; each emits Quantity separate objects.
struct PtDropEntry
{
    int genre, detail, particular, level, series, luck, weight, quantity;
};
struct PtDropTable
{
    int draws, range, count;
    PtDropEntry entries[128];
};
inline bool PtDropNameValid(const char* s)
{
    int n = 0;
    if (!s || !*s) return false;
    for (; s[n]; ++n)
        if (n >= 31 || !((s[n] >= 'a' && s[n] <= 'z') ||
            (s[n] >= 'A' && s[n] <= 'Z') ||
            (s[n] >= '0' && s[n] <= '9') || s[n] == '_')) return false;
    return true;
}
inline bool PtDropValidate(const PtDropTable& table)
{
    if (table.draws < 1 || table.draws > 10 || table.range < 1 ||
        table.range > 100000000 || table.count < 1 || table.count > 128) return false;
    int total = 0;
    for (int i = 0; i < table.count; ++i)
    {
        const PtDropEntry& e = table.entries[i];
        if (!(e.genre == 0 || e.genre == 1 || e.genre == 3 || e.genre == 4 ||
            e.genre == 6 || e.genre == 7 || e.genre == 8) || e.detail < 0 ||
            e.particular < 0 || e.level < 0 || e.series < 0 || e.series > 5 ||
            e.luck != 0 || e.quantity < 1 || e.quantity > 20 ||
            e.weight < 0 || e.weight > table.range - total) return false;
        total += e.weight;
    }
    return true;
}
inline int PtDropPick(const PtDropTable& table, int roll)
{
    if (roll < 0 || roll >= table.range) return -1;
    for (int i = 0; i < table.count; ++i)
    {
        if (roll < table.entries[i].weight) return i;
        roll -= table.entries[i].weight;
    }
    return -1;
}
template<class TIni> bool PtDropInt(TIni& ini, const char* section, const char* key, int fallback, int* value)
{
    char raw[64]; raw[0] = 0; *value = fallback;
    if (!ini.GetString(section,key,"",raw,sizeof(raw)) || !raw[0] || strlen(raw) >= sizeof(raw)-1) return false;
    char* end = 0; errno = 0; long parsed = strtol(raw,&end,10);
    if (end == raw || errno == ERANGE || parsed < INT_MIN || parsed > INT_MAX) return false;
    while (*end == ' ' || *end == '\t') ++end;
    if (*end) return false;
    *value = (int)parsed; return true;
}
template<class TIni> bool PtDropReadTable(TIni& ini, const char* name, PtDropTable& table)
{
    if (!PtDropNameValid(name)) return false;
    char section[64];
    sprintf(section, "Table_%s", name);
    if (!PtDropInt(ini,section,"Draws",0,&table.draws) ||
        !PtDropInt(ini,section,"Range",0,&table.range) ||
        !PtDropInt(ini,section,"Count",0,&table.count) || table.count < 1 || table.count > 128) return false;
    for (int i = 0; i < table.count; ++i)
    {
        sprintf(section,"%s_%d",name,i+1);
        PtDropEntry& e = table.entries[i];
        if (!PtDropInt(ini,section,"Genre",-1,&e.genre) ||
            !PtDropInt(ini,section,"Detail",-1,&e.detail) ||
            !PtDropInt(ini,section,"Particular",-1,&e.particular) ||
            !PtDropInt(ini,section,"Level",0,&e.level) ||
            !PtDropInt(ini,section,"Series",-1,&e.series) ||
            !PtDropInt(ini,section,"Luck",-1,&e.luck) ||
            !PtDropInt(ini,section,"Weight",-1,&e.weight) ||
            !PtDropInt(ini,section,"Quantity",0,&e.quantity)) return false;
    }
    return PtDropValidate(table);
}
#endif
