#include <assert.h>
#include <stdio.h>
#include "PhongThanRouletteMotion.h"
int main(){
    for(int start=0;start<12;++start)for(int target=0;target<12;++target){
        PTRouletteMotion m;m.Start(100);m.Sector=start;m.Stop(target,150);
        unsigned int previousDelay=0;int count=0;
        while(m.Remaining){unsigned int delay=m.Delay();assert(delay>=previousDelay);previousDelay=delay;
            int before=m.Sector;m.Tick(m.Last+delay-1,true);assert(m.Sector==before);
            m.Tick(m.Last+delay,true);assert(m.Sector==(before+1)%12);++count;}
        assert(m.Sector==target&&count>=24&&count<=35);
        m.Tick(m.Last+10000,true);assert(m.Sector==target);
    }
    PTRouletteMotion m;m.Start(0xfffffff0);m.Tick(80,false);assert(m.Sector==1);m.Tick(5000,false);assert(m.Sector==2);
    puts("PASS all 144 start/result pairs: adjacent clockwise motion, nondecreasing intervals, no final jump; stalled frames and tick wrap");return 0;
}
