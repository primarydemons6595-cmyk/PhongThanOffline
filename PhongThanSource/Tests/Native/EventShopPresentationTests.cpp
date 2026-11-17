#include <windows.h>
#include <assert.h>
#include <stdio.h>
#include "../../Sources/Represent/iRepresent/KRepresentUnit.h"
#include "../../Headers/PhongThanEventShopPresentation.h"
int main(int argc,char**argv){
 assert(argc==2);char ini[MAX_PATH];sprintf(ini,"%s\\ui\\ui3\\UiPubLicSetting.ini",argv[1]);
 int count=GetPrivateProfileIntA("FontList","Count",0,ini);assert(count>0);bool titleFont=false,unsupported20=false;
 for(int i=0;i<count;++i){char key[20];sprintf(key,"%d",i);int id=GetPrivateProfileIntA("FontList",key,0,ini);if(id==PT_EVENT_SHOP_TITLE_FONT)titleFont=true;if(id==20)unsupported20=true;}
 assert(titleFont&&!unsupported20);
 KRUShadow shadow;PTEventShopGlass(shadow,120,40);assert(shadow.Color.Color_b.a==18&&shadow.Color.Color_b.a<=31);assert(shadow.Color.Color_b.r==0&&shadow.Color.Color_b.g==0&&shadow.Color.Color_b.b==0);assert(shadow.oPosition.nX==145&&shadow.oPosition.nY==142&&shadow.oEndPos.nX==735&&shadow.oEndPos.nY==536);
 assert(255-(shadow.Color.Color_b.a<<3)==111);
 assert(PTEventShopLabelY(36,14)==10&&PTEventShopLabelY(30,14)==7);
 assert(PTEventShopButtonFrame(true,false,false)==0&&PTEventShopButtonFrame(true,true,true)==2&&PTEventShopButtonFrame(false,false,true)==0);
 for(i=0;i<1000;++i)assert(PTEventShopButtonFrame(true,false,true)==1);
 puts("PASS EVENT_SHOP_PRESENTATION native_alpha=18 registered_title_font=18 centered_labels=1 stable_hover=1000");return 0;
}
