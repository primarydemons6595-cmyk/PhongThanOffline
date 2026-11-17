#ifndef PHONGTHAN_WORKSHOP_RULES_H
#define PHONGTHAN_WORKSHOP_RULES_H
#include "PhongThanWorkshopPolicy.h"
#include "KItemGenerator.h"
#include "../../../Headers/PhongThanEquipmentProjection.h"
namespace PtWorkshop {
struct Station {int id,map,npc;DWORD script,alias;};
struct Input {int genre,detail,part,level,series,count;};
struct Recipe {int id,station,source,target,failure,fee,base,count,step,automatic;Input inputs[9],output;};
struct Plan {Recipe recipe;int main,used[128],indexes[128],available[128],poolCount,chance;};
static std::vector<Station> stations;static std::vector<Recipe> recipes;static bool attempted=false,valid=true;
static bool ItemTuple(const char* section,const char* key,Input& i){int v[6];if(!PtWorkshopPolicy::Tuple(section,key,v,6))return false;i.genre=v[0];i.detail=v[1];i.part=v[2];i.level=v[3];i.series=v[4];i.count=v[5];return (i.genre==0||i.genre==1||i.genre==3||i.genre==4||i.genre==6||i.genre==7||i.genre==8)&&i.level<=15&&i.series<=1000&&i.count>=1&&i.count<=10000;}
static unsigned long revision=0;
static void Load(){PtWorkshopPolicy::Load();if(revision!=PtWorkshopPolicy::revision){revision=PtWorkshopPolicy::revision;attempted=false;valid=true;stations.clear();recipes.clear();}if(attempted)return;attempted=true;if(!PtWorkshopPolicy::valid){valid=false;return;}if(!PtWorkshopPolicy::enabled)return;
 int sc=PtWorkshopPolicy::Number("Main","StationCount",-1),rc=PtWorkshopPolicy::Number("Main","RecipeCount",-1);if(sc<0||sc>64||rc<0||rc>8192){valid=false;return;}
 for(int n=1;n<=sc;++n){char sec[32],path[256],hex[512],alias[32];sprintf(sec,"Station%d",n);Station s;s.id=PtWorkshopPolicy::Number(sec,"Id",0);s.map=PtWorkshopPolicy::Number(sec,"Map",0);s.npc=PtWorkshopPolicy::Number(sec,"Template",-1);PtWorkshopPolicy::ini.GetString(sec,"ScriptHex","",hex,sizeof(hex));int len=strlen(hex);if(len<2||len>500||len%2){valid=false;return;}for(int h=0;h<len/2;++h){unsigned v=0;if(sscanf(hex+h*2,"%2x",&v)!=1||!v){valid=false;return;}path[h]=(char)v;}path[len/2]=0;PtWorkshopPolicy::ini.GetString(sec,"Alias","0",alias,sizeof(alias));s.alias=strtoul(alias,0,16);char full[260];sprintf(full,"\\%s",path);s.script=g_FileName2Id(full);if(s.id<1||s.id>64||s.map<1||s.npc<0||!path[0]){valid=false;return;}stations.push_back(s);}
 for(n=1;n<=rc;++n){char sec[32];sprintf(sec,"Recipe%d",n);if(!PtWorkshopPolicy::Number(sec,"Enabled",0))continue;Recipe r;ZeroMemory(&r,sizeof(r));r.id=PtWorkshopPolicy::Number(sec,"Id",0);r.station=PtWorkshopPolicy::Number(sec,"Station",0);r.automatic=PtWorkshopPolicy::Number(sec,"AutoMaterials",0);r.step=PtWorkshopPolicy::Number(sec,"Step",1);r.source=PtWorkshopPolicy::Number(sec,"SourceRow",-1);r.target=PtWorkshopPolicy::Number(sec,"TargetRow",-1);r.failure=PtWorkshopPolicy::Number(sec,"FailureRow",-2);r.fee=PtWorkshopPolicy::Number(sec,"Fee",-1);r.base=PtWorkshopPolicy::Number(sec,"Base",-1);r.count=PtWorkshopPolicy::Number(sec,"Inputs",0);
  if(r.id<1000000||r.step<1||r.step>12||r.count<1||r.count>9||r.source< -1||r.target< -1||r.failure< -2||r.fee<0||r.fee>1000000000||r.base<0||r.base>100||!ItemTuple(sec,"Output",r.output)||r.output.count>64){valid=false;return;}
  bool found=false;for(unsigned s=0;s<stations.size();++s)if(stations[s].id==r.station)found=true;if(!found){valid=false;return;}
  for(int j=0;j<r.count;++j){char key[20];sprintf(key,"Input%d",j+1);if(!ItemTuple(sec,key,r.inputs[j])){valid=false;return;}}
  if(r.source>=0&&(r.inputs[0].genre!=0||r.inputs[0].count!=1||r.output.genre!=0||r.output.count!=1||r.target<0)){valid=false;return;}recipes.push_back(r);
 }
}
static int StationId(int map,int npc,DWORD script){Load();if(!valid)return 0;for(unsigned i=0;i<stations.size();++i){const Station& s=stations[i];if(s.map==map&&(!s.npc||s.npc==npc)&&(s.script==script||(s.alias&&s.alias==script)))return s.id;}return 0;}
static bool Matches(KItem& a,const Input& b){return a.GetGenre()==b.genre&&a.GetDetailType()==b.detail&&a.GetParticular()==b.part&&((b.genre!=0&&b.genre!=1)||(a.GetLevel()==b.level&&(b.series==1000||a.GetSeries()==b.series)));}
// A baked template transition changes only its proven attribute deltas. The
// equipment object, random rolls, socket data, locks, seed and serial survive.
static bool Transition(KItem& item,int row){
 if(item.GetGenre()!=item_equip||item.m_CommonAttrib.nUpgradeLvl||item.m_CommonAttrib.nUpgradeRule)return false;
 const KBASICPROP_EQUIPMENT* a=ItemGen.CatalogEquipment(item.GetDetailType(),item.GetRow());const KBASICPROP_EQUIPMENT* b=ItemGen.CatalogEquipment(item.GetDetailType(),row);
 if(!a||!b||a->m_nDetailType!=b->m_nDetailType||a->m_nLevel!=b->m_nLevel||a->m_nWidth!=b->m_nWidth||a->m_nHeight!=b->m_nHeight||a->m_nSetId!=b->m_nSetId||a->m_nSeries!=b->m_nSeries)return false;
 for(int q=0;q<6;++q)if(a->m_aryPropReq[q].nType!=b->m_aryPropReq[q].nType||a->m_aryPropReq[q].nPara!=b->m_aryPropReq[q].nPara)return false;
 KItemNormalAttrib abase[MAX_ITEM_BASEATTRIB],bbase[MAX_ITEM_BASEATTRIB],amag[MAX_ITEM_MAGICATTRIB],bmag[MAX_ITEM_MAGICATTRIB]; ZeroMemory(abase,sizeof(abase));ZeroMemory(bbase,sizeof(bbase));ZeroMemory(amag,sizeof(amag));ZeroMemory(bmag,sizeof(bmag));
 for(int i=0;i<MAX_ITEM_BASEATTRIB;++i){const KEQCP_BASIC& x=a->m_aryPropBasic[i];const KEQCP_BASIC& y=b->m_aryPropBasic[i];__int64 shift=(__int64)y.sRange.nMin-x.sRange.nMin;if(shift!=(__int64)y.sRange.nMax-x.sRange.nMax||shift>2147483647||shift<(-2147483647-1)||(x.nType!=y.nType&&(x.sRange.nMin||x.sRange.nMax||y.sRange.nMin||y.sRange.nMax)))return false;abase[i].nAttribType=x.nType;abase[i].nValue[0]=x.sRange.nMin;bbase[i].nAttribType=y.nType;bbase[i].nValue[0]=y.sRange.nMin;}
 PhongThanProjectEquipmentModifiers(abase,MAX_ITEM_BASEATTRIB,amag,MAX_ITEM_MAGICATTRIB,magic_normal_begin,magic_normal_end);PhongThanProjectEquipmentModifiers(bbase,MAX_ITEM_BASEATTRIB,bmag,MAX_ITEM_MAGICATTRIB,magic_normal_begin,magic_normal_end);
 KItemNormalAttrib actual[MAX_ITEM_BASEATTRIB];memcpy(actual,item.P0BaseAttributes(),sizeof(actual));
 for(i=0;i<MAX_ITEM_BASEATTRIB;++i){if(abase[i].nAttribType!=bbase[i].nAttribType){if(actual[i].nAttribType!=abase[i].nAttribType||actual[i].nValue[0]||actual[i].nValue[1]||actual[i].nValue[2])return false;actual[i].nAttribType=bbase[i].nAttribType;}int delta=bbase[i].nValue[0]-abase[i].nValue[0];if(delta&&(actual[i].nAttribType!=abase[i].nAttribType||(delta>0&&actual[i].nValue[0]>2147483647-delta)||(delta<0&&actual[i].nValue[0]<(-2147483647-1)-delta)))return false;actual[i].nValue[0]+=delta;}
 KItemNormalAttrib magic[MAX_ITEM_MAGICATTRIB];memcpy(magic,item.m_aryMagicAttrib,sizeof(magic));for(i=0;i<MAX_ITEM_MAGICATTRIB;++i){if(amag[i].nAttribType!=bmag[i].nAttribType)return false;int delta=bmag[i].nValue[0]-amag[i].nValue[0];if(delta&&(magic[i].nAttribType!=amag[i].nAttribType||(delta>0&&magic[i].nValue[0]>2147483647-delta)||(delta<0&&magic[i].nValue[0]<(-2147483647-1)-delta)))return false;magic[i].nValue[0]+=delta;}
 if(!item.SetBaseAttrib(actual))return false;memcpy(item.m_aryMagicAttrib,magic,sizeof(magic));item.SetRow(row);item.SetParticular(b->m_nParticularType);strcpy(item.m_CommonAttrib.szItemName,b->m_szName);item.m_CommonAttrib.nEquipId=b->m_nEquipId;item.m_CommonAttrib.nPhysicVal=b->m_nBasePower;item.m_CommonAttrib.nMagicVal=b->m_nMagicPower;
#ifndef _SERVER
 strcpy(item.m_CommonAttrib.szImageName,b->m_szImageName);strcpy(item.m_CommonAttrib.szIntro,b->m_szIntro);
#endif
 item.SetLockSell(item.GetLockSell()||!b->m_nCanSell);item.SetLockTrade(item.GetLockTrade()||!b->m_nCanTrade);item.SetLockDrop(item.GetLockDrop()||!b->m_nCanDrop);return true;
}
static bool BagPlace(int place){return place==pos_equiproom||place==pos_equiproomex||place==pos_ptbag2||place==pos_ptbag3;}
static int Quote(int player,const int* indexes,const int* selected,int station,Plan& out){Load();if(!valid)return PT_COMPOSE_TABLE_ERROR;
 for(unsigned r=0;r<recipes.size();++r){const Recipe& recipe=recipes[r];if(recipe.station!=station)continue;bool mainCandidate=false;for(int mainCheck=0;mainCheck<9;++mainCheck)if(indexes[mainCheck]&&selected[mainCheck]>0&&Matches(Item[indexes[mainCheck]],recipe.inputs[0])&&(recipe.source<0||Item[indexes[mainCheck]].GetRow()==recipe.source))mainCandidate=true;if(!mainCandidate)continue;Plan plan;ZeroMemory(&plan,sizeof(plan));plan.recipe=recipe;plan.main=-1;plan.poolCount=9;
  for(int s=0;s<9;++s){plan.indexes[s]=indexes[s];plan.available[s]=indexes[s]?selected[s]:0;}
  if(recipe.automatic)for(PlayerItem* bag=Player[player].m_ItemList.GetFirstItem();bag;bag=Player[player].m_ItemList.GetNextItem()){
   int i=bag->nIdx;if(!BagPlace(bag->nPlace)||i<=0||i>=MAX_ITEM||!Item[i].GetID()||Item[i].IsTemp()||Item[i].GetLock()->IsLock())continue;
   bool duplicate=false;for(int x=0;x<plan.poolCount;++x)if(plan.indexes[x]==i)duplicate=true;if(duplicate)continue;
   bool needed=false;for(int n=1;n<recipe.count;++n)if(Matches(Item[i],recipe.inputs[n]))needed=true;
   if(!needed||Item[i].GetStackNum()<1||Item[i].GetStackNum()>10000||(Item[i].GetGenre()==0&&(Item[i].m_CommonAttrib.nUpgradeLvl||Item[i].GetParam()!=-1)))continue;
   if(plan.poolCount>=128)return PT_COMPOSE_TABLE_ERROR;plan.indexes[plan.poolCount]=i;plan.available[plan.poolCount++]=Item[i].GetStackNum();
  }
  int left[128];memcpy(left,plan.available,sizeof(left));bool match=true;
  for(int n=0;n<recipe.count&&match;++n){int amount=recipe.inputs[n].count;int limit=n==0?9:plan.poolCount;
   for(int slot=0;slot<limit&&amount>0;++slot)if(left[slot]>0&&Matches(Item[plan.indexes[slot]],recipe.inputs[n])&&(n!=0||recipe.source<0||Item[plan.indexes[slot]].GetRow()==recipe.source)){
    if(n>0&&Item[plan.indexes[slot]].GetGenre()==0&&(Item[plan.indexes[slot]].m_CommonAttrib.nUpgradeLvl||Item[plan.indexes[slot]].GetParam()!=-1))continue;
    int take=left[slot]<amount?left[slot]:amount;left[slot]-=take;amount-=take;plan.used[slot]+=take;if(n==0)plan.main=slot;
   }if(amount)match=false;
  }
  for(s=0;s<9;++s)if(left[s]&&!plan.used[s])match=false;if(!match)continue;
  if(recipe.source>=0){KItem item=Item[plan.indexes[plan.main]];if(!Transition(item,recipe.target)||!Matches(item,recipe.output))return PT_COMPOSE_TABLE_ERROR;if(recipe.failure>=0){item=Item[plan.indexes[plan.main]];if(!Transition(item,recipe.failure))return PT_COMPOSE_TABLE_ERROR;}}
  plan.chance=recipe.base;for(s=0;s<plan.poolCount;++s)if(plan.used[s]&&s!=plan.main){KItem& item=Item[plan.indexes[s]];plan.chance+=PtWorkshopPolicy::Bonus(item.GetGenre(),item.GetDetailType(),item.GetParticular(),item.GetLevel(),item.GetSeries(),recipe.step)*plan.used[s];}if(plan.chance>100)plan.chance=100;out=plan;return PT_COMPOSE_OK;
 }return PT_COMPOSE_NO_RECIPE;
}
}
#endif
