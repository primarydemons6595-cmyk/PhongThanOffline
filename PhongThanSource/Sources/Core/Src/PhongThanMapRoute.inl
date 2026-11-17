#include "../../../Headers/PhongThanRoute.h"
#include "KPakFile.h"
#include <stdio.h>
class PTNativeRouteGrid {
    struct Region { unsigned char cells[512]; };
    std::map<int,Region> regions;
    char map[256];
public:
    PTNativeRouteGrid(){map[0]=0;}
    void SetMap(const char* path) { if(strcmp(map,path)){lstrcpyn(map,path,sizeof(map));regions.clear();} }
    bool Free(int x,int y) {
        if(x<0||y<0||x>32767||y>32767)return false;
        const int rx=x/16,ry=y/32,key=ry*65536+rx;
        std::map<int,Region>::iterator it=regions.find(key);
        if(it==regions.end()) {
            Region r;memset(r.cells,255,sizeof(r.cells));
            char file[512];sprintf(file,"%s\\v_%03d\\%03d_Region_C.dat",map,ry,rx);
            KPakFile pak;
            if(pak.Open(file)) {
                unsigned int count=0,offset=0,length=0;
                pak.Read(&count,4);pak.Read(&offset,4);pak.Read(&length,4);
                unsigned int head=4+8*count;
                if(count>0&&count<=64&&length==2048&&head+offset<=pak.Size()&&length<=pak.Size()-head-offset) {
                    long cells[512];pak.Seek(head+offset,FILE_BEGIN);
                    if(pak.Read(cells,sizeof(cells))==sizeof(cells))
                        for(int yy=0;yy<32;++yy)for(int xx=0;xx<16;++xx)r.cells[yy*16+xx]=(unsigned char)cells[xx*32+yy];
                } else if(count>0&&count<=64&&head<=pak.Size()&&offset<=pak.Size()-head&&length==0) {
                    // A present region with an empty obstacle section is clear
                    // in the native client/server loaders, not an absent region.
                    memset(r.cells,0,sizeof(r.cells));
                }
                pak.Close();
            }
            // Bound cached missing regions as well as A* expansions.
            if(regions.size()>8192)return false;
            regions[key]=r;it=regions.find(key);
        }
        return it->second.cells[(y%32)*16+x%16]==0;
    }
};
static PTNativeRouteGrid s_PtRouteGrid;
static PTRouteOccupancy<PTNativeRouteGrid> s_PtRouteLive(s_PtRouteGrid);
static PTRouteProgress s_PtRouteProgress;
static std::vector<PTRoutePoint> s_PtRoute;
static int s_PtRouteMap=-1,s_PtRouteX=-1,s_PtRouteY=-1,s_PtRouteNext=0;
static int s_PtRouteArrival=0;
void PhongThanSetRouteArrival(int radius){s_PtRouteArrival=radius>=1&&radius<=128?radius:0;}
static PTRouteCommandCadence s_PtRouteCommands;
static unsigned int s_PtRouteSnapshot=0,s_PtRoutePlan=0;
#ifndef PT_NATIVE_ROUTE_TEST
static int PhongThanRouteSnapshot(int actor,int world,int x,int y){
    s_PtRouteLive.Clear(x/32,y/32);
    int count=0,other=0;
    while((other=NpcSet.GetNextIdx(other))!=0){
        if(other==actor||Npc[other].m_RegionIndex<0||Npc[other].m_SubWorldIndex!=world||
            Npc[other].m_Doing==do_death||Npc[other].m_Doing==do_revive)continue;
        if(Npc[other].m_Kind!=kind_dialoger&&Npc[other].m_CurrentLife<=0)continue;
        int nx,ny;Npc[other].GetMpsPos(&nx,&ny);
        if(abs(nx-x)>2048||abs(ny-y)>2048)continue;
        // Leave room to escape when an NPC is already overlapping the actor.
        int padding=(abs(nx-x)>64||abs(ny-y)>64)?1:0;
        for(int dy=-padding;dy<=padding;++dy)for(int dx=-padding;dx<=padding;++dx)
            s_PtRouteLive.Block(nx/32+dx,ny/32+dy);
        ++count;
    }
    return count;
}
static void PhongThanRouteTrace(const char* event,int x,int y,int tx,int ty,int occupants){
    FILE* log=fopen("map_route_trace.log","a");
    if(log){fprintf(log,"tick=%u event=%s position=%d,%d target=%d,%d occupants=%d next=%d points=%d\n",
        IR_GetCurrentTime(),event,x,y,tx,ty,occupants,s_PtRouteNext,(int)s_PtRoute.size());fclose(log);}
}
#endif
int PhongThanGetMapRoutePoint(int index,int* x,int* y){
    int i=s_PtRouteNext+index;
    if(index<0||i<0||i>=(int)s_PtRoute.size())return 0;
    *x=s_PtRoute[i].x*32+16;*y=s_PtRoute[i].y*32+16;return 1;
}
void PhongThanResetMapRoute(){s_PtRoute.clear();s_PtRouteMap=-1;s_PtRouteCommands.Reset();s_PtRouteSnapshot=0;s_PtRoutePlan=0;s_PtRouteProgress.Reset();}
