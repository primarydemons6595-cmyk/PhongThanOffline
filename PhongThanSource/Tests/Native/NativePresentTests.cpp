#include <windows.h>
#include <ddraw.h>
#include <stdio.h>
#include <assert.h>
struct Surface {
 HRESULT lost,restore,blt,clip;int releases,blits,clips;
 Surface():lost(DD_OK),restore(DD_OK),blt(DD_OK),clip(DD_OK),releases(0),blits(0),clips(0){}
 HRESULT IsLost(){return lost;}HRESULT Restore(){return restore;}
 void Release(){++releases;}
 HRESULT Blt(RECT*,Surface*,RECT*,DWORD,void*){++blits;return blt;}
 HRESULT SetClipper(void*){++clips;return clip;}
};
struct Device {Surface* next;int creates;Device():next(0),creates(0){}
 HRESULT CreateSurface(DDSURFACEDESC*,Surface** out,void*){++creates;*out=next;return next?DD_OK:DDERR_GENERIC;}
};
static int left=0,top=0,screenWidth=1920;
BOOL TestClient(HWND,RECT* r){r->left=r->top=0;r->right=1024;r->bottom=768;return TRUE;}
BOOL TestToScreen(HWND,POINT* p){p->x+=left;p->y+=top;return TRUE;}
int TestMetrics(int index){return index==SM_CXSCREEN?screenWidth:1080;}
struct KDirectDraw {Device* m_lpDirectDraw;Surface* m_lpDDSPrimary;void* m_lpClipper;
 BOOL PresentWindowCanvas(Surface*,HWND);
};
#define LPDIRECTDRAWSURFACE Surface*
#define GetClientRect TestClient
#define ClientToScreen TestToScreen
#define GetSystemMetrics TestMetrics
#include "../../Sources/Engine/Src/PhongThanNativePresent.inl"
#undef LPDIRECTDRAWSURFACE
int main(){
 Device device;Surface primary,canvas,newPrimary;KDirectDraw d;
 d.m_lpDirectDraw=&device;d.m_lpDDSPrimary=&primary;d.m_lpClipper=(void*)1;
 assert(d.PresentWindowCanvas(&canvas,(HWND)1));assert(primary.blits==1&&device.creates==0);
 left=-1500;assert(!d.PresentWindowCanvas(&canvas,(HWND)1));assert(primary.blits==1);
 left=1920;assert(!d.PresentWindowCanvas(&canvas,(HWND)1));
 left=0;screenWidth=800;assert(!d.PresentWindowCanvas(&canvas,(HWND)1));
 screenWidth=1920;primary.lost=DDERR_SURFACELOST;primary.restore=DDERR_WRONGMODE;
 assert(!d.PresentWindowCanvas(&canvas,(HWND)1));assert(d.m_lpDDSPrimary==0);
 device.next=&newPrimary;assert(d.PresentWindowCanvas(&canvas,(HWND)1));
 assert(newPrimary.clips==1&&newPrimary.blits==1);
 newPrimary.blt=DDERR_SURFACELOST;assert(!d.PresentWindowCanvas(&canvas,(HWND)1));
 newPrimary.blt=DD_OK;assert(d.PresentWindowCanvas(&canvas,(HWND)1));
 newPrimary.lost=DDERR_SURFACELOST;newPrimary.restore=DDERR_WRONGMODE;newPrimary.clip=DDERR_GENERIC;
 assert(!d.PresentWindowCanvas(&canvas,(HWND)1));assert(d.m_lpDDSPrimary==0);
 newPrimary.lost=DD_OK;newPrimary.clip=DD_OK;assert(d.PresentWindowCanvas(&canvas,(HWND)1));
 puts("PASS native fast path, secondary/negative target fallback, desktop resize, primary recreate/retry, clipper and Blt failure");
 return 0;
}
