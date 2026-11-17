#ifndef PHONGTHAN_FORGE_INHERITED_H
#define PHONGTHAN_FORGE_INHERITED_H
#include "PhongThanOrangeRecipe.h"
// Some upgrade rows intentionally omit all six ordinary attributes. Resolve
// their unique predecessor through the actual recipe inputs, not item names,
// a guessed numeric ID offset, SetID, or a newly invented property list.
inline bool PhongThanForgeHasOrdinary(KTabFile& attrs,int row){
 for(int i=0;i<6;++i){int type=0;attrs.GetInteger(row,5+i*5,0,&type);if(type)return true;}return false;
}
inline bool PhongThanForgeSameTuple(const PhongThanRecipeItem& a,const PhongThanRecipeItem& b){
 return a.genre==b.genre&&a.detail==b.detail&&a.part==b.part&&a.quantity==1&&b.quantity==1&&a.upgradeMask==b.upgradeMask;
}
inline int PhongThanForgeOrdinaryRow(KTabFile& rules,KTabFile& attrs,const PhongThanComposeRecipe& recipe,int row){
 if(row<2)return 0;if(PhongThanForgeHasOrdinary(attrs,row))return row;
 int original=0;attrs.GetInteger(row,34,0,&original);if(original<=0)return 0;
 char prefix[80];attrs.GetString(row,2,"",prefix,sizeof(prefix));if(!prefix[0])return 0;
 int resolved=0;
 for(int i=2;i<=rules.GetHeight();++i){PhongThanComposeRecipe link;
  if(!PhongThanReadOrangeRecipe(rules,i,link)||link.attributeId!=recipe.attributeId||!PhongThanForgeSameTuple(link.output,recipe.output))continue;
  for(int n=0;n<link.inputCount;++n){const PhongThanRecipeItem& input=link.inputs[n];
   if(input.genre!=0||input.detail!=recipe.output.detail||input.part!=recipe.output.part||input.quantity!=1||!input.upgradeMask||input.upgradeMask>=recipe.output.upgradeMask)continue;
   for(int j=2;j<=rules.GetHeight();++j){PhongThanComposeRecipe parent;
    if(!PhongThanReadOrangeRecipe(rules,j,parent)||parent.attributeId==recipe.attributeId||!PhongThanForgeSameTuple(parent.output,input))continue;
    char key[24],name[80];sprintf(key,"%d",parent.attributeId);int candidate=attrs.FindRow(key),effect=0;
    if(candidate<2||!PhongThanForgeHasOrdinary(attrs,candidate))continue;
    attrs.GetInteger(candidate,34,0,&effect);attrs.GetString(candidate,2,"",name,sizeof(name));
    if(effect!=original||strcmp(prefix,name))continue;
    if(resolved&&resolved!=candidate)return 0;resolved=candidate;
   }
  }
 }
 return resolved;
}
#endif
