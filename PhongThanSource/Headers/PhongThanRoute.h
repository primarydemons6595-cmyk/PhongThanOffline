#ifndef PHONGTHAN_ROUTE_H
#define PHONGTHAN_ROUTE_H
#include <map>
#include <queue>
#include <vector>
#include <algorithm>
struct PTRoutePoint { int x,y; PTRoutePoint(int a=0,int b=0):x(a),y(b){} };
struct PTRouteNode { int g,parent; PTRouteNode(int a=0,int b=-1):g(a),parent(b){} };
struct PTRouteOpen {
    int key,g,f;
    PTRouteOpen(int a,int b,int c):key(a),g(b),f(c){}
    bool operator<(const PTRouteOpen& b) const { return f>b.f || (f==b.f && g<b.g); }
};
inline int PTRouteKey(int x,int y) { return y*65536+x; }
inline PTRoutePoint PTRouteDecode(int k) { return PTRoutePoint(k&65535,(unsigned int)k>>16); }
inline int PTRouteDistance(int x,int y,int tx,int ty) {
    int a=abs(tx-x),b=abs(ty-y);return 10*(a+b)-6*(a<b?a:b);
}
inline bool PTRouteCommandWalk(bool pending,bool waypointWalk,int arrivalRadius) {
    return pending?waypointWalk:arrivalRadius>0;
}
inline bool PTRouteNear(int x,int y,int tx,int ty,int radius) {
    int dx=x-tx,dy=y-ty;
    return abs(dx)<=radius&&abs(dy)<=radius&&dx*dx+dy*dy<=radius*radius;
}
  // Transient occupants are kept separate from the immutable map cache.
