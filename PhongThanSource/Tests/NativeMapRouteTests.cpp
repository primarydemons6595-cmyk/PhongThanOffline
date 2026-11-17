#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
const char* g_RouteTestRoot="";
#define PT_NATIVE_ROUTE_TEST
#include "../Sources/Core/Src/PhongThanMapRoute.inl"
// Exercise the production follower with the native GetDir stop contract:
// no motion when the remaining distance is below the current movement step.
template<class Grid> static bool FollowRoute(Grid& grid,const std::vector<PTRoutePoint>& path,int speed){
    int x=path[0].x*32+16,y=path[0].y*32+16,next=1;
    for(int tick=0;tick<20000;++tick){
        PTRoutePoint target;bool walk=false;
        if(!PTRouteWaypoint(grid,path,next,x,y,speed,6,target,walk))return true;
        int step=walk?6:speed,dx=target.x-x,dy=target.y-y;
        double distance=sqrt((double)dx*dx+(double)dy*dy);
        if(distance<step){printf("STOP next=%d pos=%d,%d target=%d,%d speed=%d walk=%d distance=%f\n",next,x,y,target.x,target.y,speed,walk,distance);return false;} // Reproduces the mounted 18px deadlock.
        int nx=x+(int)(dx*step/distance),ny=y+(int)(dy*step/distance);
        if(!PTRouteSegmentClear(grid,x,y,nx,ny)){printf("COLLISION next=%d pos=%d,%d target=%d,%d step=%d,%d speed=%d\n",next,x,y,target.x,target.y,nx,ny,speed);return false;}
        if(nx==x&&ny==y)return false;
        x=nx;y=ny;
    }
    puts("FOLLOW TIMEOUT");return false;
}
int main(int argc,char** argv){
    if(argc!=2)return 1;g_RouteTestRoot=argv[1];
    PTNativeRouteGrid grid;grid.SetMap("\\maps\\\xC8\xFD\xC9\xBD\xB9\xD8");
    // Live regression: 199,212 lies in a real region whose obstacle section
    // has length zero. The native loader treats it as clear ground.
    if(!grid.Free(199*8,212*16)){puts("Empty obstacle section incorrectly blocked");return 7;}
    std::vector<PTRoutePoint> requested;
    int liveX=-1,liveY=-1;
    for(int ly=202*16;ly<203*16&&liveX<0;++ly)
        for(int lx=202*8;lx<203*8;++lx)if(grid.Free(lx,ly)){liveX=lx;liveY=ly;break;}
    if(liveX<0||!PTRouteFind(grid,liveX,liveY,199*8,212*16,requested)||
        !FollowRoute(grid,requested,31)){
        puts("Live mounted route 202,202 -> 199,212 failed");return 8;
    }
    printf("Live regression 202,202 -> 199,212: %u cells, mounted follower PASS\n",requested.size());
    PTRouteOccupancy<PTNativeRouteGrid> occupied(grid);occupied.Clear(liveX,liveY);
    bool testedBlocker=false;
    for(unsigned int bi=4;bi+4<requested.size();++bi){
        PTRoutePoint blocker=requested[bi];bool clear=true;
        for(int by=-1;by<=1;++by)for(int bx=-1;bx<=1;++bx)
            if(!grid.Free(blocker.x+bx,blocker.y+by))clear=false;
        if(!clear)continue;
        occupied.Block(blocker.x,blocker.y);
        std::vector<PTRoutePoint> detour;
        if(!PTRouteFind(occupied,liveX,liveY,199*8,212*16,detour)||!FollowRoute(occupied,detour,31))return 9;
        for(unsigned int di=0;di<detour.size();++di)if(occupied.Blocked(detour[di].x,detour[di].y))return 10;
        testedBlocker=true;break;
    }
    if(!testedBlocker)return 11;
    puts("Native 199,212 route with NPC on original path: mounted detour follower PASS");
    int sx=-1,sy=-1;
    for(int y=3352;y<3376&&sx<0;++y)for(int x=1664;x<1688;++x)if(grid.Free(x,y)){sx=x;sy=y;break;}
    if(sx<0){puts("No native walkable start found in map 16");return 2;}
    int routes=0,detours=0;
    for(int dy=-100;dy<=100;dy+=20)for(int dx=-100;dx<=100;dx+=20){
        if(!dx&&!dy)continue;int tx=sx+dx,ty=sy+dy;if(!grid.Free(tx,ty))continue;
        std::vector<PTRoutePoint> path;if(!PTRouteFind(grid,sx,sy,tx,ty,path,50000))continue;
        ++routes;int cost=0;
        for(unsigned int i=1;i<path.size();++i){
            PTRoutePoint a=path[i-1],b=path[i];int ax=abs(b.x-a.x),ay=abs(b.y-a.y);
            if(ax>1||ay>1||(!ax&&!ay)||!grid.Free(b.x,b.y))return 3;
            if(ax&&ay&&(!grid.Free(a.x,b.y)||!grid.Free(b.x,a.y)))return 4;
            cost+=ax&&ay?14:10;
        }
        if(cost>PTRouteDistance(sx,sy,tx,ty))++detours;
        if(!FollowRoute(grid,path,10)||!FollowRoute(grid,path,31)){
            printf("Follower failed to %d,%d\n",tx,ty);return 6;
        }
    }
    printf("Native map 16: start=%d,%d routes=%d obstacle detours=%d\n",sx,sy,routes,detours);
    if(routes<3||detours<1)return 5;
    puts("PASS production Region_C + A* + follower at 10/31px steps, native stop distance, valid cells and corners.");return 0;
}
