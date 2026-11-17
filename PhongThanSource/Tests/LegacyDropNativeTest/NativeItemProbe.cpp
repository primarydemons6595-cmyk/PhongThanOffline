// Links production server item/catalog/allocation code, without an NPC/world mock.
// It does not test network delivery or world object creation.
#include "KCore.h"
#include "KPakList.h"
#include "KItemGenerator.h"
#include "KItemSet.h"
#include "PhongThanConsumableData.h"
#include "PhongThanQuestItemTuple.h"
#include <stdio.h>
#include <malloc.h>
#include "LegacyDropEngine.h"
#include "LegacyDropItemAdapter.h"
#include "LegacyDropNativeLoader.h"
KPakList* g_pPakList=0;
static int failures,generated,unresolved,tuples,magicIdentical;
static LegacyDropItemAdapter adapter;
static void Check(bool ok,const char* what){if(!ok){++failures;printf("FAIL %s\n",what);}}
static bool CheckTuple(int genre,int detail,int particular,int level) {
    PhongThanQuestItemTuple tuple;tuple.genre=genre;tuple.detail=detail;tuple.particular=particular;tuple.level=level;tuple.series=-1;tuple.luck=0;
    int idx=0;std::string error;
    if(!adapter.GenerateMapped(tuple,idx,error)){if(error!="GENERATOR_REJECT")Check(false,error.c_str());return false;}
    ++generated;KItem& item=Item[idx];
    bool identity=item.GetGenre()==genre&&item.GetDetailType()==detail&&item.GetParticularType()==particular;
    if(!identity)printf("ITEM_IDENTITY_MISMATCH requested=%d/%d/%d/%d actual=%d/%d/%d/%d row=%d\n",genre,detail,particular,level,item.GetGenre(),item.GetDetailType(),item.GetParticularType(),item.GetLevel(),item.GetRow());
    Check(identity,"item identity");
    Check(item.GetID()>0&&ItemSet.SearchID(item.GetID())==idx,"serial lookup");
    Check(item.GetName()[0]&&item.GetWidth()>0&&item.GetHeight()>0,"metadata");
    Check(item.GetParticularType()==particular,"particular identity");
    if(genre==0||genre==1||genre==7)Check(item.GetLevel()==level,"level identity");
    DWORD id=item.GetID();ItemSet.Remove(idx);Check(ItemSet.SearchID(id)==0,"cleanup serial");
    Check(_heapchk()==_HEAPOK,"CRT heap integrity");
    return identity;
}
int main(int argc,char** argv){
    if(argc!=7)return 2;
    g_SetRootPath(argv[1]);g_SetFilePath("\\");KPakList packs;
    Check(packs.Open("\\package.ini")!=0,"open loose package");g_pPakList=&packs;g_SetPakFileMode(1);
    Check(ItemGen.Init()!=0,"native generator Init");if(failures)return 3;
    ItemSet.Init();Check(adapter.Init(ItemGen),"adapter catalog Init");FILE* input=fopen(argv[2],"rb"),*out=fopen(argv[3],"wb");if(!input||!out)return 4;
    fprintf(out,"genre,detail,particular,explicit_level,native_detail,native_particular,successful_levels,status\n");
    int genre,detail,particular,explicitLevel;
    while(fscanf(input,"%d %d %d %d",&genre,&detail,&particular,&explicitLevel)==4){
        ++tuples;int successes=0;int rawDetail=detail,rawParticular=particular;
        PhongThanQuestItemTuple tuple;
        if(!adapter.Map(genre,detail,particular,tuple)){
            ++unresolved;fprintf(out,"%d,%d,%d,%d,-1,-1,0,UNSUPPORTED_EXISTING_TUPLE_ADAPTER\n",genre,detail,particular,explicitLevel);continue;
        }
        detail=tuple.detail;particular=tuple.particular;
        if(genre==0){
            for(int row=0;row<ItemGen.CatalogEquipmentCount(detail);++row){
                const KBASICPROP_EQUIPMENT* equ=ItemGen.CatalogEquipment(detail,row);
                if(!equ||equ->m_nParticularType!=particular||(explicitLevel>=0&&equ->m_nLevel!=explicitLevel))continue;
                if(CheckTuple(genre,detail,particular,equ->m_nLevel))++successes;
            }
        }else if(genre==1&&explicitLevel<0){
            for(int level=0;level<=10;++level)if(ItemGen.Catalog().FindMedicine(detail,level)&&CheckTuple(genre,detail,particular,level))++successes;
        }else if(genre==7&&explicitLevel<0){
            if(ItemGen.Catalog().GetSkillBook(detail,particular)&&CheckTuple(genre,detail,particular,1))++successes;
        }else if(genre==6&&explicitLevel<0){
            const KBASICPROP_MAGICSCRIPT* script=ItemGen.Catalog().GetMagicScript(detail);
            if(script&&CheckTuple(genre,detail,particular,1))++successes;
        }else {
            // No guessed Level for scripts/books/equipment. Fixed-ID categories
            // ignore input level in the native generator and are tested at 0.
            int level=explicitLevel>=0?explicitLevel:0;
            if((genre!=6||explicitLevel>=0)&&CheckTuple(genre,detail,particular,level))++successes;
        }
        if(!successes)++unresolved;
        fprintf(out,"%d,%d,%d,%d,%d,%d,%d,%s\n",genre,rawDetail,rawParticular,explicitLevel,detail,particular,successes,successes?"GENERATED_CANDIDATES_LEVEL_POLICY_UNVERIFIED":"UNRESOLVED_OR_GENERATOR_REJECT");
    }fclose(input);fclose(out);
    // Observe whether the current production generator uses magic-level input.
    int tested=0;
    for(int type=0;type<11&&tested<100;++type)for(int row=0;row<ItemGen.CatalogEquipmentCount(type)&&tested<100;++row){
        const KBASICPROP_EQUIPMENT* equ=ItemGen.CatalogEquipment(type,row);if(!equ)continue;
        int white[MAX_ITEM_MAGICLEVEL]={0},blue[MAX_ITEM_MAGICLEVEL];for(int m=0;m<MAX_ITEM_MAGICLEVEL;++m)blue[m]=10;
        KItem a,b;g_RandomSeed(20261017);BOOL x=ItemGen.Gen_Equipment(type,equ->m_nParticularType,-1,equ->m_nLevel,white,0,ITEM_VERSION,&a);
        g_RandomSeed(20261017);BOOL y=ItemGen.Gen_Equipment(type,equ->m_nParticularType,-1,equ->m_nLevel,blue,0,ITEM_VERSION,&b);
        if(x&&y){++tested;if(!memcmp(a.m_aryMagicAttrib,b.m_aryMagicAttrib,sizeof(a.m_aryMagicAttrib)))++magicIdentical;}
    }
    int before=ItemSet.GetItemNumber();for(int loop=0;loop<10000;++loop){int idx=ItemSet.Add(item_medicine,-1,1,0,0,0,0,ITEM_VERSION);if(idx)ItemSet.Remove(idx);else Check(false,"repeat allocation");}
    Check(ItemSet.GetItemNumber()==before,"10000 allocation cleanup");
    Check(_heapchk()==_HEAPOK,"post-stress CRT heap integrity");
    LegacyDrop::Quarantine quarantine;FILE* qfile=fopen(argv[6],"rb");std::string manifestError;
    Check(quarantine.Load(qfile,manifestError),"quarantine manifest fail-closed load");if(qfile)fclose(qfile);
    KTabFile npcs;Check(npcs.Load("\\settings\\phongthan\\Npcs.txt")!=0,"native NPC registry");
    int examined=0,isolated=0,allowed=0,empty=0,planned=0,blockedPlans=0;FILE* npcOut=fopen("npc_preflight.csv","wb");if(!npcOut)return 7;
    fprintf(npcOut,"npc,nonempty_slots,preflight,status\n");
    for(int npcRow=2;npcRow<=npcs.GetHeight();++npcRow){LegacyDrop::NpcSnapshot npc;npc.templateId=npcRow-2;npc.entity=npcRow;int slots=0;
        for(int slotNo=0;slotNo<4;++slotNo){char column[32],value[1024];if(slotNo)sprintf(column,"DropRateFile%d",slotNo);else strcpy(column,"DropRateFile");npcs.GetString(npcRow,column,"",value,sizeof(value));npc.slots[slotNo]=value;if(value[0]&&strcmp(value,"0"))++slots;}
        std::string error;bool ok=quarantine.Allows(npc,error);++examined;if(ok){++allowed;if(!slots)++empty;}else ++isolated;
        // A second attempt cannot bypass the same immutable quarantine decision.
        std::string again;Check(quarantine.Allows(npc,again)==ok,"quarantine repeat stable");
        for(slotNo=0;slotNo<4;++slotNo){if(npc.slots[slotNo].empty()||npc.slots[slotNo]=="0")continue;
            LegacyDrop::PreparedTable plan;LegacyDropNativeLoader loader;std::string why;bool built=LegacyDrop::BuildLabPlan(npc,slotNo,quarantine,loader,plan,why);
            if(ok){if(!built)printf("PLAN_FAILURE npc=%d slot=%d error=%s\n",npc.templateId,slotNo,why.c_str());Check(built,"allowed NPC native plan");if(built)++planned;}
            else {Check(!built,"quarantined plan rejected");++blockedPlans;LegacyDrop::Random rng(17);std::vector<int> selected;Check(!plan.Draw(rng,selected,why)&&selected.empty(),"blocked plan cannot draw");}
        }
        fprintf(npcOut,"%d,%d,%s,%s\n",npc.templateId,slots,ok?"ALLOWED_FOR_LAB_ONLY":"QUARANTINED",error.c_str());
    }fclose(npcOut);Check(examined==2703,"all NPCs examined");
    FILE* refs=fopen(argv[4],"rb"),*refout=fopen(argv[5],"wb");if(!refs||!refout)return 5;
    fprintf(refout,"npc,slot,path_hex,native_hash,load_status,resolved_hash,resolved_status\n");
    int npc,slot,loaded=0,missing=0,resolvedLoaded=0;char hex[2048],raw[1024];
    while(fscanf(refs,"%d %d %2047s",&npc,&slot,hex)==3){
        int length=strlen(hex)/2;if(length>=sizeof(raw))return 6;
        for(int k=0;k<length;++k){unsigned int byte;sscanf(hex+k*2,"%2x",&byte);raw[k]=(char)byte;}raw[length]=0;
        KIniFile ini;BOOL ok=ini.Load(raw);if(ok)++loaded;else ++missing;
        LegacyDrop::NpcSnapshot snapshot;snapshot.slots[slot]=raw;std::string resolved;
        bool resolvedOK=LegacyDrop::Resolve(snapshot,slot,resolved);KIniFile second;
        if(resolvedOK)resolvedOK=second.Load(resolved.c_str())!=0;if(resolvedOK)++resolvedLoaded;
        char resolvedRaw[1024];strncpy(resolvedRaw,resolved.c_str(),1023);resolvedRaw[1023]=0;
        fprintf(refout,"%d,%d,%s,%lu,%s,%lu,%s\n",npc,slot,hex,(unsigned long)g_FileName2Id(raw),ok?"NATIVE_LOAD_PASS":"MISSING_NATIVE_LOOKUP",resolved.empty()?0UL:(unsigned long)g_FileName2Id(resolvedRaw),resolvedOK?"NATIVE_LOAD_PASS":"MISSING_RESOLVED_LOOKUP");
    }fclose(refs);fclose(refout);
    printf("NATIVE_ITEM_RESULT tuples=%d generated=%d unresolved=%d assertions_failed=%d magic_tested=%d magic_identical=%d allocation_cycles=10000 world=NOT_TESTED\n",tuples,generated,unresolved,failures,tested,magicIdentical);
    printf("NATIVE_REFERENCE_RESULT loaded=%d missing=%d resolved_loaded=%d resolved_missing=%d\n",loaded,missing,resolvedLoaded,loaded+missing-resolvedLoaded);
    printf("NATIVE_NPC_RESULT examined=%d isolated=%d allowed=%d no_drop=%d manifest_fail_closed=1\n",examined,isolated,allowed,empty);
    printf("NATIVE_PLAN_RESULT prepared=%d blocked=%d\n",planned,blockedPlans);
    g_pPakList=0;return failures?1:0;
}