// A fresh snapshot removes cells as soon as the blocking NPC has moved.
template<class Grid> class PTRouteOccupancy {
    Grid& base;
    std::map<int,unsigned char> occupied;
    int start;
public:
    PTRouteOccupancy(Grid& grid):base(grid),start(-1){}
    void Clear(int x,int y){occupied.clear();start=PTRouteKey(x,y);}
    void Block(int x,int y){if(x>=0&&y>=0&&x<=32767&&y<=32767)occupied[PTRouteKey(x,y)]=1;}
    bool Blocked(int x,int y){int key=PTRouteKey(x,y);return key!=start&&occupied.find(key)!=occupied.end();}
    bool Free(int x,int y){return !Blocked(x,y)&&base.Free(x,y);}
};
class PTRouteProgress {
    int x,y;
    unsigned int time;
    bool initialized;
public:
    PTRouteProgress():x(0),y(0),time(0),initialized(false){}
    void Reset(){initialized=false;}
    bool Stalled(int nx,int ny,unsigned int now){
        if(!initialized||abs(nx-x)>=8||abs(ny-y)>=8){x=nx;y=ny;time=now;initialized=true;return false;}
        return now-time>=1200;
    }
};
// Retarget immediately. Only repeated commands to the same waypoint/mode
// need throttling; delaying a new waypoint can stop a fast mount for a tick.
class PTRouteCommandCadence {
    unsigned int time;
    int x,y;
    bool walking,initialized;
public:
    PTRouteCommandCadence(){Reset();}
    void Reset(){time=0;x=y=0;walking=false;initialized=false;}
    bool ShouldSend(unsigned int now,int nx,int ny,bool walk){
        if(initialized&&nx==x&&ny==y&&walk==walking&&now-time<100)return false;
        time=now;x=nx;y=ny;walking=walk;initialized=true;return true;
    }
};
// Check the segment from the actual position, including both cells at a
// diagonal boundary. This prevents an early turn from clipping a wall corner.
template<class Grid> bool PTRouteSegmentClear(Grid& grid,int x,int y,int tx,int ty) {
    int dx=tx-x,dy=ty-y,steps=(abs(dx)>abs(dy)?abs(dx):abs(dy))/4+1;
    int px=x/32,py=y/32;
    for(int i=1;i<=steps;++i){
        int cx=(x+dx*i/steps)/32,cy=(y+dy*i/steps)/32;
        if(!grid.Free(cx,cy))return false;
        if(cx!=px&&cy!=py&&(!grid.Free(cx,py)||!grid.Free(px,cy)))return false;
        px=cx;py=cy;
    }
    return true;
}
// KNpcFindPath stops within one movement step, which can exceed 18 pixels
// while mounted. Accept that stop distance; walk the last few pixels only
// when advancing immediately would turn through an obstacle.
template<class Grid> bool PTRouteWaypoint(Grid& grid,const std::vector<PTRoutePoint>& path,
    int& next,int x,int y,int runSpeed,int walkSpeed,PTRoutePoint& target,bool& walk) {
    walk=false;
    int runRadius=runSpeed+1;if(runRadius<18)runRadius=18;
    int walkRadius=walkSpeed+1;if(walkRadius<18)walkRadius=18;
    while(next<(int)path.size()){
        target=PTRoutePoint(path[next].x*32+16,path[next].y*32+16);
        if(!PTRouteNear(x,y,target.x,target.y,runRadius))return true;
        if(next+1<(int)path.size()&&!PTRouteNear(x,y,target.x,target.y,walkRadius)){
            PTRoutePoint after(path[next+1].x*32+16,path[next+1].y*32+16);
            if(!PTRouteSegmentClear(grid,x,y,after.x,after.y)){walk=true;return true;}
        }
        ++next;
    }
    return false;
}
// Octile A*: native 32-pixel cells, diagonal steps cannot cross blocked corners.
// Missing map regions are blocked by the provider. No straight-line fallback.
template<class Grid> bool PTRouteFind(Grid& grid,int sx,int sy,int tx,int ty,
    std::vector<PTRoutePoint>& path,int limit=100000,bool* partial=0) {
    path.clear();
    if(partial)*partial=false;
    if(sx<0||sy<0||tx<0||ty<0||sx>32767||sy>32767||tx>32767||ty>32767||!grid.Free(tx,ty))return false;
    const int start=PTRouteKey(sx,sy),goal=PTRouteKey(tx,ty);
    std::map<int,PTRouteNode> nodes;
    std::priority_queue<PTRouteOpen> open;
    nodes[start]=PTRouteNode(0,-1);open.push(PTRouteOpen(start,0,PTRouteDistance(sx,sy,tx,ty)));
    int expanded=0,closest=start,bestDistance=PTRouteDistance(sx,sy,tx,ty),initialDistance=bestDistance;
    while(!open.empty() && expanded<limit) {
        PTRouteOpen top=open.top();open.pop();
        if(nodes[top.key].g!=top.g)continue;
        if(top.key==goal) {
            for(int k=goal;k!=-1;k=nodes[k].parent)path.push_back(PTRouteDecode(k));
            std::reverse(path.begin(),path.end());return true;
        }
        ++expanded;PTRoutePoint p=PTRouteDecode(top.key);int remaining=PTRouteDistance(p.x,p.y,tx,ty);if(remaining<bestDistance){bestDistance=remaining;closest=top.key;}
        for(int dy=-1;dy<=1;++dy)for(int dx=-1;dx<=1;++dx) {
            if(!dx&&!dy)continue;
            const int x=p.x+dx,y=p.y+dy;
            if(x<0||y<0||x>32767||y>32767||!grid.Free(x,y))continue;
            if(dx&&dy&&(!grid.Free(p.x+dx,p.y)||!grid.Free(p.x,p.y+dy)))continue;
            const int key=PTRouteKey(x,y),g=top.g+(dx&&dy?14:10);
            typename std::map<int,PTRouteNode>::iterator old=nodes.find(key);
            if(old!=nodes.end() && old->second.g<=g)continue;
            nodes[key]=PTRouteNode(g,top.key);
            open.push(PTRouteOpen(key,g,g+PTRouteDistance(x,y,tx,ty)));
        }
    }
    // A bot may consume a traversable prefix when its bounded search runs out.
    // No caller receives a partial path unless it explicitly opts in.
    if(partial&&expanded>=limit&&initialDistance-bestDistance>=80&&closest!=start){for(int k=closest;k!=-1;k=nodes[k].parent)path.push_back(PTRouteDecode(k));std::reverse(path.begin(),path.end());*partial=true;return true;}
    return false;
}
#endif
