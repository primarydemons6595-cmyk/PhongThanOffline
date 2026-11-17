// Production parser, transaction and immutable store; real original tables.
// Inventory placement/payment/RNG are isolated from live accounts.
#define Item UnallocatedEngineItems
#define main UpgradeStateUnitMain
#include "ItemUpgradeStateTests.cpp"
#undef main
#undef Item
#include "../../Headers/PhongThanCompose.h"
KItem Item[16];
#undef MAX_ITEM
#define MAX_ITEM 16
struct ItemPos {int nPlace,nX,nY;};
struct LeiBag {
 bool owned[16],room,payAllowed;int money,failedRemove;
 int GetEquipmentMoney(){return money;}
 bool SearchPosition(int,int,ItemPos* p,bool){p->nPlace=pos_equiproom;p->nX=p->nY=0;return room;}
 bool Remove(int id){if(id==failedRemove||!owned[id])return false;owned[id]=false;return true;}
 int Add(int id,int place,int,int,bool){if(place!=pos_builditem&&!room)return 0;owned[id]=true;return id;}
};
struct LeiPlayer {LeiBag m_ItemList;bool Pay(int amount){if(!m_ItemList.payAllowed||m_ItemList.money<amount)return false;m_ItemList.money-=amount;return true;}} Player[2];
struct LeiItemSet {void Remove(int id){Item[id].SetID(0);}} ItemSet;
static unsigned draw=0;
int KItem::GetPhongThanQuality() const {return m_aryMagicAttrib[0].nAttribType?1:0;}
static int LeiRandom(int bound){return draw%bound;}
#define g_Random LeiRandom
#include "../../Sources/Core/Src/PhongThanSocketRegistry.inl"
#include "../../Sources/Core/Src/PhongThanSocketCompose.inl"
#include "../../Sources/Core/Src/PhongThanSocketPresentation.inl"
static int checks=0;
#define CHECK(x) do{++checks;if(!(x)){printf("FAIL line=%d expression=%s\n",__LINE__,#x);return 1;}}while(0)
static void Seed(int id,int genre,int detail,int part=0,int level=0,int stack=1){
 ZeroMemory(&Item[id],sizeof(KItem));Item[id].SetID(100+id);Item[id].SetGenre(genre);Item[id].SetDetailType(detail);Item[id].SetParticular(part);Item[id].SetLevel(level);
 Item[id].m_CommonAttrib.nStackNum=stack;Item[id].m_CommonAttrib.nMaxStack=1000;Item[id].m_CommonAttrib.nWidth=Item[id].m_CommonAttrib.nHeight=1;Player[1].m_ItemList.owned[id]=true;
}
static void Setup(int material=88){ZeroMemory(Player,sizeof(Player));Player[1].m_ItemList.room=Player[1].m_ItemList.payAllowed=true;Player[1].m_ItemList.money=100000000;draw=0;Seed(1,0,0,9,1);Seed(2,3,material);}
#ifndef PT_SOCKET_TEST_MAIN
#define PT_SOCKET_TEST_MAIN main
#endif
int PT_SOCKET_TEST_MAIN(int argc,char** argv){
 if(argc!=2||!SetCurrentDirectoryA(argv[1]))return 2;
 g_SetRootPath(NULL);g_SetFilePath("\\");KPakList packs;assert(packs.Open("\\package.ini"));g_pPakList=&packs;g_SetPakFileMode(1);
 CHECK(PtSocketRules::Tables());CHECK(PtSocketSelector("[1,2,4]",4));CHECK(!PtSocketSelector("[1,2,4]",3));CHECK(PtSocketSelector("[21-40]",40));CHECK(!PtSocketSelector("[21-40]",41));CHECK(!PtSocketSelector("*<8,12,71>",8));CHECK(PtSocketSelector("*<8,12,71>",9));CHECK(!PtSocketSelector("[2-1]",1));CHECK(!PtSocketSelector("2147483648",1));
 int indexes[9]={1,2,0},selected[9]={1,1,0},recipe=0,money=0;
 Setup();CHECK(PtSocketCompose(1,indexes,selected,recipe,money,true)==PT_COMPOSE_OK&&recipe==1&&money==200000);CHECK(Item[1].GetParam()==0&&Item[2].GetID()==102&&Player[1].m_ItemList.money==100000000);
 CHECK(PtSocketCompose(1,indexes,selected,recipe,money,false)==PT_COMPOSE_OK);PT_SOCKET_STATE state;CHECK(PtSocketRead(Item[1].GetParam(),state)&&state.Holes[0]==1&&!state.Gems[0]);CHECK(Item[1].GetID()==101&&!Item[2].GetID()&&Player[1].m_ItemList.money==99800000);
 int firstParam=Item[1].GetParam();PtSocketCache().clear();CHECK(PtSocketRead(firstParam,state)&&state.Holes[0]==1);PT_SOCKET_STATE reread=state;int stable=0;CHECK(PtSocketStore(state,stable)&&stable==firstParam);CHECK(PtSocketCache().size()==1);
 PT_SOCKET_STATE altered=state;altered.Holes[0]=2;CHECK(!PtSocketRegister(firstParam,altered));
 PHONGTHAN_CHARACTER_ITEM_RECORD canonical;ZeroMemory(&canonical,sizeof(canonical));canonical.ScriptParam=firstParam;PHONGTHAN_CHARACTER_ITEM_RECORD saved=canonical;CHECK(PtSocketRead(saved.ScriptParam,reread)&&!memcmp(&state,&reread,sizeof(state)));
 Setup();Player[1].m_ItemList.money=199999;CHECK(PtSocketCompose(1,indexes,selected,recipe,money,false)==PT_COMPOSE_NO_MONEY&&Item[2].GetID()==102&&Item[1].GetParam()==0);
 Setup();Player[1].m_ItemList.room=false;CHECK(PtSocketCompose(1,indexes,selected,recipe,money,false)==PT_COMPOSE_NO_ROOM&&Item[2].GetID()==102);
 Setup();Player[1].m_ItemList.payAllowed=false;CHECK(PtSocketCompose(1,indexes,selected,recipe,money,false)==PT_COMPOSE_NO_MONEY);CHECK(Item[1].GetParam()==0&&Item[2].GetID()==102&&Player[1].m_ItemList.owned[1]&&Player[1].m_ItemList.owned[2]);
 Setup();Player[1].m_ItemList.failedRemove=2;CHECK(PtSocketCompose(1,indexes,selected,recipe,money,false)==PT_COMPOSE_TABLE_ERROR&&Item[1].GetParam()==0&&Player[1].m_ItemList.owned[1]);
 Setup();Item[2].SetStackNum(20);selected[1]=20;CHECK(PtSocketCompose(1,indexes,selected,recipe,money,true)==PT_COMPOSE_OK&&selected[1]==1);CHECK(PtSocketCompose(1,indexes,selected,recipe,money,false)==PT_COMPOSE_OK&&Item[2].GetStackNum()==19&&Item[2].GetID()==102);
 Setup();Item[2].SetStackNum(20);selected[1]=1;Player[1].m_ItemList.payAllowed=false;CHECK(PtSocketCompose(1,indexes,selected,recipe,money,false)==PT_COMPOSE_NO_MONEY&&Item[2].GetStackNum()==20);
 Setup();Item[1].SetParticular(8);CHECK(PtSocketCompose(1,indexes,selected,recipe,money,true)==PT_COMPOSE_NO_RECIPE);
 Setup();Item[1].SetLevel(21);CHECK(PtSocketCompose(1,indexes,selected,recipe,money,true)==PT_COMPOSE_OK&&money==400000);
 Setup(89);Item[1].m_CommonAttrib.nParam=firstParam;draw=9999;CHECK(PtSocketCompose(1,indexes,selected,recipe,money,false)==PT_COMPOSE_SOCKET_FAILED&&Item[1].GetParam()==firstParam&&!Item[2].GetID());
 Setup(89);Item[1].m_CommonAttrib.nParam=firstParam;CHECK(PtSocketCompose(1,indexes,selected,recipe,money,false)==PT_COMPOSE_OK);CHECK(PtSocketRead(Item[1].GetParam(),state)&&state.Holes[1]==2);int twoParam=Item[1].GetParam();
 Setup(248);Item[1].m_CommonAttrib.nParam=firstParam;Seed(3,3,281);indexes[2]=3;selected[2]=1;
 CHECK(PtSocketCompose(1,indexes,selected,recipe,money,true)==PT_COMPOSE_OK&&money==1000000&&recipe==10001);CHECK(PtSocketCompose(1,indexes,selected,recipe,money,false)==PT_COMPOSE_OK);CHECK(PtSocketRead(Item[1].GetParam(),state)&&state.Gems[0]==1&&state.Attributes[0].Type==121&&state.Attributes[0].Value[0]==1&&!Item[2].GetID()&&!Item[3].GetID());
 Setup(249);Item[1].m_CommonAttrib.nParam=firstParam;Seed(3,3,281);CHECK(PtSocketCompose(1,indexes,selected,recipe,money,true)==PT_COMPOSE_NO_RECIPE);
 Setup(248);Seed(3,3,281);CHECK(PtSocketCompose(1,indexes,selected,recipe,money,true)==PT_COMPOSE_NO_RECIPE);
 // Every original gem row resolves all authored value endpoints, without rerolls on store/load.
 for(int gem=1;gem<=53;++gem){PtSocketRules::Plan p;ZeroMemory(&p,sizeof(p));p.Position=0;p.Gem=gem;p.Before.Holes[0]=1;
  for(int roll=0;roll<100;++roll){PT_SOCKET_STATE a;CHECK(PtSocketRules::Resolve(p,0,roll,1000000,a));CHECK(a.Gems[0]==gem&&a.Attributes[0].Type>0&&a.Attributes[0].Value[0]>=0);}}
 // Exclusive groups reject another grade of the same original symbol.
 Setup(249);ZeroMemory(&state,sizeof(state));state.Holes[0]=1;state.Holes[1]=2;state.Gems[0]=22;state.Attributes[0].Type=230;state.Attributes[0].Value[0]=1;int groupParam=0;CHECK(PtSocketStore(state,groupParam));Item[1].m_CommonAttrib.nParam=groupParam;Seed(3,3,384);CHECK(PtSocketCompose(1,indexes,selected,recipe,money,true)==PT_COMPOSE_NO_RECIPE);
 CHECK(PtSocketSelector("[45-50, 201-203,219-248]",202));CHECK(!PtSocketSelector("[45-50, 201-203,219-248]",204));
 // Limits vary with equipment identity and quality, not one universal cap.
 PT_SOCKET_STATE base;ZeroMemory(&base,sizeof(base));base.Holes[0]=1;base.Holes[1]=2;int two=0;CHECK(PtSocketStore(base,two));
 Setup(90);indexes[2]=0;selected[2]=0;Item[1].m_CommonAttrib.nParam=two;Item[1].SetDetailType(7);Item[1].SetParticular(0);CHECK(PtSocketCompose(1,indexes,selected,recipe,money,true)==PT_COMPOSE_NO_RECIPE);
 Item[1].SetParticular(45);Item[1].SetLevel(100);CHECK(PtSocketCompose(1,indexes,selected,recipe,money,true)==PT_COMPOSE_OK&&recipe==346);
 Setup(90);Item[1].m_CommonAttrib.nParam=two;Item[1].SetDetailType(2);Item[1].SetParticular(0);CHECK(PtSocketCompose(1,indexes,selected,recipe,money,true)==PT_COMPOSE_NO_RECIPE);
 Item[1].m_aryMagicAttrib[0].nAttribType=115;CHECK(PtSocketCompose(1,indexes,selected,recipe,money,true)==PT_COMPOSE_OK);
 base.Holes[2]=3;int three=0;CHECK(PtSocketStore(base,three));Setup(252);Item[1].m_CommonAttrib.nParam=three;Item[1].SetDetailType(2);Item[1].SetParticular(0);CHECK(PtSocketCompose(1,indexes,selected,recipe,money,true)==PT_COMPOSE_NO_RECIPE);
 Item[1].SetParticular(45);Item[1].SetLevel(100);CHECK(PtSocketCompose(1,indexes,selected,recipe,money,true)==PT_COMPOSE_OK&&recipe==345);
 // Same-color and high-quality holes preserve the original roll, add only the bonus.
 PtSocketRules::Plan decorated;ZeroMemory(&decorated,sizeof(decorated));decorated.Gem=42;decorated.Before.Holes[0]=85;PT_SOCKET_STATE decoratedState;
 CHECK(PtSocketRules::Resolve(decorated,0,0,4,decoratedState)&&decoratedState.Bonus[0]==4&&decoratedState.Attributes[0].Value[0]==19);
 CHECK(PtSocketRules::DecorationBonus(17,42,234)==2);CHECK(PtSocketRules::DecorationBonus(5,42,234)==0);
 decorated.Gem=0;decorated.Count=1;decorated.HoleId[0]=37;decorated.Threshold[0]=10000;decorated.Before=decoratedState;
 CHECK(PtSocketRules::Resolve(decorated,0,99,999,decoratedState)&&decoratedState.Gems[0]==42&&decoratedState.Bonus[0]==2&&decoratedState.Attributes[0].Value[0]==17);
 int decoratedParam=0;CHECK(PtSocketStore(decoratedState,decoratedParam));Setup(248);Item[1].m_CommonAttrib.nParam=decoratedParam;Seed(3,3,281);indexes[2]=3;selected[2]=1;
 CHECK(PtSocketCompose(1,indexes,selected,recipe,money,true)==PT_COMPOSE_NO_RECIPE);CHECK(PtSocketCompose(1,indexes,selected,recipe,money,true,1)==PT_COMPOSE_OK); // explicit replacement in the first socket
 Seed(2,3,248);ZeroMemory(&base,sizeof(base));base.Holes[0]=1;base.Gems[0]=1;base.Attributes[0].Type=121;base.Attributes[0].Value[0]=5;CHECK(PtSocketStore(base,decoratedParam));Item[1].m_CommonAttrib.nParam=decoratedParam;
 CHECK(PtSocketCompose(1,indexes,selected,recipe,money,true,1)==PT_COMPOSE_OK);CHECK(PtSocketCompose(1,indexes,selected,recipe,money,false,1)==PT_COMPOSE_OK);
 CHECK(PtSocketRead(Item[1].GetParam(),state)&&state.Gems[0]==1&&state.Attributes[0].Value[0]==1); // new gem rerolls, decoration does not
 PT_SOCKET_VISUALS visuals;CHECK(PtSocketVisuals(Item[1],visuals)&&visuals.Count==1&&visuals.X[0]==16&&visuals.Y[0]==28&&visuals.Image[0][0]=='\\'&&visuals.MenuImage[0][0]=='\\');CHECK(PtSocketPower(Item[1].GetParam())==20);
 Setup(1149);indexes[2]=0;selected[2]=0;Item[1].m_CommonAttrib.nParam=firstParam;Item[1].SetDetailType(2);Item[1].SetParticular(0);Item[1].SetLevel(201);
 PtSocketRules::Plan repaired;int repairStatus=PtSocketRules::Quote(indexes,selected,false,repaired);printf("repair status=%d recipe=%d threshold=%d selected=%d,%d\n",repairStatus,repaired.Recipe,repaired.Threshold[2],selected[0],selected[1]);CHECK(repairStatus==PT_COMPOSE_OK&&repaired.Recipe==68&&repaired.Threshold[2]==9900);
 CHECK(PtSocketRules::Resolve(repaired,9800,0,0,state)&&state.Holes[0]==13);CHECK(PtSocketRules::Resolve(repaired,9900,0,0,state)&&state.Holes[0]==17);
 // Reject replay after successful materials have been consumed.
 Setup();indexes[2]=0;selected[2]=0;CHECK(PtSocketCompose(1,indexes,selected,recipe,money,false)==PT_COMPOSE_OK);CHECK(PtSocketCompose(1,indexes,selected,recipe,money,false)!=PT_COMPOSE_OK);
 printf("PASS LEI_SOCKET checks=%d original_tables parser preview fees drill success_failure inlay all53gem_rolls group identity serial immutable_restart partial_stacks payment_detach_room_rollback replay\n",checks);
 g_pPakList=NULL;return 0;
}
