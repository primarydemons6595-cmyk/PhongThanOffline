"""Exercise real equipment overlay and soul charge routines with isolated actors."""
from pathlib import Path
import runpy
ROOT=Path(__file__).resolve().parents[2]/'PhongThanSource'
fixture=runpy.run_path(str(ROOT/'Tools/prepare_soul_tests.py'))
s=fixture['prefix'].split('#include "../../Sources/Core/Src/PhongThanSocketRegistry.inl"')[0]
s=s.replace('enum {item_equip=0,pos_builditem=7,SAVE_REQUEST=1};','enum {item_equip=0,pos_builditem=7,SAVE_REQUEST=1,MAX_PLAYER=2,MAX_NPC=3,MAX_ITEM=32,itempart_num=3,itempart_horse=2,MAX_NPCSKILL=16,MAX_SKILL=4096,MAX_SKILLLEVEL=100};')
s=s.replace('int GetParam()const', 'int GetRow()const{return 0;}int GetID(){return serial;}int GetExpireTime(){return 0;}int GetParam()const')
s=s.replace('struct Bag{','struct Bag{int m_EquipItem[3],syncs;void SyncItem(int){++syncs;}')
s=s.replace('struct TestPlayer {Bag m_ItemList;int m_uMustSave;', 'struct TestPlayer {Bag m_ItemList;int m_uMustSave,m_nIndex,updates;bool locked,trading;struct Profession{int n;int GetProfession(){return n;}}m_cProfession;void UpdataCurData();bool GetLockState(){return locked;}bool CheckTrading(){return trading;}')
s+=r'''
struct NPCSKILL{int SkillId,SkillLevel,AddLevel,CurrentSkillLevel;DWORD NextCastTime;};
struct KSkillList {
 NPCSKILL m_Skills[MAX_NPCSKILL];int m_EquipmentLevel[MAX_NPCSKILL],m_EquipmentCooldownId[MAX_NPCSKILL],m_nAllSkillV;DWORD m_EquipmentCooldownTime[MAX_NPCSKILL];
 int FindSame(int id){for(int i=1;i<MAX_NPCSKILL;++i)if(m_Skills[i].SkillId==id)return i;return 0;}
 int FindFree(){for(int i=1;i<MAX_NPCSKILL;++i)if(!m_Skills[i].SkillId)return i;return 0;}
 int EquipmentLevel(int id){int i=FindSame(id);return i?m_EquipmentLevel[i]:0;}
 BOOL SetEquipmentSkills(const int*,const int*,int);
};
struct KNpc {int m_Level,m_bRideHorse,m_CurrentLife;KSkillList m_SkillList;bool IsPlayer(){return true;}int GetPlayerIdx(){return 1;}}Npc[MAX_NPC];
struct KBASICPROP_EQUIPMENT {int m_nActivateSkill,m_nLevel;};
struct Catalog {KBASICPROP_EQUIPMENT row;const KBASICPROP_EQUIPMENT* CatalogEquipment(int,int)const{return &row;}}ItemGen;
bool PhongThanIsLivePlayer(int p){return p==1&&Player[p].m_nIndex==1;}
bool PtPetCarried(int p,int i){return p==1&&i>0&&i<MAX_ITEM&&Player[p].m_ItemList.owned[i];}
DWORD KSG_GetCurSec(){return 1000;}
int PhongThanSoulOpen(int,int,unsigned long){return 1;}
#include "../../Sources/Core/Src/PhongThanSocketRegistry.inl"
#include "../../Sources/Core/Src/PhongThanSoulRegistry.inl"
#include "../../Headers/PhongThanEquipmentSkills.h"
'''
body=(ROOT/'Sources/Core/Src/KSkillList.cpp').read_text('utf8').split('BOOL KSkillList::SetEquipmentSkills(',1)[1].split('int KSkillList::GetLevel(',1)[0]
s+='BOOL KSkillList::SetEquipmentSkills('+body
s+=r'''
void TestPlayer::UpdataCurData(){++updates;PhongThanRefreshEquipmentSkills(m_ItemList.m_EquipItem,itempart_num,Item,MAX_ITEM,ItemGen,Npc[m_nIndex].m_SkillList,Npc[m_nIndex].m_bRideHorse?-1:itempart_horse,Npc[m_nIndex].m_Level,m_cProfession.GetProfession());}
#include "../../Sources/Core/Src/PhongThanSoulRuntime.inl"
static void Reset(){ZeroMemory(Item,sizeof(Item));ZeroMemory(Player,sizeof(Player));ZeroMemory(Npc,sizeof(Npc));ZeroMemory(&ItemGen,sizeof(ItemGen));Player[1].m_nIndex=1;Npc[1].m_Level=200;Item[1].serial=101;Item[1].stack=1;Player[1].m_ItemList.owned[1]=true;Player[1].m_ItemList.m_EquipItem[0]=1;}
static void Equip(const PTSoulFormula& f,int aura){Reset();Player[1].m_cProfession.n=f.profession;PT_SOUL_STATE state;ZeroMemory(&state,sizeof(state));state.Formula=f.id;state.Aura=aura;assert(PtSoulRollMagic(state,0,0)&&PtSoulStore(state,Item[1].param));Player[1].UpdataCurData();}
int main(){
 CreateDirectoryA("settings",0);CreateDirectoryA("settings\\phongthan",0);int tested=0;
 for(int i=0;i<sizeof(ptSoulFormulas)/sizeof(ptSoulFormulas[0]);++i){const PTSoulFormula& f=ptSoulFormulas[i];if(f.detail<0)continue;
  Equip(f,f.capacity);assert(Npc[1].m_SkillList.EquipmentLevel(f.skill)==f.level);int before=Item[1].param;assert(PhongThanSoulCharge(1,f.skill,f.level+10));PT_SOUL_STATE state,old;assert(PtSoulRead(before,old)&&PtSoulFromParam(Item[1].param,state)&&old.Aura==f.capacity&&state.Aura==f.capacity-f.cost&&state.MagicType==old.MagicType&&state.MagicValue==old.MagicValue);
  Equip(f,f.cost);assert(PhongThanSoulCharge(1,f.skill,f.level));assert(PtSoulFromParam(Item[1].param,state)&&state.Aura==0&&Npc[1].m_SkillList.EquipmentLevel(f.skill)==0);
  Equip(f,f.capacity);Player[1].m_ItemList.m_EquipItem[0]=0;Player[1].UpdataCurData();assert(Npc[1].m_SkillList.FindSame(f.skill)==0);
  Equip(f,f.capacity);Npc[1].m_Level=f.requiredLevel-1;Player[1].UpdataCurData();assert(Npc[1].m_SkillList.EquipmentLevel(f.skill)==0);
  Equip(f,f.capacity);Player[1].m_cProfession.n=(f.profession+1)%4;Player[1].UpdataCurData();assert(Npc[1].m_SkillList.EquipmentLevel(f.skill)==0);++tested;
 }
 const PTSoulFormula& f=ptSoulFormulas[0];Equip(f,f.capacity);int slot=Npc[1].m_SkillList.FindSame(f.skill);Npc[1].m_SkillList.m_Skills[slot].SkillLevel=2;int before=Item[1].param;assert(PhongThanSoulCharge(1,f.skill,15)&&Item[1].param==before);Player[1].m_ItemList.m_EquipItem[0]=0;Player[1].UpdataCurData();assert(Npc[1].m_SkillList.m_Skills[slot].SkillLevel==2&&Npc[1].m_SkillList.m_Skills[slot].SkillId==f.skill);
 Equip(f,f.capacity);slot=Npc[1].m_SkillList.FindSame(f.skill);Npc[1].m_SkillList.m_Skills[slot].NextCastTime=9999;Player[1].m_ItemList.m_EquipItem[0]=0;Player[1].UpdataCurData();Player[1].m_ItemList.m_EquipItem[0]=1;Player[1].UpdataCurData();slot=Npc[1].m_SkillList.FindSame(f.skill);assert(Npc[1].m_SkillList.m_Skills[slot].NextCastTime==9999);
 Equip(f,f.capacity);Item[2]=Item[1];Player[1].m_ItemList.m_EquipItem[1]=2;Player[1].UpdataCurData();before=Item[2].param;assert(PhongThanSoulCharge(1,f.skill,f.level));PT_SOUL_STATE state;assert(Item[2].param==before&&PtSoulFromParam(Item[2].param,state)&&state.Aura==f.capacity);
 Equip(f,f.capacity);ItemGen.row.m_nActivateSkill=f.skill;ItemGen.row.m_nLevel=5;Player[1].UpdataCurData();before=Item[1].param;assert(PhongThanSoulCharge(1,f.skill,5)&&Item[1].param==before);
 puts("PASS soul skill overlay/charge: original formula level/profession, pet/add-level cast, depletion, unequip, permanent learned skill retention, cooldown retention, duplicate-item isolation, original native equipment activation");printf("Formula identities tested=%d\n",tested);return 0;
}
'''
(ROOT/'Tests/Native/SoulSkillTests.generated.cpp').write_text(s,'ascii')
print('Prepared actual soul charge and actual SetEquipmentSkills tests')
