#include <windows.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
struct KRPosition2 {int nX,nY;};
struct KImageParam {int nWidth,nHeight,nNumFrames,nInterval;};
enum {ISI_T_SPR=0};
static DWORD clockTick=100;
#define GetTickCount() clockTick
struct TestRepresent {
 int params,frames;bool missing;
 TestRepresent():params(0),frames(0),missing(false){}
 bool GetImageParam(const char* path,KImageParam* info,int){
  ++params;if(missing||strstr(path,"absent"))return false;
  info->nWidth=64;info->nHeight=80;info->nNumFrames=4;info->nInterval=100;return true;
 }
 bool GetImageFrameParam(const char*,int f,KRPosition2* offset,KRPosition2* size,int){
  ++frames;if(missing)return false;offset->nX=f;offset->nY=f+1;size->nX=40+f;size->nY=60+f;return true;
 }
};
static TestRepresent rep;static TestRepresent* g_pRepresent=&rep;
#include "../../Sources/Core/Src/PhongThanUiSpriteCache.inl"
int main(){
 KRPosition2 offset,size;
 // Thousands of painted slots reuse authored metadata and every animation frame.
 for(int i=0;i<10000;++i){int frame=i%4;assert(PtUiSpriteFrameInfo("equipment.spr",frame,offset,size));assert(offset.nX==frame&&size.nY==60+frame);}
 assert(rep.params==1&&rep.frames==4);
 printf("STEADY_PAINT 10000 metadata/frame requests -> params=%d frameQueries=%d\n",rep.params,rep.frames);
 int before=rep.params;for(i=0;i<10000;++i)assert(!PtUiSprite("absent.spr"));assert(rep.params==before+1);
 clockTick+=5000;assert(!PtUiSprite("absent.spr"));assert(rep.params==before+2);
 // A failed resource can recover; never cache a transient miss forever.
 rep.missing=true;assert(!PtUiSprite("recover.spr"));rep.missing=false;clockTick+=5000;assert(PtUiSprite("recover.spr"));
 rep.missing=true;assert(!PtUiSpriteFrameInfo("equipment.spr",3+1,offset,size));assert(PtUiSpriteFrameInfo("equipment.spr",3,offset,size));
 rep.missing=false;
 // Metadata memory remains bounded when viewing many different items.
 for(i=0;i<1500;++i){char p[40];sprintf(p,"item-%d.spr",i);assert(PtUiSpriteFrameInfo(p,0,offset,size));assert(PtUiSpriteEntries().size()<=512);}
 clockTick=0xfffffff0;rep.missing=true;assert(!PtUiSprite("wrap.spr"));rep.missing=false;clockTick+=5001;assert(PtUiSprite("wrap.spr"));
 g_pRepresent=0;assert(!PtUiSprite("equipment.spr"));
 puts("UI_SPRITE_CACHE_PASS steady animation, misses/retry, overflow, bound, null renderer");return 0;
}
