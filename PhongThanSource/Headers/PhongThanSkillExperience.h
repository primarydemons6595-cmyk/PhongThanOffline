#ifndef PHONGTHAN_SKILL_EXPERIENCE_H
#define PHONGTHAN_SKILL_EXPERIENCE_H
#include <stdlib.h>
#include <limits.h>
#include <errno.h>

// skills.ini stores cumulative level thresholds, beginning with level 1 = 0.
// The learned cap is 10; native levels 11/12 are not granted by training.
inline int PhongThanSkillNextExperience(const char* value, int level)
{
    if (!value || level < 1 || level >= 10) return 0;
    char* end;
    errno = 0;
    long count = strtol(value, &end, 10);
    if (errno == ERANGE || end == value || *end != ':' || count <= level || count > 64) return 0;
    const char* cursor = end + 1;
    long previous = -1;
    for (int i = 0; i <= level; ++i)
    {
        errno = 0;
        long threshold = strtol(cursor, &end, 10);
        if (errno == ERANGE || cursor == end || threshold < 0 || threshold > INT_MAX ||
            (i == 0 && threshold != 0) || (i > 0 && threshold <= previous)) return 0;
        if (i == level) return (int)threshold;
        if (*end != ',') return 0;
        cursor = end + 1;
        previous = threshold;
    }
    return 0;
}
#endif
