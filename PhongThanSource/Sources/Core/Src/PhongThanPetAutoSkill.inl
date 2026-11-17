#include "../../../Headers/PhongThanStatusPages.h"
static int s_PetShortcut[9];
void PhongThanSetPetShortcut(int slot,int skill){if(slot>=0&&slot<9)s_PetShortcut[slot]=skill>0&&skill<MAX_SKILL?skill:0;}
static bool PhongThanPetAutoSkillTick(){
 PT_STATUS_PAGES_STATE state;
 if(!PhongThanGetStatusPages(&state)||!state.Pet.Owned||!(state.Pet.Flags&PT_PET_AUTO_SKILL)||
    (state.Pet.Flags&PT_PET_REST)||state.Pet.Strength<104400||state.Pet.AgeSeconds<90000)return false;
 int actor=Player[CLIENT_PLAYER_INDEX].m_nIndex;
 if(actor<=0||actor>=MAX_NPC||Npc[actor].m_Doing!=do_stand||Player[CLIENT_PLAYER_INDEX].GetLockState()||Player[CLIENT_PLAYER_INDEX].CheckTrading())return false;
 static DWORD last=0;static int cursor=0;DWORD now=IR_GetCurrentTime();
 if(now-last<1000)return false;last=now;
 for(int scanned=0;scanned<9;++scanned){
  int slot=cursor++%9,id=s_PetShortcut[slot];if(!id)continue;
  int level=Npc[actor].m_SkillList.GetCurrentLevel(id);if(level<=0)continue;
  ISkill* skill=g_SkillManager.GetSkill(id,level);if(!skill)continue;
  // Shortcut contents never authorize attacking somebody on the player's behalf.
  if(skill->IsTargetEnemy()||skill->IsTargetObj())continue;
  if(skill->IsAura()){
   int profession=Player[CLIENT_PLAYER_INDEX].m_cProfession.m_nProfession;
   DWORD interval=profession==0?3000:1000;
   static DWORD aura=0;if(now-aura<interval)continue;
   Player[CLIENT_PLAYER_INDEX].SetRightSkill(id);aura=now;return true;
  }
  if(skill->GetSkillStyle()!=SKILL_SS_InitiativeNpcState||(!skill->IsTargetSelf()&&!skill->IsTargetAlly())||Npc[actor].IsNpcStateExist(id))continue;
  if(PhongThanSummonAutoCast(actor,id,actor))return true;
 }
 return false;
}
