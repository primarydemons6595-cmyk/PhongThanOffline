#include "PhongThanBotCombatTarget.inl"
static bool CompanionCombatOrder(Runtime& r,Training& t,DWORD now){
 CompanionState& c=companions[&r-bots];if(!PTBotCombatCommand(c.Mode))return false;
 KPlayer& p=Player[r.Player];KNpc& actor=Npc[p.m_nIndex];int target=c.Target;
 if(target>0&&target<MAX_NPC&&Npc[target].m_dwID==c.TargetEntity&&Npc[target].m_CurrentLife<=0){CompanionNotice(r,"target_down","\115\364\143\040\164\151\252\165\040\256\267\040\147\364\143\040\162\345\151\056\040\124\165\151\040\161\165\141\171\040\154\271\151\040\164\150\145\157\040\253\156\147\056",now);if(c.PreviousCamp&&PhongThanBotRestorePk(r.Player,c.PreviousCamp))c.PreviousCamp=0;c.Mode=PTBC_FOLLOW;c.Target=0;c.TargetEntity=0;return false;}
 if(!CompanionTargetType(r,target,c.Mode)||Npc[target].m_dwID!=c.TargetEntity){target=CompanionResolveTarget(r,c);if(!target){CompanionNotice(r,"target_required","\124\165\151\040\143\150\255\141\040\170\270\143\040\256\336\156\150\040\256\255\356\143\040\256\363\156\147\040\155\364\143\040\164\151\252\165\056\040\244\156\147\040\156\343\151\040\162\342\040\164\252\156\040\147\151\363\160\040\164\165\151\040\156\150\320\056",now);t.Path.clear();actor.SendCommand(do_stand);lstrcpyA(r.State,"awaiting_target");return true;}c.Target=target;c.TargetEntity=Npc[target].m_dwID;}
 int map=SubWorld[actor.m_SubWorldIndex].m_SubWorldID,dest=SubWorld[Npc[target].m_SubWorldIndex].m_SubWorldID;c.TargetMap=dest;Npc[target].GetMpsPos(&c.TargetX,&c.TargetY);
 t.Target=target;t.Identity=c.TargetEntity;t.Map=dest;t.Level=actor.m_Level;r.GoalEntity=c.TargetEntity;r.GoalMap=dest;r.GoalLevel=Npc[target].m_Level;
 if(map!=dest){if(BotTryTravel(r,dest,now))return true;int edge=-1;if(BotRoute(r,map,dest,&edge)<1||edge<0){CompanionNotice(r,"target_unreachable","\124\165\151\040\143\150\255\141\040\143\343\040\256\255\352\156\147\040\150\157\306\143\040\160\150\357\040\160\150\357\040\150\356\160\040\256\323\040\164\355\151\040\256\343\056",now);t.Path.clear();lstrcpyA(r.State,"path_blocked");return true;}lstrcpyA(r.State,"boss_travel");MoveTraining(r,t,PTBotEdges[edge].X,PTBotEdges[edge].Y,now);return true;}
 if(c.Mode>=PTBC_PLAYER&&NpcSet.GetRelation(p.m_nIndex,target)!=relation_enemy){
  if(!c.PreviousCamp)c.PreviousCamp=actor.m_CurrentCamp;
  if(!PhongThanBotRequestPk(r.Player,8)||NpcSet.GetRelation(p.m_nIndex,target)!=relation_enemy){CompanionNotice(r,"pk_rules_blocked","\124\165\151\040\143\150\255\141\040\256\270\156\150\040\256\255\356\143\040\155\364\143\040\164\151\252\165\040\156\265\171\056\040\120\113\054\040\164\346\040\256\351\151\040\150\157\306\143\040\153\150\165\040\166\371\143\040\256\141\156\147\040\143\150\306\156\056",now);t.Path.clear();actor.SendCommand(do_stand);lstrcpyA(r.State,"pk_blocked");return true;}
 }
 if(actor.m_FightMode!=enumFightActive)actor.SetFightMode(enumFightActive);
 bool hit=c.LastLife>0&&actor.m_CurrentLife<c.LastLife;c.LastLife=actor.m_CurrentLife;int distance=NpcSet.GetDistance(p.m_nIndex,target),radius=actor.m_CurrentAttackRadius>32?actor.m_CurrentAttackRadius:32;
 BotUseRecovery(r,now,(__int64)actor.m_CurrentLife*100<(__int64)actor.m_CurrentLifeMax*65,actor.m_CurrentManaMax>0&&actor.m_CurrentMana<actor.m_CurrentManaMax/3);
 if(hit&&c.Mode>=PTBC_PLAYER)c.RetreatUntil=now+2500;
 bool critical=(__int64)actor.m_CurrentLife*100<(__int64)actor.m_CurrentLifeMax*25;
 bool kite=(int)(c.RetreatUntil-now)>0&&distance<radius+192;
 if(critical||kite){int x=0,y=0;actor.GetMpsPos(&x,&y);lstrcpyA(r.State,"retreating");MoveTraining(r,t,x+(x>=c.TargetX?192:-192),y+(y>=c.TargetY?192:-192),now,32);return true;}
 if(c.Mode>=PTBC_PLAYER&&distance>radius+192)c.RetreatUntil=0;
 if(c.Mode==PTBC_BOSS&&Npc[target].m_Level>actor.m_Level+5&&BotOwnedRecoveryCount(p,1)<1&&actor.m_CurrentLife<actor.m_CurrentLifeMax/2){CompanionNotice(r,"need_medicine","\102\157\163\163\040\155\271\156\150\040\161\165\270\054\040\164\165\151\040\150\325\164\040\164\150\165\350\143\040\155\270\165\040\162\345\151\056\040\244\156\147\040\256\255\141\040\164\150\165\350\143\040\147\151\363\160\040\164\165\151\040\256\267\056",now);t.Path.clear();actor.SendCommand(do_stand);return true;}
 BotFightTarget(r,t,now);return true;
}
