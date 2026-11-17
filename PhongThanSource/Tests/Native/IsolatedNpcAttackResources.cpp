#include "KCore.h"
#include "KNpcResNode.h"
#include "CoreUseNameDef.h"
#include "KPakList.h"
#include "../../Sources/Represent/iRepresent/iRepresentShell.h"
#include <stdio.h>
#include <set>
#include <string>
iRepresentShell* g_pRepresent = NULL;
int main(int argc, char** argv)
{
    if(argc < 2 || !SetCurrentDirectoryA(argv[1])) return 2;
    g_SetRootPath(argv[1]); g_SetFilePath("\\"); KPakList packs;
    if(!packs.Open("\\package.ini")) return 3;
    g_SetPakFileMode(1);
    KTabFile templates; CActionName human, npc;
    if(!templates.Load("\\settings\\Npcs.txt") || !human.Init(ACTION_FILE_NAME) || !npc.Init(NPC_ACTION_NAME)) return 4;
    int ids[]={0,7,11,15,18,82,83,84,85,86};
    int actions[]={cdo_stand,cdo_run,cdo_attack,cdo_attack1,cdo_magic,cdo_hurt,cdo_death};
    int failed=0,checked=0;
    std::set<std::string> seen;
    int limit=argc>2?templates.GetHeight()-1:sizeof(ids)/sizeof(ids[0]);
    for(int i=0;i<limit;++i){
        int id=argc>2?i:ids[i];
        char type[80]; templates.GetString(id+2,"NpcResType","",type,sizeof(type));
        if(argc>2 && (!type[0] || !seen.insert(type).second))continue;
        KNpcResNode node;
        if(!node.Init(type,&human,&npc)){printf("INIT_FAIL npc=%d\n",id);++failed;continue;}
        for(int j=0;j<sizeof(actions)/sizeof(actions[0]);++j){
            char path[256]; int action=node.GetActNo(actions[j],0,FALSE);
            node.GetFileName(NORMAL_NPC_PART_NO,action,0,"",path,sizeof(path));
            SPROFFS* offsets=NULL; SPRHEAD* head=path[0]?SprGetHeader(path,offsets):NULL;
            printf("%s npc=%d doing=%d action=%d spr=%s frames=%d\n",head?"OK":"MISSING",id,actions[j],action,path,head?head->Frames:0);
            ++checked;if(!head)++failed;else SprReleaseHeader(head);
        }
    }
    printf("SUMMARY checked=%d failed=%d OFFLINE_RESOURCE_TEST_NOT_WORLD_PROOF\n",checked,failed);
    return failed?1:0;
}
