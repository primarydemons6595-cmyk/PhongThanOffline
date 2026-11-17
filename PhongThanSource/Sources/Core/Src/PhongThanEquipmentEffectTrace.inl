#ifndef PHONGTHAN_EQUIPMENT_EFFECT_TRACE_INL
#define PHONGTHAN_EQUIPMENT_EFFECT_TRACE_INL
#include <stdio.h>
#include <stdarg.h>
#include <time.h>

// Temporary bounded event trace. Never changes state or forces a sprite.
static void PhongThanEquipmentEffectTrace(int stage, const void* object,
    int state, int status, int aux, const char* format, ...)
{
    struct Seen { int stage; const void* object; int state, status, aux; };
    static Seen seen[128];
    static int used = 0, next = 0;
    for (int i = 0; i < used; ++i)
        if (seen[i].stage == stage && seen[i].object == object &&
            seen[i].state == state && seen[i].status == status && seen[i].aux == aux)
            return;
    FILE* file = fopen("equipment_effect_trace.log", "a");
    if (!file) return;
    time_t now = time(NULL);
    struct tm* local = localtime(&now);
    char stamp[32] = "TIME_UNAVAILABLE";
    if (local) strftime(stamp, sizeof(stamp), "%Y-%m-%d %H:%M:%S", local);
    fprintf(file, "%s stage=%d object=%p ", stamp, stage, object);
    va_list args;
    va_start(args, format);
    vfprintf(file, format, args);
    va_end(args);
    fprintf(file, "\n");
    fclose(file);
    Seen* entry = &seen[next];
    entry->stage = stage; entry->object = object;
    entry->state = state; entry->status = status; entry->aux = aux;
    next = (next + 1) % 128;
    if (used < 128) ++used;
}
#endif
