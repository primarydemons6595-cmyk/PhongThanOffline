#ifndef PT_P2_BLUE_DROP_RUNTIME_H
#define PT_P2_BLUE_DROP_RUNTIME_H
#include "BlueMagicTestPolicy.h"
#include "LegacyDropDebugLogger.h"
#include "NativeDropCompatibility.h"
#include "NativeDropRolloutScope.h"
// Additional blue layer only. Does not own drop selection, levels or legacy RNG.
inline bool P2BlueScope(){
 if(NativeDropRollout::Active())return true;
 char root[MAX_PATH],p0[MAX_PATH],p1[MAX_PATH],exe[MAX_PATH],cwd[MAX_PATH];
 DWORD n=GetEnvironmentVariableA("PHONGTHAN_P2_BLUE_ROOT",root,sizeof(root));
 if(!n||n>=sizeof(root)||!strstr(root,"\\PhongThanRuntime-Loose\\P0-BlueMagic-Isolated\\p1_"))return false;
 if(!GetEnvironmentVariableA("PHONGTHAN_P0_MAGIC_ROOT",p0,sizeof(p0))||_stricmp(p0,root)||!GetEnvironmentVariableA("PHONGTHAN_P1_BLUE_ROOT",p1,sizeof(p1))||_stricmp(p1,root))return false;
 std::string marker=std::string(root)+"\\p2_player_test.flag";if(GetFileAttributesA(marker.c_str())==(DWORD)-1)return false;
 marker=std::string(root)+"\\p0_isolated.flag";if(GetFileAttributesA(marker.c_str())==(DWORD)-1)return false;
 std::string expected=std::string(root)+"\\Server";
 if(!GetModuleFileNameA(0,exe,sizeof(exe))||!GetCurrentDirectoryA(sizeof(cwd),cwd))return false;
 char* slash=strrchr(exe,'\\');if(!slash||_stricmp(slash+1,"GameServer.exe"))return false;*slash=0;
 return !_stricmp(exe,expected.c_str())&&!_stricmp(cwd,exe);
}
namespace P2Blue {
inline std::string Num(long n){char b[40];sprintf(b,"%ld",n);return b;}
struct State {
 BlueMagic::Config config;BlueMagic::BlueMagicTableLoader table;BlueMagic::BlueLimits limits;
 bool enabled,ready,upToLines;unsigned long seed;std::string signature,error;
 State():enabled(false),ready(false),upToLines(false),seed(0){}
};
inline State& Data(){static State s;return s;}
inline bool Unhex(const std::string& value,std::string& out){out="";if(value.size()<2||value.size()>510||value.size()%2)return false;for(size_t i=0;i<value.size();i+=2){unsigned byte;char part[3]={value[i],value[i+1],0};if(!isxdigit((unsigned char)part[0])||!isxdigit((unsigned char)part[1])||sscanf(part,"%x",&byte)!=1||byte==0)return false;out+=(char)byte;}return true;}
inline std::string Field(const LegacyDrop::Section* section,const char* key){if(!section)return "";LegacyDrop::Fields::const_iterator it=section->fields.find(key);return it==section->fields.end()?"":it->second;}
inline bool Refresh(){
 State& s=Data();FILE* f=fopen(NativeDropRelease::BluePath(),"rb");std::string bytes;
 if(f){char b[8192];size_t n=fread(b,1,sizeof(b),f);bool bad=ferror(f)!=0;fclose(f);if(!bad&&n<sizeof(b))bytes.assign(b,n);}
 std::string digest;BlueMagic::Sha256(bytes,digest);if(s.signature==digest)return false;s.signature=digest;s.ready=false;s.enabled=false;s.error="BLUE_CONFIG_INVALID";
 LegacyDrop::Table parsed=LegacyDrop::Parse(bytes);const LegacyDrop::Section* section=LegacyDrop::Find(parsed,"BlueMagicExperimental");int enabled=-1;
 if(!LegacyDrop::Integer(Field(section,"Enabled"),enabled)||(enabled!=0&&enabled!=1))return true;
 if(!enabled){s.error="OFF";return true;}s.enabled=true;
 bool official=NativeDropRelease::Active();
 BlueMagic::Config c;c.allowAllWorldNpcs=official&&Field(section,"NpcAllowlist")=="ALL";c.allowBoss86=Field(section,"NpcAllowlist")=="0,7,11,15,18,86";c.mode=BlueMagic::Config::LIVE_TEST;int seed=0;
 if((Field(section,"Mode")!=(official?"OFFICIAL":"LIVE_TEST"))||(!c.allowAllWorldNpcs&&Field(section,"NpcAllowlist")!="0,7,11,15,18"&&Field(section,"NpcAllowlist")!="0,7,11,15,18,86")||Field(section,"DuplicateAttributeRule")!="UNIQUE_TYPE"||Field(section,"BlankElementRule")!="UNRESTRICTED"||Field(section,"AttributeLevelRule")!="AT_MOST_ITEM_TIER")return true;
 if(!LegacyDrop::Integer(Field(section,"LimitTableVersion"),c.version)||!LegacyDrop::Integer(Field(section,"MagicWeightGroup"),c.group)||!LegacyDrop::Integer(Field(section,"BlueChancePerMillion"),c.chance)||!LegacyDrop::Integer(Field(section,"MagicLineCount"),c.lines)||!LegacyDrop::Integer(Field(section,"Seed"),seed)||seed<=0||c.chance<0||c.chance>1000000||c.lines<1||c.lines>6||c.group<0||c.group>2||(c.version!=0&&c.version!=1))return true;
 std::string magicPath,limitPath;
 if(!Unhex(Field(section,"MagicPathHex"),magicPath)||!Unhex(Field(section,"LimitPathHex"),limitPath))return true;
 if(!s.table.Load(magicPath,s.error)||!s.limits.Load(limitPath,s.error))return true;
 if(s.table.sha256!=Field(section,"MagicSha256")||s.limits.sha256!=Field(section,"LimitSha256")||s.limits.version!=c.version){s.error="BLUE_SOURCE_SHA_OR_VERSION_MISMATCH";return true;}
 std::string lineRule=Field(section,"LineCountRule");if(!lineRule.empty()&&lineRule!="EXACT"&&lineRule!="UP_TO_CONFIG_AND_CATALOG_LIMIT")return true;s.upToLines=lineRule=="UP_TO_CONFIG_AND_CATALOG_LIMIT";
 c.uniqueType=c.blankElementUnrestricted=c.attributeLevelIsTier=true;s.config=c;s.seed=(unsigned long)seed;s.ready=true;s.error="";return true;
}
}
inline void P2BlueStatus(LegacyDropDebugLogger& logger){
 if(!P2BlueScope()||!P2Blue::Refresh())return;P2Blue::State& s=P2Blue::Data();LegacyDrop::Fields f;
 f["status"]=s.ready?"READY":s.enabled?"CONFIG_BLOCKED":"OFF";f["reason"]=s.error;f["config_sha256"]=s.signature;
 f["mode"]=NativeDropRelease::Active()?"OFFICIAL":"LIVE_TEST";f["npc_allowlist"]=s.config.allowAllWorldNpcs?"ALL":"0,7,11,15,18,86";f["chance_per_million"]=P2Blue::Num(s.config.chance);f["magic_lines"]=P2Blue::Num(s.config.lines);f["weight_group"]=P2Blue::Num(s.config.group);f["limit_version"]=P2Blue::Num(s.config.version);
 f["magic_sha256"]=s.table.sha256;f["limit_sha256"]=s.limits.sha256;f["companion_version"]="1";f["kingsoft_rate"]=NativeDropRelease::Active()?"NOT_CLAIMED_LOOSE_RATE_V1":"UNVERIFIED_EXPERIMENTAL_ONLY";f["automated_spawn_death"]="DISABLED";
 logger.Write("P2_BLUE_STATUS","",f);
}
inline void P2BlueDecorate(KItem& item,int npc,unsigned long dropSeed,int slot,int row,const std::string& death,LegacyDropDebugLogger& logger,int rawMagicRate=1){
 if(!P2BlueScope()||(!NativeDropRelease::Active()&&!LegacyDrop::Execution::CompletionNpc(npc)))return;
 P2BlueStatus(logger);P2Blue::State& s=P2Blue::Data();LegacyDrop::Fields f;
 f["serial"]=P2Blue::Num(item.GetID());f["template"]=P2Blue::Num(npc);f["slot"]=P2Blue::Num(slot);f["section"]=P2Blue::Num(row);f["config_sha256"]=s.signature;
 const char* skip=!s.enabled?"OFF":!s.ready?s.error.c_str():item.GetGenre()!=item_equip?"NON_EQUIPMENT_UNCHANGED":item.GetQuality()!=equip_normal?"CATALOG_MAGIC_PRESERVED":0;
 if(skip){f["reason"]=skip;logger.Write("BLUE_SKIP",death,f);return;}
 BlueMagic::Config c=s.config;c.npc=npc;c.chance=LegacyDrop::Execution::ExperimentalMagicChance(c.chance,rawMagicRate);f["raw_MagicRate"]=P2Blue::Num(rawMagicRate);f["magic_rate_rule"]="EXPERIMENTAL_CONFIG_PPM_TIMES_RAW_CAPPED_1000000";f["requested_lines"]=P2Blue::Num(c.lines);if(c.chance<0){f["reason"]="INVALID_MAGIC_RATE";logger.Write("BLUE_SKIP",death,f);return;}BlueMagic::BlueMagicTestPolicy plan;std::string error;
 bool prepared=plan.PrepareWorld(s.table,s.limits,c,npc,item.GetDetailType(),item.GetLevel(),item.GetSeries(),error);while(!prepared&&s.upToLines&&c.lines>1&&(error=="BLUE_LINE_COUNT_EXCEEDS_LIMIT"||error=="BLUE_INSUFFICIENT_DISTINCT_CANDIDATES")){--c.lines;prepared=plan.PrepareWorld(s.table,s.limits,c,npc,item.GetDetailType(),item.GetLevel(),item.GetSeries(),error);}f["effective_lines"]=P2Blue::Num(c.lines);if(!prepared){f["reason"]=error;logger.Write("BLUE_SKIP",death,f);return;}
 unsigned long seed=s.seed^dropSeed^item.GetID()^0xb10e0001UL;LegacyDrop::Random blueRng(seed);BlueMagic::Result result;
 if(!plan.Draw(blueRng,result,error)){f["reason"]=error;logger.Write("BLUE_SKIP",death,f);return;}
 f["blue_seed"]=P2Blue::Num(seed);f["rng_stream"]="INDEPENDENT_FROM_DROP_AND_NATIVE_GENERATOR";f["chance_per_million"]=P2Blue::Num(c.chance);f["chance_draw"]=P2Blue::Num(result.chanceDraw);f["candidate_count"]=P2Blue::Num(plan.CandidateCount());f["limit"]=P2Blue::Num(plan.LineLimit());f["outcome"]=result.blue?"BLUE":"WHITE";
 f["magic_sha256"]=s.table.sha256;f["limit_sha256"]=s.limits.sha256;f["weight_group"]=P2Blue::Num(c.group);f["limit_version"]=P2Blue::Num(c.version);f["item_level"]=P2Blue::Num(item.GetLevel());f["item_series"]=P2Blue::Num(item.GetSeries());
 for(size_t a=0;a<result.attributes.size();++a){const BlueMagic::Attribute& v=result.attributes[a];std::string key="attribute_"+P2Blue::Num(a);f[key]=P2Blue::Num(v.type)+":"+P2Blue::Num(v.value[0])+","+P2Blue::Num(v.value[1])+","+P2Blue::Num(v.value[2]);f[key+"_source_row"]=P2Blue::Num(v.row);f[key+"_weight_draw"]=P2Blue::Num(v.roll);f[key+"_weight_sum"]=P2Blue::Num(v.totalWeight);}
 if(!logger.Write("MAGIC_ROLL",death,f)||!logger.Write("BLUE_ROLL",death,f)||!result.blue)return;
 KItemNormalAttrib original[MAX_ITEM_MAGICATTRIB],attrs[MAX_ITEM_MAGICATTRIB];memcpy(original,item.m_aryMagicAttrib,sizeof(original));ZeroMemory(attrs,sizeof(attrs));int durability=item.GetDurability();
 for(a=0;a<result.attributes.size();++a){attrs[a].nAttribType=result.attributes[a].type;memcpy(attrs[a].nValue,result.attributes[a].value,sizeof(attrs[a].nValue));}
 bool applied=item.SetMagicAttrib(attrs)!=0;item.SetDurability(durability);
 applied=applied&&item.GetQuality()==equip_magic&&!memcmp(item.m_aryMagicAttrib,attrs,sizeof(attrs));f["result"]=applied?"APPLIED":"REJECTED_WHITE_RETAINED";
 if(!applied||!logger.Write("BLUE_APPLY",death,f)){item.SetMagicAttrib(original);item.SetDurability(durability);}
}
#endif
