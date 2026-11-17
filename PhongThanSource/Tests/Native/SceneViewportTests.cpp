#include <assert.h>
#include <stdio.h>
#include "PhongThanSceneViewport.h"
struct Canvas {int width,height;int GetWidth(){return width;}int GetHeight(){return height;}};
struct NativeRepresent {
    Canvas m_Canvas;int m_nLeft,m_nTop;
    void LookAt(int nX,int nY,int nZ);
    void ViewPortCoordToSpaceCoord(int& nX,int& nY,int nZ);
};
// Definitions are extracted verbatim from the actual Represent2 implementation.
#include "../../Output/LegacyDropNativeTest/SceneViewportBodies.inl"
int main(){
    const int widths[]={800,1024,1280,1366,1536,1600,1920};
    for(int i=0;i<7;++i)for(int mode=0;mode<3;++mode){
        int width=widths[i],height=768,extra=mode==1?260:0;
        NativeRepresent r;r.m_Canvas.width=width+extra;r.m_Canvas.height=height;
        for(int step=0;step<3;++step){
            int x=51000+step*37,y=100000+step*42;
            PTAlignSceneViewport(&r,x,y,width,height);
            assert(x-r.m_nLeft==width/2);
            assert(y/2-r.m_nTop==height/2);
            int clickX=width/2,clickY=height/2;r.ViewPortCoordToSpaceCoord(clickX,clickY,0);
            assert(clickX==x&&clickY==y);
            int left=17,top=29,worldX=left,worldY=top;r.ViewPortCoordToSpaceCoord(worldX,worldY,0);
            assert(worldX-r.m_nLeft==left&&worldY/2-r.m_nTop==top);
        }
    }
    puts("PASS production Represent2 projection: dock on/off, seven widths, moving focus, body/name alignment and inverse mouse coordinates");
    return 0;
}
