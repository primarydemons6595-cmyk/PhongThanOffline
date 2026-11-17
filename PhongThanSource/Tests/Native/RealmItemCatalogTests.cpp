#include "KCore.h"
#include "KPakList.h"
#include "KBasPropTbl.h"
#include <assert.h>
#include <stdio.h>
KPakList* g_pPakList=0;
int main(int argc,char** argv){assert(argc==2&&SetCurrentDirectoryA(argv[1]));g_SetRootPath(0);g_SetFilePath("\\");KPakList packs;assert(packs.Open("\\package.ini"));g_pPakList=&packs;g_SetPakFileMode(1);
 KBPT_MagicScript magic;assert(magic.Load());int keys[]={410,411,412,413,431};
 for(int i=0;i<5;++i){const KBASICPROP_MAGICSCRIPT* item=magic.FindRecord(keys[i]);assert(item&&item->m_nItemGenre==6&&item->m_nWidth>0&&item->m_nHeight>0&&item->m_szName[0]&&item->m_szImageName[0]);}
 KBPT_Material material;assert(material.Load());assert(material.FindRecord(177)&&material.FindRecord(180));
 puts("PASS actual item table loaders: four soul tools, Bai Lien book, lure arrows and dragon whisker resolve native item keys");return 0;
}
