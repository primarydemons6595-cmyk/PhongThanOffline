#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "PhongThanRenderMotion.h"

struct Result {int frames,zero,minStep,maxStep,variation;};
static Result Run(int speed,bool cached,int fps){
    PhongThanRenderMotion motion;motion.SetTarget(0,0,0,0,true);
    unsigned int imageTime=0;int tick=1,frame=1,previous=0,previousStep=0;
    Result result={0,0,100000,-100000,0};
    for(unsigned int now=1;now<3000;++now){
        if(now==(unsigned int)(tick*1000/18)){
            motion.SetTarget(tick*speed,0,0,cached?imageTime:now,true);++tick;
        }
        if(now==(unsigned int)(frame*1000/fps)){
            int x,y,z;motion.Sample(cached?imageTime:now,x,y,z);
            int step=x-previous;
            if(now>=500){
                ++result.frames;if(!step)++result.zero;
                if(step<result.minStep)result.minStep=step;
                if(step>result.maxStep)result.maxStep=step;
                result.variation+=abs(step-previousStep);
            }
            previous=x;previousStep=step;imageTime=now;++frame;
        }
    }
    return result;
}
int main(){
    setbuf(stdout,NULL);
    for(int fps=30;fps<=54;fps+=24)for(int speed=10;speed<=31;speed+=21){
        Result old=Run(speed,true,fps),fixed=Run(speed,false,fps);
        printf("speed=%d fps=%d cached step=%d..%d variation=%d zeros=%d; live step=%d..%d variation=%d zeros=%d\n",
            speed,fps,old.minStep,old.maxStep,old.variation,old.zero,
            fixed.minStep,fixed.maxStep,fixed.variation,fixed.zero);
        assert(fixed.minStep>0&&fixed.variation<=old.variation);
        if(fps==30)assert(fixed.variation<old.variation);
    }
    puts("PASS: native helper driven by real 18Hz logic / 30,54FPS paint schedules; current-time sampling reduces step variation at running and mount speeds.");
    return 0;
}
