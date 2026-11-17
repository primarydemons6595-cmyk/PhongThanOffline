#include <windows.h>
#include <stdio.h>
#include <string.h>
#include "../../Sources/Represent/iRepresent/KRepresentUnit.h"
#include "../../Headers/PhongThanNpcDialogRules.h"
struct Draw { int type; KRUImagePart image; };
class iRepresentShell {
public:
    Draw calls[32]; int count;
    iRepresentShell():count(0){}
    void DrawPrimitives(int, void* data, int type, BOOL screen) {
        if (!screen) { puts("FAIL world-space draw"); ExitProcess(2); }
        Draw& d=calls[count++];d.type=type;
        memset(&d.image,0,sizeof(d.image));
        memcpy(&d.image,data,type==RU_T_IMAGE_PART?sizeof(KRUImagePart):sizeof(KRUImage));
    }
};
#include "../../Headers/PhongThanOverheadDraw.h"
#define CHECK(x) do { if(!(x)){printf("FAIL line=%d\n",__LINE__);return 1;} }while(0)
int main(){
    CHECK(!PhongThanShowSkull(59,40));CHECK(PhongThanShowSkull(60,40));
    CHECK(PhongThanShowSkull(61,40));CHECK(!PhongThanShowSkull(20,40));
    CHECK(!PhongThanShowSkull(60,0));CHECK(!PhongThanShowSkull(40,40));
    iRepresentShell rep;
    PhongThanDrawLife(&rep,400,200,40,40,100,0);
    CHECK(rep.count==4);CHECK(rep.calls[0].image.oPosition.nX==361);
    CHECK(rep.calls[1].type==RU_T_IMAGE_PART);
    CHECK(rep.calls[1].image.oImgRBPos.nX==50);
    CHECK(rep.calls[1].image.oPosition.nX==386 && rep.calls[1].image.oPosition.nY==205);
    CHECK(rep.calls[2].image.oPosition.nX==365 && rep.calls[2].image.oPosition.nY==202);
    CHECK(rep.calls[2].image.nFrame==4 && rep.calls[3].image.nFrame==0);
    CHECK(rep.calls[0].image.bRenderFlag==0);
    rep.count=0;PhongThanDrawLife(&rep,400,200,60,40,49,1);
    CHECK(rep.count==3);CHECK(rep.calls[0].image.oPosition.nX==286);
    CHECK(rep.calls[1].image.nFrame==1 && rep.calls[1].image.oImgRBPos.nX==98);
    CHECK(strstr(rep.calls[2].image.szImage,"skull.spr")!=0);
    CHECK(rep.calls[2].image.oPosition.nX==288 && rep.calls[2].image.oPosition.nY==201);
    rep.count=0;PhongThanDrawLife(&rep,400,200,5,40,24,0);
    CHECK(rep.calls[1].image.nFrame==2 && rep.calls[1].image.oImgRBPos.nX==12);
    rep.count=0;PhongThanDrawLife(&rep,400,200,5,40,0,0);
    CHECK(rep.count==2 && rep.calls[1].image.nFrame==5);
    rep.count=0;PhongThanDrawLevelBadge(&rep,10,20,60,40);
    CHECK(rep.count==3 && rep.calls[0].image.oImgRBPos.nX==24);
    CHECK(rep.calls[1].type==RU_T_IMAGE_PART && rep.calls[1].image.oImgLTPos.nX==77);
    CHECK(rep.calls[1].image.oPosition.nX==33);
    CHECK(strstr(rep.calls[2].image.szImage,"skull.spr")!=0);
    CHECK(PhongThanLevelLeft(100,9)==107 && PhongThanLevelLeft(100,99)==104);
    CHECK(PhongThanLevelLeft(100,100)==101);
    CHECK(PhongThanIsDialogExit("Thoat"));CHECK(PhongThanIsDialogExit("<color=yellow>Thoat (Esc)<color>"));
    CHECK(PhongThanIsDialogExit("Tho\xB8t"));CHECK(!PhongThanIsDialogExit("Mua vat pham"));
    CHECK(!PhongThanIsDialogExit("Thoat khoi bang"));
    const char encodedExit[]={2,0,(char)255,0,'T','h','o',(char)0xb8,'t',3};
    CHECK(PhongThanIsDialogExit(encodedExit,sizeof(encodedExit)));
    puts("PASS: original sprite draws, HP clipping/colors, centered digits, skull 19/20/21, team badge, exit filtering.");
    return 0;
}
