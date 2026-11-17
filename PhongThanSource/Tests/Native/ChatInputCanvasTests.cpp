#include <windows.h>
#include <assert.h>
#include <stdio.h>
#ifndef WM_MOUSEWHEEL
#define WM_MOUSEWHEEL 0x020A
#endif
static int worldWidth=1024,worldHeight=768,g_PtChatDockWidth=0,windowWidth=1024,windowHeight=768;
static void Wnd_GetScreenSize(int& w,int& h){w=worldWidth;h=worldHeight;}
static HWND g_GetMainHWnd(){return 0;}
static BOOL FakeGetClientRect(HWND,RECT* r){r->left=r->top=0;r->right=windowWidth;r->bottom=windowHeight;return TRUE;}
#define GetClientRect FakeGetClientRect
static LPARAM Transform(UINT uMsg,LPARAM lParam){
#include "../../Output/LegacyDropNativeTest/ChatInputBody.inl"
    return lParam;
}
int main(){
    const int widths[]={800,1024,1280,1536,1920};
    for(int i=0;i<5;++i)for(int mode=0;mode<3;++mode)for(int scale=1;scale<=2;++scale){
        worldWidth=widths[i];g_PtChatDockWidth=mode==1?260:0;
        int canvas=worldWidth+g_PtChatDockWidth;
        windowWidth=canvas*scale;windowHeight=worldHeight*scale;
        for(int x=0;x<canvas;x+=37){
            int y=400;LPARAM result=Transform(WM_LBUTTONDOWN,MAKELPARAM(x*scale,y*scale));
            assert((short)LOWORD(result)==x&&(short)HIWORD(result)==y);
        }
        // Modal buttons, NPCs and the chat composer share this coordinate space.
        LPARAM wheel=MAKELPARAM(777,333);assert(Transform(WM_MOUSEWHEEL,wheel)==wheel);
    }
    puts("PASS actual HandleInput mouse block: normal/docked/restored, full canvas, five resolutions and two scales; wheel unchanged");
    return 0;
}
