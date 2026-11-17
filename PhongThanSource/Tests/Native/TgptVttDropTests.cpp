#include "KCore.h"
#include "KPakList.h"
#include "KBasPropTbl.h"
#include "PhongThanQuestItemTuple.h"
#include <stdio.h>
#include <assert.h>
KPakList* g_pPakList=0;
class TgptDropAccess{
public:KLibOfBPT lib;
 const KLibOfBPT& Catalog()const{return lib;}
 const KBASICPROP_EQUIPMENT* CatalogEquipment(int k,int r)const{switch(k){case 0:return lib.GetMeleeWeaponRecord(r);case 1:return lib.GetRangeWeaponRecord(r);case 2:return lib.GetArmorRecord(r);case 3:return lib.GetRingRecord(r);case 4:return lib.GetAmuletRecord(r);case 5:return lib.GetBootRecord(r);case 6:return lib.GetBeltRecord(r);case 7:return lib.GetHelmRecord(r);case 8:return lib.GetCuffRecord(r);case 9:return lib.GetPendantRecord(r);case 10:return lib.GetHorseRecord(r);}return 0;}
 int CatalogEquipmentCount(int k)const{switch(k){case 0:return lib.GetMeleeWeaponRecordNumber();case 1:return lib.GetRangeWeaponRecordNumber();case 2:return lib.GetArmorRecordNumber();case 3:return lib.GetRingRecordNumber();case 4:return lib.GetAmuletRecordNumber();case 5:return lib.GetBootRecordNumber();case 6:return lib.GetBeltRecordNumber();case 7:return lib.GetHelmRecordNumber();case 8:return lib.GetCuffRecordNumber();case 9:return lib.GetPendantRecordNumber();case 10:return lib.GetHorseRecordNumber();}return 0;}
};
class TgptDropAdapter{public:bool Map(int g,int d,int p,PhongThanQuestItemTuple& t)const{return PhongThanMapQuestItemIdentity(g,d,p,t);}};
// Keep production catalog validation and plan code, replacing only its data
// access interfaces with the actual standalone KLibOfBPT loader above.
#define PT_LEGACY_DROP_ITEM_ADAPTER_H
#define LegacyDropItemAdapter TgptDropAdapter
#define KItemGenerator TgptDropAccess
#include "LegacyDropNativeCatalog.h"
#undef KItemGenerator
#undef LegacyDropItemAdapter
static std::string Read(const std::string& p){FILE* f=fopen(p.c_str(),"rb");if(!f)return "";std::string d;char b[4096];int n;while((n=fread(b,1,sizeof(b),f))>0)d.append(b,n);fclose(f);return d;}
int main(int argc,char** argv){
 assert(argc==4&&SetCurrentDirectoryA(argv[1]));SetErrorMode(SEM_FAILCRITICALERRORS|SEM_NOGPFAULTERRORBOX);g_SetRootPath(0);g_SetFilePath("\\");g_SetPakFileMode(0);KPakList packs;assert(packs.Open("\\package.ini"));g_pPakList=&packs;TgptDropAccess access;assert(access.lib.Init());TgptDropAdapter adapter;LegacyDropNativeCatalog catalog(adapter,access);int failures=0;
 for(int arena=1;arena<=4;++arena)for(int chest=0;chest<2;++chest){
  char rel[120];sprintf(rel,"/settings/item/droprate/pt_tgpt_%s%d.ini",chest?"baoxiang":"tongtian",arena);std::string data=Read(std::string(argv[2])+rel);if(data.empty())data=Read(std::string(argv[3])+rel);
  LegacyDrop::Table first=LegacyDrop::Parse(Read(std::string(argv[3])+"/settings/item/droprate/npcdroprate-boss1.ini")),second=LegacyDrop::Parse(data);LegacyDrop::NpcSnapshot npc;npc.templateId=chest?(arena==1?410:411+arena):128+arena*5;npc.entity=1;npc.treasure=chest?30:(arena==1?20:arena==2?40:arena==3?50:80);npc.treasure1=6;npc.slots[0]="\\settings\\item\\droprate\\npcdroprate-boss1.ini";npc.slots[1]=std::string("\\settings\\item\\droprate\\")+"pt_tgpt_"+(chest?"baoxiang":"tongtian")+char('0'+arena)+".ini";
  LegacyDrop::Execution::Config c;c.profile=LegacyDrop::Execution::PROFILE_A;c.treasure=LegacyDrop::Execution::SUCCESS_CAP;c.level=LegacyDrop::Execution::EXPERIMENTAL_MONSTER_EQUIPMENT_CATALOG;c.money=LegacyDrop::Execution::SYNTHETIC_FLAT_BERNOULLI;c.magic=LegacyDrop::Execution::EXPERIMENTAL_NATIVE_CATALOG_ONLY;c.npcLevel=20+20*arena;c.equipmentRequirementAhead=10;c.maxItems=64;
  LegacyDrop::Quarantine q;LegacyDrop::Execution::Plan plan;std::string error;FILE* quarantine=fopen((std::string(argv[2])+"/settings/phongthan/NativeDropQuarantine.txt").c_str(),"rb");if(!quarantine)quarantine=fopen((std::string(argv[3])+"/settings/phongthan/NativeDropQuarantine.txt").c_str(),"rb");assert(quarantine&&q.Load(quarantine,error));fclose(quarantine);
  if(!plan.Prepare(npc,q,c,first,second,catalog,error)){printf("FAIL %s slot=%d row=%d error=%s\n",rel,plan.RejectedSlot(),plan.RejectedRow(),error.c_str());++failures;}
  else printf("PASS %s native plan, level=%d\n",rel,c.npcLevel);
 }
 g_pPakList=0;return failures?1:0;
}
