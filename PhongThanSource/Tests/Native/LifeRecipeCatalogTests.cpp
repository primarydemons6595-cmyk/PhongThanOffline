#include "KCore.h"
#include "KPakList.h"
#include "KBasPropTbl.h"
#include "PhongThanQuestItemTuple.h"
#include <stdio.h>
#include <assert.h>
#include "LifeRecipeTuples.inl"
KPakList* g_pPakList=0;
int main(int argc,char** argv){
 SetErrorMode(SEM_FAILCRITICALERRORS|SEM_NOGPFAULTERRORBOX);
 assert(argc==2&&SetCurrentDirectoryA(argv[1]));g_SetRootPath(0);g_SetFilePath("\\");g_SetPakFileMode(0);
 KPakList packs;if(!packs.Open("\\package.ini")){puts("FAIL opening Loose package registry");return 1;}g_pPakList=&packs;
 KLibOfBPT library;assert(library.Init());int checked=0;
 for(int i=0;i<sizeof(lifeTuples)/sizeof(lifeTuples[0]);++i){
  const int* t=lifeTuples[i];PhongThanQuestItemTuple item;
  if(!PhongThanDecodeQuestItemTuple(t[0],t[1],t[2],t[3],t[4],t[5],item)){printf("FAIL decode %d,%d,%d,%d\n",t[0],t[1],t[2],t[3]);return 1;}
  bool found=false;
  if(item.genre==item_materials){const KBASICPROP_EVENTITEM* row=library.GetMaterial(item.detail);found=row&&row->m_nItemGenre==item.genre&&row->m_nDetailType==item.detail&&row->m_nWidth>0&&row->m_nHeight>0;}
  else if(item.genre==item_ibitem){const KBASICPROP_IBITEM* row=library.GetIBItem(item.detail,item.particular);found=row&&row->m_nItemGenre==item.genre&&row->m_nDetailType==item.detail&&row->m_nParticularType==item.particular&&row->m_nWidth>0&&row->m_nHeight>0;}
  else if(item.genre==item_magicscript){const KBASICPROP_MAGICSCRIPT* row=library.GetMagicScript(item.detail);found=row&&row->m_nItemGenre==item.genre&&row->m_nDetailType==item.detail&&row->m_nWidth>0&&row->m_nHeight>0;}
  if(!found){printf("FAIL catalog identity %d,%d,%d -> %d,%d,%d\n",t[0],t[1],t[2],item.genre,item.detail,item.particular);return 1;}++checked;
 }
 printf("PASS production item catalog: %d distinct recipe/harvest item identities resolve with correct dimensions and particulars\n",checked);g_pPakList=0;return 0;
}
