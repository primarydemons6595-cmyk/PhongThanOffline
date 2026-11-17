#include "PhongThanGemRules.h"
#include "PhongThanOrangeRecipe.h"
#include "PhongThanForgeInherited.h"
#include <map>
#include "../../../Headers/PhongThanStarterWeapon.h"
#include "../../../Headers/PhongThanForgeState.h"
namespace PtForge {
static KTabFile rules,attributes,expansions;
static bool ready=false;
static bool Tables(){if(!ready)ready=rules.Load("\\settings\\item\\001\\zhuang_bei_he_cheng_gui_ze_biao.txt")&&attributes.Load("\\settings\\item\\001\\zhuang_bei_dian_lan_fu_jia_shu_xing_biao.txt")&&expansions.Load("\\settings\\item\\001\\zhuang_bei_dian_lan_fu_jia_kuo_zhan_shu_xing_biao.txt");return ready;}
static unsigned Draw(unsigned& seed){seed^=seed<<13;seed^=seed>>17;seed^=seed<<5;return seed;}
static int Field(KTabFile& t,int row,int column){int n=0;t.GetInteger(row,column,0,&n);return n;}
static bool Recipe(int id,PhongThanComposeRecipe& r){char key[24];sprintf(key,"%d",id);return Tables()&&PhongThanReadOrangeRecipe(rules,rules.FindRow(key),r)&&r.type==2&&r.group>=2&&r.group<=6&&r.output.genre==item_equip&&r.output.quantity==1&&r.chance==100;}
static bool Matches(KItem& item,const PhongThanRecipeItem& wanted){return item.GetGenre()==wanted.genre&&item.GetDetailType()==wanted.detail&&item.GetParticular()==wanted.part&&item.GetLevel()>=0&&item.GetLevel()<16&&(!wanted.upgradeMask||(wanted.upgradeMask&(1U<<item.GetLevel())));}
static bool State(KItem& item,int id,char* name,KItemNormalAttrib* values){
 PhongThanComposeRecipe recipe;if(!Recipe(id,recipe)||!Matches(item,recipe.output))return false;
 char key[24];sprintf(key,"%d",recipe.attributeId);int row=attributes.FindRow(key);if(row<2)return false;
 const KBASICPROP_EQUIPMENT* source=ItemGen.CatalogEquipment(item.GetDetailType(),item.GetRow());
 if(!source||source->m_nItemGenre!=item.GetGenre()||source->m_nDetailType!=item.GetDetailType()||source->m_nParticularType!=item.GetParticular()||source->m_nLevel!=item.GetLevel())return false;
 char prefix[80];attributes.GetString(row,2,"",prefix,sizeof(prefix));int len=strlen(prefix);if(!len)return false;
 if(prefix[len-1]=='*')prefix[--len]=0;const char* p=prefix;if(*p=='#')++p;const char* base=source->m_szName;if(*base=='#')++base;
 if(strlen(p)+1+strlen(base)>=80)return false;sprintf(name,"%s %s",p,base);
 ZeroMemory(values,sizeof(KItemNormalAttrib)*MAX_ITEM_MAGICATTRIB);
 unsigned seed=item.m_GeneratorParam.uRandomSeed^((unsigned)id*2654435761U);if(!seed)seed=1;
 static std::map<int,int> inheritedRows;int ordinary=row;
 if(!PhongThanForgeHasOrdinary(attributes,row)){
  std::map<int,int>::iterator cached=inheritedRows.find(recipe.attributeId);
  if(cached==inheritedRows.end()){ordinary=PhongThanForgeOrdinaryRow(rules,attributes,recipe,row);inheritedRows[recipe.attributeId]=ordinary;}else ordinary=cached->second;
 }
 if(!ordinary)return false;
 unsigned baseSeed=seed;
 for(int i=0;i<6;++i){int col=4+i*5,percent=Field(attributes,ordinary,col),type=Field(attributes,ordinary,col+1),low=Field(attributes,ordinary,col+2),high=Field(attributes,ordinary,col+3);
  if(!type||percent<=0)continue;if(percent>100||high<low)return false;
  if((int)(Draw(baseSeed)%100)>=percent)continue;
  values[i].nAttribType=type;values[i].nValue[0]=low+(int)(Draw(baseSeed)%(unsigned)(high-low+1));values[i].nValue[2]=Field(attributes,ordinary,col+4);
 }
 // Preserve the existing expansion draws for inherited rows. Their missing
 // ordinary attributes previously consumed no RNG; repairing them must not
 // reroll the three already awarded expansion properties.
 if(ordinary==row)seed=baseSeed;
 // Original optional expansion attributes belong to slots 13-15.
 for(i=0;i<3;++i){int group=Field(attributes,row,36+i);if(!group)continue;sprintf(key,"%d",group);int er=expansions.FindRow(key),count=Field(expansions,er,2);if(er<2||count<1||count>14)return false;
  int total=0;for(int k=0;k<count;++k)total+=Field(expansions,er,5+k*3);if(total<=0)return false;
  int pick=Draw(seed)%total;for(k=0;k<count;++k){pick-=Field(expansions,er,5+k*3);if(pick<0){values[13+i].nAttribType=Field(expansions,er,3+k*3);values[13+i].nValue[0]=Field(expansions,er,4+k*3);break;}}
 }
 return true;
}
}
BOOL KItem::ApplyForgeRecipe(int recipe){
 if(GetGenre()!=item_equip)return FALSE;
 char name[80];KItemNormalAttrib values[MAX_ITEM_MAGICATTRIB];
 if(!PtForge::State(*this,recipe,name,values))return FALSE;
 int param=PhongThanForgeParam(recipe);
 PT_SOUL_STATE soul;if(PtSoulFromParam(m_CommonAttrib.nParam,soul)){soul.BaseParam=param;if(!PtSoulReplace(m_CommonAttrib.nParam,soul,param))return FALSE;}
 else if(PtSocketParam(m_CommonAttrib.nParam)){PT_SOCKET_STATE state;if(!PtSocketRead(m_CommonAttrib.nParam,state))return FALSE;state.BaseParam=param;if(!PtSocketStore(state,param))return FALSE;}
 m_CommonAttrib.nParam=param;
 g_StrCpyLen(m_CommonAttrib.szItemName,name,sizeof(m_CommonAttrib.szItemName));return SetMagicAttrib(values);
}
BOOL KItem::RestoreForgeInherited(){
 int recipeId=PhongThanForgeRecipe(GetUnderlyingParam());if(!recipeId)return FALSE;
 PhongThanComposeRecipe recipe;if(!PtForge::Recipe(recipeId,recipe))return FALSE;
 char key[24];sprintf(key,"%d",recipe.attributeId);int row=PtForge::attributes.FindRow(key);
 if(row<2||PhongThanForgeHasOrdinary(PtForge::attributes,row))return FALSE;
 char name[80];KItemNormalAttrib values[MAX_ITEM_MAGICATTRIB];if(!PtForge::State(*this,recipeId,name,values))return FALSE;
 // Socket/soul definitions are metadata layers, not extra ordinary slots.
 // Retain their exact stored records and all expansion/set slots.
 int upgrade=GetUpgradeState();KItem unupgraded=*this;
 if(!unupgraded.ApplyUpgradeState(0))return FALSE;
 KItem legacy=unupgraded,fixed=unupgraded;
 for(int i=0;i<6;++i){ZeroMemory(&legacy.m_aryMagicAttrib[i],sizeof(KItemNormalAttrib));fixed.m_aryMagicAttrib[i]=values[i];}
 if(!legacy.ApplyUpgradeState(upgrade)||!fixed.ApplyUpgradeState(upgrade))return FALSE;
 if(memcmp(m_aryMagicAttrib,legacy.m_aryMagicAttrib,6*sizeof(KItemNormalAttrib))||memcmp(m_aryBaseAttrib,legacy.m_aryBaseAttrib,sizeof(m_aryBaseAttrib))||memcmp(m_aryRequireAttrib,legacy.m_aryRequireAttrib,sizeof(m_aryRequireAttrib)))return FALSE;
 memcpy(m_aryMagicAttrib,fixed.m_aryMagicAttrib,6*sizeof(KItemNormalAttrib));
 memcpy(m_aryBaseAttrib,fixed.m_aryBaseAttrib,sizeof(m_aryBaseAttrib));memcpy(m_aryRequireAttrib,fixed.m_aryRequireAttrib,sizeof(m_aryRequireAttrib));
 m_CommonAttrib.nUpgradeAddedBaseMask=fixed.m_CommonAttrib.nUpgradeAddedBaseMask;m_CommonAttrib.nUpgradeAddedRequireMask=fixed.m_CommonAttrib.nUpgradeAddedRequireMask;return TRUE;
}
void KItem::SetParam(int param){
 if(GetGenre()==item_materials && PtGem::Identity(GetGenre(),GetDetailType(),GetParticularType(),GetLevel())){
  if(!PtGem::Restore(*this,param)){m_CommonAttrib.nParam=param;ZeroMemory(m_aryMagicAttrib,sizeof(m_aryMagicAttrib));}return;
 }
 if(GetGenre()==item_equip && PtSocketParam(param)){
  PT_SOCKET_STATE state;if(PtSocketRead(param,state)){SetParam(state.BaseParam);if(m_CommonAttrib.nParam==state.BaseParam)m_CommonAttrib.nParam=param;}
  else {m_CommonAttrib.nParam=param;
#ifndef _SERVER
   PtSocketRequest(param);
#endif
  }return;
 }
 if(GetGenre()==item_equip&&PtSoulParam(param)){
  PT_SOUL_STATE state;if(PtSoulRead(param,state)){SetParam(state.BaseParam);if(m_CommonAttrib.nParam==state.BaseParam)m_CommonAttrib.nParam=param;}
  else {m_CommonAttrib.nParam=param;
#ifndef _SERVER
   PtSoulRequest(param);
#endif
  }return;
 }
 m_CommonAttrib.nParam=param;
 int starter=PhongThanStarterWeaponClass(param);
 if(starter>=0&&GetGenre()==item_equip&&GetDetailType()==0&&GetRow()==PhongThanStarterWeaponRow(starter)&&GetLevel()==1){
  KItemNormalAttrib values[MAX_ITEM_MAGICATTRIB];ZeroMemory(values,sizeof(values));int slot=0;
  if(starter==0||starter==2){values[slot].nAttribType=115;values[slot++].nValue[0]=10;}
  if(starter==1||starter==2){values[slot].nAttribType=116;values[slot++].nValue[0]=10;}
  values[slot].nAttribType=121;values[slot].nValue[0]=10;SetMagicAttrib(values);
 }
 int recipe=PhongThanForgeRecipe(param);
 if(GetGenre()==item_equip&&recipe&&!ApplyForgeRecipe(recipe))m_CommonAttrib.nParam=-1;
}
