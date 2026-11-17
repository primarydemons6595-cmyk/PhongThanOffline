#include <assert.h>
#include <stdio.h>
#include "PhongThanRenderMotion.h"

int main(){
    PhongThanRenderMotion motion;int x,y,z;
    assert(!motion.IsInitialized());
    motion.SetTarget(1000,2000,0,1000,true);motion.Sample(1000,x,y,z);
    assert(x==1000&&y==2000&&z==0);
    motion.SetTarget(1031,1980,8,1055,true);
    motion.Sample(1082,x,y,z);assert(x>1000&&x<1031&&y<2000&&y>1980&&z>0&&z<8);
    motion.Sample(1110,x,y,z);assert(x==1031&&y==1980&&z==8);
    motion.Sample(5000,x,y,z);assert(x==1031&&y==1980); // no extrapolation
    motion.SetTarget(1062,1980,8,1110,true);motion.Sample(1130,x,y,z);int before=x;
    motion.SetTarget(1062,1980,8,1130,true);motion.Sample(1165,x,y,z);assert(x==1062); // unchanged target does not restart
    motion.SetTarget(1093,1980,8,1165,true);motion.Sample(1185,x,y,z);before=x;
    motion.SetTarget(1120,1970,8,1185,true);motion.Sample(1185,x,y,z);assert(x==before); // retarget continuity
    motion.SetTarget(2000,3000,0,1190,true);motion.Sample(1190,x,y,z);assert(x==2000&&y==3000); // teleport snaps
    motion.SetTarget(2010,3000,0,1200,false);motion.Sample(1200,x,y,z);assert(x==2010); // preview/explicit snap
    motion.SetTarget(2020,3000,0,1600,true);motion.Sample(1600,x,y,z);assert(x==2020); // long pause snaps
    motion.Reset();motion.SetTarget(0,0,0,0xfffffff0U,true);
    motion.SetTarget(-30,30,-5,0xfffffff0U,true);motion.Sample(0x0bU,x,y,z);
    assert(x<0&&x>-30&&y>0&&y<30&&z<=0); // wrap / negative axes / zero initial anchor
    motion.Sample(0x27U,x,y,z);assert(x==-30&&y==30&&z==-5);
    for(int fps=30;fps<=54;fps+=24){
        motion.Reset();motion.SetTarget(0,0,0,0,true);
        int last=0,intermediate=0,changes=0,tick=0;
        for(int frame=0;frame*1000/fps<=1045;++frame){
            unsigned int now=frame*1000/fps;
            while(tick<18&&(unsigned int)((tick+1)*55)<=now){
                ++tick;motion.SetTarget(tick*20,0,0,tick*55,true);
            }
            motion.Sample(now,x,y,z);assert(x>=last&&x<=tick*20);
            if(x>last)++changes;
            if(tick>0&&x>(tick-1)*20&&x<tick*20)++intermediate;
            last=x;
        }
        motion.Sample(19*55,x,y,z);assert(x==360);
        assert(intermediate>=18&&changes>18);
        printf("PASS %dFPS: %d displayed position changes / 18 simulation moves, %d intermediate samples\n",fps,changes,intermediate);
    }
    puts("PASS: initialization, sub-tick motion, stop, retarget, snap, pause, reset, zero anchor and timer wrap.");
    return 0;
}
