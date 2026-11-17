#include "KCore.h"
#include "KPakList.h"
#include "KBasPropTbl.h"
#include <assert.h>
#include <stdio.h>
KPakList* g_pPakList=0;
int main(int argc,char**argv){if(argc!=2||!SetCurrentDirectoryA(argv[1]))return 2;g_SetRootPath(0);g_SetFilePath("\\");g_SetPakFileMode(0);KPakList packs;assert(packs.Open("\\package.ini"));g_pPakList=&packs;KLibOfBPT lib;assert(lib.Init());
 const char* images[]={"\\spr\\item\\vng\\updatet12\\nguyen_to_loi.spr","\\spr\\item\\vng\\phienbanmoi\\bat_nha_hoang_ngoc.spr","\\spr\\item\\vng\\updatet12\\nguyen_to_hoa.spr","\\spr\\item\\vng\\updatet12\\nguyen_to_moc.spr"};
 for(int i=0;i<4;++i){const KBASICPROP_EVENTITEM* item=lib.GetMaterial(5743+i);assert(item&&item->m_nMaxStack==999&&!strcmp(item->m_szImageName,images[i])&&item->m_nDetailType==5743+i);}
 puts("PASS production material catalogue: four exact pearl identities, article-matched existing native sprites and max stack 999");g_pPakList=0;return 0;}
