#include "KCore.h"
#include "KNpcResNode.h"
#include "CoreUseNameDef.h"
#include "KPakList.h"
#include "../../Sources/Represent/iRepresent/iRepresentShell.h"
#include <stdio.h>
iRepresentShell* g_pRepresent=0;
static bool Asset(KNpcResNode& node,int part,int action,int resource){
 char path[256];node.GetFileName(part,action,resource,"",path,sizeof(path));
 if(!path[0])return false;
 SPROFFS* offsets=0;SPRHEAD* h=SprGetHeader(path,offsets);
 if(!h){printf("BAD_FILE part=%d action=%d resource=%d path=%s\n",part,action,resource,path);return false;}
 bool valid=h->Frames>=8&&h->Frames%8==0;SprReleaseHeader(h);return valid;
}
int main(int argc,char** argv){
 if(argc!=2)return 2;g_SetRootPath(argv[1]);g_SetFilePath("\\");KPakList pak;
 if(!pak.Open("\\package.ini"))return 3;g_SetPakFileMode(1);
 CActionName human,npc;if(!human.Init(ACTION_FILE_NAME)||!npc.Init(NPC_ACTION_NAME))return 4;
 KTabFile kind;if(!kind.Load(NPC_RES_KIND_FILE_NAME))return 5;
 int checked=0,failed=0;int styles[]={cdo_fightstand,cdo_run,cdo_attack,cdo_hurt};
 for(int a=0;a<8;++a){if(a==3||a==7)continue;
  char actor[80],table[256],name[80];kind.GetString(a+2,1,"",actor,sizeof(actor));
  KNpcResNode node;if(!node.Init(actor,&human,&npc)){printf("INIT_FAIL actor=%d\n",a);++failed;continue;}
  KTabFile weapon,horse,cape,mountParts[3];
  kind.GetString(a+2,13,"",name,sizeof(name));sprintf(table,"\\settings\\npcres\\%s",name);weapon.Load(table);
  kind.GetString(a+2,14,"",name,sizeof(name));sprintf(table,"\\settings\\npcres\\%s",name);horse.Load(table);
  kind.GetString(a+2,11,"",name,sizeof(name));sprintf(table,"\\settings\\npcres\\%s",name);cape.Load(table);
  for(int mp=0;mp<3;++mp){kind.GetString(a+2,14+mp,"",name,sizeof(name));sprintf(table,"\\settings\\npcres\\%s",name);mountParts[mp].Load(table);}
  for(int ride=0;ride<2;++ride)for(int w=0;w<weapon.GetHeight()-1;++w)for(int s=0;s<4;++s){
   int action=node.GetActNo(styles[s],w,ride);
   bool head=Asset(node,0,action,0),body=Asset(node,3,action,5);
   ++checked;if(!head||!body){++failed;printf("BODY_FAIL actor=%d ride=%d weapon=%d doing=%d action=%d\n",a,ride,w,styles[s],action);}
   if(w){bool right=Asset(node,7,action,w),left=Asset(node,6,action,w);
    ++checked;if(!right&&!left){++failed;printf("WEAPON_FAIL actor=%d ride=%d weapon=%d doing=%d action=%d\n",a,ride,w,styles[s],action);}}
  }
  for(int h=0;h<horse.GetHeight()-1;++h)for(int s=0;s<4;++s){
   int action=node.GetActNo(styles[s],1,TRUE);bool visible=false;
   // GetFileName intentionally hides mounted layers without a live renderer.
   // For this offline test use native KTabFile + SprGetHeader on the same
   // resolved action, not a fake iRepresent or a false missing-file count.
   for(int part=0;part<3;++part){char path[256],spr[128];mountParts[part].GetString(h+2,action+2,"",spr,sizeof(spr));
    if(spr[0]){sprintf(path,"\\spr\\npcres\\human\\%s",spr);SPROFFS* offsets=0;SPRHEAD* header=SprGetHeader(path,offsets);
     if(header){if(header->Frames>=8&&header->Frames%8==0)visible=true;SprReleaseHeader(header);}}}
   ++checked;if(!visible){++failed;printf("MOUNT_FAIL actor=%d mount=%d doing=%d action=%d\n",a,h,styles[s],action);}
  }
  for(int cap=1;cap<cape.GetHeight()-1;++cap)for(int ride=0;ride<2;++ride)for(int s=0;s<4;++s){
   int action=node.GetActNo(styles[s],1,ride);++checked;
   if(!Asset(node,9,action,cap)){++failed;printf("CAPE_FAIL actor=%d cape=%d ride=%d doing=%d action=%d\n",a,cap,ride,styles[s],action);}
  }
 }
 printf("COMPOSITE_RESOURCE_RESULT checked=%d failed=%d REAL_VISUAL_ACCEPTANCE_PENDING\n",checked,failed);return failed?1:0;
}
