#include "KCore.h"
#include "KPakList.h"
#include "KBasPropTbl.h"
#include <stdio.h>
KPakList* g_pPakList=0;
static int checked=0;
#define CHECK(x) do{++checked;if(!(x)){printf("FAIL %d %s\n",__LINE__,#x);return 1;}}while(0)
int main(int argc,char** argv) {
    if(argc!=2 || !SetCurrentDirectoryA(argv[1]))return 2;
    g_SetRootPath(NULL);g_SetFilePath("\\");KPakList packs;CHECK(packs.Open("\\package.ini"));g_pPakList=&packs;g_SetPakFileMode(1);
    KLibOfBPT library;CHECK(library.Init());
    const char* tables[6]={"potion.txt","material.txt","questkey.txt","TownPortal.txt","magicscript.txt","ibitem.txt"};
    for(int type=0;type<6;++type) {
        char path[160];sprintf(path,"\\settings\\item\\001\\%s",tables[type]);KTabFile table;CHECK(table.Load(path));
        int count=type==0?library.GetMedicineRecordNumber():type==1?library.GetMaterialRecordNumber():type==2?library.GetQuestRecordNumber():
            type==3?library.GetTownPortalRecordNumber():type==4?library.GetMagicScriptRecordNumber():library.GetIBItemRecordNumber();
        CHECK(count==table.GetHeight()-1);
        for(int row=0;row<count;++row) {
            int expected=0,actual=-1;table.GetInteger(row+2,"\xB8\xBA\xD6\xD8",0,&expected);if(expected<0)expected=0;
            switch(type) {
            case 0:actual=library.GetMedicineRecord(row)->m_nWeight;break;
            case 1:actual=library.MaterialAt(row)->m_nWeight;break;
            case 2:actual=library.QuestAt(row)->m_nWeight;break;
            case 3:actual=library.GetTownPortalRecord(row)->m_nWeight;break;
            case 4:actual=library.MagicScriptAt(row)->m_nWeight;break;
            case 5:actual=library.IBItemAt(row)->m_nWeight;break;
            }
            CHECK(actual==expected);
        }
    }
    printf("PASS actual VNG item loaders: %d weight/table assertions\n",checked);g_pPakList=0;return 0;
}
