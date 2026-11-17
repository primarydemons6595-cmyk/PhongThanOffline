#ifndef PHONGTHAN_BUFF_DURATION_H
#define PHONGTHAN_BUFF_DURATION_H
// Add duration, not effect strength. Reject timestamp overflow before charging.
inline bool PTAddBuffDuration(unsigned long now,unsigned long previous,int seconds,unsigned long& end) {
    if(seconds<=0)return false;
    unsigned long start=previous>now?previous:now;
    if(start>0xffffffffUL-(unsigned long)seconds || start-now>0x7fffffffUL-(unsigned long)seconds)return false;
    end=start+(unsigned long)seconds;return true;
}
#endif
