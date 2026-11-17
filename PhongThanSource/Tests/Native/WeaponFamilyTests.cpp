#include "KWin32.h"
#include "KPakFile.h"
#include "KPakList.h"
#include "KFilePath.h"
#include "KTabFile.h"
#define CORE_API
#define FILE_NAME_LENGTH 80
#include "KNpcResNode.h"
#include "KPhongThanAppearance.h"
#include "CoreUseNameDef.h"
#include "../../Sources/Represent/iRepresent/iRepresentShell.h"
#include "../../Sources/Represent/iRepresent/KRepresentUnit.h"
#include "MetadataRenderer.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
PTMetadataRenderer renderer;iRepresentShell* g_pRepresent=&renderer;KPakList* g_pPakList=NULL;
static void Asset(char* path){SPROFFS* off=0;SPRHEAD* h=SprGetHeader(path,off);if(!h){printf("SPR_FAIL %s\n",path);exit(1);}assert(h->Frames>0);SprReleaseHeader(h);}
int main(int argc,char** argv){
 assert(argc==2||argc==3);g_SetRootPath(argv[1]);g_SetPakFileMode(0);KPakList packages;if(argc==3){assert(packages.Open("\\package.ini"));g_pPakList=&packages;g_SetPakFileMode(1);}
 KPhongThanAppearance appearance;assert(appearance.Init());CActionName actions,npc;assert(actions.Init(ACTION_FILE_NAME));
 const char* actors[]={"\xC4\xD0\xBC\xD7\xCA\xBF","\xC4\xD0\xCA\xF5\xCA\xBF","\xC4\xD0\xD2\xEC\xC8\xCB","\xC5\xAE\xBC\xD7\xCA\xBF","\xC5\xAE\xCA\xF5\xCA\xBF","\xC5\xAE\xD2\xEC\xC8\xCB"};
 for(int key=291;key<=300;++key){PHONGTHAN_VISUAL_PART h;assert(appearance.ResolveHorse(key,&h)&&h.nResourceId==5);}
 int checked=0;for(int actor=0;actor<6;++actor){KNpcResNode node;assert(node.Init((char*)actors[actor],&actions,&npc));
  for(int level=1;level<=10;++level){PHONGTHAN_VISUAL_PART horse;int key=4000+(actor%3)*10+level;assert(appearance.ResolveHorse(key,&horse)&&horse.nResourceId==5);
   for(int state=0;state<3;++state){const int doing[]={1,6,9};int action=node.GetActNo(doing[state],1,TRUE);bool visible=false;
    for(int part=12;part<15;++part){char image[260];node.GetFileName(part,action,horse.nResourceId,"",image,sizeof(image));if(!image[0])continue;const char* group=actor%3==0?"jsx06_":actor%3==1?"ssx06_":"yrx06_";assert(strstr(image,group));Asset(image);visible=true;}assert(visible);
   }
  }
  for(int family=0;family<16;++family){int kind=family%4;int particular=family<8?family:family+19;int profession=kind<2?0:kind-1;if(profession!=actor%3)continue;
   for(int tier=1;tier<=10;++tier){int key=particular*10+tier;int expected=tier+(family%4==1?20:0);PHONGTHAN_VISUAL_PART v;
    assert(appearance.ResolveWeapon(0,key,&v)&&v.bVisible&&v.nResourceId==expected);
    for(int ride=0;ride<2;++ride)for(int state=0;state<3;++state){const int doing[]={1,6,9};int action=node.GetActNo(doing[state],v.nResourceId,ride);char path[260];assert(node.GetFileName(7,action,v.nResourceId,"",path,sizeof(path))&&path[0]);Asset(path);
     if(!ride&&state==0&&tier==1){char expectedFile[64];const char* prefixes[]={"jsm","ssm","yrm","jsf","ssf","yrf"};sprintf(expectedFile,"%s00_rw%03d_",prefixes[actor],tier+(family%4==1?50:10));if(!strstr(path,expectedFile))printf("FAMILY_FAIL actor=%d family=%d tier=%d resource=%d action=%d expected=%s actual=%s\n",actor,family,tier,v.nResourceId,action,expectedFile,path);assert(strstr(path,expectedFile));}
     ++checked;
    }
   }
  }
 }
 for(int starter=91;starter<=111;starter+=10){PHONGTHAN_VISUAL_PART v;assert(appearance.ResolveWeapon(0,starter,&v)&&v.nResourceId==1);}
 printf("PASS %d actual appearance/action/SPR cases: 160 original weapon records, ordinary/gold and quest families, all tiers 1..10, three professions, male/female, idle/run/attack, mounted/unmounted; starter 1x retained; 30 Nghich Tinh mount records plus 10 Bach Kim Phong Loi Song Duc records resolve correct jsx06/ssx06/yrx06 layers for both sexes and three actions.\n",checked);return 0;
}
