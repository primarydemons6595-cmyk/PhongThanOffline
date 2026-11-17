#ifndef PHONGTHAN_SOUL_RULES_H
#define PHONGTHAN_SOUL_RULES_H
#include "../../../Headers/PhongThanSoulState.h"
#include "../../../Headers/PhongThanSoulRecipeData.h"
// PtSocketTuple is the strict original selector grammar shared by both crafts.
namespace PtSoulRules {
struct Plan {int Main,Formula,Money,Recipe,Used[9];bool Refill;PT_SOUL_STATE Before;};
static bool Stone(KItem& item){return item.GetGenre()==6&&item.GetDetailType()==1&&item.GetParticularType()==587&&item.GetLevel()==1;}
static int Quote(const int* indexes,const int* selected,bool exact,Plan& out,int mode){
 ZeroMemory(&out,sizeof(out));out.Main=-1;
 int formula=-1;const PTSoulFormula* descriptor=0;
 for(int slot=0;slot<9;++slot)if(indexes[slot]&&selected[slot]>0){
  KItem& item=Item[indexes[slot]];
  if(item.GetGenre()==item_equip){if(out.Main>=0||selected[slot]!=1)return PT_COMPOSE_INVALID_ITEMS;out.Main=slot;}
  const PTSoulFormula* f=PtSoulFormulaItem(item.GetGenre(),item.GetDetailType(),item.GetParticularType());
  if(f){if(formula>=0)return PT_COMPOSE_INVALID_ITEMS;formula=slot;descriptor=f;}
 }
 if(out.Main<0)return PT_COMPOSE_NO_RECIPE;KItem& main=Item[indexes[out.Main]];
 if(PtSoulParam(PtSoulInnerParam(main.GetParam()))&&!PtSoulFromParam(main.GetParam(),out.Before))return PT_COMPOSE_TABLE_ERROR;
 if(!out.Before.Formula)out.Before.BaseParam=PtEquipmentBaseParam(main.GetParam());
 if(formula<0){
  if(mode!=9||!out.Before.Formula)return PT_COMPOSE_NO_RECIPE;
  const PTSoulFormula* f=PtSoulFormula(out.Before.Formula);if(!f||out.Before.Aura>=f->capacity)return PT_COMPOSE_NO_RECIPE;
  int stone=-1;for(int at=0;at<9;++at)if(indexes[at]&&selected[at]>0&&at!=out.Main){if(stone>=0||!Stone(Item[indexes[at]]))return PT_COMPOSE_NO_RECIPE;stone=at;}
  if(stone<0||(exact&&selected[stone]!=1))return PT_COMPOSE_NO_RECIPE;
  out.Used[out.Main]=out.Used[stone]=1;out.Formula=out.Before.Formula;out.Recipe=out.Formula;out.Refill=true;return PT_COMPOSE_OK;
 }
 if(mode!=8||!descriptor)return PT_COMPOSE_NO_RECIPE;
 for(int row=0;row<sizeof(ptSoulRecipes)/sizeof(ptSoulRecipes[0]);++row){const PTSoulRecipe& r=ptSoulRecipes[row];
  const char* fp=r.materials[0];int fq=0;if(!PtSocketNumber(fp,fq)||*fp!='.')continue;char fi[128];sprintf(fi,"*%s",fp);
  if(!PtSocketTuple(r.equipment,main)||!PtSocketTuple(fi,Item[indexes[formula]]))continue;
  int left[9],used[9];ZeroMemory(used,sizeof(used));for(int s=0;s<9;++s)left[s]=indexes[s]?selected[s]:0;
  --left[out.Main];used[out.Main]=1;bool matched=true;
  for(int material=0;material<8&&matched;++material){const char* text=r.materials[material];if(!text[0])continue;const char* p=text;int count=0;
   if(!PtSocketNumber(p,count)||count<1||count>10000||*p!='.'){matched=false;break;}char identity[128];sprintf(identity,"*%s",p);
   for(int s=0;s<9&&count>0;++s)if(s!=out.Main&&left[s]>0&&PtSocketTuple(identity,Item[indexes[s]])){int take=left[s]<count?left[s]:count;left[s]-=take;used[s]+=take;count-=take;}
   if(count)matched=false;
  }
  for(int rest=0;rest<9;++rest)if(left[rest]>0&&(exact||!used[rest]))matched=false;
  if(matched){out.Formula=descriptor->id;out.Money=r.money;out.Recipe=row+1;memcpy(out.Used,used,sizeof(used));return PT_COMPOSE_OK;}
 }
 return PT_COMPOSE_NO_RECIPE;
}
}
#endif
