#include "KCore.h"
#include "KNpcResNode.h"
#include "CoreUseNameDef.h"
#include "KPakList.h"
#include "../../Sources/Represent/iRepresent/iRepresentShell.h"
#include <stdio.h>
iRepresentShell* g_pRepresent=0;
int main(int argc,char** argv){
 if(argc!=2)return 2;g_SetRootPath(argv[1]);g_SetFilePath("\\");KPakList pak;
 if(!pak.Open("\\package.ini"))return 3;g_SetPakFileMode(0);
 CActionName human,npc;if(!human.Init(ACTION_FILE_NAME)||!npc.Init(NPC_ACTION_NAME))return 4;
 KNpcResNode node;char actor[]="\xC5\xAE\xD2\xEC\xC8\xCB";
 if(!node.Init(actor,&human,&npc))return 5;
 KTabFile tables[2];tables[0].Load("\\settings\\npcres\\nv_yi_ren_ma_qian.txt");tables[1].Load("\\settings\\npcres\\nv_yi_ren_ma_hou.txt");
 int failed=0;
 for(int action=16;action<=27;action++)for(int equip=0;equip<2;equip++){
  for(int part=12;part<=14;part+=2){char name[256],spr[128];tables[(part-12)/2].GetString(equip+2,action+2,"",spr,sizeof(spr));sprintf(name,"\\spr\\npcres\\human\\%s",spr);
   int total=node.GetTotalFrames(part,action,equip,0),dirs=node.GetTotalDirs(part,action,equip,0);
   SPROFFS* offsets=0;SPRHEAD* h=SprGetHeader(name,offsets);
   printf("SPR equip=%d action=%d part=%d metadata=%d/%d actual=%d/%d %s\n",equip,action,part,total,dirs,h?h->Frames:0,h?h->Directions:0,name);
   if(!h||total!=h->Frames||dirs!=8)failed++;
   if(h)SprReleaseHeader(h);
  }
  int frames=node.GetTotalFrames(12,action,equip,0)/8;
  for(int dir=0;dir<8;dir++)for(int f=0;f<frames;f++){
   int order[15];node.GetSort(action,dir,dir*frames+f,order,15);bool front=false,back=false;
   for(int k=0;k<15;k++){front|=order[k]==12;back|=order[k]==14;}
   if(!front||!back){failed++;printf("MISSING action=%d dir=%d frame=%d front=%d back=%d\n",action,dir,f,front,back);}
  }
 }
 KTabFile kinds;if(!kinds.Load(NPC_RES_KIND_FILE_NAME))return 6;
 int total=0;
 for(int actorIndex=0;actorIndex<8;actorIndex++){
  if(actorIndex==3||actorIndex==7)continue;
  char actorName[80];kinds.GetString(actorIndex+2,1,"",actorName,sizeof(actorName));KNpcResNode actorNode;
  if(!actorNode.Init(actorName,&human,&npc)){failed++;continue;}
  for(int layer=0;layer<3;layer++){
   char tableName[128],tablePath[256];kinds.GetString(actorIndex+2,14+layer,"",tableName,sizeof(tableName));sprintf(tablePath,"\\settings\\npcres\\%s",tableName);KTabFile mountTable;
   if(!mountTable.Load(tablePath)){failed++;continue;}
   for(int model=0;model<12;model++)for(int act=16;act<=27;act++){
    char spr[128],path[256];mountTable.GetString(model+2,act+2,"",spr,sizeof(spr));if(!spr[0])continue;
    sprintf(path,"\\spr\\npcres\\human\\%s",spr);SPROFFS* offsets=0;SPRHEAD* h=SprGetHeader(path,offsets);
    int frames=actorNode.GetTotalFrames(12+layer,act,model,0),dirs=actorNode.GetTotalDirs(12+layer,act,model,0);
    ++total;
    if(!h||frames!=h->Frames||dirs!=8){failed++;printf("BAD_ALL actor=%d model=%d act=%d part=%d metadata=%d/%d actual=%d %s\n",actorIndex,model,act,layer,frames,dirs,h?h->Frames:0,path);}
    if(h)SprReleaseHeader(h);
   }
  }
 }
 printf("RESULT all_mount_cells=%d failures=%d\n",total,failed);return failed?1:0;
}
