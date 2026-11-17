#include "KCore.h"
#include "KPakList.h"
#include "PhongThanConsumableData.h"
#include "LegacyDropNativeCatalog.h"
#include "LegacyDropNativeLoader.h"
#include "NativeDropCompatibility.h"
#include "NativeDropRelease.h"
#include "BlueMagicTestPolicy.h"
#include <malloc.h>
KPakList* g_pPakList=0;
using namespace LegacyDrop;using namespace LegacyDrop::Execution;
std::string ReadBytes(const std::string& path){FILE*f=fopen(path.c_str(),"rb");if(!f)return "";std::string s;char b[8192];size_t n;while((n=fread(b,1,sizeof(b),f)))s.append(b,n);fclose(f);return s;}
std::string Unhex(const std::string& hex){std::string s;for(size_t i=0;i+1<hex.size();i+=2){char b[3]={hex[i],hex[i+1],0};s+=(char)strtol(b,0,16);}return s;}
int main(int argc,char**argv){
 if(argc!=5||!SetCurrentDirectoryA(argv[1]))return 2;
 int failed=0,checks=0;
 for(int official=0;official<2;++official)for(int eligible=0;eligible<2;++eligible)for(int handled=0;handled<2;++handled){
  ++checks;if(NativeDropRelease::OwnsNormalDrop(official!=0,eligible!=0,handled!=0)!=(handled||(official&&eligible)))++failed;
 }
 ++checks;if(RuntimeProfile("NPC_STATE",0,"A","B")!=PROFILE_A||RuntimeProfile("NPC_STATE",1,"A","B")!=PROFILE_B||RuntimeProfile("NPC_STATE",2,"A","B")!=PROFILE_A)++failed;
 std::string blueConfig=ReadBytes(std::string(argv[1])+"\\settings\\phongthan\\NativeDropBlue.ini");
 if(blueConfig.empty())blueConfig=ReadBytes(std::string(argv[1])+"\\settings\\phongthan\\BlueMagicExperimental.ini");
 Table bc=Parse(blueConfig);const Section*bs=Find(bc,"BlueMagicExperimental");if(!bs)return 8;
 std::string be;BlueMagic::BlueMagicTableLoader magic;BlueMagic::BlueLimits limits;
 if(!magic.Parse(ReadBytes(std::string(argv[1])+Unhex(bs->fields.find("MagicPathHex")->second)),be)||!limits.Parse(ReadBytes(std::string(argv[1])+Unhex(bs->fields.find("LimitPathHex")->second)),be)){printf("BLUE_TEST_SOURCE_ERROR %s\n",be.c_str());return 9;}
 limits.version=0;BlueMagic::Config blue;blue.mode=BlueMagic::Config::LIVE_TEST;blue.npc=555;blue.version=0;blue.group=0;blue.chance=100000;blue.lines=1;blue.uniqueType=blue.blankElementUnrestricted=blue.attributeLevelIsTier=true;
 BlueMagic::BlueMagicTestPolicy bluePlan;
 ++checks;if(bluePlan.PrepareWorld(magic,limits,blue,555,0,1,0,be))++failed;
 blue.allowAllWorldNpcs=true;++checks;if(!bluePlan.PrepareWorld(magic,limits,blue,555,0,1,0,be))++failed;
 ++checks;if(bluePlan.Prepare(magic,limits,blue,555,0,1,0,be))++failed;
 g_SetRootPath(argv[1]);g_SetFilePath("\\");KPakList packs;if(!packs.Open("\\package.ini"))return 3;g_pPakList=&packs;g_SetPakFileMode(1);if(!ItemGen.Init())return 4;
 ItemSet.Init();LegacyDropItemAdapter adapter;if(!adapter.Init(ItemGen))return 5;LegacyDropNativeCatalog catalog(adapter,ItemGen);LegacyDropNativeLoader loader;std::string error;
 FILE*f=fopen(argv[2],"rb");Quarantine q;if(!f||!q.Load(f,error))return 6;fclose(f);
 FILE*cases=fopen(argv[3],"rb"),*out=fopen(argv[4],"w");if(!cases||!out)return 7;
 fprintf(out,"npc,probe_level,profile,ready,error,slot,row\n");
 int id,tr,tr1,total=0,ready=0;char slots[4][512];
 const int levels[]={20,80,120};
 while(fscanf(cases,"%d %d %d %511s %511s %511s %511s",&id,&tr,&tr1,slots[0],slots[1],slots[2],slots[3])==7){
  NpcSnapshot n;n.templateId=id;n.treasure=tr;n.treasure1=tr1;for(int k=0;k<4;++k)n.slots[k]=(!strcmp(slots[k],"-")||!strcmp(slots[k],"0"))?"":slots[k];
  for(int lv=0;lv<3;++lv)for(int profile=0;profile<2;++profile){
   Config c;c.profile=(Profile)profile;c.treasure=SUCCESS_CAP;c.level=EXPERIMENTAL_MONSTER_EQUIPMENT_CATALOG;c.money=SYNTHETIC_FLAT_BERNOULLI;c.magic=EXPERIMENTAL_NATIVE_CATALOG_ONLY;c.npcLevel=levels[lv];c.equipmentRequirementAhead=10;c.maxItems=64;c.allowInactiveMissingSlots=c.selectedProfileQuarantine=true;
   Plan plan;bool ok=Build(n,q,c,loader,catalog,plan,error);++total;if(ok)++ready;
   fprintf(out,"%d,%d,%c,%d,%s,%d,%d\n",id,levels[lv],profile?'B':'A',ok?1:0,error.c_str(),plan.RejectedSlot(),plan.RejectedRow());
  }
 }
 fclose(cases);fclose(out);
 // Missing active slots must reject; explicitly absent inactive slots need no fake INI.
 NpcSnapshot missing;missing.templateId=99999;missing.treasure=missing.treasure1=0;
 Config c;c.profile=PROFILE_A;c.treasure=SUCCESS_CAP;c.level=EXPERIMENTAL_MONSTER_EQUIPMENT_CATALOG;c.money=SYNTHETIC_FLAT_BERNOULLI;c.magic=EXPERIMENTAL_NATIVE_CATALOG_ONLY;c.npcLevel=20;c.allowInactiveMissingSlots=c.selectedProfileQuarantine=true;
 Plan plan;++checks;if(!Build(missing,q,c,loader,catalog,plan,error))++failed;
 missing.treasure=1;++checks;if(Build(missing,q,c,loader,catalog,plan,error))++failed;
 missing.treasure=0;c.allowInactiveMissingSlots=false;++checks;if(Build(missing,q,c,loader,catalog,plan,error))++failed;
 c.allowInactiveMissingSlots=true;missing.slots[0]="0";++checks;if(!Build(missing,q,c,loader,catalog,plan,error))++failed;
 missing.treasure=1;++checks;if(Build(missing,q,c,loader,catalog,plan,error))++failed;
 missing.treasure=0;missing.slots[0]="../bad.ini";++checks;if(Build(missing,q,c,loader,catalog,plan,error))++failed;
 std::string unit=std::string(argv[4])+".quarantine-unit";FILE*held=fopen(unit.c_str(),"w+b");if(!held)return 10;fputs("V1\nT \\settings\\item\\droprate\\bad_b.ini\n",held);rewind(held);Quarantine selective;if(!selective.Load(held,error))return 11;fclose(held);
 NpcSnapshot scoped;scoped.templateId=99998;scoped.slots[0]="good_a.ini";scoped.slots[2]="bad_b.ini";c.selectedProfileQuarantine=true;c.profile=PROFILE_A;++checks;if(!PlanQuarantineAllows(scoped,selective,c,error))++failed;
 c.profile=PROFILE_B;++checks;if(PlanQuarantineAllows(scoped,selective,c,error))++failed;
 c.profile=PROFILE_A;c.selectedProfileQuarantine=false;++checks;if(PlanQuarantineAllows(scoped,selective,c,error))++failed;
 ++checks;if(ItemSet.GetItemNumber()||_heapchk()!=_HEAPOK)++failed;
 printf("OFFICIAL_PREFLIGHT_RESULT checks=%d failures=%d plans=%d ready=%d held=%d world_spawn_death=0 item_generation=0\n",checks,failed,total,ready,total-ready);
 g_pPakList=0;return failed?1:0;
}
