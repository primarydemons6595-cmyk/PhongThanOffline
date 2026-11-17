#include "../../../Headers/PhongThanCombatLog.h"
#ifdef _SERVER
// Private combat history goes only to the two participants (or summon owners).
// Spectators and socketless bot-vs-bot combat produce no history traffic.
static int PTCombatOwner(int actor){
 if(actor<=0||actor>=MAX_NPC)return 0;
 int owner=Npc[actor].IsPlayer()?actor:PtSummonOwner(actor);
 if(owner<=0||owner>=MAX_NPC||!Npc[owner].IsPlayer())return 0;
 int p=Npc[owner].GetPlayerIdx();
 return p>0&&p<MAX_PLAYER&&Player[p].m_PlayerDBLoad&&!Player[p].m_bIsBot&&Player[p].m_nNetConnectIdx>=0?p:0;
}
void PhongThanCombatLog(int attacker,int target,int kind,int type,int amount,int critical){
 if(!g_pServer||attacker<=0||attacker>=MAX_NPC||target<=0||target>=MAX_NPC||amount<0||type<0||type>=damage_num)return;
 KNpc& a=Npc[attacker];KNpc& b=Npc[target];int world=b.m_SubWorldIndex;
 if(world<0||world>=MAX_SUBWORLD||a.m_SubWorldIndex!=world||!a.m_dwID||!b.m_dwID)return;
 int recipients[2]={PTCombatOwner(attacker),PTCombatOwner(target)};
 if(!recipients[0]&&!recipients[1])return;
 PTCombatLogEvent event;ZeroMemory(&event,sizeof(event));
 PhongThanInitializeWireHeader(&event.Header,PT_COMBAT_LOG_EVENT,sizeof(event),PHONGTHAN_WIRE_FLAG_RESPONSE,0);
 event.MapId=SubWorld[world].m_SubWorldID;event.Attacker=a.m_dwID;event.Target=b.m_dwID;
 event.Amount=amount;event.Kind=kind;event.DamageType=type;event.Critical=critical?1:0;
 const char* name=GetNpcNetworkDisplayName(a.Name);g_StrCpyLen(event.AttackerName,name?name:a.Name,sizeof(event.AttackerName));
 name=GetNpcNetworkDisplayName(b.Name);g_StrCpyLen(event.TargetName,name?name:b.Name,sizeof(event.TargetName));
 for(int r=0;r<2;++r){int p=recipients[r];if(!p||(r&&p==recipients[0]))continue;
  event.Recipient=Npc[Player[p].m_nIndex].m_dwID;
  event.Direction=r==0?1:2;
  g_pServer->PackDataToClient(Player[p].m_nNetConnectIdx,&event,sizeof(event));
 }
}
#endif
