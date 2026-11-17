// Included only by LegacyDropRuntime.cpp; no item generation or global RNG use.
#include "RandomBlueNpcPolicy.h"
#include "PhongThanSummon.h"
namespace RandomBlueState {
RandomBlueNpc::Life life[MAX_NPC];
struct Settings {
 bool ready,enabled,spawn,respawn;int chance,maxTreasure,seed;DWORD next;
 std::string bytes;Random rng;std::map<int,bool> bosses;
 Settings():ready(false),enabled(false),spawn(false),respawn(false),chance(0),maxTreasure(1),seed(0),next(0),rng(1){}
};
Settings& Data(){static Settings s;return s;}
bool Refresh(){
 Settings& c=Data();DWORD now=GetTickCount();if(c.next&&(long)(now-c.next)<0)return c.ready;c.next=now+1000;
 FILE*f=fopen("settings\\phongthan\\RandomBlueNpc.ini","rb");char b[8192];size_t n=f?fread(b,1,sizeof(b),f):0;bool ok=f&&ferror(f)==0&&n<sizeof(b);if(f)fclose(f);
 std::string bytes=ok?std::string(b,n):"";if(c.bytes==bytes&&c.seed)return c.ready;
 c.bytes=bytes;c.ready=c.enabled=false;Table parsed=Parse(bytes);const Section* s=Find(parsed,"RandomBlueNpc");int enabled=0,spawn=0,respawn=0,version=0,seed=0;
 bool syntax=true;for(size_t e=0;e<parsed.errors.size();++e)if(parsed.errors[e]!="INVALID_COUNT"&&parsed.errors[e]!="INVALID_RANGE"&&parsed.errors[e].substr(0,19)!="UNEXPECTED_SECTION:")syntax=false;
 if(syntax&&s&&Get(s,"Version",version)&&version==1&&Get(s,"Enabled",enabled)&&(enabled==0||enabled==1)&&
   P2Blue::Field(s,"Scope")=="ALL_NORMAL"&&Get(s,"ChancePerMillion",c.chance)&&c.chance>=0&&c.chance<=1000000&&
   Get(s,"OnInitialSpawn",spawn)&&(spawn==0||spawn==1)&&Get(s,"OnRespawn",respawn)&&(respawn==0||respawn==1)&&
   Get(s,"MaxTreasure",c.maxTreasure)&&c.maxTreasure>=0&&c.maxTreasure<=256&&Get(s,"Seed",seed)&&seed>0){
  if(!c.seed||c.seed!=seed){c.rng=Random((unsigned long)seed^GetCurrentProcessId()^now);c.seed=seed;}
  c.enabled=enabled!=0;c.spawn=spawn!=0;c.respawn=respawn!=0;c.ready=true;
  c.bosses.clear();KTabFile bosses;if(bosses.Load("\\settings\\phongthan\\OriginalBossState.txt"))for(int row=2;row<=bosses.GetHeight();++row){int id=-1;bosses.GetInteger(row,"Template",-1,&id);if(id>=0)c.bosses[id]=true;}else{c.ready=false;c.seed=0;}
 }
 Fields fields;fields["status"]=c.ready?(c.enabled?"READY":"OFF"):"CONFIG_BLOCKED";fields["scope"]="ALL_NORMAL";fields["chance_per_million"]=Number(c.chance);fields["rng_stream"]="INDEPENDENT_RANDOM_BLUE_NPC_V1";fields["stats"]="BASE_STATS_UNCHANGED";logger.Write("RANDOM_BLUE_STATUS","",fields);return c.ready;
}
bool DropReady(KNpc& npc,std::string& error){
 LocalConfig ini;if(!LoadConfig(ini)||Read(ini,"Main","Mode")!="OFFICIAL"){error="NATIVE_DROP_NOT_READY";return false;}
 char section[48];sprintf(section,"NPC_%d",npc.m_NpcSettingIdx);
 if(Read(ini,section,"Enabled")!="1"||Read(ini,section,"Profile")!="NPC_STATE"||Read(ini,section,"NormalProfile")!="A"||Read(ini,section,"BlueProfile")!="B"||
   Read(ini,section,"TreasureRule")!="SUCCESS_CAP"||Read(ini,section,"LevelRule")!="MONSTER_EQUIPMENT_CATALOG"||Read(ini,section,"MagicRule")!="NATIVE_CATALOG_ONLY"||Read(ini,section,"MoneyRule")!="FLAT_BERNOULLI"){error="RANDOM_BLUE_NATIVE_CONTRACT_MISMATCH";return false;}
 KNpcTemplate*t=npc.m_NpcSettingIdx>=0&&npc.m_NpcSettingIdx<MAX_NPCSTYLE?g_pNpcTemplate[npc.m_NpcSettingIdx]:0;if(!t||!t->m_bHaveLoadedFromTemplate){error="NPC_TEMPLATE_NOT_READY";return false;}
 NpcSnapshot n;n.templateId=npc.m_NpcSettingIdx;n.entity=npc.m_dwID;n.treasure=t->m_LegacyTreasure;n.treasure1=t->m_LegacyTreasure1;for(int k=0;k<4;++k)n.slots[k]=t->m_LegacyDropFiles[k];
 Config c;c.allowInactiveMissingSlots=c.selectedProfileQuarantine=true;c.treasure=SUCCESS_CAP;c.level=EXPERIMENTAL_MONSTER_EQUIPMENT_CATALOG;c.magic=EXPERIMENTAL_NATIVE_CATALOG_ONLY;c.money=SYNTHETIC_FLAT_BERNOULLI;c.maxMoney=1000000;c.npcLevel=npc.m_Level;
 if(!Int(ini,section,"MaxItems",c.maxItems)||!Int(ini,section,"MaxTrials",c.maxTrials)||!Int(ini,section,"EquipmentMaxRequirementAhead",c.equipmentRequirementAhead)){error="NATIVE_BUDGET_CONFIG_INVALID";return false;}
 Quarantine q;FILE*f=fopen(QuarantinePath(),"rb");bool valid=q.Load(f,error);if(f)fclose(f);if(!valid)return false;
 static LegacyDropItemAdapter adapter;static bool initialized=false;if(!initialized)initialized=adapter.Init(ItemGen);if(!initialized){error="NATIVE_ADAPTER_NOT_READY";return false;}
 LegacyDropNativeCatalog catalog(adapter,ItemGen);LegacyDropNativeLoader loader;Plan plan;
 // Promotion needs only the destination B. An unrelated A error never blocks B.
 c.profile=PROFILE_B;return Build(n,q,c,loader,catalog,plan,error);
}
}
void LegacyDropRandomBlue(KNpc& npc,bool respawn){
 if(npc.m_Index<=0||npc.m_Index>=MAX_NPC||!npc.m_dwID||npc.m_Kind!=kind_normal||npc.m_RegionIndex<0||npc.m_SubWorldIndex<0||npc.m_SubWorldIndex>=MAX_SUBWORLD||(!respawn&&(npc.m_Doing==do_death||npc.m_Doing==do_revive)))return;
 RandomBlueNpc::Life& state=RandomBlueState::life[npc.m_Index];if(!respawn&&state.entity==npc.m_dwID&&state.seen)return;
 if(!NativeDropRelease::Active())return;
 try{
  if(npc.m_ActionScriptID||npc.m_DropScriptID||npc.m_nOwnerIdx||PhongThanSummonTracked(npc.m_Index))state.assigned=false;
  int base=npc.m_btSpecial;if(!state.Begin(npc.m_dwID,base,respawn,base))return;bool changed=npc.m_btSpecial!=base;npc.m_btSpecial=base;
  RandomBlueState::Settings& c=RandomBlueState::Data();
  bool loaded=RandomBlueState::Refresh();
  KNpcTemplate*t=npc.m_NpcSettingIdx>=0&&npc.m_NpcSettingIdx<MAX_NPCSTYLE?g_pNpcTemplate[npc.m_NpcSettingIdx]:0;
  if(loaded&&c.enabled&&(respawn?c.respawn:c.spawn)&&t&&
    RandomBlueNpc::Eligible(true,npc.m_nOwnerIdx!=0||PhongThanSummonTracked(npc.m_Index),npc.m_ActionScriptID!=0||npc.m_DropScriptID!=0,c.bosses.find(npc.m_NpcSettingIdx)!=c.bosses.end(),base,t->m_LegacyTreasure,t->m_LegacyTreasure1,c.maxTreasure)){
   unsigned long draw=c.rng.Below(1000000);Fields f;f["template"]=Number(npc.m_NpcSettingIdx);f["npc_instance"]=Number(npc.m_dwID);f["npc_index"]=Number(npc.m_Index);f["npc_level"]=Number(npc.m_Level);int x=0,y=0;npc.GetMpsPos(&x,&y);f["map_id"]=Number(SubWorld[npc.m_SubWorldIndex].m_SubWorldID);f["x"]=Number(x);f["y"]=Number(y);f["generation"]=Number(lives[npc.m_Index].generation);f["phase"]=respawn?"RESPAWN":"INITIAL_ACTIVATION";f["chance_per_million"]=Number(c.chance);f["draw"]=Number(draw);f["rng_stream"]="INDEPENDENT_RANDOM_BLUE_NPC_V1";
   std::string error;
   if(!RandomBlueNpc::Selected(c.chance,draw))f["outcome"]="NORMAL";
   else if(!NativeDropRollout::Active()||!RandomBlueState::DropReady(npc,error)){f["outcome"]="NORMAL_PROFILE_B_BLOCKED";f["reason"]=error.empty()?"PERSISTENCE_ADMISSION_FAILED":error;}
   else{npc.m_btSpecial=npc_blue;state.assigned=true;changed=true;f["outcome"]="BLUE";f["profile"]="B";}
   if(!logger.Write("RANDOM_BLUE_ROLL","",f)&&state.assigned){npc.m_btSpecial=base;state.assigned=false;changed=true;}
  }
  if(changed){npc.SendSyncData(0,TRUE);Fields f;f["npc_instance"]=Number(npc.m_dwID);f["special"]=Number(npc.m_btSpecial);f["packet"]="PHONGTHAN_NPC_SNAPSHOT";logger.Write("RANDOM_BLUE_SYNC","",f);}
 }catch(...){try{Fields f;f["template"]=Number(npc.m_NpcSettingIdx);f["reason"]="RANDOM_BLUE_EXCEPTION";logger.Write("RANDOM_BLUE_ERROR","",f);}catch(...){}}
}
