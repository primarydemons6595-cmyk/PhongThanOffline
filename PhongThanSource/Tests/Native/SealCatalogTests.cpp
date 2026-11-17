#include "KCore.h"
#include "KPakList.h"
#include "KBasPropTbl.h"
#include <assert.h>
#include <stdio.h>
KPakList* g_pPakList=0;
int main(int argc,char** argv){
 SetErrorMode(SEM_FAILCRITICALERRORS|SEM_NOGPFAULTERRORBOX);assert(argc==2&&SetCurrentDirectoryA(argv[1]));g_SetRootPath(0);g_SetFilePath("\\");g_SetPakFileMode(0);
 KPakList packs;assert(packs.Open("\\package.ini"));g_pPakList=&packs;KLibOfBPT library;assert(library.Init());assert(library.GetPtSealRecordNumber()==160);
 FILE* expected=fopen("seal-expected.tsv","r");assert(expected);int row,part,level,count=0,values[12];
 while(fscanf(expected,"%d%d%d",&row,&part,&level)==3){for(int i=0;i<12;++i)assert(fscanf(expected,"%d",values+i)==1);
  const KBASICPROP_EQUIPMENT* seal=library.GetPtSealRecord(row);assert(seal&&seal->m_nItemGenre==0&&seal->m_nDetailType==13&&seal->m_nParticularType==part&&seal->m_nLevel==level);
  for(i=0;i<6;++i){assert(seal->m_aryPropReq[i].nType==values[i*2]);assert(seal->m_aryPropReq[i].nPara==values[i*2+1]);}++count;
 }
 fclose(expected);assert(count==160);assert(library.GetIBItem(7446,0)&&library.GetMagicScript(110));
 puts("PASS production catalogue: 160 original seals, exact part/level/requirements, existing dragon buff and treasure-map identities");g_pPakList=0;return 0;
}
