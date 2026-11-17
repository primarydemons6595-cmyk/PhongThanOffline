#include "KCore.h"
#include "KPakList.h"
#include "KBasPropTbl.h"
#include "PhongThanDropRules.h"
#include <stdio.h>
KPakList* g_pPakList = NULL;
#define CHECK(x) do { if(!(x)){printf("FAIL line=%d %s\n",__LINE__,#x);return 1;} } while(0)
int main(int argc,char** argv)
{
    if(argc!=2 || !SetCurrentDirectoryA(argv[1]))return 2;
    g_SetRootPath(NULL);g_SetFilePath("\\");KPakList packs;
    CHECK(packs.Open("\\package.ini"));g_pPakList=&packs;g_SetPakFileMode(1);
    KIniFile ini;CHECK(ini.Load("\\settings\\phongthan\\NpcDropPolicy.ini"));
    PtDropTable table;CHECK(PtDropReadTable(ini,"basic",table));CHECK(table.count==3);
    KLibOfBPT library;CHECK(library.Init());
    CHECK(library.FindMedicine(0,1));CHECK(library.FindMedicine(3,1));
    const KBASICPROP_EVENTITEM* material=library.GetMaterial(15);
    CHECK(material && material->m_nItemGenre==3 && material->m_nDetailType==15);
    CHECK(table.entries[2].level==0);
    printf("PASS NATIVE_LOOSE_DROP_CONTRACT policy_read=1 medicine_red=1 medicine_blue=1 material15=1\n");
    g_pPakList=NULL;return 0;
}
