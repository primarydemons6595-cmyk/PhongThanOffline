#include <windows.h>
#include <stdio.h>
#include <string.h>
#include "../../Headers/PhongThanPotProgress.h"
static DWORD testTick=1000;
static DWORD TestTick(){return testTick;}
#define GetTickCount TestTick
#include "../../Sources/Core/Src/PhongThanPotProgressClient.inl"
#undef GetTickCount
enum {ISI_T_SPR=1,IMAGE_IS_POSITION_INIT=-1,IMAGE_RENDER_STYLE_ALPHA=1,RU_T_IMAGE_PART=2,KRF_ZERO_END=-1,TEXT_IN_SINGLE_PLANE_COORD=1};
struct Point {int nX,nY;};
struct KRUImagePart {int nType,nISPosition,bRenderStyle,uImage;struct {struct {int a;} Color_b;} Color;Point oPosition,oImgLTPos,oImgRBPos;char szImage[256];};
struct Renderer {int track,fill,labels;void DrawPrimitives(int,KRUImagePart* image,int type,int){
 if(type!=RU_T_IMAGE_PART)return;
 if(!strcmp(image->szImage,"\\spr\\phongthan\\gather\\background.spr")&&image->oImgLTPos.nX==28&&image->oImgRBPos.nX==269)++track;
 if(!strcmp(image->szImage,"\\spr\\phongthan\\gather\\fill.spr")&&image->oImgRBPos.nX==235)++fill;
 }void OutputText(int,const char*,int,int,int,unsigned int,int,int,unsigned int){++labels;}};
#include "../../Headers/PhongThanGatherProgress.h"
int checks=0;
#define CHECK(c) do{if(!(c)){printf("FAIL line=%d %s\n",__LINE__,#c);return 1;}++checks;}while(0)
int main(){
 PTPotProgress p;memset(&p,0,sizeof(p));PhongThanInitializeWireHeader(&p.Header,PT_POT_PROGRESS,sizeof(p),PHONGTHAN_WIRE_FLAG_RESPONSE,0);
 p.MapId=1002;p.EntityId=42;p.Started=1000;p.Ready=1120;p.Burn=1135;p.Now=1060;strcpy(p.Text,"ready");
 CHECK(PTValidatePotProgress(&p,sizeof(p)));CHECK(!PTValidatePotProgress(&p,sizeof(p)-1));
 CHECK(PTPotRemainingPercent(p,1000)==100&&PTPotRemainingPercent(p,1060)==50&&PTPotRemainingPercent(p,1120)==0);
 PhongThanReceivePotProgress(p);PTPotProgress view;CHECK(PhongThanGetPotProgress(1002,42,view)&&view.Now==1060);
 testTick+=2000;CHECK(PhongThanGetPotProgress(1002,42,view)&&view.Now==1062);
 CHECK(!PhongThanGetPotProgress(1003,42,view)&&!PhongThanGetPotProgress(1002,43,view));
 testTick+=4000;CHECK(!PhongThanGetPotProgress(1002,42,view));
 p.Now=p.Burn;PhongThanReceivePotProgress(p);CHECK(!PhongThanGetPotProgress(1002,42,view));
 p.Ready=p.Started;CHECK(!PTValidatePotProgress(&p,sizeof(p)));p.Ready=1120;p.Text[255]='x';CHECK(!PTValidatePotProgress(&p,sizeof(p)));
 Renderer renderer;memset(&renderer,0,sizeof(renderer));PhongThanDrawGatherProgress(&renderer,300,200,0,"");
 CHECK(renderer.track==1&&renderer.fill==1&&renderer.labels==0);
 PhongThanDrawGatherProgress(&renderer,300,200,0);CHECK(renderer.track==1);
 puts("PASS POT_PROGRESS native_deadlines_and_existing_game_progress_art=1");return 0;
}
