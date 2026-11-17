#include "PhongThanSoulRuntime.h"
// Equipment skills never become permanently learned character skills.
// Copy-on-write descriptors keep aura private to each item's current state.
bool PhongThanSoulCharge(int actor,int skill,int level){
 if(actor<=0||actor>=MAX_NPC||!Npc[actor].IsPlayer())return true;
 int p=Npc[actor].GetPlayerIdx();if(p<=0||p>=MAX_PLAYER)return false;
 int idx=Npc[actor].m_SkillList.FindSame(skill);
 if(!idx)return true;int equipment=Npc[actor].m_SkillList.EquipmentLevel(skill);
 if(equipment<=Npc[actor].m_SkillList.m_Skills[idx].SkillLevel)return true;
 level=min(level,equipment);
 int chosen=0,best=0;PT_SOUL_STATE state;
 for(int slot=0;slot<itempart_num;++slot){int n=Player[p].m_ItemList.m_EquipItem[slot];if(n<=0||n>=MAX_ITEM||(slot==itempart_horse&&!Npc[actor].m_bRideHorse))continue;
  const KBASICPROP_EQUIPMENT* original=ItemGen.CatalogEquipment(Item[n].GetDetailType(),Item[n].GetRow());
  if(original&&original->m_nActivateSkill==skill&&original->m_nLevel>=level)return true;
  PT_SOUL_STATE candidate;if(!PtSoulFromParam(Item[n].GetParam(),candidate))continue;const PTSoulFormula* f=PtSoulFormula(candidate.Formula);
  if(!f||f->skill!=skill||f->level<level||candidate.Aura<f->cost||Npc[actor].m_Level<f->requiredLevel||Player[p].m_cProfession.GetProfession()!=f->profession)continue;
  if(!chosen||f->level>best){chosen=n;best=f->level;state=candidate;}
 }
 if(!chosen)return false;
 const PTSoulFormula* f=PtSoulFormula(state.Formula);state.Aura-=f->cost;int param;
 if(!PtSoulReplace(Item[chosen].GetParam(),state,param))return false;
 Item[chosen].SetParam(param);if(Item[chosen].GetParam()!=param)return false;
 Player[p].m_uMustSave=SAVE_REQUEST;Player[p].m_ItemList.SyncItem(chosen);Player[p].UpdataCurData();return true;
}
int PhongThanSoulUseStone(int p,int item){
 if(item<=0||item>=MAX_ITEM||Item[item].GetGenre()!=6||Item[item].GetDetailType()!=1||Item[item].GetParticularType()!=587||Item[item].GetLevel()!=1)return -1;
 if(!PhongThanIsLivePlayer(p)||!PtPetCarried(p,item)||Player[p].GetLockState()||Player[p].CheckTrading()||Npc[Player[p].m_nIndex].m_CurrentLife<=0||(Item[item].GetExpireTime()>0&&(unsigned)Item[item].GetExpireTime()<=KSG_GetCurSec()))return 0;
 return PhongThanSoulOpen(p,9,Item[item].GetID());
}
