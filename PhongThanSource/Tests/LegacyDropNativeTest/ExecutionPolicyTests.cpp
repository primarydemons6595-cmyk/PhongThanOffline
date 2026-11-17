#include "KCore.h"
#include "KPakList.h"
#include "KItemGenerator.h"
#include "KItemSet.h"
#include "PhongThanConsumableData.h"
#include "LegacyDropExecutionPolicy.h"
#include "LegacyDropItemAdapter.h"
#include "LegacyDropNativeLoader.h"
#include <malloc.h>
KPakList* g_pPakList=0;
using namespace LegacyDrop;
using namespace LegacyDrop::Execution;
static int checks=0,failed=0;
static void Check(bool ok,const char* name){++checks;if(!ok){++failed;printf("FAIL %s\n",name);}}
class TestCatalog:public Catalog {
public:bool Has(int g,int d,int p,int l) const{return g==0&&d==0&&p==0&&l>=1&&l<=10;}
};
class NativeEquipmentCatalog:public Catalog {
public:bool Has(int g,int d,int p,int l) const {
    if(g!=item_equip||d<0||d>=11||l<0)return false;
    for(int row=0;row<ItemGen.CatalogEquipmentCount(d);++row){
        const KBASICPROP_EQUIPMENT* equ=ItemGen.CatalogEquipment(d,row);
        if(equ&&equ->m_nParticularType==p&&equ->m_nLevel==l)return true;
    }return false;
}};
static Table Synthetic(const char* tail="") {
    std::string s="[Main]\nCount=2\nRandRange=100\nMagicRate=0\nMoneyRate=0\nMoneyScale=0\nMinItemLevel=1\nMaxItemLevel=1\nMinItemLevelScale=0\nMaxItemLevelScale=0\n[1]\nGenre=0\nDetail=0\nParticular=0\nRandRate=100\n[2]\nGenre=0\nDetail=0\nParticular=0\nRandRate=100\n";
    s+=tail;return Parse(s);
}
static void Set(Table& t,const char* section,const char* key,const char* value){
    for(size_t i=0;i<t.sections.size();++i)if(t.sections[i].name==section)t.sections[i].fields[key]=value;
}
static void Laboratory(const Quarantine& q) {
    NpcSnapshot npc;npc.templateId=2704;npc.entity=1;npc.treasure=3;npc.treasure1=0;
    for(int s=0;s<4;++s)npc.slots[s]="synthetic.ini";
    TestCatalog cat;Config c;Plan p;std::string e;Table t=Synthetic();Random rng(123),control(123);Result result;
    Check(!p.Prepare(npc,q,c,t,t,cat,e)&&e=="PROFILE_UNBOUND","default profile refuses inference");
    Check(!p.Draw(rng,result,e)&&result.items.empty()&&rng.Next()==control.Next(),"failed prepare no RNG or intents");
    c.profile=PROFILE_A;Check(!p.Prepare(npc,q,c,t,t,cat,e)&&e=="TREASURE_UNBOUND","Treasure requires declared experiment");
    c.treasure=REPEATED_ROW_TRIALS;c.level=EXPLICIT_OR_UNIFORM_ZERO_SCALE;c.money=SYNTHETIC_FLAT_BERNOULLI;
    Check(p.Prepare(npc,q,c,t,t,cat,e),"synthetic repeated prepare");
    Check(p.Draw(rng,result,e)&&result.items.size()==6,"three trials two guaranteed rows gives six");
    c.treasure=SUCCESS_CAP;Check(p.Prepare(npc,q,c,t,t,cat,e)&&p.Draw(rng,result,e)&&result.items.size()==2,"cap hypothesis gives two not six");
    npc.treasure=1;Check(p.Prepare(npc,q,c,t,t,cat,e)&&p.Draw(rng,result,e)&&result.items.size()==1,"cap boundary one");
    npc.treasure=0;Check(p.Prepare(npc,q,c,t,t,cat,e)&&p.Draw(rng,result,e)&&result.items.empty(),"zero Treasure both hypotheses zero");
    npc.treasure=1;c.profile=PROFILE_B;Check(p.Prepare(npc,q,c,t,t,cat,e)&&p.Draw(rng,result,e)&&result.items[0].slot==2,"explicit B slot 2 no filename heuristic");
    npc.slots[2]="";Check(!p.Prepare(npc,q,c,t,t,cat,e)&&e=="REQUIRED_PROFILE_SLOT_MISSING","required slot missing refuses whole plan");npc.slots[2]="synthetic.ini";
    Table bad=t;Set(bad,"Main","MagicRate","1");Check(!p.Prepare(npc,q,c,bad,t,cat,e)&&e=="MAGIC_POLICY_UNVERIFIED","original magic one not silently ignored");
    Set(bad,"Main","MagicRate","100");Check(!p.Prepare(npc,q,c,bad,t,cat,e)&&e=="MAGIC_POLICY_UNVERIFIED","other positive magic refuses");
    bad=t;Set(bad,"Main","MinItemLevelScale","20");Check(!p.Prepare(npc,q,c,bad,t,cat,e)&&e=="SCALE_POLICY_UNVERIFIED","positive scale refuses");
    bad=t;Set(bad,"Main","MinItemLevel","2");Set(bad,"Main","MaxItemLevel","1");Check(!p.Prepare(npc,q,c,bad,t,cat,e),"inverted levels refuse");
    bad=t;Set(bad,"Main","MaxItemLevel","11");Check(!p.Prepare(npc,q,c,bad,t,cat,e)&&e=="CATALOG_LEVEL_MISSING","no clamp to catalog");
    bad=t;Set(bad,"1","Level","10");Check(p.Prepare(npc,q,c,bad,t,cat,e)&&p.Draw(rng,result,e)&&result.items[0].level==10,"explicit Level overrides zero-scale synthetic bounds");
    Set(bad,"1","Level","2147483647");Check(!p.Prepare(npc,q,c,bad,t,cat,e)&&e=="CATALOG_LEVEL_MISSING","explicit Level INT_MAX safe rejection");
    npc.treasure=-1;Check(!p.Prepare(npc,q,c,t,t,cat,e),"blank converted to unresolved not zero");npc.treasure=257;Check(!p.Prepare(npc,q,c,t,t,cat,e),"trial budget");npc.treasure=2;
    c.treasure=REPEATED_ROW_TRIALS;c.maxItems=1;Check(!p.Prepare(npc,q,c,t,t,cat,e)&&e=="ITEM_BUDGET_EXCEEDED","preflight item budget");c.maxItems=4096;
    bad=t;Set(bad,"Main","MoneyRate","100");Set(bad,"Main","MoneyScale","7");
    Check(p.Prepare(npc,q,c,bad,bad,cat,e)&&p.Draw(rng,result,e)&&result.money==14,"synthetic guaranteed flat currency twice, independent Treasure");
    Set(bad,"Main","MoneyScale","2147483647");Check(!p.Prepare(npc,q,c,bad,bad,cat,e)&&e=="MONEY_OVERFLOW","currency overflow before RNG");
    int amount=99;Check(MoneyAtRoll(0,100,7,0,amount)&&amount==0,"money zero");
    Check(MoneyAtRoll(1,100,7,0,amount)&&amount==7,"money lower boundary");
    Check(MoneyAtRoll(1,100,7,1,amount)&&amount==0,"money exact threshold");
    Check(!MoneyAtRoll(101,100,7,0,amount)&&amount==0,"money invalid rate");
    Check(!MoneyAtRoll(1,100,-1,0,amount),"negative money scale");
    int x,y,z;Check(ProbeScale(ADD_LEVEL,1,20,80,x)&&x==21&&ProbeScale(MULTIPLY_LEVEL,1,20,80,y)&&y==20&&ProbeScale(PERCENT_NPC_LEVEL,1,20,80,z)&&z==17,"three scale hypotheses demonstrably differ");
    Check(!ProbeScale(MULTIPLY_LEVEL,INT_MAX,INT_MAX,80,x),"hypothesis overflow rejected");
    // Uniform synthetic interval: same seed yields identical intents, all levels
    // are preflighted. This proves implementation, not original distribution.
    bad=t;Set(bad,"Main","MaxItemLevel","10");c.treasure=SUCCESS_CAP;npc.treasure=1;
    Check(p.Prepare(npc,q,c,bad,t,cat,e),"uniform zero-scale prepare");
    Random r1(17),r2(17);int bins[11]={0};
    for(int i=0;i<1000;++i){Result a,b;Check(p.Draw(r1,a,e)&&p.Draw(r2,b,e)&&a.items.size()==1&&b.items.size()==1&&a.items[0].level==b.items[0].level,"deterministic uniform replay");if(a.items.size())++bins[a.items[0].level];}
    for(i=1;i<=10;++i)Check(bins[i]>0,"uniform candidate level reachable");
    // Existing fail-closed production gate remains closed, independent of config.
    PreparedTable production;Check(!BuildProductionPlan(production,e)&&e=="PRODUCTION_POLICY_NOT_APPROVED","no production activation");
    Check(_heapchk()==_HEAPOK,"policy heap integrity");
    printf("LAB_RESULT checks=%d failures=%d parity=PARITY_UNVERIFIED\n",checks,failed);
}
int main(int argc,char** argv) {
    if(argc!=6)return 2;
    Quarantine q;std::string e;FILE* f=fopen(argv[2],"rb");Check(q.Load(f,e),"existing quarantine load");if(f)fclose(f);if(failed)return 3;
    Laboratory(q);
    g_SetRootPath(argv[1]);g_SetFilePath("\\");KPakList packs;Check(packs.Open("\\package.ini")!=0,"existing native loose fixture");g_pPakList=&packs;g_SetPakFileMode(1);
    Check(ItemGen.Init()!=0,"native catalog Init");ItemSet.Init();LegacyDropItemAdapter adapter;Check(adapter.Init(ItemGen),"shared native adapter Init");if(failed)return 4;
    // End-to-end laboratory intent -> EXISTING adapter -> native generator.
    // Identity and Level come from an actual catalog row, not a new item ID.
    const KBASICPROP_EQUIPMENT* fixtureEqu=ItemGen.CatalogEquipment(0,0);
    Check(fixtureEqu!=0,"native equipment fixture exists");
    if(fixtureEqu){
        Table nativeTable=Synthetic();char v[32];sprintf(v,"%d",fixtureEqu->m_nParticularType);
        Set(nativeTable,"1","Particular",v);Set(nativeTable,"2","Particular",v);
        sprintf(v,"%d",fixtureEqu->m_nLevel);Set(nativeTable,"Main","MinItemLevel",v);Set(nativeTable,"Main","MaxItemLevel",v);
        NpcSnapshot synthetic;synthetic.templateId=2704;synthetic.entity=1;synthetic.treasure=1;synthetic.treasure1=0;
        for(int ns=0;ns<4;++ns)synthetic.slots[ns]="synthetic.ini";
        Config config;config.profile=PROFILE_A;config.treasure=REPEATED_ROW_TRIALS;config.level=EXPLICIT_OR_UNIFORM_ZERO_SCALE;config.money=SYNTHETIC_FLAT_BERNOULLI;
        NativeEquipmentCatalog catalog;Plan plan;Result intents;Random r(91);
        Check(plan.Prepare(synthetic,q,config,nativeTable,nativeTable,catalog,e)&&plan.Draw(r,intents,e)&&intents.items.size()==2,"real catalog shared execution plan");
        for(size_t ni=0;ni<intents.items.size();++ni){PhongThanQuestItemTuple tuple;int idx=0;
            bool mapped=adapter.Map(0,0,fixtureEqu->m_nParticularType,tuple);tuple.level=intents.items[ni].level;
            Check(mapped&&adapter.GenerateMapped(tuple,idx,e),"intent native generation");if(idx)ItemSet.Remove(idx);
        }
    }
    int explicitRows=0,explicitGenerated=0,magicTests=0,magicEqual=0,scaleFound[3]={0},scaleMissing[3]={0};
    f=fopen(argv[3],"rb");if(!f)return 5;int g,d,part,level;
    while(fscanf(f,"%d %d %d %d",&g,&d,&part,&level)==4){++explicitRows;PhongThanQuestItemTuple tuple;int item=0;
        bool ok=adapter.Map(g,d,part,tuple);Check(ok,"explicit identity mapping");if(!ok)continue;tuple.level=level;
        ok=adapter.GenerateMapped(tuple,item,e);Check(ok,"explicit native generation");if(ok){++explicitGenerated;Check(Item[item].GetLevel()==level,"native exact Level");ItemSet.Remove(item);}
        // Same real tuple under three unselected scale hypotheses at spawn level 80.
        for(int h=0;h<3;++h){int candidate;bool valid=ProbeScale((ScaleHypothesis)h,1,20,80,candidate);
            bool found=false;if(valid)for(int er=0;er<ItemGen.CatalogEquipmentCount(tuple.detail);++er){
                const KBASICPROP_EQUIPMENT* equ=ItemGen.CatalogEquipment(tuple.detail,er);
                if(equ&&equ->m_nParticularType==tuple.particular&&equ->m_nLevel==candidate){found=true;break;}}
            if(found)++scaleFound[h];else ++scaleMissing[h];}
    }fclose(f);
    for(int type=0;type<11&&magicTests<100;++type)for(int row=0;row<ItemGen.CatalogEquipmentCount(type)&&magicTests<100;++row){
        const KBASICPROP_EQUIPMENT* equ=ItemGen.CatalogEquipment(type,row);if(!equ)continue;
        int zero[MAX_ITEM_MAGICLEVEL]={0},one[MAX_ITEM_MAGICLEVEL],ten[MAX_ITEM_MAGICLEVEL];for(int j=0;j<MAX_ITEM_MAGICLEVEL;++j){one[j]=1;ten[j]=10;}
        KItem a,b,c;g_RandomSeed(17);bool x=ItemGen.Gen_Equipment(type,equ->m_nParticularType,-1,equ->m_nLevel,zero,0,ITEM_VERSION,&a)!=0;
        g_RandomSeed(17);bool y=ItemGen.Gen_Equipment(type,equ->m_nParticularType,-1,equ->m_nLevel,one,0,ITEM_VERSION,&b)!=0;
        g_RandomSeed(17);bool z=ItemGen.Gen_Equipment(type,equ->m_nParticularType,-1,equ->m_nLevel,ten,0,ITEM_VERSION,&c)!=0;
        Check(x&&y&&z,"magic generator differential calls");if(x&&y&&z){++magicTests;if(!memcmp(a.m_aryMagicAttrib,b.m_aryMagicAttrib,sizeof(a.m_aryMagicAttrib))&&!memcmp(a.m_aryMagicAttrib,c.m_aryMagicAttrib,sizeof(a.m_aryMagicAttrib)))++magicEqual;}
    }
    f=fopen(argv[4],"rb");FILE* out=fopen(argv[5],"wb");if(!f||!out)return 6;
    fprintf(out,"npc,profile,decision,error\n");int id,tr,tr1,npcCases=0,quarantined=0;char paths[4][1024];TestCatalog cat;
    while(fscanf(f,"%d %d %d %1023s %1023s %1023s %1023s",&id,&tr,&tr1,paths[0],paths[1],paths[2],paths[3])==7){
        NpcSnapshot npc;npc.templateId=id;npc.entity=id+1;npc.treasure=tr;npc.treasure1=tr1;
        for(int k=0;k<4;++k)npc.slots[k]=strcmp(paths[k],"-")?paths[k]:"";
        for(int profile=0;profile<2;++profile){Config c;c.profile=(Profile)profile;c.treasure=REPEATED_ROW_TRIALS;c.level=EXPLICIT_OR_UNIFORM_ZERO_SCALE;c.money=SYNTHETIC_FLAT_BERNOULLI;
            Plan p;LegacyDropNativeLoader loader;bool ok=Build(npc,q,c,loader,cat,p,e);++npcCases;Check(!ok,"existing NPC must not acquire guessed policy");
            if(e=="NPC_QUARANTINED")++quarantined;fprintf(out,"%d,%d,%s,%s\n",id,profile,ok?"UNEXPECTED_EXECUTABLE":"REJECT",e.c_str());
            Random rng(91),control(91);Result result;Check(!p.Draw(rng,result,e)&&result.items.empty()&&result.money==0&&rng.Next()==control.Next(),"rejected NPC no side effects or RNG");
        }
    }fclose(f);fclose(out);
    Check(explicitRows==525&&explicitGenerated==525,"all existing explicit Level sections generated");
    Check(ItemSet.GetItemNumber()==0,"all generated items removed");Check(_heapchk()==_HEAPOK,"native final heap integrity");
    printf("NATIVE_FOCUSED_RESULT explicit_rows=%d generated=%d magic_cases=%d magic_attrib_identical=%d npc_profile_cases=%d quarantine_rejects=%d checks=%d failures=%d\n",explicitRows,explicitGenerated,magicTests,magicEqual,npcCases,quarantined,checks,failed);
    printf("SCALE_DIFFERENTIAL add_found=%d add_missing=%d multiply_found=%d multiply_missing=%d percent_found=%d percent_missing=%d\n",scaleFound[0],scaleMissing[0],scaleFound[1],scaleMissing[1],scaleFound[2],scaleMissing[2]);
    g_pPakList=0;return failed?1:0;
}
