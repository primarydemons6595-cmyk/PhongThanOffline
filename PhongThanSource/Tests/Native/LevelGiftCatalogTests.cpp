#include "KCore.h"
#include "KPakList.h"
#include "KBasPropTbl.h"
#include <assert.h>
#include <stdio.h>
KPakList* g_pPakList=0;
int main(int argc,char** argv){
 assert(argc==2&&SetCurrentDirectoryA(argv[1]));g_SetRootPath(0);g_SetFilePath("\\");
 KPakList packs;assert(packs.Open("\\package.ini"));g_pPakList=&packs;g_SetPakFileMode(1);
 KBPT_IBItem ib;assert(ib.Load());const KBASICPROP_IBITEM* card=ib.FindRecord(9700,2);
 assert(card&&card->m_nItemGenre==8&&card->m_nWidth==1&&card->m_nHeight==1);
 assert(card->m_nUseCount==1&&!card->m_bCanStack&&card->m_bCanSell&&card->m_bCanTrade&&!card->m_bCanDrop);
 assert(!strcmp(card->m_szScript,"\\script\\phongthan\\rewards\\level_gift.lua"));
 KTabFile script; // Native loose lookup must find the new script through the named index.
 KPakFile f;assert(f.Open(card->m_szScript)&&f.Size()>1000);f.Close();
 puts("PASS NATIVE_LEVEL_GIFT_CATALOG key=8.9700.2 retained_IB_script=1 sellable=1 drop=0 native_loose_lookup=1");return 0;
}
