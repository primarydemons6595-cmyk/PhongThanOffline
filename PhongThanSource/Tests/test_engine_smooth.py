"""Compile the actual collision/render guards; exercise boundaries and RLE safety.

The benchmark measures guard overhead on a synthetic 80x120 indexed sprite,
not gameplay FPS. Run with the VC6 compiler used by Build-PhongThan.ps1.
"""
from pathlib import Path
import os
import subprocess

ROOT = Path(__file__).resolve().parents[1]
OUT = ROOT / 'Output/Tools/EngineSmoothTests'


def function(text, signature):
    start = text.index(signature)
    brace = text.index('{', start)
    depth = 1
    end = brace + 1
    while depth:
        depth += (text[end] == '{') - (text[end] == '}')
        end += 1
    return text[start:end]


def main():
    scene = (ROOT / 'Sources/Core/Src/Scene/KScenePlaceRegionC.cpp').read_text('latin1')
    renderer = (ROOT / 'Sources/Represent/Represent2/KRepresentShell2.cpp').read_text('latin1')
    store = (ROOT / 'Sources/Represent/Represent2/KImageStore2.cpp').read_text('latin1')
    assert 'KSprIsValidIndexedRle' not in function(store, 'void* KImageStore2::GetSprFrame(')
    assert 'KSprIsValidIndexedRle' in function(store, 'void* KImageStore2::LoadImage(')
    assert 'IsUsableIndexedRle' not in renderer
    cpp = r'''
#include <windows.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "KWin32.h"
#include "KSpriteValidation.h"
#include "ObstacleDef.h"
#define _ASSERT assert
class KScenePlaceRegionC {
public:
    enum {RWP_NUM_GROUND_CELL_H=16, RWP_NUM_GROUND_CELL_V=16,
          RWP_OBSTACLE_WIDTH=32, RWP_OBSTACLE_HEIGHT=32};
    POINT m_LeftTopCornerScenePos;
    long m_ObstacleInfo[16][32];
    long GetObstacleInfo(int x,int y);
    long GetObstacleInfoMin(int x,int y,int ox,int oy);
};
#define GET_SPR_PALETTE(p) (((char*)(p))+sizeof(SPRHEAD))
'''
    cpp += function(scene, 'long KScenePlaceRegionC::GetObstacleInfo(') + '\n'
    cpp += function(scene, 'long KScenePlaceRegionC::GetObstacleInfoMin(') + '\n'
    cpp += function(renderer, 'static char* GetSprFramePalette(') + '\n'
    cpp += function(renderer, 'static bool IsUsableSprFrame(') + '\n'
    loop = (ROOT / 'Sources/GameClient/PhongThanClient.cpp').read_text('latin1')
    cpp += r'''
struct FakeTimer { DWORD now; DWORD GetElapse(){return now;} };
struct FakeCore { int ticks; bool Breathe(){++ticks;return true;} } core;
FakeCore* g_pCoreShell=&core;
struct FakeNet { void Breathe(){} } g_NetConnectAgent;
int paints=0,sleeps=0;
int UiHeartBeat(){return 1;}
void UiPaint(int){++paints;}
void FakeSleep(int){++sleeps;}
#define GAME_FPS 18
class KMyApp {
public:
    FakeTimer m_Timer;DWORD m_GameCounter,m_LastPaintTime;unsigned m_RenderFps;
    BOOL GameLoop();
};
'''
    cpp += function(loop, 'BOOL KMyApp::GameLoop()').replace('Sleep(1)', 'FakeSleep(1)')
    cpp += r'''
// Historical per-draw RLE scan, retained only for cost comparison.
static bool OldScan(const SPRFRAME* f) {
    unsigned long pixels=(unsigned long)f->Width*f->Height, done=0;
    unsigned line=0; const unsigned char* p=f->Sprite;
    while(done<pixels) {
        if(IsBadReadPtr(p,2)) return false;
        unsigned run=p[0],alpha=p[1];p+=2;
        if(!run || run>pixels-done || run>f->Width-line) return false;
        if(alpha) {if(IsBadReadPtr(p,run)) return false;p+=run;}
        done+=run;line+=run;if(line==f->Width)line=0;
    }
    return line==0;
}
static int Reference(int type,int kind,int x,int y,int width) {
    if(type==2 && x+y>width)return 0;
    if(type==3 && x<y)return 0;
    if(type==4 && x>y)return 0;
    if(type==5 && x+y<width)return 0;
    return kind;
}
int main() {
    for(int mode=0;mode<2;++mode) {
        KMyApp app;app.m_GameCounter=0;app.m_LastPaintTime=0;
        app.m_RenderFps=mode?54:30;core.ticks=0;paints=0;sleeps=0;
        for(DWORD ms=0;ms<10000;++ms){app.m_Timer.now=ms;assert(app.GameLoop());}
        assert(core.ticks==180);
        assert(paints>=(mode?500:290) && paints<=(mode?556:304));
        assert(sleeps>8000);
        int previous=paints;app.m_Timer.now=15000;
        // A stall must preserve logic ticks instead of changing world speed.
        for(int n=0;n<91;++n)assert(app.GameLoop());
        assert(core.ticks==271 && paints==previous);
        assert(app.GameLoop() && paints==previous+1);
        // 64-bit arithmetic: products exceed DWORD after a long session.
        app.m_Timer.now=300000000;app.m_GameCounter=5400000;
        assert(app.GameLoop() && app.m_GameCounter==5400001);
    }
    KScenePlaceRegionC scene; scene.m_LeftTopCornerScenePos.x=5120;
    scene.m_LeftTopCornerScenePos.y=10240;
    unsigned checks=0;
    for(int kind=0;kind<5;++kind) for(int type=0;type<7;++type) {
        for(int gx=0;gx<16;++gx)for(int gy=0;gy<32;++gy)
            scene.m_ObstacleInfo[gx][gy]=kind|(type<<4);
        for(int x=0;x<32;++x)for(int y=0;y<32;++y) {
            int wx=5120+7*32+x,wy=10240+15*32+y;
            assert(scene.GetObstacleInfo(wx,wy)==Reference(type,kind,x,y,32));
            assert(scene.GetObstacleInfoMin(wx,wy,0,0)==Reference(type,kind,x,y,32));
            // Fractional query must continue matching the server fixed-point rules.
            assert(scene.GetObstacleInfoMin(wx,wy,700,200)==
                Reference(type,kind,x*1024+700,y*1024+200,32*1024));
            checks+=3;
        }
    }
    assert(scene.GetObstacleInfo(5119,10240)==Obstacle_Normal);
    assert(scene.GetObstacleInfo(5120,10239)==Obstacle_Normal);
    assert(scene.GetObstacleInfo(5120+512,10240)==Obstacle_Normal);
    assert(scene.GetObstacleInfo(5120,10240+1024)==Obstacle_Normal);
    unsigned char blob[sizeof(SPRHEAD)+768+8+120*44];
    memset(blob,0,sizeof(blob));SPRHEAD* head=(SPRHEAD*)blob;
    head->Colors=256;SPRFRAME* frame=(SPRFRAME*)(blob+sizeof(SPRHEAD)+768);
    frame->Width=80;frame->Height=120;unsigned char* data=frame->Sprite;
    for(int row=0;row<120;++row) {
        *data++=40;*data++=255;memset(data,1,40);data+=40;
        *data++=40;*data++=0;
    }
    unsigned size=data-frame->Sprite;
    assert(KSprIsValidIndexedRle(frame->Sprite,size,80,120,256));
    assert(!KSprIsValidIndexedRle(frame->Sprite,size-1,80,120,256));
    assert(!KSprIsValidIndexedRle(frame->Sprite,size,80,120,1));
    frame->Sprite[0]=0;assert(!KSprIsValidIndexedRle(frame->Sprite,size,80,120,256));
    frame->Sprite[0]=81;assert(!KSprIsValidIndexedRle(frame->Sprite,size,80,120,256));
    frame->Sprite[0]=40;assert(IsUsableSprFrame(head,frame,"benchmark",0));
    LARGE_INTEGER freq,a,b,c;QueryPerformanceFrequency(&freq);
    volatile int valid=0;const int repeats=20000;
    QueryPerformanceCounter(&a);
    for(int i=0;i<repeats;++i)valid+=OldScan(frame);
    QueryPerformanceCounter(&b);
    for(i=0;i<repeats;++i)valid+=IsUsableSprFrame(head,frame,"benchmark",0);
    QueryPerformanceCounter(&c);
    assert(valid==repeats*2);
    printf("GAME_LOOP_MODES=2 PASS (18Hz, render cap, catch-up, long session)\n");
    printf("COLLISION_CHECKS=%u PASS\nRLE_MALFORMED_CASES=4 PASS\n",checks+4);
    printf("SYNTHETIC_GUARD_BENCHMARK repeats=%d old_ms=%.3f new_ms=%.3f\n",repeats,
        (double)(b.QuadPart-a.QuadPart)*1000/freq.QuadPart,
        (double)(c.QuadPart-b.QuadPart)*1000/freq.QuadPart);
    return 0;
}
'''
    OUT.mkdir(parents=True, exist_ok=True)
    source = OUT / 'EngineSmoothTests.cpp'
    source.write_bytes(cpp.encode('latin1'))
    compiler = Path(os.environ.get('PHONGTHAN_CL',
        r'C:\Program Files (x86)\Microsoft Visual Studio\VC98\Bin\cl.exe'))
    vc = compiler.parent.parent
    env = dict(os.environ)
    env['INCLUDE'] = str(vc / 'Include')
    env['LIB'] = str(vc / 'Lib')
    env['PATH'] = str(compiler.parent) + ';' + str(vc.parent / 'Common/MSDev98/Bin') + ';' + env['PATH']
    exe = OUT / 'EngineSmoothTests.exe'
    subprocess.run([str(compiler), '/nologo', '/MT', '/O2', '/W3', '/DWIN32',
        '/I' + str(ROOT / 'Sources/Engine/Src'),
        '/I' + str(ROOT / 'Sources/Core/Src/Scene'),
        '/Fo' + str(OUT / 'EngineSmoothTests.obj'), '/Fe' + str(exe), str(source)],
        env=env, check=True)
    subprocess.run([str(exe)], check=True)


if __name__ == '__main__':
    main()
