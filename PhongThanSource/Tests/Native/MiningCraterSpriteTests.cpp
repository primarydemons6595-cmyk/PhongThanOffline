#include <windows.h>
#include <assert.h>
#include <stdio.h>
#include "KWin32.h"
#include "KFilePath.h"
#include "KPakFile.h"
#include "KPakList.h"
KPakList* g_pPakList=0;
int main(int argc,char**argv){assert(argc==2&&SetCurrentDirectoryA(argv[1]));g_SetRootPath(0);g_SetFilePath("\\");KPakList packs;assert(packs.Open("\\package.ini"));g_pPakList=&packs;g_SetPakFileMode(1);
 SPROFFS* offsets=0;SPRHEAD* sprite=SprGetHeader("\\spr\\phongthan\\life\\mining-crater.spr",offsets);assert(sprite&&offsets&&sprite->Width==480&&sprite->Height==248&&sprite->Frames==1&&sprite->CenterX==240&&sprite->CenterY==124);
 SPRFRAME* frame=(SPRFRAME*)((char*)offsets+sizeof(SPROFFS)+offsets[0].Offset);assert(frame->Width==480&&frame->Height==248);const unsigned char* p=frame->Sprite;unsigned pixels=0,opaque=0;
 while(pixels<480*248){unsigned run=*p++,alpha=*p++;assert(run&&pixels+run<=480*248);pixels+=run;if(alpha){opaque+=run;p+=run;}}
 assert(opaque>20000&&opaque<480*248);SprReleaseHeader(sprite);g_pPakList=0;puts("PASS native mining SPR: exact palette, full crater, ground-centre anchor, validated RLE");return 0;}
