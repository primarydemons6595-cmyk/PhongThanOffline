#ifndef PHONGTHAN_PILLS_H
#define PHONGTHAN_PILLS_H
// Server-authoritative offline alchemy; epoch timestamps survive logout/restart.
struct PTPillsState {
    int Count,NewCount,Armed,LastSeen,Until,Bonus,Seconds,Day,MadeToday;
};
inline void PTPillsExpire(PTPillsState& s,int now) {
    if(s.Until<=0 || (s.Bonus!=50 && s.Bonus!=75)){s.Until=0;s.Bonus=0;return;}
    while(s.Until<=now && s.Count>0){--s.Count;s.Until+=600;}
    if(s.Until<=now){s.Until=0;s.Bonus=0;}
}
inline void PTPillsRestore(PTPillsState& s,int now,int secondsPerPill) {
    PTPillsExpire(s,now);
    if(secondsPerPill<60 || secondsPerPill>86400)secondsPerPill=3600;
    int day=(now+25200)/86400;
    if(s.Day!=day){s.Day=day;s.MadeToday=0;}
    if(s.Armed && s.LastSeen>0 && now>s.LastSeen) {
        int elapsed=now-s.LastSeen;
        if(elapsed>86400)elapsed=86400;
        int total=s.Seconds+elapsed;
        int made=total/secondsPerPill;
        if(made>12-s.NewCount)made=12-s.NewCount;
        if(made>12-s.MadeToday)made=12-s.MadeToday;
        if(made<0)made=0;
        s.NewCount+=made;s.MadeToday+=made;s.Seconds=total%secondsPerPill;
    }
    s.Armed=0;s.LastSeen=now;
}
#endif
