#include <stdlib.h>
#include <stdio.h>
#include "PhongThanRoute.h"
#include "PhongThanBotPortal.h"
struct Grid {
    bool walls[20][20];
    Grid(){for(int y=0;y<20;++y)for(int x=0;x<20;++x)walls[y][x]=false;}
    bool Free(int x,int y){return x>=0&&y>=0&&x<20&&y<20&&!walls[y][x];}
    int Trap(int x,int y){return (x==12&&y>=8&&y<=10)?123:0;}
};
int main(){
    Grid g;std::vector<PTRoutePoint> p;
    if(!PTRouteFind(g,1,1,10,10,p)||p.size()!=10)return 1;
    g.walls[1][2]=true;g.walls[2][1]=true;
    if(!PTRouteFind(g,1,1,3,3,p))return 2;
    if(p.size()<7)return 3; // Cannot cut the blocked diagonal corner.
    Grid u;
    for(int y=2;y<=14;++y)u.walls[y][5]=u.walls[y][13]=true;
    for(int x=5;x<=13;++x)u.walls[14][x]=true;
    if(!PTRouteFind(u,9,12,9,17,p))return 4;
    bool exits=false;
    for(unsigned int i=0;i<p.size();++i){
        if(!u.Free(p[i].x,p[i].y))return 5;
        if(p[i].y<2)exits=true;
    }
    if(!exits)return 6; // Must leave the U before going to its other side.
    u.walls[17][9]=true;
    if(PTRouteFind(u,9,12,9,17,p)||!p.empty())return 7;
    Grid closed;closed.walls[1][2]=closed.walls[2][1]=closed.walls[0][1]=closed.walls[1][0]=true;
    if(PTRouteFind(closed,1,1,10,10,p))return 8;
    Grid open;
    PTRouteOccupancy<Grid> live(open);
    live.Clear(1,10);live.Block(9,10);
    if(!PTRouteFind(live,1,10,18,10,p))return 9;
    bool detour=false;
    for(unsigned int j=0;j<p.size();++j){
        if(live.Blocked(p[j].x,p[j].y))return 10;
        if(p[j].y!=10)detour=true;
    }
    if(!detour)return 11;
    // The blocker moves: rebuild the overlay, then replan from a later actor
    // position. The old cell must be usable again; the new one must be avoided.
    live.Clear(8,10);live.Block(12,10);
    if(!live.Free(9,10)||!PTRouteFind(live,8,10,18,10,p))return 12;
    for(unsigned int k=0;k<p.size();++k)if(live.Blocked(p[k].x,p[k].y))return 13;
    live.Block(18,10);if(PTRouteFind(live,8,10,18,10,p))return 14;
    live.Clear(8,10);live.Block(8,10); // Can escape an overlapping NPC.
    if(!live.Free(8,10)||!PTRouteFind(live,8,10,18,10,p))return 15;
    PTRouteProgress progress;
    if(progress.Stalled(100,100,1000)||progress.Stalled(100,100,2199))return 16;
    if(!progress.Stalled(100,100,2200))return 17;
    if(progress.Stalled(110,100,2250))return 18;
    Grid bounded;bool partial=false;
    if(PTRouteFind(bounded,1,1,18,18,p,10)||!p.empty())return 19;
    if(!PTRouteFind(bounded,1,1,18,18,p,10,&partial)||!partial||p.empty())return 20;
    for(unsigned z=1;z<p.size();++z)if(!bounded.Free(p[z].x,p[z].y)||abs(p[z].x-p[z-1].x)>1||abs(p[z].y-p[z-1].y)>1)return 21;
    if(p.back().x==18&&p.back().y==18)return 22;
    if(PTRouteFind(closed,1,1,18,18,p,10,&partial)||!p.empty()||partial)return 23;
    int portalX=0,portalY=0;Grid portal;portal.walls[9][12]=true;
    if(!PTBotPortalCell(portal,9,9,1,9,portalX,portalY)||!portal.Trap(portalX,portalY)||!portal.Free(portalX,portalY)||portalX!=12)return 24;
    portal.walls[8][12]=portal.walls[10][12]=true;if(PTBotPortalCell(portal,9,9,1,9,portalX,portalY))return 25;
    puts("PASS: moving NPC detour, refreshed occupancy, occupied goal retry, overlapping start, stalled movement deadline.");
    puts("PASS: shortest open route, U wall, no corner cutting, blocked target, unreachable target.");
    return 0;
}
