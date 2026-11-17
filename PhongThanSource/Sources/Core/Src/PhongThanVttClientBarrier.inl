// Included only by client KRegion.cpp. Reliable world state drives the same
// cell mask used by server movement; never infer open/closed from sprite visibility.
#include "../../../Headers/PhongThanVttBarrierProtocol.h"
#include "PhongThanVttLayout.h"
static PT_VTT_BARRIERS s_PtVttBarrierState;
void PhongThanVttResetBarrierState(){memset(&s_PtVttBarrierState,0,sizeof(s_PtVttBarrierState));}
bool PhongThanVttApplyBarrierState(const void* bytes,unsigned int size){
    if(size!=sizeof(PT_VTT_BARRIERS))return false;const PT_VTT_BARRIERS& p=*(const PT_VTT_BARRIERS*)bytes;
    if(p.MapId!=(unsigned int)SubWorld[0].m_SubWorldID||p.MapId<1067||p.MapId>1070||p.GateClosed>1||p.Phase>2)return false;
    const PT_VTT_LAYOUT& layout=PT_VTT_LAYOUTS[p.MapId-1067];PT_VTT_MASK valid=layout.visualCount==64?~(PT_VTT_MASK)0:(((PT_VTT_MASK)1<<layout.visualCount)-1);
    if(p.ClosedWalls&~valid)return false;
    if(s_PtVttBarrierState.MapId==p.MapId&&(int)(p.Sequence-s_PtVttBarrierState.Sequence)<0)return false;
    s_PtVttBarrierState=p;return true;
}
bool PhongThanVttBarrierAt(const KRegion* region,int x,int y){
    int map=SubWorld[0].m_SubWorldID;if(!region||map<1067||map>1070)return false;
    const PT_VTT_LAYOUT& layout=PT_VTT_LAYOUTS[map-1067];
    PT_VTT_MASK mask=s_PtVttBarrierState.MapId==(unsigned int)map?s_PtVttBarrierState.ClosedWalls:~(PT_VTT_MASK)0;
    bool gate=s_PtVttBarrierState.MapId==(unsigned int)map?s_PtVttBarrierState.GateClosed!=0:true;
    int px=region->m_nRegionX+x*32+16,py=region->m_nRegionY+y*32+16;
    if(gate)for(int i=0;i<layout.gateCount;++i)if(layout.door[i].x==px&&layout.door[i].y==py)return true;
    for(int n=0;n<layout.count;++n)if(layout.fence[n].x==px&&layout.fence[n].y==py&&(mask&((PT_VTT_MASK)1<<layout.groups[n])))return true;
    return false;
}
bool PhongThanVttSegmentBlocked(int world,int x0,int y0,int x1,int y1){
    if(world!=0||SubWorld[0].m_SubWorldID<1067||SubWorld[0].m_SubWorldID>1070)return false;
    int dx=x1-x0,dy=y1-y0,steps=(max(abs(dx),abs(dy))+7)/8;if(!steps)return false;int px=x0/32,py=y0/32;
    for(int n=1;n<=steps;++n){int gx=(x0+(int)((double)dx*n/steps))/32,gy=(y0+(int)((double)dy*n/steps))/32;int coords[3][2]={{gx,gy},{px,gy},{gx,py}};
        for(int p=0;p<(gx!=px&&gy!=py?3:1);++p){int r,x,y,ox,oy;SubWorld[0].Mps2Map(coords[p][0]*32+16,coords[p][1]*32+16,&r,&x,&y,&ox,&oy);if(r>=0&&PhongThanVttBarrierAt(&SubWorld[0].m_Region[r],x,y))return true;}
        px=gx;py=gy;
    }return false;
}
