#include "KCore.h"
#include "../../../Headers/PhongThanRewardOwner.h"
#ifdef _SERVER
#include "LegacyDropRuntime.h"
#include "PhongThanLiveCampaigns.h"
#include "PhongThanLiveDropBonus.h"
#include "PhongThanEventControl.h"
#include "LegacyDropDebugLogger.h"
#include "LegacyDropNativeCatalog.h"
#include "LegacyDropNativeLoader.h"
#include "KNpcTemplate.h"
#include "KNpcSet.h"
#include "KPlayer.h"
#include "KItemSet.h"
#include "KObjSet.h"
#include "KObj.h"
#include "KSubWorld.h"
#include "KSubWorldSet.h"
#include "KRegion.h"
#include <malloc.h>
#include "P2BlueDropRuntime.h"
#include "KPhongThanProfessionSkills.h"
#include "KPhongThanSkillLearning.h"
#include "P2TestCharacterSetup.h"
using namespace LegacyDrop;
using namespace LegacyDrop::Execution;
namespace {
// Config path is selected by the independent Main official ownership latch.
const char* QuarantinePath(){return NativeDropRelease::Active()?"settings\\phongthan\\NativeDropQuarantine.txt":"settings\\phongthan\\LegacyDropQuarantine.txt";}
LegacyDropDebugLogger logger;
std::string Number(long n){char b[48];sprintf(b,"%ld",n);return b;}
struct Life {unsigned long entity,generation;std::string death;int hits;bool consumed;Life():entity(0),generation(0),hits(0),consumed(false){}};
Life lives[MAX_NPC];
struct Tracked {std::string death;unsigned long serial;int object,worldId;DWORD born;bool cleanup;};
std::map<unsigned long,Tracked> tracked;
struct MoneyTracked {std::string death;int object,worldId,amount;};
std::map<int,MoneyTracked> moneyTracked;
unsigned long sequence=0;
std::string NewDeath(unsigned long generation){char b[128];sprintf(b,"%lu-%lu-%lu-%lu",GetCurrentProcessId(),(unsigned long)GetTickCount(),++sequence,generation);return b;}
bool selfTest=false;
bool committed=false;
std::vector<std::string> luaDeaths;
int selfPlayerLevel=0;
std::vector<int> selfObjects;
// GameServer only, exact Loose root suffix + matching working directory + flag.
// No executable in another runtime may opt in by copying just the INI.
bool IsLoose(){
    if(PtReleaseEnabled())return PtReleaseMount();
    char exe[MAX_PATH],cwd[MAX_PATH];if(!GetModuleFileNameA(0,exe,sizeof(exe))||!GetCurrentDirectoryA(sizeof(cwd),cwd))return false;
    char* slash=strrchr(exe,'\\');if(!slash||_stricmp(slash+1,"GameServer.exe"))return false;*slash=0;
    const char* suffix="\\PhongThanRuntime-Loose\\Server";int a=strlen(exe),b=strlen(suffix);
    return (a>=b&&!_stricmp(exe+a-b,suffix)&&!_stricmp(exe,cwd)&&GetFileAttributesA("loose_only.flag")!=INVALID_FILE_ATTRIBUTES)||P2BlueScope();
}
struct LocalConfig {Table table;};
std::string Read(LocalConfig& ini,const char* section,const char* key){const Section* s=Find(ini.table,section);Fields::const_iterator i;if(s){i=s->fields.find(key);if(i!=s->fields.end())return i->second;}
    if(NativeDropRelease::Active()&&!strncmp(section,"NPC_",4)){s=Find(ini.table,"Defaults");if(s){i=s->fields.find(key);if(i!=s->fields.end())return i->second;}}return "";}
bool Int(LocalConfig& ini,const char* section,const char* key,int& out){return Integer(Read(ini,section,key),out);}
bool LoadConfig(LocalConfig& ini){
    // Physical control file only; reuse shared byte/section parser. Its DROP
    // schema errors are irrelevant to configuration sections, syntax errors are not.
    FILE* f=fopen(NativeDropRelease::ConfigPath(),"rb");if(!f)return false;char b[32769];size_t size=fread(b,1,sizeof(b),f);bool bad=ferror(f)!=0;fclose(f);if(bad||size<8||size>=sizeof(b))return false;
    ini.table=Parse(std::string(b,size));
    for(size_t i=0;i<ini.table.sections.size();++i){const std::string& s=ini.table.sections[i].name;int id;
        if(s!="Main"&&s!="Defaults"&&(s.substr(0,4)!="NPC_"||!Integer(s.substr(4),id)||id<0))return false;}
    for(i=0;i<ini.table.errors.size();++i){const std::string& e=ini.table.errors[i];
        if(e!="INVALID_COUNT"&&e!="INVALID_RANGE"&&e.substr(0,19)!="UNEXPECTED_SECTION:")return false;}
    if(NativeDropRelease::Active()&&(Read(ini,"Main","Version")!="1"||Read(ini,"Main","Release")!="LOOSE_NATIVE_DROP_V1"))return false;
    return Find(ini.table,"Main")!=0;
}
Fields Context(const NpcSnapshot& snap,int index,int level,int special,int owner,int attacker,int world,int x,int y){
    Fields f;f["npc_index"]=Number(index);f["npc_instance"]=Number(snap.entity);f["template"]=Number(snap.templateId);
    f["npc_level"]=Number(level);f["npc_special"]=Number(special);f["killer_npc"]=Number(attacker);f["reward_owner"]=Number(owner);
    f["world"]=Number(world);f["x"]=Number(x);f["y"]=Number(y);f["Treasure"]=Number(snap.treasure);f["Treasure1"]=Number(snap.treasure1);
    f["player_level"]=Number(selfTest?selfPlayerLevel:0);f["party_id"]="-1";f["party_flag"]="0";
    if(owner>0&&owner<MAX_PLAYER){int pn=Player[owner].m_nIndex;if(pn>0&&pn<MAX_NPC)f["player_level"]=Number(Npc[pn].m_Level);
        f["party_flag"]=Number(Player[owner].m_cTeam.m_nFlag);f["party_id"]=Number(Player[owner].m_cTeam.m_nID);}
    for(int i=0;i<4;++i)f[std::string("DropRateFile")+(i?Number(i):std::string())]=snap.slots[i];
    f["test_scope"]=selfTest?"AUTOMATED_UNOWNED":"PLAYER_GAMEPLAY";return f;
}
bool Error(const std::string& death,const char* reason,const char* action="CURRENT_NORMAL_ENGINE_RETAINED"){
    Fields f;f["reason"]=reason;f["action"]=NativeDropRelease::Active()?"NATIVE_HOLD_NO_NPC_DROP_POLICY_FALLBACK":action;logger.Write("DROP_ERROR",death,f);return false;
}
struct Trace:Observer {
    std::string death;bool ok;Trace(const std::string& d):death(d),ok(true){}
    void Roll(int slot,int row,int trial,int range,int threshold,unsigned long roll,bool hit){
        Fields f;f["slot"]=Number(slot);f["section"]=Number(row);f["trial"]=Number(trial);f["range"]=Number(range);f["rng_draw"]=Number(roll);f["outcome"]=hit?"HIT":"MISS";
        f["threshold"]=Number(threshold);f["comparison"]="rng_draw<threshold";
        if(!logger.Write("DROP_ROLL",death,f))ok=false;
    }
    void Money(int slot,int range,int rate,int scale,unsigned long draw,int amount){Fields f;f["slot"]=Number(slot);f["range"]=Number(range);f["raw_MoneyRate"]=Number(rate);f["raw_MoneyScale"]=Number(scale);f["rng_draw"]=rate?Number(draw):"NOT_CONSUMED_ZERO_RATE";f["amount"]=Number(amount);f["rule"]="EXPERIMENTAL_FLAT_BERNOULLI_NATIVE_CURRENCY_UNITS";if(!logger.Write("MONEY_ROLL",death,f))ok=false;}
    void Level(int slot,int row,int low,int high,int chosen,unsigned long draw,int candidates){Fields f;f["slot"]=Number(slot);f["section"]=Number(row);f["low"]=Number(low);f["high"]=Number(high);f["chosen_level"]=Number(chosen);f["rng_draw"]=candidates>1?Number(draw):"NOT_CONSUMED_FIXED_LEVEL";f["candidate_count"]=Number(candidates);
        if(!logger.Write("ITEM_LEVEL",death,f))ok=false;}
};
struct Pending {int item,slot,row;unsigned long serial;};
void RemovePending(std::vector<Pending>& p,const std::string& death){
    for(size_t i=0;i<p.size();++i)if(p[i].item>0){ItemSet.Remove(p[i].item);p[i].item=0;
        try{Fields f;f["serial"]=Number(p[i].serial);f["reason"]="PRECOMMIT_ROLLBACK";f["result"]="REMOVED";logger.Write("ITEM_ROLLBACK",death,f);}catch(...){}
    }p.clear();
}
struct PendingGuard {std::vector<Pending>& items;const std::string& death;PendingGuard(std::vector<Pending>& p,const std::string& d):items(p),death(d){}~PendingGuard(){RemovePending(items,death);}};
int Ground(int item,int world,int x,int y,int owner){
    if(world<0||world>=MAX_SUBWORLD)return -1;
    char kind[32];kind[0]=0;if(Item[item].GetObjIdx()<=0||!ObjSet.m_cTabFile.GetString(Item[item].GetObjIdx()+1,3,"",kind,sizeof(kind))||strcmp(kind,"Item"))return -1;
    POINT pos;pos.x=x;pos.y=y;SubWorld[world].GetFreeObjPos(pos);KMapPos map;map.nSubWorld=world;
    SubWorld[world].Mps2Map(pos.x,pos.y,&map.nRegion,&map.nMapX,&map.nMapY,&map.nOffX,&map.nOffY);
    if(map.nRegion<0||map.nRegion>=SubWorld[world].m_nTotalRegion)return -1;
    KObjItemInfo info;ZeroMemory(&info,sizeof(info));info.m_nItemID=item;info.m_nItemWidth=Item[item].GetWidth();info.m_nItemHeight=Item[item].GetHeight();
    info.m_nColorID=Item[item].GetColorItem();info.m_nGenre=Item[item].GetGenre();info.m_nDetailType=Item[item].GetDetailType();info.m_nMovieFlag=info.m_nSoundFlag=1;
    g_StrCpyLen(info.m_szName,Item[item].GetName(),sizeof(info.m_szName));int obj=ObjSet.Add(Item[item].GetObjIdx(),map,info);
    if(obj>0)Object[obj].SetItemBelong(owner>0?owner:-1);return obj;
}
}

