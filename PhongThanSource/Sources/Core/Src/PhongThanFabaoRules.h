#ifndef PHONGTHAN_FABAO_RULES_H
#define PHONGTHAN_FABAO_RULES_H
#include "PhongThanComposeRecipe.h"
#ifndef PHONGTHAN_FABAO_TEST
#include "KItemGenerator.h"
#endif
#include <vector>
namespace PtFabao {
struct Rule {PhongThanComposeRecipe base;unsigned native[11];bool tienMa;};
struct Plan {Rule rule;int used[9],main,fee,row,stars;};
static bool ParseItem(const char* text,PhongThanRecipeItem& item,unsigned& native){
 native=0;item.upgradeMask=0;const char* p=text;
 if(!(PhongThanRecipeNumber(p,item.genre)&&*p++=='.'&&PhongThanRecipeNumber(p,item.detail)&&*p++=='.'&&PhongThanRecipeNumber(p,item.part)&&*p++=='.'&&PhongThanRecipeNumber(p,item.quantity))||item.quantity<1||item.quantity>10000)return false;
 if(*p=='('){++p;do{int n=0;if(!PhongThanRecipeNumber(p,n)||n>15)return false;item.upgradeMask|=1U<<n;if(*p!=',')break;++p;}while(true);if(*p++!=')')return false;}
 if(*p=='['){++p;int n=0;if(!PhongThanRecipeNumber(p,n)||n>15||*p++!=']')return false;native=1U<<n;}
 if(*p)return false;
 // Original recipes use the Lua spelling 6.1.<key>; generated/stored
 // MagicScript items use genre 6, detail <key>, particular 0.
 if(item.genre==6){if(item.detail!=1||item.part<=0)return false;item.detail=item.part;item.part=0;}
 return true;
}
static bool Read(KTabFile& table,int row,Rule& r){
 ZeroMemory(&r,sizeof(r));if(row<2||row>table.GetHeight()||table.GetWidth()!=19)return false;
 PhongThanComposeRecipe& b=r.base;
 table.GetInteger(row,1,0,&b.id);table.GetInteger(row,2,0,&b.attributeId);table.GetInteger(row,3,0,&b.inputCount);
 table.GetInteger(row,15,0,&b.group);table.GetInteger(row,16,0,&b.type);table.GetInteger(row,17,0,&b.chance);table.GetString(row,18,"",b.requiredRecipe,sizeof(b.requiredRecipe));
 if(b.id<=0||b.inputCount<1||b.inputCount>9||(b.group!=1&&b.group!=100&&b.group!=101)||(b.type!=1&&b.type!=6)||b.chance<1||b.chance>100||b.requiredRecipe[0])return false;
 char text[128];for(int n=0;n<b.inputCount;++n){table.GetString(row,4+n,"",text,sizeof(text));if(!ParseItem(text,b.inputs[n],r.native[n]))return false;}
 table.GetString(row,14,"",text,sizeof(text));if(!ParseItem(text,b.output,r.native[10])||b.output.genre!=0||b.output.detail!=4||b.output.quantity!=1)return false;
 if(b.type==1&&(b.inputs[0].genre!=0||b.inputs[0].detail!=4||b.inputs[0].quantity!=1||b.inputs[0].part!=b.output.part||b.attributeId<=0||b.chance!=100))return false;
 if(b.type==1&&b.inputs[0].upgradeMask){b.inputs[0].upgradeMask&=0xfffU;if(!b.inputs[0].upgradeMask)return false;}
 return true;
}
static bool Tables(std::vector<Rule>*& result,KTabFile*& costs){
 static std::vector<Rule> rules;static KTabFile fees;static bool ready=false;
 if(!ready){KTabFile source;if(!source.Load("\\settings\\item\\001\\zhuang_bei_he_cheng_gui_ze_biao.txt")||!fees.Load("\\settings\\item\\001\\zhuang_bei_he_cheng_jin_qian_xu_qiu.txt"))return false;
  for(int row=2;row<=source.GetHeight();++row){Rule r;if(!Read(source,row,r))continue;r.tienMa=r.base.group==100||r.base.group==101;
   if(!r.tienMa){int level=1;if(r.native[10])for(int l=0;l<=15;++l)if(r.native[10]&(1U<<l)){level=l;break;}
    for(int er=0;er<ItemGen.CatalogEquipmentCount(4);++er){const KBASICPROP_EQUIPMENT* e=ItemGen.CatalogEquipment(4,er);
     if(!e||e->m_nParticularType!=r.base.output.part||e->m_nLevel!=level)continue;
     for(int req=0;req<6;++req)if(e->m_aryPropReq[req].nType==magic_requiregdlevel&&e->m_aryPropReq[req].nPara>0)r.tienMa=true;break;
    }
   }rules.push_back(r);
  }ready=!rules.empty();
 }result=&rules;costs=&fees;return ready;
}
static bool Matches(KItem& item,const PhongThanRecipeItem& wanted,unsigned native){
 int level=item.GetLevel(),stars=item.m_CommonAttrib.nUpgradeLvl;
 return item.GetGenre()==wanted.genre&&item.GetDetailType()==wanted.detail&&item.GetParticular()==wanted.part&&
  (!native||(level>=0&&level<=15&&(native&(1U<<level))))&&(!wanted.upgradeMask||(stars>=0&&stars<=15&&(wanted.upgradeMask&(1U<<stars))));
}
static int Quote(const int* indexes,const int* selected,int group,Plan& plan){
 std::vector<Rule>* rules=0;KTabFile* costs=0;if(!Tables(rules,costs))return PT_COMPOSE_TABLE_ERROR;int unmatched=PT_COMPOSE_NO_RECIPE;
 for(unsigned ri=0;ri<rules->size();++ri){const Rule& r=(*rules)[ri];const PhongThanComposeRecipe& b=r.base;
  if(group==1?b.group!=1:!r.tienMa)continue;
  int left[9],used[9]={0},main=-1;bool match=true;
  for(int s=0;s<9;++s)left[s]=indexes[s]?selected[s]:0;
  for(int n=0;n<b.inputCount&&match;++n){int amount=b.inputs[n].quantity;
   for(int slot=0;slot<9&&amount>0;++slot)if(left[slot]>0&&Matches(Item[indexes[slot]],b.inputs[n],r.native[n])){
    int take=left[slot]<amount?left[slot]:amount;left[slot]-=take;used[slot]+=take;amount-=take;if(n==0)main=slot;
   }if(amount)match=false;
  }
  for(int extra=0;extra<9;++extra)if(left[extra]>0&&!used[extra])match=false;
  if(!match)continue;
  ZeroMemory(&plan,sizeof(plan));plan.rule=r;plan.main=main;memcpy(plan.used,used,sizeof(used));plan.row=-1;
  if(b.type==1){KItem item=Item[indexes[main]];int old=item.m_CommonAttrib.nUpgradeLvl;
   if(old>=12)continue;
   if(old&&item.m_CommonAttrib.nUpgradeRule!=b.attributeId){unmatched=PT_COMPOSE_UPGRADE_MODE_MIX;continue;}
   plan.stars=old+1;if(!item.ApplyUpgradeState(PhongThanEncodeUpgradeState(plan.stars,b.attributeId)))return PT_COMPOSE_TABLE_ERROR;
  }else{
   int level=1;if(r.native[10])for(int l=0;l<=15;++l)if(r.native[10]&(1U<<l)){level=l;break;}
   for(int row=0;row<ItemGen.CatalogEquipmentCount(4);++row){const KBASICPROP_EQUIPMENT* e=ItemGen.CatalogEquipment(4,row);
    if(e&&e->m_nItemGenre==0&&e->m_nDetailType==4&&e->m_nParticularType==b.output.part&&e->m_nLevel==level){plan.row=row;break;}
   }if(plan.row<0)return PT_COMPOSE_TABLE_ERROR;
  }
  if(!PhongThanReadComposeCost(*costs,b.id,b.type==1?plan.stars:1,plan.fee)){
   // A present synthesis row with an empty first-attempt cell has no silver
   // surcharge. Do not borrow fees from unrelated later upgrade columns.
   char key[24],text[32];sprintf(key,"%d",b.id);int feeRow=costs->FindRow(key);
   if(b.type!=6||feeRow<2||costs->GetWidth()!=16)return PT_COMPOSE_NO_COST;
   costs->GetString(feeRow,2,"",text,sizeof(text));if(text[0])return PT_COMPOSE_NO_COST;plan.fee=0;
  }
  return PT_COMPOSE_OK;
 }return unmatched;
}
}
#endif
