#include "KCore.h"
#include "KPakList.h"
#include "PhongThanConsumableData.h"
#include "LegacyDropNativeCatalog.h"
#include "LegacyDropNativeLoader.h"
#include "NativeDropCompatibility.h"
#include "BlueMagicTestPolicy.h"
#include <malloc.h>
KPakList* g_pPakList=0;
using namespace LegacyDrop;using namespace LegacyDrop::Execution;
static int checks=0,failures=0;
void Check(bool ok,const char* s){++checks;if(!ok){++failures;printf("FAIL %s\n",s);}}
std::string Read(const char* p){FILE* f=fopen(p,"rb");if(!f)return "";std::string s;char b[8192];size_t n;while((n=fread(b,1,sizeof(b),f)))s.append(b,n);fclose(f);return s;}
class Tiny:public Catalog{public:bool Has(int g,int d,int p,int l)const{return g==0&&d==0&&p==0&&l==1;}};
void PolicyTests(){
 Check(RuntimeProfile("NPC_STATE_EXPERIMENTAL",0,"A","B")==PROFILE_A,"normal A");
 Check(RuntimeProfile("NPC_STATE_EXPERIMENTAL",1,"A","B")==PROFILE_B,"blue B");
 Check(RuntimeProfile("NPC_STATE_EXPERIMENTAL",2,"A","B")==PROFILE_UNBOUND,"gold state no guess");
 Check(RuntimeProfile("B",0,"A","B")==PROFILE_B,"explicit profile preserved");
 Check(ExperimentalMagicChance(100000,0)==0,"zero magic");Check(ExperimentalMagicChance(100000,1)==100000,"rate one");Check(ExperimentalMagicChance(100000,20)==1000000,"bounded positive magic");Check(ExperimentalMagicChance(100000,-1)==-1,"invalid magic");
 FILE* f=fopen("completion_quarantine.tmp","w+b");if(!f){Check(false,"test quarantine file");return;}fputs("V1\n",f);rewind(f);Quarantine q;std::string e;Check(q.Load(f,e),"lab quarantine");fclose(f);
 Table t=Parse("[Main]\nCount=2\nRandRange=100\nMagicRate=1\nMoneyRate=100\nMoneyScale=7\nMinItemLevel=1\nMaxItemLevel=1\nMinItemLevelScale=0\nMaxItemLevelScale=0\n[1]\nGenre=0\nDetail=0\nParticular=0\nRandRate=100\n[2]\nGenre=0\nDetail=0\nParticular=0\nRandRate=100\n");
 NpcSnapshot n;n.templateId=18;n.treasure=3;n.treasure1=0;for(int k=0;k<4;k++)n.slots[k]="lab.ini";
 Config c;c.profile=PROFILE_A;c.treasure=SUCCESS_CAP;c.level=EXPLICIT_OR_UNIFORM_ZERO_SCALE;c.magic=EXPERIMENTAL_NATIVE_CATALOG_ONLY;c.money=SYNTHETIC_FLAT_BERNOULLI;Tiny cat;Plan plan;Result result;Random rng(99);
 Check(plan.Prepare(n,q,c,t,t,cat,e)&&plan.Draw(rng,result,e)&&result.items.size()==2&&result.money==14,"sum>range independent rows with money");
 c.maxMoney=13;Check(!plan.Prepare(n,q,c,t,t,cat,e)&&e=="MONEY_NATIVE_BUDGET_EXCEEDED","currency native budget before RNG");c.maxMoney=2147483647;
 c.treasure=REPEATED_ROW_TRIALS;Check(plan.Prepare(n,q,c,t,t,cat,e)&&plan.Draw(rng,result,e)&&result.items.size()==6,"repeated Treasure");
 n.treasure=0;Check(plan.Prepare(n,q,c,t,t,cat,e)&&plan.Draw(rng,result,e)&&result.items.empty(),"zero Treasure does not suppress money trial");
}
void BlueTests(){
 BlueMagic::BlueMagicTableLoader table;table.valid=true;
 for(int i=0;i<3;i++){BlueMagic::Row r;r.reason="";r.sourceRow=i+2;r.type=85+i;r.level=1;r.element="";for(int g=0;g<3;g++)for(int d=0;d<11;d++)r.weights[g][d]=1;for(int a=0;a<3;a++)r.minimum[a]=r.maximum[a]=i+1;table.rows.push_back(r);}
 BlueMagic::BlueLimits limits;limits.valid=true;limits.version=0;for(int a=0;a<4;a++)for(int t=0;t<10;t++)limits.caps[a][t]=3;
 BlueMagic::Config c;c.mode=BlueMagic::Config::LIVE_TEST;c.npc=86;c.allowBoss86=true;c.version=0;c.group=0;c.chance=1000000;c.lines=3;c.uniqueType=c.blankElementUnrestricted=c.attributeLevelIsTier=true;
 BlueMagic::BlueMagicTestPolicy plan;std::string e;Check(plan.PrepareWorld(table,limits,c,86,6,2,0,e),"reuse world blue for boss allowlist");LegacyDrop::Random rng(42);BlueMagic::Result result;Check(plan.Draw(rng,result,e)&&result.blue&&result.attributes.size()==3&&result.attributes[0].type!=result.attributes[1].type&&result.attributes[0].type!=result.attributes[2].type&&result.attributes[1].type!=result.attributes[2].type,"three distinct native magic lines");
 c.chance=0;Check(plan.PrepareWorld(table,limits,c,86,6,2,0,e)&&plan.Draw(rng,result,e)&&!result.blue,"zero magic leaves white");c.chance=100000;Check(plan.PrepareWorld(table,limits,c,86,6,2,0,e)&&plan.Draw(rng,result,e)&&(result.blue==(result.chanceDraw<100000)),"experimental ten percent exact threshold");
 c.lines=4;Check(!plan.PrepareWorld(table,limits,c,86,6,2,0,e),"excess native line cap rejected");c.lines=3;Check(!plan.PrepareWorld(table,limits,c,86,11,2,0,e),"special equipment no invented blue mapping");
}
int main(int argc,char** argv){setvbuf(stdout,0,_IONBF,0);printf("START argc=%d\n",argc);if(argc!=6)return 2;PolicyTests();printf("POLICY_DONE\n");BlueTests();printf("BLUE_DONE\n");g_SetRootPath(argv[1]);g_SetFilePath("\\");KPakList packs;Check(packs.Open("\\package.ini")!=0,"native isolated fixture package");g_pPakList=&packs;g_SetPakFileMode(1);Check(ItemGen.Init()!=0,"native generator Init");ItemSet.Init();LegacyDropItemAdapter adapter;Check(adapter.Init(ItemGen),"native adapter Init");LegacyDropNativeCatalog cat(adapter,ItemGen);std::string e;
 int parsed=0,rejected=0;WIN32_FIND_DATAA fd;std::string dir=std::string(argv[1])+"\\settings\\item\\droprate\\",pattern=dir+"*.ini";HANDLE h=FindFirstFileA(pattern.c_str(),&fd);FILE* out=fopen(argv[2],"w");if(!out)return 3;fprintf(out,"file,count,status,reason\n");if(h!=INVALID_HANDLE_VALUE){do{Table t=Parse(Read((dir+fd.cFileName).c_str()));++parsed;if(!t.errors.empty())++rejected;fprintf(out,"%s,%d,%s,%s\n",fd.cFileName,t.count,t.errors.empty()?"VALID":"DATA_BLOCKED",t.errors.empty()?"":t.errors[0].c_str());}while(FindNextFileA(h,&fd));FindClose(h);}fclose(out);Check(parsed==478,"all 478 original INI read");
 int equip=0,gold=0,special=0;for(int d=0;d<equip_detailnum;d++){bool basicDone=false,goldDone=false;int count=ItemGen.CatalogEquipmentCount(d);for(int row=0;row<count;row++){const KBASICPROP_EQUIPMENT* b=ItemGen.CatalogEquipment(d,row);if(!b)continue;bool set=b->m_nSetId>0;if(set?goldDone:basicDone)continue;PhongThanQuestItemTuple tuple;bool mapped=adapter.Map(0,d,b->m_nParticularType,tuple);tuple.level=b->m_nLevel;tuple.series=b->m_nSeries;int item=0;bool ok=mapped&&adapter.GenerateMapped(tuple,item,e);Check(ok,"real equipment native generation");if(ok){++equip;if(d>=11)++special;if(set){++gold;Check(Item[item].GetSetID()==b->m_nSetId&&Item[item].GetQuality()==equip_set,"original gold SetID and quality");}ItemSet.Remove(item);}if(set)goldDone=true;else basicDone=true;}}
 printf("EQUIPMENT sampled=%d gold=%d special=%d\n",equip,gold,special);
 FILE* f=fopen(argv[3],"rb");int g,detailCase,p,l,generated=0;while(f&&fscanf(f,"%d %d %d %d",&g,&detailCase,&p,&l)==4){PhongThanQuestItemTuple t;int item=0;bool ok=adapter.Map(g,detailCase,p,t);t.level=l;ok=ok&&cat.Has(g,detailCase,p,l)&&adapter.GenerateMapped(t,item,e);Check(ok,"real non-equipment tuple");printf("CATEGORY genre=%d detail=%d particular=%d level=%d result=%s\n",g,detailCase,p,l,ok?"CREATED":e.c_str());if(item){++generated;ItemSet.Remove(item);}}if(f)fclose(f);
 PhongThanQuestItemTuple missing;int idx=0;bool mapped=adapter.Map(6,1,2020,missing);missing.level=1;Check(!cat.Has(6,1,2020,1)&&(!mapped||!adapter.GenerateMapped(missing,idx,e)),"2020 remains genuinely missing");
 f=fopen(argv[4],"rb");Quarantine q;Check(q.Load(f,e),"current unchanged quarantine");if(f)fclose(f);
 FILE* cases=fopen(argv[5],"rb");int id,npcLevel,tr,tr1;char slots[4][512];int opened=0,blocked=0;LegacyDropNativeLoader loader;
 while(cases&&fscanf(cases,"%d %d %d %d %511s %511s %511s %511s",&id,&npcLevel,&tr,&tr1,slots[0],slots[1],slots[2],slots[3])==8){
 NpcSnapshot npc;npc.templateId=id;npc.treasure=tr;npc.treasure1=tr1;for(int k=0;k<4;k++)npc.slots[k]=slots[k];
 for(int profile=0;profile<2;profile++){Config config;config.profile=(Profile)profile;config.treasure=SUCCESS_CAP;config.level=EXPERIMENTAL_MONSTER_EQUIPMENT_CATALOG;config.money=SYNTHETIC_FLAT_BERNOULLI;config.magic=EXPERIMENTAL_NATIVE_CATALOG_ONLY;config.npcLevel=npcLevel;config.maxItems=64;Plan plan;bool ok=Build(npc,q,config,loader,cat,plan,e);if(ok)++opened;else ++blocked;printf("NPC_PLAN id=%d level=%d profile=%d result=%s error=%s slot=%d row=%d\n",id,npcLevel,profile,ok?"READY":"DATA_BLOCKED",e.c_str(),plan.RejectedSlot(),plan.RejectedRow());Check(id==18?ok:!ok,"targeted NPC expected readiness or retained quarantine");}}
 if(cases)fclose(cases);Check(opened==2&&blocked==10,"six NPC dual profile preflight");
 Check(ItemSet.GetItemNumber()==0,"no item leak");Check(_heapchk()==_HEAPOK,"heap intact");
 printf("COMPLETION_RESULT files=%d parser_rejected=%d generated_non_equipment=%d checks=%d failures=%d npc_spawn_death=0 pickups=0\n",parsed,rejected,generated,checks,failures);g_pPakList=0;return failures?1:0;
}