#include "RandomBlueNpcRuntime.inl"
#include "NormalRareEquipmentPolicy.h"

#include "PhongThanRealmTrialRuntime.h"
void LegacyDropResetNpc(int index){PhongThanRealmTrialBossReset(index);PhongThanLiveCampaignReset(index);if(index>0&&index<MAX_NPC){Life& l=lives[index];++l.generation;l.entity=0;l.death="";l.hits=0;l.consumed=false;}}
static bool LegacyDropOnDeathImpl(KNpc& npc,int owner,int attacker){
    if(!IsLoose()||npc.IsPlayer()||npc.m_Kind!=kind_normal)return false;
    LocalConfig ini;if(!LoadConfig(ini))return false;std::string mode=Read(ini,"Main","Mode");if(mode=="OFF"||mode.empty())return false;
    char section[48];sprintf(section,"NPC_%d",npc.m_NpcSettingIdx);int enabled=0;
    if(!Int(ini,section,"Enabled",enabled)||enabled!=1)return false;
    int index=npc.m_Index;if(index<=0||index>=MAX_NPC)return false;
    Life& life=lives[index];
    if(life.entity==npc.m_dwID&&life.consumed){Error(life.death,"DUPLICATE_DEATH","NORMAL_DROP_SUPPRESSED");return mode=="LIVE_TEST"||mode=="OFFICIAL";}
    if(life.entity!=npc.m_dwID){life.death="";life.entity=npc.m_dwID;}
    if(life.death.empty())life.death=NewDeath(life.generation);life.consumed=true;
    const std::string death=life.death;
    // Immutable snapshot BEFORE any Lua callback and before any generation.
    NpcSnapshot snap;snap.templateId=npc.m_NpcSettingIdx;snap.entity=npc.m_dwID;snap.treasure=snap.treasure1=-1;
    KNpcTemplate* temp=snap.templateId>=0&&snap.templateId<MAX_NPCSTYLE?g_pNpcTemplate[snap.templateId]:0;
    if(!temp||!temp->m_bHaveLoadedFromTemplate)return Error(death,"NPC_TEMPLATE_NOT_LOADED");
    snap.treasure=temp->m_LegacyTreasure;snap.treasure1=temp->m_LegacyTreasure1;
    for(int sl=0;sl<4;++sl)snap.slots[sl]=temp->m_LegacyDropFiles[sl];
    int world=npc.m_SubWorldIndex,x=0,y=0,level=npc.m_Level,special=npc.m_btSpecial;npc.GetMpsPos(&x,&y);
    Fields ctx=Context(snap,index,level,special,owner,attacker,world,x,y);ctx["mode"]=mode;
    const unsigned long customDropScript=npc.m_DropScriptID;
    ctx["drop_script_id"]=Number(customDropScript);ctx["death_script_id"]=Number(npc.m_DeathScriptID);ctx["action_script_id"]=Number(npc.m_ActionScriptID);
    if(!logger.Write("NPC_DEATH",death,ctx))return false;
    if(mode!="SHADOW"&&mode!="LIVE_TEST"&&mode!="OFFICIAL")return Error(death,"INVALID_MODE");
    if((mode=="LIVE_TEST"||mode=="OFFICIAL")&&!LiveNormalEngineEligible(customDropScript))return Error(death,"CUSTOM_DROP_SCRIPT_UNCLASSIFIED");
    if(NativeDropRelease::Active()&&!NativeDropRollout::Active())return Error(death,"PERSISTENCE_ADMISSION_FAILED","NO_OLD_POLICY_FALLBACK");
    bool noFiles=true;for(int nf=0;nf<4;++nf)if(!snap.slots[nf].empty()&&snap.slots[nf]!="0")noFiles=false;
    if(NativeDropRelease::Active()&&noFiles){Fields end;end["reason"]="NO_NATIVE_DROP_FILES";end["created"]="0";logger.Write("DROP_END",death,end);return true;}
    Config c;c.allowInactiveMissingSlots=c.selectedProfileQuarantine=NativeDropRelease::Active();std::string profile=Read(ini,section,"Profile"),treasure=Read(ini,section,"TreasureRule"),levels=Read(ini,section,"LevelRule");
    c.profile=RuntimeProfile(profile,special,Read(ini,section,"NormalProfile"),Read(ini,section,"BlueProfile"));
    c.treasure=treasure=="REPEATED_TRIALS"?REPEATED_ROW_TRIALS:treasure=="SUCCESS_CAP"?SUCCESS_CAP:TREASURE_UNBOUND;
    c.level=levels=="MAIN_CATALOG"?EXPERIMENTAL_MAIN_CATALOG:levels=="NPC_DIVISOR_CATALOG"?EXPERIMENTAL_NPC_DIVISOR_CATALOG:LEVEL_UNBOUND;
    if(levels=="MONSTER_EQUIPMENT_CATALOG"){
        if(!P2BlueScope()&&snap.templateId!=18)return Error(death,"EQUIPMENT_EXPERIMENT_NPC_NOT_APPROVED");
        c.level=EXPERIMENTAL_MONSTER_EQUIPMENT_CATALOG;
    }
    c.magic=Read(ini,section,"MagicRule")=="NATIVE_CATALOG_ONLY"?EXPERIMENTAL_NATIVE_CATALOG_ONLY:MAGIC_UNBOUND;
    if(c.magic!=EXPERIMENTAL_NATIVE_CATALOG_ONLY)return Error(death,"MAGIC_RULE_NOT_EXPLICIT");
    // Zero-only remains available. Flat currency is explicitly experimental and
    // isolated-only; original zero money values still create no currency.
    std::string moneyRule=Read(ini,section,"MoneyRule");if(moneyRule!="ZERO_ONLY"&&((moneyRule!="EXPERIMENTAL_FLAT_BERNOULLI"&&moneyRule!="FLAT_BERNOULLI")||!P2BlueScope()))return Error(death,"MONEY_RULE_NOT_EXPLICIT");
    c.money=SYNTHETIC_FLAT_BERNOULLI;c.npcLevel=level;c.maxMoney=1000000;
    std::string ahead=Read(ini,section,"EquipmentMaxRequirementAhead");if(!ahead.empty()&&(!Integer(ahead,c.equipmentRequirementAhead)||c.equipmentRequirementAhead<0||c.equipmentRequirementAhead>100))return Error(death,"INVALID_EQUIPMENT_REQUIREMENT_AHEAD");
    if(!Int(ini,section,"MaxItems",c.maxItems)||!Int(ini,section,"MaxTrials",c.maxTrials))return Error(death,"MISSING_BUDGET");
    Fields p;p["profile"]=profile;p["treasure_rule"]=treasure;p["level_rule"]=levels;p["magic_rule"]="NATIVE_CATALOG_ONLY";p["money_rule"]=moneyRule;p["resolved_profile"]=c.profile==PROFILE_A?"A":c.profile==PROFILE_B?"B":"UNBOUND";p["state_rule_parity"]=NativeDropRelease::Active()?"LOOSE_RULE_V1":"KINGSOFT_PARITY_UNVERIFIED";
    p["level_formula"]=levels=="MAIN_CATALOG"?"uniform(catalog_levels intersect [MinItemLevel,MaxItemLevel]);scales=recorded_not_applied":
        "uniform(catalog_levels intersect [clamp(floor(npcLevel/MinScale),Min,Max),clamp(floor(npcLevel/MaxScale),Min,Max)])";
    if(c.level==EXPERIMENTAL_MONSTER_EQUIPMENT_CATALOG)p["level_formula"]="equipment_only:uniform(catalog intersect divisor_bounds intersect required_level<=npcLevel+configured_ahead);non_equipment:MAIN_CATALOG_category_rules;explicit_equipment_must_fit";
    p["category_levels"]="explicit Level overrides; fixed-ID genre3/4/5/8=0; script/book=1";p["rand_rule"]="LAB_INDEPENDENT_ROWS_V1";p["max_items"]=Number(c.maxItems);p["max_trials"]=Number(c.maxTrials);
    p["equipment_requirement_ahead"]=Number(c.equipmentRequirementAhead);p["equipment_requirement_rule"]="EXPERIMENTAL_required_level<=npc_level+configured_ahead;not_wearability_of_monster";
    if(!logger.Write("DROP_PROFILE",death,p))return false;
    Fields gate;std::string rule=Read(ini,section,"LevelGate");gate["system"]="LEGACY_EXPERIMENT";gate["rule"]=rule;gate["player_level"]=ctx["player_level"];gate["npc_level"]=Number(level);
    int pl=atoi(ctx["player_level"].c_str()),maxGap=0;bool pass=rule=="NONE";
    if(rule=="MAX_OVERLEVEL"&&Int(ini,section,"MaxOverLevel",maxGap)&&maxGap>=0)pass=LevelGateAllows(true,maxGap,pl,level);
    gate["outcome"]=pass?"ALLOW":"REJECT";if(!logger.Write("LEVEL_GATE",death,gate))return false;
    if(rule!="NONE"&&rule!="MAX_OVERLEVEL")return Error(death,"INVALID_LEVEL_GATE");
    if(!pass){Fields end;end["reason"]="LEVEL_GATE_REJECT";end["created"]="0";logger.Write("DROP_END",death,end);return mode=="LIVE_TEST"||mode=="OFFICIAL";}
    Quarantine q;FILE* manifest=fopen(QuarantinePath(),"rb");std::string error;bool good=q.Load(manifest,error);if(manifest)fclose(manifest);if(!good)return Error(death,error.c_str());if(!PlanQuarantineAllows(snap,q,c,error)){
        Fields blocked;blocked["reason"]=error;blocked["action"]=NativeDropRelease::Active()?"NATIVE_HOLD_NO_NPC_DROP_POLICY_FALLBACK":"CURRENT_NORMAL_ENGINE_RETAINED";blocked["template"]=Number(snap.templateId);
        blocked["npc_quarantined"]=q.npcs.find(snap.templateId)!=q.npcs.end()?"1":"0";
        for(int qs=0;qs<4;++qs){std::string path;if(Resolve(snap,qs,path)&&q.tables.find(path)!=q.tables.end())blocked[std::string("blocked_slot_")+Number(qs)]=path;}
        logger.Write("QUARANTINE_DETAIL",death,blocked);return Error(death,error.c_str());}
    // Original NPC/table quarantine has already passed above. Event profiles
    // replace only the immutable per-entity slot filenames, never RNG or rules.
    const bool eventDropProfile=PhongThanEventApplyDropProfile(index,npc.m_dwID,snap.slots);
    if(eventDropProfile){
        Fields event;for(int es=0;es<4;++es)event[std::string("slot_")+Number(es)]=snap.slots[es];
        if(!logger.Write("EVENT_PINNED_DROP_PROFILE",death,event))return false;
    }
    if(c.profile==PROFILE_UNBOUND)return Error(death,"PROFILE_UNBOUND");
    LegacyDropNativeLoader loader;Table tables[2];
    for(int k=0;k<2;++k){int slot=(c.profile==PROFILE_A?0:2)+k;std::string path,bytes;
        if(c.allowInactiveMissingSlots&&(k?snap.treasure1:snap.treasure)==0&&(snap.slots[slot].empty()||snap.slots[slot]=="0"))continue;
        bool eventHandled=false;bool loaded=Resolve(snap,slot,path);if(loaded){loaded=PhongThanEventLoadTable(path,bytes,error,eventHandled);if(!eventHandled)loaded=loader.Load(path,bytes,error);}
        if(!loaded)return Error(death,error.empty()?"SLOT_PATH_INVALID":error.c_str());
        tables[k]=Parse(bytes);if(!tables[k].errors.empty())return Error(death,tables[k].errors[0].c_str());
        Fields bind;bind["slot"]=Number(slot);bind["path"]=path;bind["bytes"]=Number(bytes.size());
        // Byte-level FNV diagnostic; immutable input SHA-256 is in deployment receipt.
        unsigned long fingerprint=2166136261UL;for(size_t bi=0;bi<bytes.size();++bi)fingerprint=(fingerprint^(unsigned char)bytes[bi])*16777619UL;
        bind["fnv32"]=Number(fingerprint);if(!logger.Write("DROP_BIND",death,bind))return false;
        for(size_t si=0;si<tables[k].sections.size();++si){Fields fields=tables[k].sections[si].fields;fields["slot"]=Number(slot);fields["section"]=tables[k].sections[si].name;
            if(!logger.Write("TABLE_SECTION",death,fields))return false;}
        int mr=-1,ms=-1;if(!Get(Find(tables[k],"Main"),"MoneyRate",mr)||!Get(Find(tables[k],"Main"),"MoneyScale",ms)||(moneyRule=="ZERO_ONLY"&&(mr||ms)))return Error(death,"NONZERO_MONEY_UNSUPPORTED");
    }
    static LegacyDropItemAdapter adapter;static bool adapterReady=false;if(!adapterReady)adapterReady=adapter.Init(ItemGen);
    if(!adapterReady)return Error(death,"NATIVE_ADAPTER_INIT_FAILED");
    LegacyDropNativeCatalog catalog(adapter,ItemGen);Plan plan;if(!plan.Prepare(snap,q,c,tables[0],tables[1],catalog,error)){
        Fields f;f["reason"]=error;f["slot"]=Number(plan.RejectedSlot());f["section"]=Number(plan.RejectedRow());f["action"]=NativeDropRelease::Active()?"NATIVE_HOLD_NO_NPC_DROP_POLICY_FALLBACK":"CURRENT_NORMAL_ENGINE_RETAINED";logger.Write("DROP_ERROR",death,f);return false;}
    int seedInt=0;if(!Int(ini,"Main","Seed",seedInt)||seedInt<=0)return Error(death,"INVALID_SEED");
    unsigned long seed=(unsigned long)seedInt^snap.entity^sequence;Random rng(seed);Fields seedFields;seedFields["seed"]=Number(seed);seedFields["rng"]="XORSHIFT32_REJECTION_BOUNDED";
    if(!logger.Write("RNG_BEGIN",death,seedFields))return false;
    Result result;Trace trace(death);if(!plan.Draw(rng,result,error,&trace)||!trace.ok)return Error(death,"DRAW_OR_LOG_WRITE_FAILED");
    int liveRate=PhongThanLiveDropRate(SubWorld[world].m_SubWorldID);
    if(!PhongThanAppendLiveDropBonus(plan,liveRate,seed,c.maxItems,result,error))return Error(death,"EVENT_DRAW_FAILED");
    if(result.money>=999999999)return Error(death,"MONEY_NATIVE_GROUND_LIMIT");
    if(mode=="SHADOW"){Fields end;end["reason"]=result.items.empty()?"ALL_ROLLS_MISS_OR_ZERO_TREASURE":"SHADOW_NO_SIDE_EFFECTS";end["selected"]=Number(result.items.size());end["created"]="0";logger.Write("DROP_END",death,end);return false;}
    if(!selfTest){
        if(owner<=0||owner>=MAX_PLAYER||!PhongThanRewardOwnerAvailable(Player[owner].m_nNetConnectIdx,Player[owner].m_bIsBot!=0,Player[owner].m_PlayerDBLoad!=0))return Error(death,"INVALID_REWARD_OWNER");
        int pn=Player[owner].m_nIndex;if(pn<=0||pn>=MAX_NPC||!Npc[pn].IsPlayer()||Npc[pn].m_SubWorldIndex!=world)return Error(death,"REWARD_OWNER_WORLD_MISMATCH");
    }
    // Allocate and validate ALL items before the first ground side effect.
    // Use the same original-boss registry as random-blue promotion. If that
    // registry is unavailable, preserve drops rather than risk changing bosses.
    const bool bossRegistryReady=RandomBlueState::Refresh();
    const bool normalRareScope=NativeDropRelease::Active()&&NormalRareEquipment::Eligible(
        bossRegistryReady,
        RandomBlueState::Data().bosses.find(snap.templateId)!=RandomBlueState::Data().bosses.end(),
        npc.m_nOwnerIdx!=0||PhongThanSummonTracked(index),
        npc.m_ActionScriptID!=0||customDropScript!=0,
        special,snap.treasure,snap.treasure1,eventDropProfile);
    Random rareRng(seed^0x6e726531UL); // Separate stream; base table/boss RNG untouched.
    std::vector<Pending> pending;PendingGuard cleanup(pending,death);pending.reserve(result.items.size());
    for(size_t i=0;i<result.items.size();++i){const Intent& intent=result.items[i];int which=intent.slot%2;char rowName[32];sprintf(rowName,"%d",intent.row);
        const Section* row=Find(tables[which],rowName);int genre,detail,particular;
        if(!Get(row,"Genre",genre)||!Get(row,"Detail",detail)||!Get(row,"Particular",particular)){RemovePending(pending,death);return Error(death,"INTERNAL_ROW_BIND_FAILED");}
        PhongThanQuestItemTuple tuple;if(!adapter.Map(genre,detail,particular,tuple)){RemovePending(pending,death);return Error(death,"TUPLE_MAP_FAILED");}tuple.level=intent.level;
        Fields magic;magic["slot"]=Number(intent.slot);magic["section"]=Number(intent.row);magic["rule"]="NATIVE_CATALOG_ONLY";magic["raw_MagicRate"]=Find(tables[which],"Main")->fields.find("MagicRate")->second;
        magic["rng_draw"]="NOT_APPLICABLE";magic["requested_magic_levels"]="0,0,0,0,0,0";magic["reason"]=P2BlueScope()?"CATALOG_BASE_THEN_CONFIGURED_BLUE_LAYER":"EXPERIMENTAL_no_additional_random_magic_layer";
        if(!logger.Write("MAGIC_ROLL",death,magic)){RemovePending(pending,death);return false;}
        Pending item;item.item=0;item.slot=intent.slot;item.row=intent.row;
        unsigned long nativeSeed=g_GetRandomSeed();
        bool injected=selfTest&&Read(ini,"Main","SelfTestGeneratorFailAfter")=="1"&&!pending.empty();
        bool made=!injected&&adapter.GenerateMapped(tuple,item.item,error);if(injected)error="SELF_TEST_GENERATOR_FAILURE";
        // Establish RAII ownership before allocating log strings. reserve above
        // makes this POD push nonallocating, including low-memory error paths.
        if(made){item.serial=Item[item.item].GetID();pending.push_back(item);}
        if(made){int rawRate=0;Get(Find(tables[which],"Main"),"MagicRate",rawRate);P2BlueDecorate(Item[item.item],snap.templateId,seed,intent.slot,intent.row,death,logger,rawRate);}
        if(made&&normalRareScope&&Item[item.item].GetGenre()==item_equip){
            const int quality=Item[item.item].GetPhongThanQuality();
            const unsigned long draw=rareRng.Below(10000);
            const bool keep=NormalRareEquipment::Keep(true,true,quality,draw);
            Fields rare;rare["serial"]=Number(item.serial);rare["quality"]=Number(quality);
            rare["keep_per_10000"]=Number(NormalRareEquipment::KeepRate(quality));
            rare["draw"]=Number(draw);rare["result"]=keep?"KEEP":"SUPPRESS";
            if(!logger.Write("NORMAL_RARE_EQUIPMENT",death,rare))return false;
            if(!keep){ItemSet.Remove(item.item);pending.pop_back();continue;}
        }
        Fields f;f["slot"]=Number(intent.slot);f["section"]=Number(intent.row);f["genre"]=Number(genre);f["detail"]=Number(detail);f["particular"]=Number(particular);f["native_detail"]=Number(tuple.detail);f["native_particular"]=Number(tuple.particular);f["level"]=Number(tuple.level);f["native_seed"]=Number(nativeSeed);f["result"]=made?"CREATED":error;
        if(made){item.serial=Item[item.item].GetID();f["serial"]=Number(item.serial);f["item_index"]=Number(item.item);f["quality"]=Number(Item[item.item].GetQuality());f["set_id"]=Number(Item[item.item].GetSetID());int lines=0;
            int required=0;for(int rq=0;rq<6;++rq){const KItemNormalAttrib& a=*(KItemNormalAttrib*)Item[item.item].GetRequirement(rq);char key[32];sprintf(key,"require_%d",rq);f[key]=Number(a.nAttribType)+":"+Number(a.nValue[0]);if(a.nAttribType==magic_requirelevel&&a.nValue[0]>required)required=a.nValue[0];}
            f["actual_native_level"]=Number(Item[item.item].GetLevel());f["required_player_level"]=Number(required);
            for(int m=0;m<MAX_ITEM_MAGICATTRIB;++m){const KItemNormalAttrib& a=Item[item.item].m_aryMagicAttrib[m];if(a.nAttribType)++lines;
                char key[32];sprintf(key,"magic_%d",m);f[key]=Number(a.nAttribType)+":"+Number(a.nValue[0])+","+Number(a.nValue[1])+","+Number(a.nValue[2]);}
            f["magic_lines"]=Number(lines);f["actual_generator_seed"]=Number(Item[item.item].GetItemParam()->uRandomSeed);}
        if(!logger.Write("ITEM_CREATE",death,f)||!made){RemovePending(pending,death);return Error(death,made?"LOG_WRITE_FAILED":injected?"SELF_TEST_GENERATOR_FAILURE":"GENERATOR_REJECT");}
    }
    int created=0,failed=0;
    committed=true;
    for(i=0;i<pending.size();++i){Pending& item=pending[i];int object=Ground(item.item,world,x,y,owner);Fields f;f["serial"]=Number(item.serial);f["object"]=Number(object);f["owner"]=Number(owner>0?owner:-1);f["slot"]=Number(item.slot);f["section"]=Number(item.row);
        if(object>0){++created;f["result"]="SPAWNED";f["world_object_id"]=Number(Object[object].m_nID);Tracked t;t.death=death;t.serial=item.serial;t.object=object;t.worldId=Object[object].m_nID;t.born=GetTickCount();t.cleanup=selfTest;tracked[item.serial]=t;if(selfTest)selfObjects.push_back(object);}
        else{++failed;ItemSet.Remove(item.item);f["result"]="GROUND_REJECT_ITEM_REMOVED";}
        item.item=0;if(!logger.Write("GROUND_SPAWN",death,f)){RemovePending(pending,death);Error(death,"GROUND_LOG_WRITE_FAILED","LEGACY_SELECTED_NO_FALLBACK");return true;}
    }
    if(result.money>0){
        POINT pos;pos.x=x;pos.y=y;SubWorld[world].GetFreeObjPos(pos);KMapPos map;map.nSubWorld=world;
        SubWorld[world].Mps2Map(pos.x,pos.y,&map.nRegion,&map.nMapX,&map.nMapY,&map.nOffX,&map.nOffY);
        int object=-1;if(map.nRegion>=0&&map.nRegion<SubWorld[world].m_nTotalRegion)object=ObjSet.AddMoneyObj(map,result.money);
        Fields cash;cash["amount"]=Number(result.money);cash["owner"]=Number(owner);cash["object"]=Number(object);
        cash["result"]=object>0?"SPAWNED":"GROUND_REJECT";cash["kind"]="MONEY";
        if(object>0){Object[object].SetItemBelong(owner>0?owner:-1);cash["world_object_id"]=Number(Object[object].m_nID);MoneyTracked t;t.death=death;t.object=object;t.worldId=Object[object].m_nID;t.amount=result.money;moneyTracked[t.worldId]=t;}
        logger.Write("GROUND_SPAWN",death,cash);
    }
    Fields end;end["selected"]=Number(result.items.size());end["created"]=Number(created);end["ground_failed"]=Number(failed);end["reason"]=failed?"GROUND_FAILURE":created?"LIVE_ITEMS_SPAWNED":"ALL_ROLLS_MISS_OR_ZERO_TREASURE";logger.Write("DROP_END",death,end);
    return true; // LIVE owns this death even on MISS; never run a second normal engine.
}
bool LegacyDropOnDeath(KNpc& npc,int owner,int attacker){
    bool official=NativeDropRelease::Active()&&!npc.IsPlayer()&&npc.m_Kind==kind_normal;
    committed=false;try{bool handled=LegacyDropOnDeathImpl(npc,owner,attacker);
        if(official&&!handled){int n=npc.m_Index;if(n>0&&n<MAX_NPC){Life& l=lives[n];if(l.death.empty())l.death=NewDeath(l.generation);l.consumed=true;Fields f;f["template"]=Number(npc.m_NpcSettingIdx);f["reason"]="NATIVE_REJECTED_OR_DISABLED";f["action"]="NO_NPC_DROP_POLICY_FALLBACK";logger.Write("DROP_HOLD",l.death,f);}}
        return NativeDropRelease::OwnsNormalDrop(official,true,handled);
    }catch(...){
        try{if(npc.m_Index>0&&npc.m_Index<MAX_NPC)Error(lives[npc.m_Index].death,"NATIVE_EXECUTION_EXCEPTION",committed?"LEGACY_SELECTED_NO_FALLBACK":"CURRENT_NORMAL_ENGINE_RETAINED");}catch(...){}
        return committed||official;
    }
}
void LegacyDropNormalEngineDispatch(int npc){
    if(npc<=0||npc>=MAX_NPC||lives[npc].death.empty())return;
    try{Fields f;f["engine"]="NEW_PROJECT_RULES_V1";f["meaning"]="DISPATCH_NOT_PROOF_OF_ITEM_OUTPUT";logger.Write("NORMAL_ENGINE_DISPATCH",lives[npc].death,f);}catch(...){}
}
void LegacyDropHit(int index,int attacker,int damage){
    if(index<=0||index>=MAX_NPC||!IsLoose())return;LocalConfig ini;if(!LoadConfig(ini)||Read(ini,"Main","Mode")=="OFF"||Read(ini,"Main","HitDetails")!="1")return;
    char section[40];sprintf(section,"NPC_%d",Npc[index].m_NpcSettingIdx);if(Read(ini,section,"Enabled")!="1")return;
    Life& l=lives[index];if(l.entity!=Npc[index].m_dwID){l.entity=Npc[index].m_dwID;l.death="";l.consumed=false;l.hits=0;}
    if(l.hits++>=10)return;if(l.death.empty())l.death=NewDeath(l.generation);
    Fields f;f["npc_instance"]=Number(Npc[index].m_dwID);f["npc_index"]=Number(index);f["template"]=Number(Npc[index].m_NpcSettingIdx);f["attacker"]=Number(attacker);f["damage"]=Number(damage);f["hit_limit"]="10_per_life";logger.Write("NPC_HIT",l.death,f);
}
void LegacyDropLua(int npc,unsigned long script,const char* phase,int result){
    bool begin=strstr(phase,"_BEGIN")!=0;std::string death;
    if(begin){if(npc>0&&npc<MAX_NPC)death=lives[npc].death;luaDeaths.push_back(death);}
    else if(!luaDeaths.empty()){death=luaDeaths.back();luaDeaths.pop_back();}
    if(death.empty())return;Fields f;f["script_id"]=Number(script);f["phase"]=phase;f["result"]=Number(result);f["meaning"]="CALLBACK_OBSERVATION_not_reward_proof";logger.Write("LUA_REWARD",death,f);
}
void LegacyDropLuaItem(const char* api,int item,int object){
    if(luaDeaths.empty()||luaDeaths.back().empty())return;Fields f;f["api"]=api;f["item_index"]=Number(item);f["object"]=Number(object);
    if(item>0&&item<MAX_ITEM){f["serial"]=Number(Item[item].GetID());f["genre"]=Number(Item[item].GetGenre());f["detail"]=Number(Item[item].GetDetailType());f["particular"]=Number(Item[item].GetParticularType());f["level"]=Number(Item[item].GetLevel());}
    logger.Write("LUA_REWARD",luaDeaths.back(),f);
}
void LegacyDropPickup(unsigned long serial,int player,int object,const char* result){
    std::map<unsigned long,Tracked>::iterator i=tracked.find(serial);if(i==tracked.end())return;
    Fields f;f["serial"]=Number(serial);f["player"]=Number(player);f["object"]=Number(object);f["result"]=result;logger.Write("ITEM_PICKUP",i->second.death,f);
    if(!strcmp(result,"PICKED_UP"))tracked.erase(i);
}
bool LegacyDropMoneyIsTracked(int object){return object>0&&object<MAX_OBJECT&&Object[object].m_nKind==Obj_Kind_Money&&moneyTracked.find(Object[object].m_nID)!=moneyTracked.end();}
void LegacyDropMoneyPickup(int object,int player,const char* result){
 if(!LegacyDropMoneyIsTracked(object))return;std::map<int,MoneyTracked>::iterator it=moneyTracked.find(Object[object].m_nID);
 Fields f;f["kind"]="MONEY";f["amount"]=Number(it->second.amount);f["world_object_id"]=Number(it->second.worldId);f["player"]=Number(player);f["result"]=result;
 if(player>0&&player<MAX_PLAYER)f["wallet"]=Number(Player[player].m_ItemList.GetMoney(room_equipment));
 logger.Write("ITEM_PICKUP",it->second.death,f);if(!strcmp(result,"PICKED_UP"))moneyTracked.erase(it);
}
void LegacyDropTick(){
    static DWORD next=0;DWORD now=GetTickCount();if((long)(now-next)<0)return;next=now+1000;if(!IsLoose())return;
    // Bounded by the world's live ground item population, prune on release.
    for(std::map<unsigned long,Tracked>::iterator it=tracked.begin();it!=tracked.end();){
        Tracked t=it->second;bool alive=t.object>0&&t.object<MAX_OBJECT&&Object[t.object].m_nID==t.worldId&&Object[t.object].m_nItemDataID>0&&Object[t.object].m_nItemDataID<MAX_ITEM&&Item[Object[t.object].m_nItemDataID].GetID()==t.serial;
        if(!alive){Fields f;f["serial"]=Number(t.serial);f["reason"]=t.cleanup?"SELF_TEST_CLEANUP":"EXPIRED_OR_REMOVED_WITHOUT_PICKUP";logger.Write("GROUND_REMOVE",t.death,f);std::map<unsigned long,Tracked>::iterator old=it++;tracked.erase(old);}else ++it;
    }
    for(std::map<int,MoneyTracked>::iterator mt=moneyTracked.begin();mt!=moneyTracked.end();){int o=mt->second.object;if(o<=0||o>=MAX_OBJECT||Object[o].m_nID!=mt->first||Object[o].m_nKind!=Obj_Kind_Money){std::map<int,MoneyTracked>::iterator gone=mt++;moneyTracked.erase(gone);}else ++mt;}
    if(NativeDropRelease::Active()){
        LocalConfig status;bool loaded=LoadConfig(status);std::string mode=loaded?Read(status,"Main","Mode"):"CONFIG_BLOCKED";
        static std::string lastMode;if(lastMode!=mode){Fields f;f["mode"]=mode;f["status"]=(mode=="OFFICIAL"&&NativeDropRollout::Active())?"READY":mode=="OFF"?"OFF":"CONFIG_BLOCKED";f["scope"]="ALL_NORMAL_NPCS";f["old_policy"]="NPC_DROP_POLICY_DISABLED_NO_FALLBACK";f["config"]="settings/phongthan/NativeDrop.ini";logger.Write("NATIVE_DROP_STATUS","",f);lastMode=mode;}
    }
    if(NativeDropRelease::Active())RandomBlueState::Refresh();
    P2BlueStatus(logger);
    if(NativeDropRollout::Active())return; // Main never runs isolated support/GM or automated death commands.
    if(P2BlueScope()){P2TestCharacterSetup(logger);return;} // No automated spawn/death.
    LocalConfig ini;if(!LoadConfig(ini)||Read(ini,"Main","SelfTestEnabled")!="1"||Read(ini,"Main","Mode")=="OFF")return;
    const char* request="legacy_drop_selftest.request";FILE* f=fopen(request,"rb");if(!f)return;char nonce[65];int got=fscanf(f,"%64s",nonce);fclose(f);if(got!=1||!MoveFileA(request,"legacy_drop_selftest.running"))return;
    Fields run;run["nonce"]=nonce;run["scope"]="REAL_GAMESERVER_SPAWN_DEATH_UNOWNED_GROUND";logger.Write("SELF_TEST_BEGIN","",run);
    int anchor=0;for(int n=1;n<MAX_NPC;++n)if(Npc[n].m_dwID&&Npc[n].m_RegionIndex>=0&&Npc[n].m_SubWorldIndex>=0&&Npc[n].m_SubWorldIndex<MAX_SUBWORLD){anchor=n;break;}
    if(!anchor){Error("","SELF_TEST_NO_LOADED_REGION","NO_WORLD_SIDE_EFFECTS");DeleteFileA("legacy_drop_selftest.running");return;}
    int x,y;Npc[anchor].GetMpsPos(&x,&y);int world=Npc[anchor].m_SubWorldIndex;
    const int ids[2]={2506,125},playerLevels[5]={10,20,21,80,80},npcLevels[5]={10,10,10,10,80};
    const bool equipmentTest=Read(ini,"Main","SelfTestEquipment18")=="1";
    const int equipmentLevels[5]={10,20,40,80,100};
    int repetitions=1;if(equipmentTest){if(!Int(ini,"Main","SelfTestRepetitions",repetitions)||repetitions<1||repetitions>100){Error("","SELF_TEST_INVALID_REPETITIONS");DeleteFileA("legacy_drop_selftest.running");return;}}
    int spawned=0;selfTest=true;
    for(int repeat=0;repeat<repetitions;++repeat)for(int a=0;a<(equipmentTest?1:2);++a)for(int b=0;b<5;++b){selfPlayerLevel=playerLevels[b];int n=NpcSet.Add(MAKELONG(equipmentTest?equipmentLevels[b]:npcLevels[b],equipmentTest?18:ids[a]),world,x,y,FALSE);if(n<=0){Error("","SELF_TEST_SPAWN_FAILED","NO_NORMAL_DROP");continue;}
        unsigned long entity=Npc[n].m_dwID,originalDeathScript=Npc[n].m_DeathScriptID;++spawned;Npc[n].m_ProcessAI=0;Npc[n].m_DeathScriptID=Npc[n].m_ActionScriptID=Npc[n].m_DropScriptID=Npc[n].m_TimerScriptID=0;
        // Controlled negative test on one newly-created entity. Use its real
        // registered script ID; no callback is called with the unowned test kill.
        if(Read(ini,"Main","SelfTestCustomScript")=="1"&&a==1&&b==0&&originalDeathScript&&g_GetScript(originalDeathScript))Npc[n].m_DropScriptID=originalDeathScript;
        LegacyDropHit(n,0,1);Npc[n].DoDeath(0,0);Npc[n].DoDeath(0,0); // actual duplicate guard
        if(Npc[n].m_dwID==entity&&Npc[n].m_RegionIndex>=0)NpcSet.Remove(n);
    }
    selfTest=false;
    for(size_t o=0;o<selfObjects.size();++o){int obj=selfObjects[o];if(obj>0&&obj<MAX_OBJECT)Object[obj].Remove(FALSE);}selfObjects.clear();
    run["spawned"]=Number(spawned);run["heap"]=_heapchk()==_HEAPOK?"HEAPOK":"HEAP_ERROR";run["pickup"]="NOT_TESTED_NO_CONNECTED_TEST_PLAYER";run["quest_callbacks"]="SUPPRESSED_ON_ISOLATED_SELFTEST_ENTITIES_ONLY";logger.Write("SELF_TEST_END","",run);
    f=fopen("legacy_drop_selftest.result","wb");if(f){fprintf(f,"nonce=%s spawned=%d heap=%d pickup=NOT_TESTED\n",nonce,spawned,_heapchk());fclose(f);}DeleteFileA("legacy_drop_selftest.running");
}
#endif
