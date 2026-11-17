#include <windows.h>
#include <assert.h>
#include <stdio.h>
#include "KWin32.h"
#include "KFilePath.h"
#include "KPakFile.h"
#include "KPakList.h"
#include "KIniFile.h"
KPakList* g_pPakList=0;
int main(int argc,char**argv){
 assert(argc==2&&SetCurrentDirectoryA(argv[1]));g_SetRootPath(0);g_SetFilePath("\\");KPakList packs;assert(packs.Open("\\package.ini"));g_pPakList=&packs;g_SetPakFileMode(1);
 const char* themes[]={"classic","tet","christmas","midautumn","halloween"};const char* images[]={"frame","buy","nav","close","confirm","confirm-small","confirm-button","confirm-step","confirm-tight"};int widths[]={640,128,88,72,400,320,110,32,300},heights[]={560,36,30,30,320,190,26,24,140};int files=0,frames=0;
 for(int t=0;t<5;++t)for(int a=0;a<9;++a){char path[180];sprintf(path,"\\spr\\phongthan\\event_shop_%s\\%s.spr",themes[t],images[a]);SPROFFS* offsets=0;SPRHEAD* h=SprGetHeader(path,offsets);assert(h&&offsets&&h->Width==widths[a]&&h->Height==heights[a]&&h->Frames==((a==0||a==4||a==5||a==8)?1:3));
  for(int f=0;f<h->Frames;++f){SPRFRAME* frame=(SPRFRAME*)((char*)offsets+h->Frames*sizeof(SPROFFS)+offsets[f].Offset);unsigned target=(frame->Height/2)*frame->Width+frame->Width/2,at=0;bool center=false;const unsigned char* p=frame->Sprite;
   while(at<(unsigned)frame->Width*frame->Height){unsigned run=*p++,alpha=*p++;assert(run&&at+run<=(unsigned)frame->Width*frame->Height);if(at<=target&&at+run>target){assert((a==0||a==4||a==5||a==8)?alpha==0:alpha>=240);center=true;}if(alpha)p+=run;at+=run;}assert(center);++frames;
  }SprReleaseHeader(h);++files;
 }
 const char* bank[]={"bank-lock","bank-rent","bank-money","bank-sort"};int bw[]={64,64,56,44},bh[]={26,18,26,26};
 for(int b=0;b<4;++b){char path[160];sprintf(path,"\\spr\\phongthan\\event_shop_classic\\%s.spr",bank[b]);SPROFFS* offsets=0;SPRHEAD* h=SprGetHeader(path,offsets);assert(h&&offsets&&h->Width==bw[b]&&h->Height==bh[b]&&h->Frames==3);frames+=3;++files;SprReleaseHeader(h);}
 KIniFile ini;assert(ini.Load("\\ui\\ui3\\UiPlayerLock.ini"));int value=0;ini.GetInteger("Main","Height",0,&value);assert(value==230);ini.GetInteger("UnlockBtn","Width",0,&value);assert(value==64);
 assert(ini.Load("\\ui\\ui3\\UiGetMoney.ini"));ini.GetInteger("Main","Width",0,&value);assert(value==300);ini.GetInteger("MoneyInput","MaxLen",0,&value);assert(value==10);ini.GetInteger("CancelBtn","Over",-1,&value);assert(value==0);
 g_pPakList=0;printf("PASS EVENT_SHOP_SPRITES assets=%d frames=%d production_loader=1 transparent_centers=1 opaque_buttons=1 bank_schemes=2\n",files,frames);return 0;
}
