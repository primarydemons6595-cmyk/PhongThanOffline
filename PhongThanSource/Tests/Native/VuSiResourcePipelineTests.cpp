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
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../../Sources/Represent/iRepresent/iRepresentShell.h"
#include "../../Sources/Represent/iRepresent/KRepresentUnit.h"
#include "MetadataRenderer.h"
PTMetadataRenderer renderer;
iRepresentShell* g_pRepresent=&renderer;
KPakList* g_pPakList=NULL;
static void spr(const char* name){SPROFFS* off=NULL;SPRHEAD* h=SprGetHeader((char*)name,off);if(!h){printf("MISSING SPR %s\n",name);exit(1);}assert(h->Frames>0);SprReleaseHeader(h);}
int main(int argc,char** argv){
 assert(argc==2||argc==3);g_SetRootPath(argv[1]);g_SetPakFileMode(0);
 KPakList packages;if(argc==3){assert(packages.Open("\\package.ini"));g_pPakList=&packages;g_SetPakFileMode(1);}
 KPhongThanAppearance appearance;assert(appearance.Init());
 const int starterKeys[]={91,101,111};for(int k=0;k<3;++k){PHONGTHAN_VISUAL_PART v;assert(appearance.ResolveWeapon(0,starterKeys[k],&v)&&v.nResourceId==1);}
 PHONGTHAN_VISUAL_PART unchanged;assert(appearance.ResolveWeapon(0,92,&unchanged)&&unchanged.nResourceId==2);
 CActionName actions,npcs;assert(actions.Init(ACTION_FILE_NAME));
 const char* names[]={"\xC4\xD0\xBC\xD7\xCA\xBF","\xC4\xD0\xCA\xF5\xCA\xBF","\xC4\xD0\xD2\xEC\xC8\xCB","\xC4\xD0\xD3\xF0\xCA\xBF","\xC5\xAE\xBC\xD7\xCA\xBF","\xC5\xAE\xCA\xF5\xCA\xBF","\xC5\xAE\xD2\xEC\xC8\xCB","\xC5\xAE\xD3\xF0\xCA\xBF"};
 int cases=0;
 for(int sex=0;sex<8;++sex){KNpcResNode node;assert(node.Init((char*)names[sex],&actions,&npcs));assert(node.GetNpcKind()==NPC_RES_SPECIAL);
  // Every authored armed row must have a visible idle weapon in its own active hand.
  int active=(sex==7)?6:7;char table[260];sprintf(table,"\\settings\\npcres\\%s",node.m_nSectInfo[active].szSectResName);
  KTabFile weaponTable;assert(weaponTable.Load(table));
  for(int resource=1;resource<weaponTable.GetHeight()-1;++resource){
   char label[80];weaponTable.GetString(resource+2,1,"",label,sizeof(label));if(strstr(label,"\xBF\xD5\xCA\xD6"))continue; // Authored empty-hand spacer, not an equipped weapon.
   int idle=node.GetActNo(1,resource,FALSE);char image[260];assert(idle!=0);node.GetFileName(active,idle,resource,"",image,sizeof(image));
   if(!image[0]){printf("IDLE_EMPTY actor=%d resource=%d action=%d\n",sex,resource,idle);return 1;}spr(image);
  }
  for(int gear=0;gear<9;++gear)for(int ride=0;ride<2;++ride)for(int w=0;w<3;++w)for(int state=0;state<3;++state){
   const int doing[]={1,6,9};int act=node.GetActNo(doing[state],w,ride);assert(act>=0&&act<30);if(w&&state==0)assert(act!=0);
   int sort[16];assert(node.GetSort(act,0,0,sort,16));bool body=false,head=false,mount=false,weapon=false;
   for(int j=0;j<16;++j){int part=sort[j];if(part<0||part>=MAX_PART)continue;
    int equip=(part==0||part==3)?gear:((part==6||part==7)?w:1);char file[260];node.GetFileName(part,act,equip,"",file,sizeof(file));
    if(!file[0])continue;
    if(part==0||part==3||part==12||part==14||((part==6||part==7)&&w>0)){spr(file);}
    if(part==3)body=true;if(part==0)head=true;if(part==12||part==14)mount=true;if(part==6||part==7)weapon=true;
   }
   if(!body||!head||(ride&&(sex==3||sex==7)&&!mount))printf("CASE sex=%d ride=%d weapon=%d state=%d action=%d body=%d head=%d mount=%d\n",sex,ride,w,state,act,body,head,mount);assert(body&&head);if(ride&&(sex==3||sex==7))assert(mount);if(w)assert(weapon);++cases;
  }
 }
 printf("PASS %d real KNpcResNode init/action/sort/SPR cases: male/female Giap/Dao/Di Nhan/Vu Si, stand/run/attack, mounted/unmounted actions, weapon rows 0..2; Vu Si mount layers; starter keys 91/101/111 map to 1x.\n",cases);return 0;
}
