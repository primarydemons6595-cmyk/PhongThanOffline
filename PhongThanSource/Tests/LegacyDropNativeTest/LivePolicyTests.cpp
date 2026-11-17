#include "KCore.h"
#include "KPakList.h"
#include "KItemGenerator.h"
#include "KItemSet.h"
#include "PhongThanConsumableData.h"
#include "LegacyDropNativeCatalog.h"
#include "LegacyDropNativeLoader.h"
#include "LegacyDropDebugLogger.h"
#include <malloc.h>
KPakList* g_pPakList=0;
using namespace LegacyDrop;
using namespace LegacyDrop::Execution;
static int checks=0,failures=0;
static void Check(bool pass,const char* name){++checks;if(!pass){++failures;printf("FAIL %s\n",name);}}
class Trace:public Observer {public:int rolls,hits,levels;Trace():rolls(0),hits(0),levels(0){}
    void Roll(int,int,int,int range,int rate,unsigned long roll,bool hit){++rolls;if(hit)++hits;Check(hit==Selected(rate,range,roll),"trace threshold parity");}
    void Level(int,int,int low,int high,int chosen,unsigned long draw,int count){++levels;Check(chosen>=low&&chosen<=high&&draw<(unsigned long)count,"trace level bounds");}
};
int main(int argc,char** argv){
    if(argc!=4||!SetCurrentDirectoryA(argv[1]))return 2;
    g_SetRootPath(argv[1]);g_SetFilePath("\\");KPakList packs;Check(packs.Open("\\package.ini")!=0,"native packages");g_pPakList=&packs;g_SetPakFileMode(1);
    Check(ItemGen.Init()!=0,"native generator");ItemSet.Init();LegacyDropItemAdapter adapter;Check(adapter.Init(ItemGen),"shared item adapter");
    Quarantine q;std::string error;FILE* f=fopen(argv[2],"rb");Check(q.Load(f,error),"quarantine");if(f)fclose(f);if(failures)return 3;
    KTabFile npcs;Check(npcs.Load("\\settings\\phongthan\\Npcs.txt")!=0,"native NPC registry");LegacyDropNativeCatalog catalog(adapter,ItemGen);LegacyDropNativeLoader loader;
    const int ids[3]={2506,125,82};int plans=0,rejects=0,generated=0,divisorRejected=0;
    for(int id=0;id<3;++id){NpcSnapshot npc;npc.templateId=ids[id];npc.entity=ids[id]+1;
        char text[1024];npcs.GetString(ids[id]+2,"Treasure","",text,sizeof(text));if(!Integer(text,npc.treasure))npc.treasure=-1;
        npcs.GetString(ids[id]+2,"Treasure1","",text,sizeof(text));if(!Integer(text,npc.treasure1))npc.treasure1=-1;
        for(int s=0;s<4;++s){char key[40];if(s)sprintf(key,"DropRateFile%d",s);else strcpy(key,"DropRateFile");npcs.GetString(ids[id]+2,key,"",text,sizeof(text));npc.slots[s]=text;}
        for(int profile=0;profile<2;++profile)for(int tr=1;tr<=2;++tr)for(int lr=2;lr<=3;++lr)for(int lv=10;lv<=80;lv+=70){
            Config c;c.profile=(Profile)profile;c.treasure=(TreasureRule)tr;c.level=(LevelRule)lr;c.magic=EXPERIMENTAL_NATIVE_CATALOG_ONLY;c.money=SYNTHETIC_FLAT_BERNOULLI;c.npcLevel=lv;
            Plan p;bool ok=Build(npc,q,c,loader,catalog,p,error);
            if(ids[id]==82){Check(!ok&&error=="NPC_QUARANTINED","Pangu stays quarantined");++rejects;continue;}
            // Catalog evidence: divisor bounds at NPC80 exclude at least one
            // required tuple of boss125. Reject this explicit hypothesis rather
            // than silently widening its level range to make it pass.
            if(ids[id]==125&&lr==3&&lv==80){Check(!ok&&error=="CATALOG_LEVEL_MISSING","divisor hypothesis refuses unsupported boss level");++divisorRejected;continue;}
            if(!ok)printf("PLAN_REJECT npc=%d profile=%d treasure=%d level=%d npcLevel=%d error=%s\n",ids[id],profile,tr,lr,lv,error.c_str());
            Check(ok,"allowlisted native plan");if(!ok)continue;++plans;
            Random rng(20261018);Result result;Trace trace;Check(p.Draw(rng,result,error,&trace),"native experimental Draw");Check(trace.hits==(int)result.items.size()&&trace.levels==trace.hits,"one level selection per hit");
            for(size_t i=0;i<result.items.size();++i){const Intent& intent=result.items[i];std::string path,bytes;Resolve(npc,intent.slot,path);loader.Load(path,bytes,error);Table t=Parse(bytes);char row[40];sprintf(row,"%d",intent.row);const Section* section=Find(t,row);int g,d,part;Get(section,"Genre",g);Get(section,"Detail",d);Get(section,"Particular",part);
                PhongThanQuestItemTuple tuple;Check(adapter.Map(g,d,part,tuple),"native raw identity");tuple.level=intent.level;int item=0;bool made=adapter.GenerateMapped(tuple,item,error);Check(made,"native generated intent");if(made){++generated;ItemSet.Remove(item);}
            }
        }
    }
    #include "MonsterEquipmentTests.inl"
    int players[5]={10,20,21,80,80},monsters[5]={10,10,10,10,80};bool bq[5]={true,true,false,false,true};
    Check(LiveNormalEngineEligible(0),"no competing custom drop script");Check(!LiveNormalEngineEligible(1),"nonzero custom script rejects LIVE normal replacement");
    for(int g=0;g<5;++g){Check(LevelGateAllows(false,10,players[g],monsters[g]),"Legacy NONE independent of Lua gate");Check(LevelGateAllows(true,10,players[g],monsters[g])==bq[g],"explicit overlevel10 boundary");}
    // Logger tests have isolated destinations, never runtime log files.
    std::string log=std::string(argv[3])+"/logger_test.jsonl";DeleteFileA(log.c_str());for(int r=1;r<=4;++r){char suffix[8];sprintf(suffix,".%d",r);DeleteFileA((log+suffix).c_str());}
    LegacyDropDebugLogger logger(log.c_str(),200);Fields fields;fields["raw"]=std::string("a\"\\\n")+(char)0x81;for(int r2=0;r2<10;++r2)Check(logger.Write("DROP_ERROR","test-death",fields),"logger rotation write");
    Check(GetFileAttributesA((log+".4").c_str())!=INVALID_FILE_ATTRIBUTES,"bounded rotation archive4");
    LegacyDropDebugLogger missing("__missing_legacy_test_dir__/bad.log");Check(!missing.Write("DROP_ERROR","test",fields),"logger missing directory fails without crash");
    Check(LegacyDropDebugLogger::Quote(fields["raw"])=="\"a\\\"\\\\\\u000a\\u0081\"","raw bytes JSON escape");
    Check(ItemSet.GetItemNumber()==0,"native allocation cleanup");Check(_heapchk()==_HEAPOK,"native heap");
    printf("LIVE_POLICY_NATIVE_RESULT plans=%d quarantine_rejects=%d divisor_rejects=%d generated=%d checks=%d failures=%d parity=KINGSOFT_PARITY_UNVERIFIED\n",plans,rejects,divisorRejected,generated,checks,failures);g_pPakList=0;return failures?1:0;
}
