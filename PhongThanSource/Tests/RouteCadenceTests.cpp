#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "PhongThanRoute.h"

struct CadenceGrid {
    bool Free(int x,int y){return x>=0&&x<160&&y>=0&&y<20;}
};
struct CadenceResult {int pauses,ticks;};

// Follow at the actual 18Hz logic cadence, with the native stop-within-one-step
// rule. Unlike the older route tests, destination changes are subject to the
// production command cadence, so a stale near waypoint can stop movement.
static CadenceResult Follow(int speed){
    CadenceGrid grid;std::vector<PTRoutePoint> path;
    PTRouteFind(grid,1,2,120,2,path);
    int next=1,x=48,y=80,tx=x,ty=y;bool walking=false;
    unsigned int sent=0;CadenceResult result={0,0};
#ifndef PT_ROUTE_EXPECT_LEGACY
    PTRouteCommandCadence cadence;
#endif
    for(int tick=0;tick<10000;++tick){
        unsigned int now=1000+(unsigned int)(tick*1000/18);
        PTRoutePoint target;bool walk=false;
        if(!PTRouteWaypoint(grid,path,next,x,y,speed,6,target,walk)){
            result.ticks=tick;return result;
        }
#ifdef PT_ROUTE_EXPECT_LEGACY
        bool send=now-sent>=100;
#else
        bool send=cadence.ShouldSend(now,target.x,target.y,walk);
#endif
        if(send){sent=now;tx=target.x;ty=target.y;walking=walk;}
        int step=walking?6:speed,dx=tx-x,dy=ty-y;
        double distance=sqrt((double)dx*dx+(double)dy*dy);
        if(distance<step){++result.pauses;continue;}
        int nx=x+(int)(dx*step/distance),ny=y+(int)(dy*step/distance);
        if(!PTRouteSegmentClear(grid,x,y,nx,ny)){result.ticks=-1;return result;}
        x=nx;y=ny;
    }
    result.ticks=-1;return result;
}
int main(){
    CadenceResult normal=Follow(10),mounted=Follow(31);
    printf("18Hz straight route: normal pauses=%d ticks=%d; mounted pauses=%d ticks=%d\n",
        normal.pauses,normal.ticks,mounted.pauses,mounted.ticks);
    if(normal.ticks<0||mounted.ticks<0)return 1;
#ifdef PT_ROUTE_EXPECT_LEGACY
    if(mounted.pauses==0)return 2;
    puts("REPRODUCED: legacy follower stops between short waypoints while command throttle holds the next destination.");
#else
    if(normal.pauses||mounted.pauses)return 3;
    PTRouteCommandCadence cadence;
    if(!cadence.ShouldSend(0,100,200,false))return 4;
    if(cadence.ShouldSend(55,100,200,false))return 5;
    if(!cadence.ShouldSend(55,101,200,false))return 6;
    if(!cadence.ShouldSend(56,101,200,true))return 7;
    if(cadence.ShouldSend(155,101,200,true))return 8;
    if(!cadence.ShouldSend(156,101,200,true))return 9;
    cadence.Reset();if(!cadence.ShouldSend(157,101,200,true))return 10;
    cadence.Reset();if(!cadence.ShouldSend(0xfffffff0U,101,200,true))return 11;
    if(cadence.ShouldSend(0x10U,101,200,true))return 12;
    if(!cadence.ShouldSend(0x80U,101,200,true))return 13;
    puts("PASS: continuous straight movement at 10/31px steps; immediate retarget/mode changes, duplicate throttle, reset and timer wrap.");
#endif
    return 0;
}
