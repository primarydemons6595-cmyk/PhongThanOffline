#include <stdio.h>
#include "../../Headers/PhongThanUiGrid.h"
#define CHECK(x) do {++checks;if(!(x)){printf("FAIL line=%d\n",__LINE__);return 1;}}while(0)
int main(){int checks=0;
    for(int count=1;count<=65;++count)for(int i=0;i<count;++i){int x,y;
        PtSkillGridPosition(i,count,4,34,34,x,y);
        CHECK(PtSkillGridIndex(x+17,y+17,count,4,34,34)==i);
    }
    CHECK(PtSkillGridIndex(-1,0,65,4,34,34)==-1);
    CHECK(PtSkillGridIndex(136,0,65,4,34,34)==-1);
    CHECK(PtSkillGridIndex(34,0,65,4,34,34)==-1);
    CHECK(PtSkillGridIndex(0,0,0,4,34,34)==-1);
    for(int expanded=0;expanded<=1;++expanded){int count=expanded?95:47;
        for(int i=0;i<count;++i){int panel=i/48,cell=i%48;
            int x=(panel*8+cell%8)*26+13,y=cell/8*26+13;
            CHECK(PtFaceGridIndex(x,y,95,8,6,26,26,expanded!=0)==i);
        }
        CHECK(PtFaceGridIndex((expanded?15:7)*26+13,5*26+13,95,8,6,26,26,expanded!=0)==-1);
    }
    CHECK(PtFaceGridIndex(208,0,95,8,6,26,26,false)==-1);
    CHECK(PtFaceGridIndex(0,156,95,8,6,26,26,true)==-1);
    printf("PASS UI skill/emoji geometry checks=%d failures=0\n",checks);return 0;
}
