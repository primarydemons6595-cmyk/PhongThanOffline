#include "KCore.h"
#include "KNpcResNode.h"
#include "CoreUseNameDef.h"
#include "KPakList.h"
#include "../../Sources/Represent/iRepresent/iRepresentShell.h"
#include <assert.h>
#include <stdio.h>
iRepresentShell* g_pRepresent=0;
int main(int argc,char**argv){
 assert(argc==2&&SetCurrentDirectoryA(argv[1]));g_SetRootPath(0);g_SetFilePath("\\");KPakList packs;assert(packs.Open("\\package.ini"));g_SetPakFileMode(1);
 KTabFile table;CActionName human,npc;assert(table.Load("\\settings\\Npcs.txt")&&human.Init(ACTION_FILE_NAME)&&npc.Init(NPC_ACTION_NAME));
 const int ids[]={156,205,198,176,175,100,109,116,107,105,106,606,12};int total=0;
 for(int i=0;i<13;++i){char type[80],path[256];table.GetString(ids[i]+2,"NpcResType","",type,sizeof(type));KNpcResNode node;assert(node.Init(type,&human,&npc));
  node.GetFileName(NORMAL_NPC_PART_NO,node.GetActNo(cdo_stand,0,FALSE),0,"",path,sizeof(path));printf("CHECK template=%d path=%s\n",ids[i],path);fflush(stdout);SPROFFS* offsets=0;SPRHEAD* h=SprGetHeader(path,offsets);assert(h&&h->Frames&&h->Colors);int visible=0;
  for(int f=0;f<h->Frames;++f){SPRFRAME* frame=offsets?(SPRFRAME*)((char*)offsets+h->Frames*sizeof(SPROFFS)+offsets[f].Offset):SprGetFrame(h,f);assert(frame&&frame->Width&&frame->Height);
   const BYTE* p=frame->Sprite;unsigned done=0,pixels=0;
   while(done<(unsigned)frame->Width*frame->Height){unsigned run=*p++,alpha=*p++;assert(run&&done+run<=(unsigned)frame->Width*frame->Height);
    if(alpha){pixels+=run;for(unsigned k=0;k<run;++k)assert(*p++<h->Colors);}done+=run;}
   if(pixels)++visible;if(!offsets)SprReleaseFrame(frame);++total;
  }
  assert(visible>=h->Frames/2&&visible>0);printf("PASS actual realm NPC template=%d frames=%d visible=%d valid indexed sprite/RLE\n",ids[i],h->Frames,visible);SprReleaseHeader(h);
 }printf("PASS %d realm sprite animation frames\n",total);return 0;
}
