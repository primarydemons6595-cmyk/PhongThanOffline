#ifndef PHONGTHAN_EQUIPMENT_RECIPE_H
#define PHONGTHAN_EQUIPMENT_RECIPE_H
#include "PhongThanComposeRecipe.h"
// Equipment upgrades carry two independent qualifiers: (star attempts) and
// [native template level]. Keep their decoder local to this workstation.
struct PhongThanEquipmentRecipe : PhongThanComposeRecipe {unsigned native[11];};
inline bool PhongThanParseEquipmentUpgradeItem(const char* text,PhongThanRecipeItem& item,unsigned& native){
 native=0;if(!text||strlen(text)>=128)return false;char plain[128];strcpy(plain,text);
 char* bracket=strchr(plain,'[');if(bracket){
  const char* at=bracket+1;int level=0;if(!PhongThanRecipeNumber(at,level)||level>15||*at++!=']'||*at)return false;
  native=1U<<level;*bracket=0;
 }
 return PhongThanParseRecipeItem(plain,item);
}
inline bool PhongThanReadEquipmentRecipe(KTabFile& table,int row,PhongThanEquipmentRecipe& r){
 memset(&r,0,sizeof(r));if(row<2||row>table.GetHeight()||table.GetWidth()!=19)return false;
 table.GetInteger(row,1,0,&r.id);table.GetInteger(row,2,0,&r.attributeId);table.GetInteger(row,3,0,&r.inputCount);
 table.GetInteger(row,15,0,&r.group);table.GetInteger(row,16,0,&r.type);table.GetInteger(row,17,-1,&r.chance);table.GetString(row,18,"",r.requiredRecipe,sizeof(r.requiredRecipe));
 if(r.id<=0||r.inputCount<1||r.inputCount>9||(r.group!=1&&r.group!=8)||(r.type!=1&&r.type!=7)||r.requiredRecipe[0]||r.chance<0||r.chance>100)return false;
 char text[128];for(int i=0;i<r.inputCount;++i){table.GetString(row,4+i,"",text,sizeof(text));if(!PhongThanParseEquipmentUpgradeItem(text,r.inputs[i],r.native[i]))return false;}
 table.GetString(row,14,"",text,sizeof(text));return PhongThanParseEquipmentUpgradeItem(text,r.output,r.native[10]);
}
#endif
