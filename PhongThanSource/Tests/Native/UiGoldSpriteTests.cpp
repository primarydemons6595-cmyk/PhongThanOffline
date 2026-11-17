#include <windows.h>
#include <stdio.h>
#include <assert.h>
#include "KWin32.h"
#include "KFilePath.h"
#include "KPakFile.h"
#include "KPakList.h"
KPakList* g_pPakList=0;
int main(int argc,char**argv){
 assert(argc==3&&SetCurrentDirectoryA(argv[1]));g_SetRootPath(0);g_SetFilePath("\\");KPakList packs;assert(packs.Open("\\package.ini"));g_pPakList=&packs;g_SetPakFileMode(1);
 FILE* list=fopen(argv[2],"rb");assert(list);char hex[4096],path[2048];int goldRequired,files=0,frames=0,goldFrames=0;
 while(fscanf(list,"%d %4095s",&goldRequired,hex)==2){
  size_t len=strlen(hex);assert(len%2==0&&len/2<sizeof(path));
  for(size_t j=0;j<len/2;++j){unsigned v;assert(sscanf(hex+2*j,"%2x",&v)==1);path[j]=(char)v;}path[len/2]=0;
  SPROFFS* offsets=0;SPRHEAD* h=SprGetHeader(path,offsets);assert(h&&offsets&&h->Reserved[SPR_FRAME_PALETTE_RESERVED_INDEX]==SPR_FRAME_PALETTE_MARK);
  for(int f=0;f<h->Frames;++f){SPRFRAME* frame=(SPRFRAME*)((char*)offsets+h->Frames*sizeof(SPROFFS)+offsets[f].Offset);
   const unsigned char* palette=(const unsigned char*)frame-h->Colors*3;const unsigned char* p=frame->Sprite;unsigned done=0,gold=0;
   while(done<(unsigned)frame->Width*frame->Height){unsigned run=*p++,alpha=*p++;assert(run&&done+run<=(unsigned)frame->Width*frame->Height);
    if(alpha)for(unsigned k=0;k<run;++k){unsigned index=*p++;assert(index<h->Colors);const unsigned char* rgb=palette+index*3;
     if((done+k)/frame->Width<40&&alpha>150&&rgb[0]>80&&rgb[1]>60&&rgb[0]*10>rgb[2]*14&&rgb[1]*10>rgb[2]*12)++gold;
    }done+=run;
   }
   printf("ASSET %d frame %d gold=%u required=%d\n",files,f,gold,goldRequired);fflush(stdout);if(goldRequired&&f==0){assert(gold>10);++goldFrames;}++frames;
  }SprReleaseHeader(h);++files;
 }fclose(list);assert(files==8&&frames==11&&goldFrames==5);printf("PASS UI gold sprites: %d assets %d frames; %d gold title/tab frames through production loader; valid palette and RLE\n",files,frames,goldFrames);g_pPakList=0;return 0;
}
