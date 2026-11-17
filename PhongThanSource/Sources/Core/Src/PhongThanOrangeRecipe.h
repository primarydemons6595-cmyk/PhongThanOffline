#ifndef PHONGTHAN_ORANGE_RECIPE_H
#define PHONGTHAN_ORANGE_RECIPE_H
#include "PhongThanComposeRecipe.h"
// Orange rules use [native item level]. Upgrade rules use (star attempts).
// Keep these separate: accepting brackets in the generic upgrade decoder would
// accidentally open unrelated NPC workstations and change their item matching.
inline bool PhongThanParseOrangeItem(const char* text,PhongThanRecipeItem& item){
 const char* p=text;item.upgradeMask=0;
 if(!(PhongThanRecipeNumber(p,item.genre)&&*p++=='.'&&PhongThanRecipeNumber(p,item.detail)&&*p++=='.'&&PhongThanRecipeNumber(p,item.part)&&*p++=='.'&&PhongThanRecipeNumber(p,item.quantity)&&item.quantity>0&&item.quantity<=10000))return false;
 if(*p=='['){++p;int level=0;if(!PhongThanRecipeNumber(p,level)||level>15||*p++!=']')return false;item.upgradeMask=1U<<level;}
 return !*p&&(item.genre!=0||item.upgradeMask!=0);
}
inline bool PhongThanReadOrangeRecipe(KTabFile& table,int row,PhongThanComposeRecipe& r){
 memset(&r,0,sizeof(r));if(row<2||row>table.GetHeight()||table.GetWidth()!=19)return false;
 table.GetInteger(row,1,0,&r.id);table.GetInteger(row,2,0,&r.attributeId);table.GetInteger(row,3,0,&r.inputCount);table.GetInteger(row,15,0,&r.group);table.GetInteger(row,16,0,&r.type);table.GetInteger(row,17,0,&r.chance);table.GetString(row,18,"",r.requiredRecipe,sizeof(r.requiredRecipe));
 if(r.id<=0||r.type!=2||r.group<2||r.group>6||r.inputCount<1||r.inputCount>10||r.chance!=100)return false;
 char text[128];for(int i=0;i<r.inputCount;++i){table.GetString(row,4+i,"",text,sizeof(text));if(!PhongThanParseOrangeItem(text,r.inputs[i]))return false;}
 table.GetString(row,14,"",text,sizeof(text));return PhongThanParseOrangeItem(text,r.output)&&r.output.genre==0&&r.output.quantity==1;
}
#endif
