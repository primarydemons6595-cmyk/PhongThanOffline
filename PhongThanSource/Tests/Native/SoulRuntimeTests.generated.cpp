
#include <windows.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <limits.h>
#define _SERVER
#include "../../Headers/PhongThanSoulState.h"
#include "../../Headers/PhongThanCompose.h"
enum {item_equip=0,pos_builditem=7,SAVE_REQUEST=1};
struct KItem {int genre,detail,part,level,stack,serial,param,marker;
 int GetGenre()const{return genre;}int GetDetailType()const{return detail;}int GetParticularType()const{return part;}int GetSeries()const{return 0;}int GetLevel()const{return level;}int GetStackNum()const{return stack;}
 int GetParam()const{return param;}void SetParam(int v){param=v;}int GetWidth(){return 1;}int GetHeight(){return 1;}void SetStackNum(int n){stack=n;}
}Item[32];
struct ItemPos{int nPlace,nX,nY;};
struct Bag{bool owned[32],room,pay;int money,failedRemove,failedAdd;
 int GetEquipmentMoney(){return money;}
 bool SearchPosition(int,int,ItemPos* p,bool){p->nPlace=1;p->nX=p->nY=0;return room;}
 bool Remove(int id){if(!owned[id]||id==failedRemove)return false;owned[id]=false;return true;}
 int Add(int id,int place,int,int,bool){if(place!=pos_builditem&&id==failedAdd)return 0;owned[id]=true;return id;}
};
struct TestPlayer {Bag m_ItemList;int m_uMustSave;bool Pay(int amount){if(!m_ItemList.pay||m_ItemList.money<amount)return false;m_ItemList.money-=amount;return true;}}Player[2];
struct TestItems{void Remove(int n){Item[n].serial=0;}}ItemSet;
void g_DebugLog(const char*,int,int){}
int g_Random(int){return 0;}
#include "../../Sources/Core/Src/PhongThanSocketRegistry.inl"
#include "../../Sources/Core/Src/PhongThanSoulRegistry.inl"
#undef _SERVER
// Original tuple: quantity.genre.detail.particular.series.level.
// Lists, ranges and *<exclusions> retain their authored meaning.
inline bool PtSocketNumber(const char*& p,int& n){n=0;while(*p==' '||*p=='\t')++p;if(*p<'0'||*p>'9')return false;do{int d=*p++-'0';if(n>(2147483647-d)/10)return false;n=n*10+d;}while(*p>='0'&&*p<='9');while(*p==' '||*p=='\t')++p;return true;}
inline bool PtSocketSelector(const char* text,int value){
 if(!strcmp(text,"*"))return true;bool negate=text[0]=='*'&&text[1]=='<';const char* p=text+(negate?2:0);char end=negate?'>':*p=='['?']':0;if(!negate&&end)++p;
 bool found=false;do{int lo=0,hi=0;if(!PtSocketNumber(p,lo))return false;hi=lo;if(*p=='-'){++p;if(!PtSocketNumber(p,hi)||hi<lo)return false;}if(value>=lo&&value<=hi)found=true;if(!end)return !*p&&found;if(*p==end)return !p[1]&&(negate?!found:found);if(*p++!=',')return false;}while(*p);return false;
}
inline bool PtSocketTuple(const char* text,KItem& item){
 if(!strcmp(text,"*"))return true;int values[6]={item.GetStackNum(),item.GetGenre(),item.GetDetailType(),item.GetParticularType(),item.GetSeries(),item.GetLevel()};const char* p=text;
 for(int i=0;i<6;++i){char part[128];int n=0;while(*p&&*p!='.'){if(n>=127)return false;part[n++]=*p++;}part[n]=0;if(!PtSocketSelector(part,values[i]))return false;if(!*p)return i>=2;++p;}return !*p;
}

