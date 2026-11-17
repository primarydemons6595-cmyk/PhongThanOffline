#include "PhongThanOrangeRecipe.h"
#include "PhongThanForgeInherited.h"
#include <map>
#include "../../../Headers/PhongThanForgeState.h"
// Original power routine obtains blue-property maxima from the crafting group
// when the item has a crafting identity (game.exe 7089a8..708c31), rather than
// comparing those properties with an unrelated white template's blue slots.
static bool PhongThanForgePower(const KItem& item,int weight,int& total,bool& forged)
{
 total=0;forged=false;int param=item.GetUnderlyingParam();
 if(PtSocketParam(param)){PT_SOCKET_STATE sockets;if(!PtSocketRead(param,sockets))return false;param=sockets.BaseParam;}
 int recipeId=PhongThanForgeRecipe(param);if(!recipeId)return true;forged=true;
 static KTabFile recipes,attributes;static bool ready=false;
 if(!ready)ready=recipes.Load("\\settings\\item\\001\\zhuang_bei_he_cheng_gui_ze_biao.txt")&&attributes.Load("\\settings\\item\\001\\zhuang_bei_dian_lan_fu_jia_shu_xing_biao.txt");
 if(!ready)return false;char key[24];sprintf(key,"%d",recipeId);PhongThanComposeRecipe recipe;
 if(!PhongThanReadOrangeRecipe(recipes,recipes.FindRow(key),recipe))return false;
 sprintf(key,"%d",recipe.attributeId);int row=attributes.FindRow(key);if(row<2)return false;
 if(!PhongThanForgeHasOrdinary(attributes,row)){
  static std::map<int,int> inheritedRows;std::map<int,int>::iterator cached=inheritedRows.find(recipe.attributeId);
  if(cached==inheritedRows.end()){row=PhongThanForgeOrdinaryRow(recipes,attributes,recipe,row);inheritedRows[recipe.attributeId]=row;}else row=cached->second;
 }
 if(row<2)return false;
 __int64 sum=0;
 for(int slot=0;slot<6;++slot){int type=0,maximum=0;
  attributes.GetInteger(row,5+slot*5,0,&type);attributes.GetInteger(row,7+slot*5,0,&maximum);
  const KItemNormalAttrib& value=item.m_aryMagicAttrib[slot];if(!value.nAttribType)continue;
  if(value.nAttribType!=type)return false;
  if(weight>0&&maximum>0&&value.nValue[0]>0)sum+=(__int64)value.nValue[0]*weight/maximum;
  if(sum>INT_MAX)return false;
 }
 total=(int)sum;return true;
}
