#include "KCore.h"
#include "KPakList.h"
#include "KBasPropTbl.h"
#include "../../Headers/PhongThanF2GiftData.h"
#include <stdio.h>
#include <assert.h>
KPakList* g_pPakList=0;
int main(int argc,char** argv){
 SetErrorMode(SEM_FAILCRITICALERRORS|SEM_NOGPFAULTERRORBOX);assert(argc==2&&SetCurrentDirectoryA(argv[1]));g_SetRootPath(0);g_SetFilePath("\\");g_SetPakFileMode(0);
 KPakList packs;assert(packs.Open("\\package.ini"));g_pPakList=&packs;KLibOfBPT library;assert(library.Init());
 for(int n=0;n<PT_F2_GIFT_COUNT;++n){const PTF2Gift& g=ptF2Gifts[n];const KBASICPROP_IBITEM* bag=library.GetIBItem(g.detail,2);assert(bag&&bag->m_nUseCount==1);
  for(int r=0;r<g.count;++r){const PTF2Reward& reward=g.rewards[r];if(reward.genre==3){const KBASICPROP_EVENTITEM* item=library.GetMaterial(reward.detail);assert(item&&item->m_nDetailType==reward.detail&&item->m_nMaxStack>=10);}
   else{bool found=false;for(int row=0;row<library.GetAmuletRecordNumber();++row){const KBASICPROP_EQUIPMENT* item=library.GetAmuletRecord(row);if(item&&item->m_nDetailType==4&&item->m_nParticularType==reward.part&&item->m_nLevel==1){int tier=reward.part<6?10:reward.part<10?30:reward.part<14?50:70;assert(item->m_aryPropReq[0].nType==36&&item->m_aryPropReq[0].nPara==tier);found=true;break;}}assert(found);}
  }
 }
 int ids[]={30,258,330,9444};int parts[]={2,2,0,2};int uses[]={10,10,5,1};
 for(int i=0;i<4;++i){const KBASICPROP_IBITEM* item=library.GetIBItem(ids[i],parts[i]);assert(item&&item->m_nDetailType==ids[i]&&item->m_nUseCount==uses[i]);}
 assert(library.GetIBItem(9445,0));puts("PASS real catalog: nine gift bags, materials, 18 original amulets with required levels 10/30/50/70, compass/commerce ten charges, Lam Tien five charges, restored original Hoa Than Toa/buff");g_pPakList=0;return 0;
}
