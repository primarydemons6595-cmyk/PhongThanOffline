#ifndef PT_ROULETTE_MOTION_H
#define PT_ROULETTE_MOTION_H
// A stop is a finite clockwise path from the currently displayed sector.
struct PTRouletteMotion {
    int Sector,Remaining,Total;unsigned int Last,FinishedAt;
    void Start(unsigned int now){Sector=0;Remaining=Total=0;Last=now;FinishedAt=0;}
    void Stop(int outcome,unsigned int now){Total=Remaining=24+(outcome-Sector+12)%12;Last=now;FinishedAt=0;}
    unsigned int Delay()const {int done=Total-Remaining;return Total?70+(unsigned int)(330*done*done/(Total*Total)):70;}
    void Tick(unsigned int now,bool stopping){
        if(stopping&&!Remaining)return;
        unsigned int delay=stopping?Delay():70;
        if(now-Last<delay)return;
        // One visible adjacent step per paint; a stalled frame never skips cells.
        Sector=(Sector+1)%12;Last=now;
        if(stopping&&--Remaining==0)FinishedAt=now;
    }
};
#endif
