#include "KWin32.h"
#include "KIniFile.h"
#include "KFilePath.h"
#include "KPakFile.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
int main(int argc,char** argv){
    assert(argc==2);g_SetRootPath(argv[1]);g_SetPakFileMode(0);
    KIniFile ini;assert(ini.Load("\\ui\\vusi_skills.ini"));int pages=0;
    ini.GetInteger("Main","PageCount",0,&pages);assert(pages==3);
    int count=0;
    for(int page=0;page<pages;++page){char section[80];int slots=0;
        sprintf(section,"SkillPage_%d",page);ini.GetInteger(section,"SkillCount",0,&slots);
        assert(slots==(page<2?9:2));
        for(int slot=0;slot<slots;++slot){int id=0;char name[128];
            sprintf(section,"Page_%d_Skill_%d",page,slot);
            ini.GetInteger(section,"SkillID",0,&id);ini.GetString(section,"SkillName","",name,sizeof(name));
            assert(id==1696+count*5&&name[0]=='$'&&strlen(name)>2);++count;
        }
    }
    assert(count==20);puts("PASS native Engine KIniFile: Vu Si 3 pages, 20 canonical skill IDs and names, direct loose path.");return 0;
}
