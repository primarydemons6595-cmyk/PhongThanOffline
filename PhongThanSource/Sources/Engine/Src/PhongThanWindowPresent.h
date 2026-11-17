#ifndef PHONGTHAN_WINDOW_PRESENT_H
#define PHONGTHAN_WINDOW_PRESENT_H
#include <stdlib.h>
// Present in client coordinates. No cached primary-monitor rectangle or
// DirectDraw primary surface participates in windowed presentation.
static bool PhongThanPresentDib(HDC dc, const void* pixels, int width, int height,
    int pitch, int destWidth, int destHeight, DWORD red, DWORD green, DWORD blue)
{
    if (!dc || !pixels || width<=0 || height<=0 || destWidth<=0 || destHeight<=0 ||
        pitch<width*2 || (pitch&3)) return false;
    if(width>8192 || height>8192)return false;
    // Expand once into a reusable 32-bit DIB. HALFTONE on a 16-bit source
    // truncates endpoint colors (e.g. red255 becomes248 on tested GDI).
    static DWORD palette[65536],lastRed=0,lastGreen=0,lastBlue=0;
    static DWORD* staging=NULL;static unsigned long capacity=0;
    if(!red || !green || !blue)return false;
    if(red!=lastRed || green!=lastGreen || blue!=lastBlue){
        int rs=0,gs=0,bs=0;
        while(((red>>rs)&1)==0)++rs;
        while(((green>>gs)&1)==0)++gs;
        while(((blue>>bs)&1)==0)++bs;
        for(unsigned int c=0;c<65536;++c){
            DWORD r=((c&red)>>rs)*255/(red>>rs);
            DWORD g=((c&green)>>gs)*255/(green>>gs);
            DWORD b=((c&blue)>>bs)*255/(blue>>bs);
            palette[c]=(r<<16)|(g<<8)|b;
        }
        lastRed=red;lastGreen=green;lastBlue=blue;
    }
    unsigned long required=(unsigned long)width*height;
    if(required>capacity){
        DWORD* replacement=(DWORD*)realloc(staging,required*sizeof(DWORD));
        if(!replacement)return false;
        staging=replacement;capacity=required;
    }
    for(int y=0;y<height;++y){
        const WORD* row=(const WORD*)((const char*)pixels+y*pitch);
        DWORD* dest=staging+y*width;
        for(int x=0;x<width;++x)dest[x]=palette[row[x]];
    }
    BITMAPINFO info;ZeroMemory(&info,sizeof(info));
    info.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
    info.bmiHeader.biWidth=width;info.bmiHeader.biHeight=-height;
    info.bmiHeader.biPlanes=1;info.bmiHeader.biBitCount=32;
    info.bmiHeader.biCompression=BI_RGB;
    int saved=SaveDC(dc);if(!saved)return false;
    // Exact-size output preserves pixels. Scaled output uses area filtering
    // instead of dropping/duplicating rows of small text and sprite details.
    SetStretchBltMode(dc,(destWidth==width && destHeight==height)?COLORONCOLOR:HALFTONE);
    SetBrushOrgEx(dc,0,0,NULL);
    int result=StretchDIBits(dc,0,0,destWidth,destHeight,0,0,width,height,
        staging,&info,DIB_RGB_COLORS,SRCCOPY);
    RestoreDC(dc,saved);
    return result!=0 && result!=GDI_ERROR;
}
#endif
