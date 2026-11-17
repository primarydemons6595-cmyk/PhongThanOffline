#include <stdio.h>
#include <string.h>
#include <map>
#include "../../Headers/PhongThanBotCommand.h"
enum {MAX_NPC=16,MAX_SUBWORLD=4,MAX_PLAYER=4,kind_normal=0,kind_player=1,kind_dialoger=3,npc_gold=2};
struct Runtime {int Player;}bots[1];
struct CompanionState {int Owner,Mode,TargetExplicit;char TargetName[256];}companions[1];
struct KPlayer {int m_nIndex;bool m_PlayerDBLoad;}Player[MAX_PLAYER];
struct NpcType {int m_SubWorldIndex,m_CurrentLife,m_RegionIndex,m_Kind,m_nMissionGroup,m_NpcSettingIdx,m_Level,m_btSpecial,m_SkillParam1,m_SkillParam2;char Owner[32],Name[64];bool IsPlayer(){return m_Kind==kind_player;}int GetPlayerIdx(){return m_Kind==kind_player?3:0;}}Npc[MAX_NPC];
struct World {int m_SubWorldID;}SubWorld[MAX_SUBWORLD];
struct NpcSetType {int GetNextIdx(int n){return n<5?n+1:0;}int GetDistance(int,int n){return n==3?100:300;}}NpcSet;
namespace PtEvents {struct Definition {int adapter,map,tid;};static std::map<int,Definition> definitions;}
static int PhongThanEscortCargoKind(int,int*){return 0;}
static int BotRoute(Runtime&,int,int){return 0;}
static void PTBotFoldPartySpeech(const char* raw,char* to,int n){strncpy(to,raw,n-1);to[n-1]=0;}
static void PTPlainItemName(char* to,int n,const char* raw){strncpy(to,raw,n-1);to[n-1]=0;}
static const char* PhongThanRestoredNpcDisplayName(const char* raw){return !strcmp(raw,"original_gbk_boss")?"cuu linh":!strcmp(raw,"registered_gbk_boss")?"cuu anh":0;}
#include "../../Sources/Core/Src/PhongThanBotCombatTarget.inl"
#define CHECK(x) do{if(!(x)){printf("FAIL %d %s\n",__LINE__,#x);return 1;}++checks;}while(0)
int main(){int checks=0;bots[0].Player=1;Player[1].m_nIndex=1;Player[2].m_nIndex=2;companions[0].Owner=2;companions[0].Mode=PTBC_BOSS;SubWorld[0].m_SubWorldID=1016;
 for(int i=1;i<=5;++i){Npc[i].m_SubWorldIndex=0;Npc[i].m_RegionIndex=0;Npc[i].m_CurrentLife=100;Npc[i].m_nMissionGroup=-1;}Npc[1].m_Kind=Npc[2].m_Kind=kind_player;
 strcpy(Npc[3].Name,"nearby ordinary monster");strcpy(Npc[4].Name,"original_gbk_boss");Npc[4].m_btSpecial=npc_gold;Npc[4].m_NpcSettingIdx=70;
 strcpy(Npc[5].Name,"registered_gbk_boss");Npc[5].m_NpcSettingIdx=86;PtEvents::Definition d;d.adapter=2;d.map=1016;d.tid=86;PtEvents::definitions[1101]=d;
 CHECK(CompanionBoss(4)&&CompanionBoss(5)&&!CompanionBoss(3));strcpy(companions[0].TargetName,"danh boss cuu linh");companions[0].TargetExplicit=1;CHECK(CompanionResolveTarget(bots[0],companions[0])==4);strcpy(companions[0].TargetName,"cuu anh");CHECK(CompanionResolveTarget(bots[0],companions[0])==5);
 strcpy(companions[0].TargetName,"unknown boss");CHECK(!CompanionResolveTarget(bots[0],companions[0]));companions[0].TargetExplicit=0;companions[0].TargetName[0]=0;CHECK(CompanionResolveTarget(bots[0],companions[0])==4);CHECK(!CompanionTargetType(bots[0],3,PTBC_BOSS));CHECK(!CompanionTargetType(bots[0],4,PTBC_FIGHT));
 Npc[4].m_CurrentLife=0;CHECK(!CompanionTargetType(bots[0],4,PTBC_BOSS));Npc[4].m_CurrentLife=100;strcpy(Npc[4].Owner,"player pet");CHECK(!CompanionBoss(4));printf("PASS %d production combat target checks: localized Cuu Linh, registered boss, no ordinary mob substitution, explicit missing names and dead/pet guards\n",checks);return 0;}
