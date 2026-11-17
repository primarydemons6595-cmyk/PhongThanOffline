#include "KWin32.h"
#include "KPakFile.h"
#include "KPakList.h"
#include "KFilePath.h"
#include "KTabFile.h"
#include "KStrBase.h"
#define CORE_API
#define FILE_NAME_LENGTH 80
#include "GameDataDef.h"
#include "KNpcResNode.h"
#include "CoreUseNameDef.h"
#include "../../Sources/Represent/iRepresent/iRepresentShell.h"
#include "../../Sources/Represent/iRepresent/KRepresentUnit.h"
#include "MetadataRenderer.h"
#include <assert.h>
#include <stdio.h>
struct LabelRenderer:PTMetadataRenderer {
 int count,left,top,right,bottom;
 void OutputText(int font,const char* text,int,int x,int y,unsigned int,int,int,unsigned int){
  assert(font==12 && text[0]);assert(x>=left && y>=top);
  assert(x+g_StrLen(text)*6<=right && y+12<=bottom);++count;
 }
} renderer;
iRepresentShell* g_pRepresent=&renderer;
KPakList* g_pPakList=NULL;
#include "../../Sources/Core/Src/Scene/PhongThanMapServiceLabels.inl"
int main(int argc,char** argv){
 assert(argc==3||argc==4);g_SetRootPath(argv[1]);g_SetPakFileMode(0);
 KPakList packages;if(argc==4){assert(packages.Open("\\package.ini"));g_pPakList=&packages;g_SetPakFileMode(1);}
 RECT cover={2900,2900,3400,3400};renderer.left=7;renderer.top=13;renderer.right=507;renderer.bottom=513;
 PhongThanPaintMapServiceLabels(7,13,1072,cover,16,32);assert(renderer.count==2);
 PhongThanPaintMapServiceLabels(7,13,1001,cover,16,32);assert(renderer.count==2);
 RECT outside={0,0,100,100};PhongThanPaintMapServiceLabels(7,13,1072,outside,16,32);assert(renderer.count==2);
 RECT edge={3196,3220,3260,3270};renderer.left=7;renderer.top=13;renderer.right=71;renderer.bottom=63;
 PhongThanPaintMapServiceLabels(7,13,1072,edge,16,32);assert(renderer.count==4);
 g_SetRootPath(argv[2]);CActionName actions,npcs;assert(actions.Init(ACTION_FILE_NAME));assert(npcs.Init(NPC_ACTION_NAME));
 char* names[]={"yuwong","passerby001"};
 for(int i=0;i<2;++i){KNpcResNode node;assert(node.Init(names[i],&actions,&npcs));
  int action=node.GetActNo(1,0,FALSE);char image[260];assert(node.GetFileName(NORMAL_NPC_PART_NO,action,0,"",image,sizeof(image))&&image[0]);
  SPROFFS* off=NULL;SPRHEAD* head=SprGetHeader(image,off);assert(head&&head->Frames>0);SprReleaseHeader(head);
 }
 puts("PASS actual service label table: projection/clipping/map filtering; boatman and doctor native idle SPR resources.");return 0;
}
