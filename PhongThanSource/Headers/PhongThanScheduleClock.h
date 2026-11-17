#ifndef PHONGTHAN_SCHEDULE_CLOCK_H
#define PHONGTHAN_SCHEDULE_CLOCK_H

// A restart executes only its current minute. A backward clock change cannot
// repeat a previously processed minute, and a pause never replays old events.
struct PhongThanScheduleClock
{
    long LastMinute;
    PhongThanScheduleClock() : LastMinute(-1) {}
    bool Advance(long minute)
    {
        if (minute <= LastMinute) return false;
        LastMinute = minute;
        return true;
    }
};
#endif
