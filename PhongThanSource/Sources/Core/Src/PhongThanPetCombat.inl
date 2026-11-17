// Restoration policy: each displayed pet attribute adds one current attribute
// point. Base character values and persisted learned skill levels never change.
static void PtPetRefreshCombat(int player,PT_PET_RUNTIME* r){
#ifndef PHONGTHAN_PET_NO_LUA
 DWORD signature=PtPetCombatSignature(r->Data);
 if(r->CombatSignature==signature)return;
 r->CombatSignature=signature;
 int actor=Player[player].m_nIndex;
 if(actor<=0||actor>=MAX_NPC)return;
 Player[player].UpdataCurData();
 int skills[8];
 for(int i=0;i<4;++i){skills[i]=r->CombatSkills[i];skills[i+4]=r->Data.Skills[i]&65535;r->CombatSkills[i]=skills[i+4];}
 for(i=0;i<8;++i){
  int id=skills[i];if(!id)continue;bool duplicate=false;
  for(int before=0;before<i;++before)if(skills[before]==id)duplicate=true;
  if(duplicate)continue;
  int level=Npc[actor].m_SkillList.GetCurrentLevel(id);
  KSkill* skill=(KSkill*)g_SkillManager.GetSkill(id,level);
  if(!skill||skill->GetSkillStyle()!=SKILL_SS_PassivityNpcState)continue;
  // Native SetStateSkillEffect upgrades levels, but does not downgrade them.
  // Remove the old inverse-state record before recasting the passive.
  KStateNode* node=(KStateNode*)Npc[actor].m_StateSkillList.GetHead();
  while(node){KStateNode* next=(KStateNode*)node->GetNext();
   if(node->m_SkillID==id){
    for(int a=0;a<MAX_SKILL_STATE;++a)if(node->m_State[a].nAttribType>0&&node->m_State[a].nAttribType<magic_normal_end)
     Npc[actor].ModifyAttrib(actor,&node->m_State[a]);
    node->Remove();delete node;
   }node=next;
  }
  skill->Cast(actor,-1,actor);
 }
#endif
}
