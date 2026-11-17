#include "KCore.h"
#include "KPakList.h"
#include "KBasPropTbl.h"
#include "PhongThanQuestItemTuple.h"
#include "../../Headers/PhongThanTgptVttData.h"
#include <stdio.h>
#include <assert.h>
KPakList* g_pPakList=0;
int main(int argc,char** argv){
 SetErrorMode(SEM_FAILCRITICALERRORS|SEM_NOGPFAULTERRORBOX);assert(argc==2&&SetCurrentDirectoryA(argv[1]));g_SetRootPath(0);g_SetFilePath("\\");g_SetPakFileMode(0);
 KPakList packs;assert(packs.Open("\\package.ini"));g_pPakList=&packs;KLibOfBPT library;assert(library.Init());
 for(int i=0;i<4;++i){const KBASICPROP_EVENTITEM* p=library.GetMaterial(PT_TGPT_PEARLS[i]);assert(p&&p->m_nItemGenre==3&&p->m_nDetailType==PT_TGPT_PEARLS[i]&&p->m_nMaxStack==10000&&p->m_nWidth==1&&p->m_nHeight==1);
  for(int profession=0;profession<3;++profession){const PTTgptWeapon& w=PT_TGPT_WEAPONS[i][profession];const KBASICPROP_EQUIPMENT* row=library.GetMeleeWeaponRecord(w.row);assert(row&&row->m_nItemGenre==0&&row->m_nDetailType==0&&row->m_nParticularType==w.part&&row->m_nLevel==w.level);}
 }
 const KBASICPROP_MAGICSCRIPT* token=library.GetMagicScript(12);assert(token&&token->m_nItemGenre==6&&token->m_nWidth==1&&token->m_nHeight==1);
 for(i=0;i<3;++i){const KBASICPROP_MAGICSCRIPT* p=library.GetMagicScript(PT_TGPT_BLUEPRINTS[i]);assert(p&&p->m_nItemGenre==6&&p->m_nWidth>0&&p->m_nHeight>0);}
 g_SetPakFileMode(1);const char* paths[]={"\\script\\phongthan\\van_tien\\pearl_chest.lua","\\settings\\phongthan\\npc_regions\\1067\\v_102\\092_region_s.dat","\\settings\\item\\droprate\\pt_tgpt_tongtian1.ini"};
 for(i=0;i<3;++i){KPakFile file;assert(file.Open(paths[i])&&file.Size()>0);}
 puts("PASS actual production catalog: 4 stackable pearl identities, 12 exact gold weapon rows/levels/parts, lottery token and 3 third-tier orange blueprint identities");g_pPakList=0;return 0;
}