#include "../../Sources/Core/Src/PhongThanSoulCompose.inl"
static void Reset(){ZeroMemory(Item,sizeof(Item));ZeroMemory(Player,sizeof(Player));Player[1].m_ItemList.money=100000000;Player[1].m_ItemList.room=Player[1].m_ItemList.pay=true;}
static void Seed(int i,int genre,int detail,int part,int level,int count){Item[i].genre=genre;Item[i].detail=detail;Item[i].part=part;Item[i].level=level;Item[i].stack=count;Item[i].serial=100+i;Player[1].m_ItemList.owned[i]=true;}
static void Inputs(const PTSoulRecipe& recipe,int* indexes,int* selected){
 Reset();ZeroMemory(indexes,9*sizeof(int));ZeroMemory(selected,9*sizeof(int));int detail=strstr(recipe.equipment,"[0]")?0:strstr(recipe.equipment,"[10]")?10:2;
 Seed(1,0,detail,detail?0:9,1,1);Item[1].param=0xc17f0012;Item[1].marker=777;indexes[0]=1;selected[0]=1;
 for(int m=0;m<8;++m)if(recipe.materials[m][0]){int quantity=0,genre=0,id=0;assert(sscanf(recipe.materials[m],"%d.%d.%d",&quantity,&genre,&id)==3);Seed(m+2,genre,id,0,0,quantity+3);indexes[m+1]=m+2;selected[m+1]=quantity+3;}
}
int main(){
 CreateDirectoryA("settings",0);CreateDirectoryA("settings\\phongthan",0);
 assert(sizeof(PT_SOUL_STATE)==20);int indexes[9],selected[9],recipe=0,money=0;int done=0;
 for(int row=0;row<sizeof(ptSoulRecipes)/sizeof(ptSoulRecipes[0]);++row){
  Inputs(ptSoulRecipes[row],indexes,selected);PtSoulRules::Plan plan;
  assert(PtSoulRules::Quote(indexes,selected,false,plan,8)==PT_COMPOSE_OK);assert(plan.Money==ptSoulRecipes[row].money);
  assert(PtSoulCompose(1,indexes,selected,8,recipe,money,true)==PT_COMPOSE_OK);
  const PTSoulFormula* f=PtSoulFormula(plan.Formula);assert(f&&Item[2].stack==4);
  assert(PtSoulCompose(1,indexes,selected,8,recipe,money,false)==PT_COMPOSE_OK);
  PT_SOUL_STATE soul;assert(PtSoulFromParam(Item[1].param,soul)&&soul.Formula==f->id&&soul.Aura==f->capacity&&soul.BaseParam==(int)0xc17f0012);
  assert(Item[1].serial==101&&Item[1].marker==777&&Player[1].m_ItemList.money==100000000-money&&Item[2].stack==3);
  ++done;
 }
 Inputs(ptSoulRecipes[0],indexes,selected);assert(PtSoulCompose(1,indexes,selected,8,recipe,money,true)==PT_COMPOSE_OK);Player[1].m_ItemList.money=0;
 assert(PtSoulCompose(1,indexes,selected,8,recipe,money,false)==PT_COMPOSE_NO_MONEY&&Item[1].param==(int)0xc17f0012&&Item[2].stack==4);
 Inputs(ptSoulRecipes[0],indexes,selected);assert(PtSoulCompose(1,indexes,selected,8,recipe,money,true)==PT_COMPOSE_OK);Player[1].m_ItemList.room=false;assert(PtSoulCompose(1,indexes,selected,8,recipe,money,false)==PT_COMPOSE_NO_ROOM&&Item[2].stack==4);
 for(int fail=1;fail<=4;++fail){Inputs(ptSoulRecipes[0],indexes,selected);assert(PtSoulCompose(1,indexes,selected,8,recipe,money,true)==PT_COMPOSE_OK);Player[1].m_ItemList.failedRemove=fail;assert(PtSoulCompose(1,indexes,selected,8,recipe,money,false)!=PT_COMPOSE_OK);for(int i=1;i<=4;++i)assert(Player[1].m_ItemList.owned[i]&&Item[i].serial==100+i);assert(Item[1].param==(int)0xc17f0012&&Item[2].stack==4&&Player[1].m_ItemList.money==100000000);}
 Inputs(ptSoulRecipes[0],indexes,selected);assert(PtSoulCompose(1,indexes,selected,8,recipe,money,true)==PT_COMPOSE_OK);Player[1].m_ItemList.pay=false;assert(PtSoulCompose(1,indexes,selected,8,recipe,money,false)==PT_COMPOSE_NO_MONEY);for(int i=1;i<=4;++i)assert(Player[1].m_ItemList.owned[i]&&Item[i].serial==100+i);assert(Item[1].param==(int)0xc17f0012&&Item[2].stack==4);
 Inputs(ptSoulRecipes[0],indexes,selected);assert(PtSoulCompose(1,indexes,selected,8,recipe,money,true)==PT_COMPOSE_OK);assert(PtSoulCompose(1,indexes,selected,8,recipe,money,false)==PT_COMPOSE_OK);
 PT_SOUL_STATE soul;assert(PtSoulFromParam(Item[1].param,soul));int full=Item[1].param;soul.Aura=1;int depleted;assert(PtSoulStore(soul,depleted));Item[1].param=depleted;
 PtSoulCache().clear();assert(PtSoulRead(depleted,soul)&&soul.Aura==1);PT_SOUL_STATE clash=soul;clash.Aura=2;assert(!PtSoulRegister(depleted,clash));clash=soul;clash.Aura=-1;assert(!PtSoulValid(clash));
 ZeroMemory(indexes,sizeof(indexes));ZeroMemory(selected,sizeof(selected));Seed(2,6,1,587,1,2);indexes[0]=1;indexes[1]=2;selected[0]=1;selected[1]=2;
 assert(PtSoulCompose(1,indexes,selected,9,recipe,money,true)==PT_COMPOSE_OK&&selected[1]==1);assert(PtSoulCompose(1,indexes,selected,9,recipe,money,false)==PT_COMPOSE_OK&&Item[2].stack==1);assert(PtSoulFromParam(Item[1].param,soul)&&soul.Aura==201);
 Item[1].param=full;assert(PtSoulCompose(1,indexes,selected,9,recipe,money,true)==PT_COMPOSE_NO_RECIPE&&Item[2].stack==1);
 PT_SOCKET_STATE socket;ZeroMemory(&socket,sizeof(socket));socket.BaseParam=full;socket.Holes[0]=1;int wrapped;assert(PtSocketStore(socket,wrapped));soul.Aura=300;assert(PtSoulReplace(wrapped,soul,depleted));PT_SOCKET_STATE after;assert(PtSocketRead(depleted,after)&&after.Holes[0]==1&&PtEquipmentBaseParam(depleted)==(int)0xc17f0012);assert(PtSoulFromParam(depleted,soul)&&soul.Aura==300);
 printf("PASS SOUL: %d original recipes, real quote/commit, stacked formulas, costs, money/room/detach rollback, aura refill/full cap, immutable restart, socket/forge preservation\n",done);return 0;
}
