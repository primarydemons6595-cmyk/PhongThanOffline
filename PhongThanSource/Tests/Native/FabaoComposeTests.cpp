#define Item UnallocatedEngineItems
#define main UpgradeStateUnitMain
#include "ItemUpgradeStateTests.cpp"
#undef main
#undef Item
#include "../../Headers/PhongThanCompose.h"
#include "../../Sources/Core/Src/PhongThanXichTungTu.h"
#include "KBasPropTbl.h"
#include <vector>
KItem Item[16];
struct FabaoCatalog {
 std::vector<KBASICPROP_EQUIPMENT> entries;
 int CatalogEquipmentCount(int){return entries.size();}
 const KBASICPROP_EQUIPMENT* CatalogEquipment(int,int row){return row>=0&&row<(int)entries.size()?&entries[row]:0;}
 void Load(){KTabFile table;assert(table.Load("\\settings\\item\\001\\amulet.txt"));
  for(int row=2;row<=table.GetHeight();++row){KBASICPROP_EQUIPMENT e;ZeroMemory(&e,sizeof(e));
   table.GetInteger(row,2,0,&e.m_nItemGenre);table.GetInteger(row,3,0,&e.m_nDetailType);table.GetInteger(row,4,0,&e.m_nParticularType);table.GetInteger(row,12,0,&e.m_nLevel);
   table.GetInteger(row,7,0,&e.m_nWidth);table.GetInteger(row,8,0,&e.m_nHeight);
   for(int rq=0;rq<6;++rq){table.GetInteger(row,45+rq*2,0,&e.m_aryPropReq[rq].nType);table.GetInteger(row,46+rq*2,0,&e.m_aryPropReq[rq].nPara);}
   for(int b=0;b<10;++b){table.GetInteger(row,15+b*3,0,&e.m_aryPropBasic[b].nType);table.GetInteger(row,16+b*3,0,&e.m_aryPropBasic[b].sRange.nMin);table.GetInteger(row,17+b*3,0,&e.m_aryPropBasic[b].sRange.nMax);}
   entries.push_back(e);
  }
 }
} ItemGen;
struct ItemPos {int nPlace,nX,nY;};
struct FabaoBag {
 bool owned[16],room,payAllowed;int money,failedRemove;
 int GetEquipmentMoney(){return money;}
 bool SearchPosition(int,int,ItemPos* p,bool){p->nPlace=pos_equiproom;p->nX=p->nY=0;return room;}
 bool Remove(int id){if(id==failedRemove||!owned[id])return false;owned[id]=false;return true;}
 int Add(int id,int place,int,int,bool){if(owned[id]||(place!=pos_builditem&&!room))return 0;owned[id]=true;return id;}
};
struct FabaoProfession {int GetProfession(){return 2;}};
struct FabaoPlayer {FabaoProfession m_cProfession;FabaoBag m_ItemList;bool Pay(int amount){if(!m_ItemList.payAllowed||m_ItemList.money<amount)return false;m_ItemList.money-=amount;return true;}} Player[2];
static int serial=200;
static void LoadItem(int id,int row){
 const KBASICPROP_EQUIPMENT& e=ItemGen.entries[row];ZeroMemory(&Item[id],sizeof(KItem));Item[id].SetID(++serial);Item[id].SetGenre(0);Item[id].SetDetailType(4);Item[id].SetParticular(e.m_nParticularType);Item[id].SetLevel(e.m_nLevel);Item[id].SetRow(row);
 Item[id].m_CommonAttrib.nWidth=e.m_nWidth;Item[id].m_CommonAttrib.nHeight=e.m_nHeight;Item[id].m_CommonAttrib.nStackNum=1;Item[id].m_GeneratorParam.uRandomSeed=123456;
 for(int b=0;b<5;++b){Item[id].m_aryBaseAttrib[b].nAttribType=e.m_aryPropBasic[b].nType;Item[id].m_aryBaseAttrib[b].nValue[0]=e.m_aryPropBasic[b].sRange.nMin;}
 for(b=5;b<10;++b){Item[id].m_aryMagicAttrib[b-5].nAttribType=e.m_aryPropBasic[b].nType;Item[id].m_aryMagicAttrib[b-5].nValue[0]=e.m_aryPropBasic[b].sRange.nMin;}
}
struct FabaoItemSet {int fail;
 int Add(int,int,int,int,int,int,int*,int,int,int row){if(fail)return 0;for(int i=10;i<16;++i)if(!Item[i].GetID()){LoadItem(i,row);return i;}return 0;}
 void Remove(int i){Item[i].SetID(0);}
} ItemSet;
struct FabaoWorldVersion {int GetGameVersion(){return 1;}} g_SubWorldSet;
static unsigned forced=0;static int FabaoRandom(int bound){return forced%bound;}
#define g_Random FabaoRandom
#define PHONGTHAN_FABAO_TEST
#include "../../Sources/Core/Src/PhongThanFabaoCompose.inl"
#undef g_Random
static void Reset(){ZeroMemory(Item,sizeof(Item));ZeroMemory(Player,sizeof(Player));Player[1].m_ItemList.room=Player[1].m_ItemList.payAllowed=true;Player[1].m_ItemList.money=2000000000;ItemSet.fail=0;forced=0;}
static int Row(int part,int level){for(unsigned k=0;k<ItemGen.entries.size();++k)if(ItemGen.entries[k].m_nParticularType==part&&ItemGen.entries[k].m_nLevel==level)return k;return -1;}
static void SeedRule(const PtFabao::Rule& r,int* indexes,int* selected){
 Reset();ZeroMemory(indexes,sizeof(int)*9);ZeroMemory(selected,sizeof(int)*9);
 for(int n=0;n<r.base.inputCount;++n){const PhongThanRecipeItem& want=r.base.inputs[n];int id=n+1;indexes[n]=id;selected[n]=want.quantity;
  int level=want.genre==0?1:0;if(r.native[n])for(int l=0;l<=15;++l)if(r.native[n]&(1U<<l)){level=l;break;}
  if(want.genre==0){int row=Row(want.part,level);assert(row>=0);LoadItem(id,row);}else{Item[id].SetID(++serial);Item[id].SetGenre(want.genre);Item[id].SetDetailType(want.detail);Item[id].SetParticular(want.part);Item[id].SetLevel(level);Item[id].m_CommonAttrib.nWidth=Item[id].m_CommonAttrib.nHeight=1;}
  Item[id].m_CommonAttrib.nMaxStack=10000;Item[id].m_CommonAttrib.nStackNum=want.quantity;Player[1].m_ItemList.owned[id]=true;
  if(want.upgradeMask){int star=0;while(star<=15&&!(want.upgradeMask&(1U<<star)))++star;
   if(star){int rule=r.base.attributeId;if(!rule){std::vector<PtFabao::Rule>* all=0;KTabFile* costs=0;assert(PtFabao::Tables(all,costs));for(unsigned a=0;a<all->size();++a)if((*all)[a].base.type==1&&(*all)[a].base.inputs[0].part==want.part){rule=(*all)[a].base.attributeId;break;}}
    assert(rule>0&&Item[id].ApplyUpgradeState(PhongThanEncodeUpgradeState(star,rule)));
   }
  }
 }
}
int main(int argc,char**argv){
 assert(argc==2&&SetCurrentDirectoryA(argv[1]));g_SetRootPath(0);g_SetFilePath("\\");KPakList packs;assert(packs.Open("\\package.ini"));g_pPakList=&packs;g_SetPakFileMode(1);ItemGen.Load();
 std::vector<PtFabao::Rule>* rules=0;KTabFile* costs=0;assert(PtFabao::Tables(rules,costs));int craft=0,upgrades=0,quotes=0;int indexes[9],selected[9];
 for(unsigned r=0;r<rules->size();++r){const PtFabao::Rule& rule=(*rules)[r];SeedRule(rule,indexes,selected);PtFabao::Plan plan;int group=rule.base.group==1?1:100;
  int status=PtFabao::Quote(indexes,selected,group,plan);if(status!=PT_COMPOSE_OK){printf("FAIL quote recipe=%d status=%d\n",rule.base.id,status);return 1;}++quotes;
  if(rule.tienMa){PtFabao::Plan atTao;assert(PtFabao::Quote(indexes,selected,100,atTao)==PT_COMPOSE_OK);}
  int id=0,fee=0;assert(PhongThanFabaoCompose(1,indexes,selected,group,id,fee,true)==PT_COMPOSE_OK);int money=Player[1].m_ItemList.money;
  DWORD old=Item[indexes[plan.main]].GetID();unsigned seed=Item[indexes[plan.main]].m_GeneratorParam.uRandomSeed;
  assert(PhongThanFabaoCompose(1,indexes,selected,group,id,fee,false)==(plan.rule.base.type==1?PT_COMPOSE_UPGRADE_OK:PT_COMPOSE_OK));assert(Player[1].m_ItemList.money==money-fee);
  if(plan.rule.base.type==1){assert(Item[indexes[plan.main]].GetID()==old&&Item[indexes[plan.main]].m_GeneratorParam.uRandomSeed==seed&&Item[indexes[plan.main]].m_CommonAttrib.nUpgradeLvl==plan.stars);++upgrades;}
  else{assert(Item[10].GetID()&&Item[10].GetParticular()==plan.rule.base.output.part&&Item[10].GetRow()==plan.row);++craft;}
  SeedRule(rule,indexes,selected);KItem before=Item[indexes[plan.main]];Player[1].m_ItemList.payAllowed=false;money=Player[1].m_ItemList.money;
  assert(PhongThanFabaoCompose(1,indexes,selected,group,id,fee,false)==PT_COMPOSE_TABLE_ERROR);if(memcmp(&before,&Item[indexes[plan.main]],sizeof(KItem))||Player[1].m_ItemList.money!=money){printf("ROLLBACK recipe=%d main=%d type=%d\n",rule.base.id,plan.main,rule.base.type);unsigned char* a=(unsigned char*)&before;unsigned char* b=(unsigned char*)&Item[indexes[plan.main]];for(int diff=0;diff<sizeof(KItem);++diff)if(a[diff]!=b[diff])printf("byte %d %u -> %u\n",diff,a[diff],b[diff]);return 1;}
 }
 PtFabao::Rule base;bool found=false;for(r=0;r<rules->size();++r)if((*rules)[r].base.id==82){base=(*rules)[r];found=true;}assert(found);
 SeedRule(base,indexes,selected);forced=99;int id=0,fee=0;assert(PhongThanFabaoCompose(1,indexes,selected,1,id,fee,false)==PT_COMPOSE_FABAO_FAILED);for(int s=0;s<base.base.inputCount;++s)assert(!Item[indexes[s]].GetID());assert(!Item[10].GetID());
 SeedRule(base,indexes,selected);Player[1].m_ItemList.room=false;assert(PhongThanFabaoCompose(1,indexes,selected,1,id,fee,false)==PT_COMPOSE_NO_ROOM&&Item[1].GetID());
 SeedRule(base,indexes,selected);ItemSet.fail=1;assert(PhongThanFabaoCompose(1,indexes,selected,1,id,fee,false)==PT_COMPOSE_TABLE_ERROR&&Item[1].GetID());
 SeedRule(base,indexes,selected);Player[1].m_ItemList.money=0;assert(PhongThanFabaoCompose(1,indexes,selected,1,id,fee,false)==PT_COMPOSE_NO_MONEY&&Item[1].GetID());
 SeedRule(base,indexes,selected);assert(PhongThanFabaoCompose(1,indexes,selected,100,id,fee,false)==PT_COMPOSE_NO_RECIPE);
 SeedRule(base,indexes,selected);Item[1].SetLockTrade(TRUE);assert(PhongThanFabaoCompose(1,indexes,selected,1,id,fee,false)==PT_COMPOSE_OK&&Item[10].GetLockTrade());
 SeedRule(base,indexes,selected);KItem saved[9];for(int snap=0;snap<9;++snap)if(indexes[snap])saved[snap]=Item[indexes[snap]];Player[1].m_ItemList.failedRemove=2;
 assert(PhongThanFabaoCompose(1,indexes,selected,1,id,fee,false)==PT_COMPOSE_TABLE_ERROR);for(snap=0;snap<9;++snap)if(indexes[snap])assert(!memcmp(&saved[snap],&Item[indexes[snap]],sizeof(KItem))&&Player[1].m_ItemList.owned[indexes[snap]]);
 SeedRule(base,indexes,selected);Item[3].m_CommonAttrib.nMaxStack=100;Item[3].SetStackNum(20);selected[2]=20;assert(PhongThanFabaoCompose(1,indexes,selected,1,id,fee,true)==PT_COMPOSE_OK&&selected[2]==1);
 assert(PhongThanFabaoCompose(1,indexes,selected,1,id,fee,false)==PT_COMPOSE_OK&&Item[3].GetStackNum()==19&&Player[1].m_ItemList.owned[3]);
 DWORD tao=g_FileName2Id("\\script\\phongthan\\npc_restore\\1073_06.lua");assert(PhongThanIsTaoBao(1073,759,tao)&&!PhongThanIsTaoBao(1003,759,tao)&&!PhongThanIsTaoBao(1073,760,tao)&&!PhongThanIsTaoBao(1073,759,0));
 printf("PASS Fabao original tables: %d quotes, %d craft/promote and %d upgrades; all payment rollback; chance failure consumes ingredients; money/room/generation/realm guards\n",quotes,craft,upgrades);g_pPakList=0;return 0;
}
