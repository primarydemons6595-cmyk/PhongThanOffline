#include <windows.h>
#include <ddraw.h>
#include <assert.h>
#include <stdio.h>
#include "../../Sources/Engine/Src/PhongThanWindowPresent.h"
struct Surface {
    HRESULT lost,restore;int releases;
    Surface():lost(DD_OK),restore(DD_OK),releases(0){}
    HRESULT IsLost(){return lost;}
    HRESULT Restore(){return restore;}
    void Release(){++releases;}
};
struct Device { Surface* next;Surface* CreateSurface(int w,int h){assert(w==1024&&h==768);return next;} } device;
Device* g_pDirectDraw=&device;
enum { WINDOWMODE=1,FULLSCREEN=0 };
struct KCanvas {
    Surface* m_pSurface;int m_dwScreenMode,m_nWidth,m_nHeight;
    KCanvas():m_pSurface(0),m_dwScreenMode(WINDOWMODE),m_nWidth(1024),m_nHeight(768){}
    int PrepareSurface();
};
#define LPDIRECTDRAWSURFACE Surface*
#include "../../Sources/Engine/Src/PhongThanCanvasRecovery.inl"
#undef LPDIRECTDRAWSURFACE
void Recovery(){
    KCanvas c;Surface old,fresh;c.m_pSurface=&old;
    assert(c.PrepareSurface()==1&&old.releases==0);
    old.lost=DDERR_SURFACELOST;assert(c.PrepareSurface()==2);
    old.restore=DDERR_WRONGMODE;device.next=0;
    assert(c.PrepareSurface()==0&&c.m_pSurface==&old&&old.releases==0);
    device.next=&fresh;assert(c.PrepareSurface()==2&&c.m_pSurface==&fresh&&old.releases==1);
    assert(c.PrepareSurface()==1);
    c.m_pSurface=&old;c.m_dwScreenMode=FULLSCREEN;
    assert(c.PrepareSurface()==0&&old.releases==1);
    old.lost=DDERR_SURFACEBUSY;assert(c.PrepareSurface()==0);
    c.m_pSurface=0;assert(c.PrepareSurface()==0);
    puts("PASS: ready/lost/restore/wrong-mode/recreate/retry/fullscreen guards");
}
int main(){
    HDC dc=CreateCompatibleDC(NULL);assert(dc);
    BITMAPINFO dst;ZeroMemory(&dst,sizeof(dst));dst.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
    dst.bmiHeader.biWidth=64;dst.bmiHeader.biHeight=-64;dst.bmiHeader.biPlanes=1;dst.bmiHeader.biBitCount=32;
    void* bits=0;HBITMAP bmp=CreateDIBSection(dc,&dst,DIB_RGB_COLORS,&bits,NULL,0);assert(bmp&&bits);
    HGDIOBJ prior=SelectObject(dc,bmp);
    // Four visible pixels plus two padding pixels per line. Positive surface
    // pitch, top-down rows; the helper must not expose padding or invert rows.
    WORD source[12]={0xf800,0xf800,0x07e0,0x07e0,0xffff,0xffff,
                     0x001f,0x001f,0xffff,0xffff,0xf800,0xf800};
    for(int width=4;width<=64;width*=2){
        SetStretchBltMode(dc,BLACKONWHITE);
        assert(PhongThanPresentDib(dc,source,4,2,12,width,width,0xf800,0x07e0,0x001f));
        GdiFlush();assert(GetPixel(dc,0,0)==RGB(255,0,0));
        assert(GetPixel(dc,width-1,0)==RGB(0,255,0));
        assert(GetPixel(dc,0,width-1)==RGB(0,0,255));
        assert(GetPixel(dc,width-1,width-1)==RGB(255,255,255));
        assert(GetStretchBltMode(dc)==BLACKONWHITE);
    }
    WORD pattern[256];
    for(int yy=0;yy<16;++yy)for(int xx=0;xx<16;++xx)pattern[yy*16+xx]=((xx+yy)&1)?0xffff:0;
    assert(PhongThanPresentDib(dc,pattern,16,16,32,8,8,0xf800,0x07e0,0x001f));
    GdiFlush();COLORREF filtered=GetPixel(dc,4,4);
    assert(GetRValue(filtered)>32 && GetRValue(filtered)<223);
    assert(GetRValue(filtered)==GetGValue(filtered)&&GetGValue(filtered)==GetBValue(filtered));
    puts("PASS scaled checkerboard retains averaged detail rather than dropping alternate pixels");
    source[0]=0x7c00;
    assert(PhongThanPresentDib(dc,source,4,2,12,4,2,0x7c00,0x03e0,0x001f));
    GdiFlush();assert(GetPixel(dc,0,0)==RGB(255,0,0));
    assert(!PhongThanPresentDib(dc,source,4,2,6,4,2,0xf800,0x07e0,0x001f));
    assert(!PhongThanPresentDib(dc,source,4,2,12,0,2,0xf800,0x07e0,0x001f));
    assert(!PhongThanPresentDib(NULL,source,4,2,12,4,2,0xf800,0x07e0,0x001f));
    LPDIRECTDRAW draw=0;assert(DirectDrawCreate(NULL,&draw,NULL)==DD_OK);
    assert(draw->SetCooperativeLevel(GetDesktopWindow(),DDSCL_NORMAL)==DD_OK);
    DDSURFACEDESC desc;ZeroMemory(&desc,sizeof(desc));desc.dwSize=sizeof(desc);
    desc.dwFlags=DDSD_WIDTH|DDSD_HEIGHT|DDSD_CAPS|DDSD_PIXELFORMAT;
    desc.dwWidth=4;desc.dwHeight=2;
    desc.ddsCaps.dwCaps=DDSCAPS_OFFSCREENPLAIN|DDSCAPS_SYSTEMMEMORY;
    desc.ddpfPixelFormat.dwSize=sizeof(DDPIXELFORMAT);desc.ddpfPixelFormat.dwFlags=DDPF_RGB;
    desc.ddpfPixelFormat.dwRGBBitCount=16;desc.ddpfPixelFormat.dwRBitMask=0xf800;
    desc.ddpfPixelFormat.dwGBitMask=0x07e0;desc.ddpfPixelFormat.dwBBitMask=0x001f;
    LPDIRECTDRAWSURFACE surface=0;assert(draw->CreateSurface(&desc,&surface,NULL)==DD_OK);
    ZeroMemory(&desc,sizeof(desc));desc.dwSize=sizeof(desc);
    assert(surface->Lock(NULL,&desc,DDLOCK_WAIT,NULL)==DD_OK);
    for(int row=0;row<2;++row)for(int col=0;col<4;++col)
        ((WORD*)((char*)desc.lpSurface+row*desc.lPitch))[col]=row?0x001f:0xf800;
    assert(PhongThanPresentDib(dc,desc.lpSurface,4,2,desc.lPitch,32,32,
        desc.ddpfPixelFormat.dwRBitMask,desc.ddpfPixelFormat.dwGBitMask,desc.ddpfPixelFormat.dwBBitMask));
    GdiFlush();assert(GetPixel(dc,0,0)==RGB(255,0,0)&&GetPixel(dc,0,31)==RGB(0,0,255));
    surface->Unlock(NULL);surface->Release();draw->Release();
    puts("PASS real DirectDraw system-memory canvas -> GDI pixels and lock metadata");
    SelectObject(dc,prior);DeleteObject(bmp);DeleteDC(dc);
    puts("PASS real GDI pixels: 565/555 colors, padded pitch, top-down, scaling, state restoration and invalid/zero target");
    Recovery();return 0;
}
