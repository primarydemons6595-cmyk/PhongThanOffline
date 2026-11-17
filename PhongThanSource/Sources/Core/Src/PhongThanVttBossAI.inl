// Only the four mission bosses use this selector. Never inherit a stale target
// from a recycled NPC slot or rely on a skill at level one being castable.
bool KNpcAI::ProcessVttBoss()
{
#ifndef _SERVER
 return false;
#else
 KNpc& boss=Npc[m_nIndex];int world=boss.m_SubWorldIndex;
 if(world<0||world>=MAX_SUBWORLD)return false;
 int id=SubWorld[world].m_SubWorldID-1066;
 if(id<1||id>4||boss.m_NpcSettingIdx!=133+(id-1)*5)return false;
 if(g_TaskGlobalValue[id]!=2)return true;
 if(boss.m_Doing==do_attack||boss.m_Doing==do_magic||boss.m_Doing==do_skill)return true;
 int target=0,best=boss.m_VisionRadius;
 for(int p=1;p<MAX_PLAYER;++p){
  int n=Player[p].m_nIndex;
  if(!Player[p].m_dwID||n<=0||n>=MAX_NPC||Npc[n].m_SubWorldIndex!=world||!Npc[n].IsAlive()||Npc[n].m_FightMode==enumFightNone)continue;
  if(NpcSet.GetRelation(m_nIndex,n)!=relation_enemy||CheckNpc(n))continue;
  int distance=NpcSet.GetDistance(m_nIndex,n);if(distance<best){best=distance;target=n;}
 }
 if(!target&&boss.m_nPeopleIdx>0&&boss.m_nPeopleIdx<MAX_NPC){
  int n=boss.m_nPeopleIdx;
  if(Npc[n].m_SubWorldIndex==world&&!CheckNpc(n)&&NpcSet.GetRelation(m_nIndex,n)==relation_enemy&&InEyeshot(n))target=n;
 }
 boss.m_nPeopleIdx=target;if(!target)return true;
 // Rotate through the authored four slots, skipping unavailable skill levels.
 int first=(SubWorld[world].m_dwCurrentTime/20)%4;
 for(int attempt=0;attempt<4;++attempt){
  if(!boss.SetActiveSkill(1+(first+attempt)%4))continue;
  ISkill* skill=boss.GetActiveSkill();if(!skill||skill->IsAura()||!boss.m_SkillList.CanCast(boss.m_ActiveSkillID,SubWorld[world].m_dwCurrentTime))continue;
  int distance=NpcSet.GetDistance(m_nIndex,target);
  if(distance<=boss.m_CurrentAttackRadius){
   if(skill->IsTargetSelf()&&!skill->IsTargetEnemy()){int x,y;boss.GetMpsPos(&x,&y);boss.SendCommand(do_skill,boss.m_ActiveSkillID,x,y);}
   else boss.SendCommand(do_skill,boss.m_ActiveSkillID,-1,target);
  }else{int x,y;Npc[target].GetMpsPos(&x,&y);boss.SendCommand(do_walk,x,y);}
  return true;
 }
 return true;
#endif
}
