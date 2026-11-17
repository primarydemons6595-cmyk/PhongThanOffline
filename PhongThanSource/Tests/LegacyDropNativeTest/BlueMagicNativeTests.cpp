#include "KCore.h"
#include "KPakList.h"
#include "KItemGenerator.h"
#include "KItemSet.h"
#include "LegacyDropItemAdapter.h"
#include "PhongThanConsumableData.h"
#include "BlueMagicTestPolicy.h"
#include "LegacyDropDebugLogger.h"
#include "LegacyDropNativeCatalog.h"
#include "../../../Headers/PhongThanCharacter.h"
#include <malloc.h>
KPakList* g_pPakList=0;
static int checks=0,failures=0;
static void Check(bool ok,const char* name){++checks;if(!ok){++failures;printf("FAIL %s\n",name);}}
static std::string Unhex(const char* hex){std::string out;for(int i=0;hex[i]&&hex[i+1];i+=2){unsigned x=0;sscanf(hex+i,"%2x",&x);out+=(char)x;}return out;}
static int Count(KItem& item){int n=0;for(int i=0;i<MAX_ITEM_MAGICATTRIB;++i)if(item.m_aryMagicAttrib[i].nAttribType>0)++n;return n;}
int main(int argc,char** argv){
    if(argc!=4||!SetCurrentDirectoryA(argv[1]))return 2;g_SetRootPath(argv[1]);g_SetFilePath("\\");KPakList packs;Check(packs.Open("\\package.ini")!=0,"real native fixture");g_pPakList=&packs;g_SetPakFileMode(1);
    Check(ItemGen.Init()!=0,"native generator");ItemSet.Init();LegacyDropItemAdapter adapter;Check(adapter.Init(ItemGen),"existing adapter");
    FILE* manifest=fopen(argv[2],"rb");if(!manifest)return 3;char hex[2048],sha[65];std::string paths[3],hashes[3];
    for(int i=0;i<3;++i){if(fscanf(manifest,"%2047s %64s",hex,sha)!=2)return 4;paths[i]=Unhex(hex);hashes[i]=sha;}fclose(manifest);
    std::string error;BlueMagic::BlueMagicTableLoader table;BlueMagic::BlueLimits limits[2];
    Check(table.Load(paths[0],error),"KPakFile magic TSV load");Check(table.sha256==hashes[0],"magic source SHA256");Check(table.rows.size()==480&&table.header.size()==45,"480x45 raw table");Check(table.quarantined==182,"182 invalid prefix quarantined, not true");
    for(i=0;i<2;++i){Check(limits[i].Load(paths[i+1],error),"KPakFile limit version load");Check(limits[i].sha256==hashes[i+1],"limit source SHA256");}
    if(failures)return 5;
    std::string eventPath=std::string(argv[3])+".events.jsonl";DeleteFileA(eventPath.c_str());LegacyDropDebugLogger trace(eventPath.c_str(),32*1024*1024);
    int differences=0;for(int r=0;r<4;++r)for(int tier=0;tier<10;++tier)if(limits[0].caps[r][tier]!=limits[1].caps[r][tier])++differences;Check(differences==12,"twelve source limit differences");
    std::string bad="header\n";BlueMagic::BlueMagicTableLoader invalid;Check(!invalid.Parse(bad,error),"reject short TSV");
    std::string corrupt;for(size_t col=0;col<table.header.size();++col){if(col)corrupt+='\t';corrupt+=table.header[col];}corrupt+='\n';
    for(size_t row=0;row<table.rows.size();++row){BlueMagic::Cells cells=table.rows[row].raw;if(row==0){cells[5]="999";cells[6]="1";}for(col=0;col<cells.size();++col){if(col)corrupt+='\t';corrupt+=cells[col];}corrupt+='\n';}
    Check(invalid.Parse(corrupt,error)&&invalid.quarantined==183&&invalid.rows[0].reason=="BLUE_INVALID_PARAMETER_RANGE","invalid min max row quarantined from raw fixture");
    BlueMagic::Config c;c.mode=BlueMagic::Config::SHADOW;c.version=0;c.group=0;c.chance=1000000;c.lines=1;c.uniqueType=true;c.blankElementUnrestricted=true;c.attributeLevelIsTier=true;
    const int tiers[4]={1,2,5,10};int samples=0;
    for(int v=0;v<2;++v)for(int group=0;group<3;++group)for(int t=0;t<4;++t){c.version=v;c.group=group;BlueMagic::BlueMagicTestPolicy plan;
        Check(plan.Prepare(table,limits[v],c,18,0,tiers[t],0,error),"explicit group/version/type/tier eligibility");
        printf("BLUE_ELIGIBILITY fixture=1 version=%d group=%d detail=0 tier=%d candidates=%d limit=%d\n",v,group,tiers[t],plan.CandidateCount(),plan.LineLimit());
        LegacyDrop::Fields context;context["test_scope"]="NATIVE_FIXTURE_NOT_WORLD";context["template"]="18";char number[48];sprintf(number,"%d",tiers[t]);context["item_level"]=number;sprintf(number,"%d",v);context["limit_version"]=number;sprintf(number,"%d",group);context["weight_group"]=number;context["magic_source_sha256"]=table.sha256;context["limit_source_sha256"]=limits[v].sha256;context["magic_source_path"]=table.path;context["limit_source_path"]=limits[v].path;context["read_backend"]="LOOSE_ONLY_KPakFile";context["archive_provenance"]="serverlist.pak:521; completion manifest hash match";
        std::string eligibleRows;for(int cr=0;cr<plan.CandidateCount();++cr){sprintf(number,"%d",plan.Candidate(cr)->sourceRow);if(cr)eligibleRows+=",";eligibleRows+=number;}context["eligible_rows"]=eligibleRows;sprintf(number,"%d",plan.CandidateCount());context["eligible_count"]=number;sprintf(number,"%d",plan.LineLimit());context["line_limit"]=number;Check(trace.Write("BLUE_ELIGIBILITY","NATIVE_FIXTURE",context),"fixture eligibility log");
        LegacyDrop::Random rng(1978),replay(1978);for(int draw=0;draw<50;++draw){BlueMagic::Result a,b;Check(plan.Draw(rng,a,error)&&plan.Draw(replay,b,error),"seed replay draws");Check(a.attributes.size()==1&&b.attributes.size()==1,"forced one line");
            if(a.attributes.size()!=1||b.attributes.size()!=1)continue;const BlueMagic::Attribute& x=a.attributes[0];const BlueMagic::Attribute& y=b.attributes[0];Check(x.row==y.row&&x.type==y.type&&!memcmp(x.value,y.value,sizeof(x.value)),"deterministic exact attribute replay");const BlueMagic::Row& raw=table.rows[x.row-2];Check(raw.reason.empty()&&raw.weights[group][0]>0&&raw.level<=tiers[t],"nonzero weight / valid source row only");
            LegacyDrop::Fields f=context;sprintf(number,"%d",x.row);f["source_row"]=number;sprintf(number,"%d",13+11*group);f["weight_column_one_based"]=number;sprintf(number,"%d",x.roll);f["rng_draw"]=number;sprintf(number,"%d",x.totalWeight);f["weight_sum"]=number;sprintf(number,"%d",x.type);f["type"]=number;sprintf(number,"%d,%d,%d",x.value[0],x.value[1],x.value[2]);f["values"]=number;sprintf(number,"%d",a.chanceDraw);f["chance_draw"]=number;f["blue_chance_per_million"]="1000000_FORCED_FIXTURE";Check(trace.Write("BLUE_ATTR_ROLL","NATIVE_FIXTURE",f),"every trial source/group/candidate trace");
            for(int p=0;p<3;++p)Check(x.value[p]>=raw.minimum[p]&&x.value[p]<=raw.maximum[p],"parameter in original range");++samples;
        }
    }
    c.version=0;c.group=0;c.lines=0;BlueMagic::BlueMagicTestPolicy zero;Check(zero.Prepare(table,limits[0],c,18,0,1,0,error),"zero line white policy");LegacyDrop::Random zr(42);BlueMagic::Result result;Check(zero.Draw(zr,result,error)&&result.attributes.empty()&&!result.blue,"zero lines stays white");
    c.lines=5;BlueMagic::BlueMagicTestPolicy max;
    Check(!max.Prepare(table,limits[0],c,18,0,10,0,error)&&error=="BLUE_INSUFFICIENT_DISTINCT_CANDIDATES","cap5 is not a guarantee of five valid distinct attributes");
    c.lines=2;Check(max.Prepare(table,limits[0],c,18,0,10,0,error),"two distinct valid source attributes");Check(max.Draw(zr,result,error)&&result.attributes.size()==2,"two-line weighted selection");std::map<int,bool> seen;for(size_t j=0;j<result.attributes.size();++j){Check(!seen[result.attributes[j].type],"no duplicate attribute type");seen[result.attributes[j].type]=true;}
    c.lines=6;BlueMagic::BlueMagicTestPolicy reject;LegacyDrop::Random unconsumed(55),control(55);Check(!reject.Prepare(table,limits[0],c,18,0,10,0,error),"over cap rejected before RNG");Check(!reject.Draw(unconsumed,result,error)&&unconsumed.Next()==control.Next(),"failed preflight no RNG");
    c.lines=1;c.group=-1;Check(!reject.Prepare(table,limits[0],c,18,0,1,0,error),"UNBOUND group blocks");c.group=0;c.chance=-1;Check(!reject.Prepare(table,limits[0],c,18,0,1,0,error),"UNBOUND blue chance blocks");c.chance=0;Check(reject.Prepare(table,limits[0],c,18,0,1,0,error)&&reject.Draw(zr,result,error)&&!result.blue,"explicit chance zero");
    c.chance=1000000;Check(!reject.Prepare(table,limits[0],c,86,0,1,0,error),"NPC86 not approved");Check(!reject.Prepare(table,limits[0],c,18,3,1,0,error),"unmapped ring limit rejected");
    c.mode=BlueMagic::Config::LIVE_TEST;Check(!reject.Prepare(table,limits[0],c,18,0,1,0,error)&&error=="BLUE_PERSISTENCE_ABI_UNSUPPORTED","LIVE gate blocks known serialization loss");c.mode=BlueMagic::Config::SHADOW;
    Check(!reject.Prepare(table,limits[1],c,18,0,1,0,error)&&error=="BLUE_SOURCE_OR_VERSION_NOT_VALIDATED","explicit version must match loaded limit table");
    BlueMagic::BlueMagicTableLoader empty=table;for(size_t e=0;e<empty.rows.size();++e)for(int g=0;g<3;++g)for(int d=0;d<11;++d)empty.rows[e].weights[g][d]=0;
    Check(!reject.Prepare(empty,limits[0],c,18,0,1,0,error),"zero weights give zero candidates");
    BlueMagic::BlueMagicTableLoader wrongElement=table;for(e=0;e<wrongElement.rows.size();++e)wrongElement.rows[e].element="4";
    Check(!reject.Prepare(wrongElement,limits[0],c,18,0,1,0,error),"incompatible element excluded");
    Check(reject.Prepare(wrongElement,limits[0],c,18,0,1,4,error),"explicit same numeric element mapping");
    const int playerLevels[4]={10,20,21,80};for(int pl=0;pl<4;++pl){LegacyDrop::Fields f;char value[48];sprintf(value,"%d",playerLevels[pl]);f["player_level"]=value;sprintf(value,"%d",playerLevels[pl]-20);f["overlevel_delta"]=value;f["npc_level"]="20";f["template"]="18";f["test_scope"]="NATIVE_FIXTURE_SIMULATED_LEVEL_METADATA";f["rarity_policy"]="UNKNOWN_NO_GLOBAL_GATE";Check(trace.Write("PLAYER_NPC_RARITY_CONTEXT","NATIVE_FIXTURE",f),"rarity context diagnostic fixture");}
    KItem levelsOnly;int generatorLevels[MAX_ITEM_MAGICLEVEL];ZeroMemory(generatorLevels,sizeof(generatorLevels));generatorLevels[0]=1;
    Check(ItemGen.Gen_Equipment(6,0,0,2,generatorLevels,0,ITEM_VERSION,&levelsOnly)!=0&&Count(levelsOnly)==0,"nonzero LEVEL array does not synthesize native magic TYPE/VALUE");
    // Actual API and canonical persistence fields, not a guessed memory offset.
    int made=0,wireOK=0,reloadPreserved=0,baseOK=0;FILE* abi=fopen(argv[3],"wb");if(!abi)return 6;
    for(int trial=0;trial<500;++trial){PhongThanQuestItemTuple tuple;Check(adapter.Map(0,6,0,tuple),"ordinary belt identity eligible for source type85");tuple.level=2;int index=0;Check(adapter.GenerateMapped(tuple,index,error),"native ordinary equipment");if(!index)continue;KItem& item=Item[index];
        Check(Count(item)==0&&item.GetQuality()==equip_normal&&item.GetSetID()==0,"zero-line white baseline");
        std::vector<char> before(sizeof(KItem));memcpy(&before[0],&item,sizeof(KItem));KItemCommonAttrib common=item.m_CommonAttrib;
        KItemNormalAttrib attrs[MAX_ITEM_MAGICATTRIB];ZeroMemory(attrs,sizeof(attrs));attrs[0].nAttribType=85;attrs[0].nValue[0]=17;attrs[0].nValue[1]=-1;attrs[0].nValue[2]=6;
        Check(item.SetMagicAttrib(attrs)!=0,"supported native TYPE+VALUE setter");Check(item.GetQuality()==equip_magic&&Count(item)==1,"native blue classification from actual attributes");
        LegacyDropNativeCatalog catalog(adapter,ItemGen);int required=0;Check(catalog.EquipmentRequirement(6,0,2,required)&&required<=20,"existing monster equipment rule remains valid with fixture blue");
        Check(!memcmp(&common,&item.m_CommonAttrib,sizeof(common))&&item.GetSetID()==0,"no gold/set/class/name/base common mutation");++made;
        PHONGTHAN_ITEM_PROPERTIES_MESSAGE message;ZeroMemory(&message,sizeof(message));message.ItemId=item.GetID();for(int a=0;a<MAX_ITEM_MAGICATTRIB;++a){message.Attributes[a].Type=item.m_aryMagicAttrib[a].nAttribType;for(int p=0;p<3;++p)message.Attributes[a].Value[p]=item.m_aryMagicAttrib[a].nValue[p];}
        KItemNormalAttrib decoded[MAX_ITEM_MAGICATTRIB];for(a=0;a<MAX_ITEM_MAGICATTRIB;++a){decoded[a].nAttribType=message.Attributes[a].Type;for(int p=0;p<3;++p)decoded[a].nValue[p]=message.Attributes[a].Value[p];}
        Check(!memcmp(decoded,attrs,sizeof(attrs))&&message.ItemId==item.GetID(),"client-facing explicit native metadata exact");++wireOK;
        PHONGTHAN_CHARACTER_ITEM_RECORD record;ZeroMemory(&record,sizeof(record));record.SchemaVersion=PHONGTHAN_EMBEDDED_ITEM_VERSION;record.Genre=0;record.DetailType=item.GetDetailType();record.ParticularType=item.GetParticularType();record.Level=item.GetLevel();record.Series=item.GetSeries();record.TemplateRow=item.GetRow();record.TableVersion=item.m_GeneratorParam.nVersion;record.RandomSeed=item.m_GeneratorParam.uRandomSeed;record.Luck=item.m_GeneratorParam.nLuck;memcpy(record.GeneratorLevel,item.m_GeneratorParam.nGeneratorLevel,sizeof(record.GeneratorLevel));
        // Byte serialization through the actual canonical record ABI.
        unsigned char serialized[sizeof(record)];memcpy(serialized,&record,sizeof(record));PHONGTHAN_CHARACTER_ITEM_RECORD loaded;memcpy(&loaded,serialized,sizeof(loaded));
        KItem restored;restored.m_GeneratorParam.uRandomSeed=loaded.RandomSeed;Check(ItemGen.Gen_ExistEquipmentByTemplateRow(loaded.DetailType,loaded.TemplateRow,loaded.Series,loaded.GeneratorLevel,loaded.Luck,loaded.TableVersion,&restored)!=0,"actual DB load generator succeeds");
        if(!memcmp(restored.m_aryMagicAttrib,attrs,sizeof(attrs)))++reloadPreserved;
        Check(Count(restored)==0&&restored.GetQuality()==equip_normal,"KNOWN_BLOCKER current serializer/reconstructor loses added line");
        LegacyDrop::Fields event;char number[64];sprintf(number,"%lu",item.GetID());event["serial"]=number;event["test_scope"]="NATIVE_ABI_FIXTURE_NOT_WORLD";event["template"]="18";event["npc_level"]="20_SIMULATED";event["item_level"]="2";event["tuple"]="0/6/0";event["attribute_type"]="85";event["attribute_values"]="17,-1,6";event["source_row"]="2";event["weight_group"]="NOT_USED_EXPLICIT_ABI_VALUE";event["limit_version"]="NOT_USED_EXPLICIT_ABI_VALUE";event["magic_source_sha256"]=table.sha256;sprintf(number,"%d",required);event["required_player_level"]=number;event["quality_before"]="0";event["quality_after"]="1";Check(trace.Write("BLUE_NATIVE_APPLY","NATIVE_FIXTURE",event),"native fixture exact type value trace");event["reload_magic_lines"]="0";event["result"]="BLOCKED_MAGIC_PAYLOAD_LOST_AFTER_RECONSTRUCTION";Check(trace.Write("BLUE_NATIVE_VERIFY","NATIVE_FIXTURE",event),"native persistence failure trace is explicit");
        if(trial==0)fprintf(abi,"{\"scope\":\"NATIVE_ABI_FIXTURE_NOT_WORLD\",\"serial\":%lu,\"native_level\":%d,\"blue_quality\":%d,\"type\":85,\"values\":[17,-1,6],\"reload_magic_lines\":%d,\"record_bytes\":%u,\"generator_values\":40}\n",item.GetID(),item.GetLevel(),item.GetQuality(),Count(restored),sizeof(record));
        ZeroMemory(attrs,sizeof(attrs));Check(item.SetMagicAttrib(attrs)!=0&&!memcmp(&before[0],&item,sizeof(KItem)),"removing fixture line restores complete baseline object bytes");++baseOK;ItemSet.Remove(index);
    }
    fclose(abi);Check(ItemSet.GetItemNumber()==0&&_heapchk()==_HEAPOK,"all native fixture items removed, heap OK");
    printf("BLUE_NATIVE_RESULT rows=%d prefix_quarantined=%d limit_differences=%d weighted_samples=%d created_blue=%d wire_exact=%d persistence_preserved=%d base_unchanged=%d assertions=%d failures=%d\n",table.rows.size(),table.quarantined,differences,samples,made,wireOK,reloadPreserved,baseOK,checks,failures);
    printf("BLUE_LIVE_GATE BLOCKED PERSISTENCE_MAGIC_PAYLOAD_NOT_RESTORED; KINGSOFT_PARITY_UNVERIFIED; no world/ground/player test\n");g_pPakList=0;return failures?1:0;
}
