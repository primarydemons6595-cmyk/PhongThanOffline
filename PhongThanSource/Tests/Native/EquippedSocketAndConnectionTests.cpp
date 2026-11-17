#include <windows.h>
#include <assert.h>
#include <stdio.h>
#include <map>
#include <string>
#include "../../Sources/Represent/iRepresent/KRepresentUnit.h"
#include "../../Headers/PhongThanSocketState.h"
#include "../../Headers/PhongThanConnectionInfo.h"
#include "../../Headers/PhongThanBakedEquipmentVisual.h"
#include "../../Headers/PhongThanLifeSkills.h"
enum {item_equip=0};
class KItem {public:int genre,param,detail;int GetGenre(){return genre;}int GetParam(){return param;}int GetDetailType(){return detail;}};
static bool known=true;static int requested=0,drawn=0;static DWORD tick=0;static PT_SOCKET_VISUALS visuals;
bool PtSocketRead(int,PT_SOCKET_STATE&){return known;}
void PtSocketRequest(int){++requested;}
bool PtSocketVisuals(KItem&,PT_SOCKET_VISUALS& out){out=visuals;return true;}
struct RepresentMock {
 KRUImagePart draws[8];
 int GetImageParam(const char*,KImageParam* p,int){ZeroMemory(p,sizeof(*p));p->nWidth=p->nHeight=20;p->nNumFrames=2;p->nInterval=100;return 1;}
 int GetImageFrameParam(const char*,int frame,KRPosition2* offset,KRPosition2* size,int){offset->nX=offset->nY=frame?2:0;size->nX=size->nY=frame?16:20;return 1;}
 void DrawPrimitives(int count,void* data,int type,BOOL){assert(count==1&&type==RU_T_IMAGE_PART&&drawn<8);draws[drawn++]=*(KRUImagePart*)data;}
} renderer;
static RepresentMock* g_pRepresent=&renderer;
static void g_StrCpyLen(char* out,const char* in,int n){lstrcpyn(out,in,n);}
static DWORD TestTick(){return tick;}
#define GetTickCount TestTick
#include "../../Sources/Core/Src/PhongThanEquippedSocketArt.inl"
#undef GetTickCount
int main(){
 KItem item;item.genre=0;item.param=0xc2000001;item.detail=7;ZeroMemory(&visuals,sizeof(visuals));visuals.Count=1;visuals.X[0]=visuals.Y[0]=16;strcpy(visuals.HoleImage[0],"hole.spr");strcpy(visuals.Image[0],"gem.spr");
 PhongThanPaintEquippedSockets(item,10,20,68,68);assert(drawn==2&&!strcmp(renderer.draws[0].szImage,"hole.spr")&&!strcmp(renderer.draws[1].szImage,"gem.spr"));assert(renderer.draws[0].oPosition.nX==18&&renderer.draws[0].oPosition.nY==28);
 drawn=0;tick=100;PhongThanPaintEquippedSockets(item,10,20,68,68);assert(drawn==2&&renderer.draws[0].oPosition.nX==20&&renderer.draws[0].oPosition.nY==30); // cropped offset retained
 drawn=0;strcpy(visuals.Image[0],"hole.spr");PhongThanPaintEquippedSockets(item,10,20,68,68);assert(drawn==1);
 drawn=0;known=false;PhongThanPaintEquippedSockets(item,10,20,68,68);assert(!drawn&&requested==1);item.param=-1;PhongThanPaintEquippedSockets(item,10,20,68,68);assert(!drawn&&requested==1);
 int x,y;PhongThanEquippedSocketCenter(6,0,0,66,45,14,11,x,y);assert(x==15&&y==12);PhongThanEquippedSocketCenter(2,0,0,66,98,48,72,x,y);assert(x==49&&y==73);
 PT_CONNECTION_INFO net;ZeroMemory(&net,sizeof(net));assert(PhongThanConnectionQuality(false,net)==0&&PhongThanConnectionQuality(true,net)==1);net.HasSample=1;assert(PhongThanConnectionQuality(true,net)==2);net.RoundTripMs=100;assert(PhongThanConnectionQuality(true,net)==2);net.RoundTripMs=101;assert(PhongThanConnectionQuality(true,net)==3);net.RoundTripMs=301;assert(PhongThanConnectionQuality(true,net)==4);net.RoundTripMs=0;net.SampleAgeMs=10001;assert(PhongThanConnectionQuality(true,net)==1);net.SampleAgeMs=67000;net.RoundTripMs=400;assert(PhongThanConnectionQuality(true,net)==1);net.SampleAgeMs=0;assert(PhongThanConnectionQuality(true,net)==4);
 assert(PhongThanBakedEquipmentVisualLevel("#<c=pk>Tinh Qu\xA9n*Huyen Thiet*Ma C\xCAp 1 (+12)<c>",1)==12);assert(!PhongThanBakedEquipmentVisualLevel("#Tinh Qu\xA9n C\xCAp 4 (+12)",1));assert(!PhongThanBakedEquipmentVisualLevel("normal (+12)",1));assert(!PhongThanBakedEquipmentVisualLevel("#Tinh Qu\xA9n C\xCAp 1 (+11)",2));assert(PhongThanLifeLearnCost(0)==1000&&PhongThanLifeRequiredLevel(1)==20);assert(PhongThanLifeTrainerAllows(1021,1363,4)&&PhongThanLifeTrainerAllows(1021,1363,6)&&!PhongThanLifeTrainerAllows(1020,1362,6));
 puts("PASS F4 socket/gem order, original pixel planes, animation canvas/offsets, empty/missing state; connection unknown/fresh/stale thresholds; baked TQ12 visual guard; cooking/fishing distinct trainer/initial-level rules");return 0;
}
