#include <windows.h>
#include <limits.h>
#include <assert.h>
#include "KWin32.h"
#include "KFilePath.h"
#include "KPakList.h"
#include "KTabFile.h"
#include "../../Headers/PhongThanForgeState.h"
KPakList* g_pPakList=0;
struct KItemNormalAttrib {int nAttribType,nValue[3];};
struct KItem {struct {int nParam;} m_CommonAttrib;KItemNormalAttrib m_aryMagicAttrib[6];};
struct PT_SOCKET_STATE {int BaseParam;};
static bool PtSocketParam(int p){return (unsigned)p==0xc2000001U;}
static bool PtSocketRead(int p,PT_SOCKET_STATE& s){s.BaseParam=PhongThanForgeParam(4000);return PtSocketParam(p);}
#include "../../Sources/Core/Src/PhongThanForgePower.inl"
int main(int argc,char**argv){
 assert(argc==2&&SetCurrentDirectoryA(argv[1]));g_SetRootPath(0);g_SetFilePath("\\");g_SetPakFileMode(0);
 KTabFile recipes,attrs;assert(recipes.Load("\\settings\\item\\001\\zhuang_bei_he_cheng_gui_ze_biao.txt"));
 assert(attrs.Load("\\settings\\item\\001\\zhuang_bei_dian_lan_fu_jia_shu_xing_biao.txt"));
 int cases=0,groups=0;
 for(int row=2;row<=recipes.GetHeight();++row){PhongThanComposeRecipe r;if(!PhongThanReadOrangeRecipe(recipes,row,r))continue;
  char key[24];sprintf(key,"%d",r.attributeId);int ar=attrs.FindRow(key);assert(ar>=2);++groups;
  for(int half=1;half<=2;++half){KItem item;ZeroMemory(&item,sizeof(item));item.m_CommonAttrib.nParam=PhongThanForgeParam(r.id);int expected=0;
   for(int slot=0;slot<6;++slot){int chance=0,type=0,maximum=0;attrs.GetInteger(ar,4+5*slot,0,&chance);attrs.GetInteger(ar,5+5*slot,0,&type);attrs.GetInteger(ar,7+5*slot,0,&maximum);
    if(chance<=0||!type||maximum<=0)continue;item.m_aryMagicAttrib[slot].nAttribType=type;item.m_aryMagicAttrib[slot].nValue[0]=maximum/half;
    expected+=item.m_aryMagicAttrib[slot].nValue[0]*80/maximum;
   }
   int power=-1;bool forged=false;assert(PhongThanForgePower(item,80,power,forged)&&forged&&power==expected);++cases;
   KItem copy=item;assert(PhongThanForgePower(copy,80,power,forged)&&power==expected);++cases;
  }
 }
 KItem item;ZeroMemory(&item,sizeof(item));int power=99;bool forged=true;
 assert(PhongThanForgePower(item,80,power,forged)&&!forged&&power==0);
 item.m_CommonAttrib.nParam=PhongThanForgeParam(4000);item.m_aryMagicAttrib[0].nAttribType=92;item.m_aryMagicAttrib[0].nValue[0]=10;
 assert(PhongThanForgePower(item,80,power,forged)&&power==80);
 item.m_CommonAttrib.nParam=(int)0xc2000001U;assert(PhongThanForgePower(item,80,power,forged)&&power==80);
 item.m_aryMagicAttrib[0].nAttribType=999;assert(!PhongThanForgePower(item,80,power,forged));
 printf("PASS orange power %d cases %d original recipes; zero/half/max/restore/socket/invalid-type\n",cases+4,groups);return 0;
}
