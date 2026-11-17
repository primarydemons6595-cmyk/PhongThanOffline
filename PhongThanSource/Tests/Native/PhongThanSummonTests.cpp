#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <windows.h>
#include "../../Headers/PhongThanGameplayProtocol.h"
#define _SERVER
#define MAX_NPC 32
#define MAX_PLAYER 8
#define MAX_SUBWORLD 2
#define MAX_NPCSKILL 8
#define MAX_NPC_USE_SKILL 4
#define TASKVALUE_PT_SUMMON_MORPH_BEGIN 4880
#define TRUE 1

typedef unsigned long DWORD;
enum {kind_normal,kind_player,relation_none=0,relation_ally=1,relation_enemy=2,
    SKILL_SS_Missles=0,SKILL_SS_Melee=1,SKILL_SS_Thief=5,
    SKILL_SS_Reserved14=14,SKILL_SS_PhongThanAttack=15,do_walk=1,do_run=2,do_stand=3,obj_npc=1};

#include "../../Sources/Core/Src/PhongThanSummon.h"
#include "../../Headers/PhongThanSummonStats.h"
struct ISkill {
    int style;
    ISkill(int s=0):style(s){}
    int GetSkillStyle(){return style;}
    bool IsTargetEnemy(){return style==0;}
    int GetAttackRadius(){return 500;}
};
struct KSkill:ISkill {
    KSkill(int s=14):ISkill(s){}
    const char* GetSkillName(){return "Summon";}
    int GetSkillCostType(){return 0;}
    int GetSkillCost(void*){return 30;}
    int GetDelayPerCast(bool){return 270;}
};
struct Manager {
    KSkill summon, attack;
    Manager():summon(14),attack(0){}
    KSkill* GetSkill(int id,int level){return level>0 && level<64 ?
        (PhongThanIsSummon(id)?&summon:(id==100?&attack:0)):0;}
} g_SkillManager;
struct SkillList {
    int id,level; DWORD next;
    SkillList():id(0),level(0),next(0){}
    int GetCurrentLevel(int n){return n==id?level:0;}
    bool CanCast(int n,DWORD now){return n==id && level>0 && next<=now;}
    int GetSkillId(int slot){return slot==1?id:0;}
    void SetNextCastTime(int,DWORD,DWORD n){next=n;}
};
struct NpcMock {
    int m_LifeMax,m_AttackRating,m_Defend,m_ExDefend,m_CurrentAttackRating,m_CurrentDefend,m_CurrentExDefend;
    struct {int nValue[3];} m_PhysicsDamage;
    int m_CurrentAttackSpeed,m_CurrentCastSpeed;
    void RestoreNpcBaseInfo(){m_CurrentLife=m_CurrentLifeMax=m_LifeMax;m_CurrentAttackRating=m_AttackRating;m_CurrentDefend=m_Defend;m_CurrentExDefend=m_ExDefend;m_CurrentAttackSpeed=m_CurrentCastSpeed=0;}
    int m_nPlayerIdx,m_SubWorldIndex,m_RegionIndex,m_Level,m_Kind;
    int m_OffX,m_OffY,m_MapZ,m_OriginX,m_OriginY,m_RunSpeed; float m_CurrentRunSpeed;
    char Name[32]; int lastCommand,castPackets,syncPackets,removePackets,removedFromX;
    int m_MapX,m_MapY,m_CurrentLifeMax,m_CurrentLife,m_CurrentMana,m_VisionRadius,m_CurrentVisionRadius;
    int m_nOwnerIdx,m_AiMode,m_CurrentCamp,m_Camp,m_Experience,m_CurrentExperience;
    DWORD m_dwID,m_NextAITime,m_ActionScriptID,m_DeathScriptID,m_DropScriptID;
    bool m_FightMode,m_bRideHorse,m_ProcessAI;
    struct State {int nTime;} m_SilentState,m_HideState;
    int m_NpcSettingIdx;struct {int CmdKind;}m_NowCommand;
    int m_DeathSkill[4]; char ActionScript[64]; SkillList m_SkillList;
    bool IsPlayer(){return m_Kind==kind_player;}
    int GetPlayerIdx(){return m_nPlayerIdx;}
    bool IsAlive(){return m_CurrentLife>0;}
    bool Cost(int,int n,bool audit=false){if(m_CurrentMana<n)return false;
        if(!audit)m_CurrentMana-=n;return true;}
    void GetMpsPos(int*x,int*y){*x=m_OriginX;*y=m_OriginY;}
    bool SetPos(int x,int y){m_OriginX=x;m_OriginY=y;return true;}
    void SendDataToNearRegion(void* data,int size){if(size==sizeof(PHONGTHAN_SKILL_CAST)){++castPackets; assert(PhongThanIsSummon(((PHONGTHAN_SKILL_CAST*)data)->SkillId));}if(size==sizeof(PHONGTHAN_ENTITY_REFERENCE)&&((PHONGTHAN_ENTITY_REFERENCE*)data)->Header.MessageType==PHONGTHAN_MSG_WORLD_ENTITY_REMOVE){++removePackets;removedFromX=m_OriginX;}}
    void SendSyncData(int,bool){++syncPackets;}
    bool SetActiveSkill(int){return true;}
    void SendCommand(int cmd,int=0,int=0){lastCommand=cmd;}
} Npc[MAX_NPC];
typedef NpcMock KNpc;
struct Profession {int n;int GetProfession(){return n;}};
struct MorphTasks{int values[9];int GetSaveVal(int task){assert(task>=4880&&task<=4888);return values[task-4880];}void SetSaveVal(int task,int value,BOOL){assert(task>=4880&&task<=4888);values[task-4880]=value;}};
struct PlayerMock {MorphTasks m_cTask;bool locked;bool GetLockState(){return locked;}int m_nIndex;Profession m_cProfession;bool trading;
    bool CheckTrading(){return trading;}} Player[MAX_PLAYER];
struct Region {int refs;void RemoveNpc(int){} void AddNpc(int){} void AddRef(int,int,int){++refs;} void DecRef(int,int,int){--refs;}};
typedef Region KRegion;
struct World {DWORD m_dwCurrentTime;int m_nTotalRegion,m_SubWorldID;Region m_Region[1];
 void Mps2Map(int x,int y,int*r,int*mx,int*my,int*ox,int*oy){*r=0;*mx=x;*my=y;*ox=*oy=0;}
 bool TestBarrier(int,int){return false;} } SubWorld[MAX_SUBWORLD];
struct SetMock {
    bool failSpawn,failTemplate;int removeCount,next; int allyA,allyB;
    SetMock():failSpawn(false),failTemplate(false),removeCount(0),next(2),allyA(0),allyB(0){}
    int Add(unsigned long spec,int world,int,int,bool barrier){assert(barrier);
        if(failSpawn)return 0;int n=next++;memset(&Npc[n],0,sizeof(Npc[n]));
        Npc[n].m_dwID=100+n;Npc[n].m_SubWorldIndex=world;
        Npc[n].m_CurrentLife=Npc[n].m_CurrentLifeMax=100;
        Npc[n].m_Level=(int)(spec&65535); Npc[n].m_VisionRadius=400;
        Npc[n].m_SkillList.id=failTemplate?0:100;Npc[n].m_SkillList.level=Npc[n].m_Level;
        ++SubWorld[world].m_Region[0].refs;return n;}
    void Remove(int n){PhongThanSummonRemoving(n);Npc[n].m_dwID=0;++removeCount;}
    int GetRelation(int a,int b){int summon=PhongThanSummonRelation(a,b);if(summon>=0)return summon;
        return ((a==allyA && b==allyB)||(a==allyB && b==allyA))?relation_ally:relation_enemy;}
    int SearchID(DWORD id){if(!id)return 0;for(int n=1;n<MAX_NPC;++n)if(Npc[n].m_dwID==id)return n;return 0;}
    int GetNextIdx(int n){for(++n;n<MAX_NPC;++n)if(Npc[n].m_dwID)return n;return 0;}
    int GetDistance(int a,int b){int x=Npc[a].m_OriginX-Npc[b].m_OriginX;int y=Npc[a].m_OriginY-Npc[b].m_OriginY;return abs(x)+abs(y);}
} NpcSet;
struct ServerMock {int PackDataToClient(int,BYTE*,int){return 0;}} server;
ServerMock* g_pServer=&server;
static bool PhongThanEscortMounted(int){return false;}
struct MockSummonStats {int Read(int,const char*,const char*){return 100;}};
static bool PtConfigureSummonCombatLevel(int pet,int level,int rank) {
    MockSummonStats reader;return PhongThanApplySummonCombatLevel(Npc[pet],level,reader,rank);
}
#include "../../Sources/Core/Src/PhongThanSummonServer.inl"
int main()
{
    PHONGTHAN_ENTITY_REFERENCE targetRequest;
    memset(&targetRequest,0,sizeof(targetRequest));
    PhongThanInitializeWireHeader(&targetRequest.Header,PHONGTHAN_MSG_GAMEPLAY_SUMMON_TARGET_REQUEST,
        sizeof(targetRequest),PHONGTHAN_WIRE_FLAG_REQUEST,0);
    targetRequest.MapId=1007; targetRequest.EntityId=105;
    assert(PhongThanWorldFixedPacket(&targetRequest,sizeof(targetRequest),
        PHONGTHAN_MSG_GAMEPLAY_SUMMON_TARGET_REQUEST,PHONGTHAN_WIRE_FLAG_REQUEST,sizeof(targetRequest)));
    assert(!PhongThanWorldFixedPacket(&targetRequest,sizeof(targetRequest)-1,
        PHONGTHAN_MSG_GAMEPLAY_SUMMON_TARGET_REQUEST,PHONGTHAN_WIRE_FLAG_REQUEST,sizeof(targetRequest)));
    assert(!PhongThanWorldFixedPacket(&targetRequest,sizeof(targetRequest),
        PHONGTHAN_MSG_GAMEPLAY_SUMMON_TARGET_REQUEST,PHONGTHAN_WIRE_FLAG_RESPONSE,sizeof(targetRequest)));
    int expected[11]={359,360,361,362,403,404,405,406,407,1345,1346};
    for(int id=450;id<=460;++id)assert(PhongThanSummonTemplate(id)==expected[id-450]);
    assert(!PhongThanIsSummon(449)&&!PhongThanIsSummon(461));
    assert(PhongThanSummonCost(29,1024,1)==30);
    assert(PhongThanSummonCost(199,28672,63)==1963);
    assert(PhongThanSummonCost(1,1048577,1)==-1);
    assert(PhongThanSummonCost(29,1024,64)==-1);
    memset(Npc,0,sizeof(Npc));memset(Player,0,sizeof(Player));memset(SubWorld,0,sizeof(SubWorld));
    SubWorld[1].m_nTotalRegion=1; SubWorld[1].m_dwCurrentTime=280;
    SubWorld[0].m_nTotalRegion=1;SubWorld[0].m_dwCurrentTime=10;
    Npc[1].m_dwID=1;Npc[1].m_Kind=kind_player;Npc[1].m_CurrentLife=100;
    Npc[1].m_Level=200;Npc[1].m_CurrentMana=100;Npc[1].m_nPlayerIdx=1;
    strcpy(Npc[1].Name,"Owner"); Npc[1].m_CurrentRunSpeed=12;
    Npc[1].m_FightMode=true;Npc[1].m_VisionRadius=40;
    Npc[1].m_SkillList.id=450;Npc[1].m_SkillList.level=1;
    Player[1].m_nIndex=1;Player[1].m_cProfession.n=2;
    assert(!PhongThanCastSummon(0,450));assert(!PhongThanCastSummon(1,459));
    Player[1].m_cProfession.n=1;assert(!PhongThanCastSummon(1,450));Player[1].m_cProfession.n=2;
    Npc[1].m_CurrentMana=29;assert(!PhongThanCastSummon(1,450));Npc[1].m_CurrentMana=100;
    NpcSet.failSpawn=true;assert(!PhongThanCastSummon(1,450));assert(Npc[1].m_CurrentMana==100);
    NpcSet.failSpawn=false;assert(PhongThanCastSummon(1,450));
    assert(PhongThanSummonTracked(2));assert(Npc[1].m_CurrentMana==70);
    assert(!strcmp(Npc[2].Name,"Summon (Owner)")); assert(Npc[1].castPackets==1); assert(Npc[2].syncPackets==1);
    assert(!PhongThanSummonMorph(1,451,408));assert(!PhongThanSummonMorph(1,450,100));
    Player[1].locked=true;assert(!PhongThanSummonMorph(1,450,408));Player[1].locked=false;
    assert(PhongThanSummonMorph(1,450,408));assert(Npc[2].m_NpcSettingIdx==408&&Player[1].m_cTask.values[0]==408);
    assert(!strcmp(Npc[2].Name,"Summon (Owner)"));
    assert(Npc[2].m_Level==200);assert(Npc[2].m_SkillList.level==1);
    assert(Npc[1].m_SkillList.next==280);assert(!PhongThanCastSummon(1,450));
    SubWorld[0].m_dwCurrentTime=280;NpcSet.failTemplate=true;
    assert(!PhongThanCastSummon(1,450));assert(PhongThanSummonTracked(2));
    assert(Npc[1].m_CurrentMana==70);NpcSet.failTemplate=false;
    assert(PhongThanCastSummon(1,450));assert(!PhongThanSummonTracked(2));
    assert(PhongThanSummonTracked(4));assert(Npc[1].m_CurrentMana==40);
    assert(Npc[4].m_NpcSettingIdx==408&&!strcmp(Npc[4].Name,"Summon (Owner)"));
    assert(SubWorld[0].m_Region[0].refs==1);
    assert(PhongThanSummonRelation(4,1)==relation_ally);
    Npc[5].m_Kind=kind_player;Npc[5].m_CurrentLife=10;Npc[5].m_dwID=105;
    assert(PhongThanSummonRelation(4,5)==relation_enemy);
    assert(PhongThanSummonRelation(5,4)==relation_enemy);
    NpcSet.allyA=1;NpcSet.allyB=5;
    assert(PhongThanSummonRelation(4,5)==relation_ally);
    assert(PhongThanSummonRelation(5,4)==relation_ally);
    NpcSet.allyA=NpcSet.allyB=0;
    PhongThanSummonTakingHit(4,5); // Player hits pet: pet can retaliate.
    assert(PhongThanSummonThink(4)==5);
    SubWorld[0].m_dwCurrentTime+=2;
    Npc[5].m_Kind=kind_normal;Npc[5].m_RegionIndex=0;Npc[5].m_SubWorldIndex=0;
    Npc[5].m_OriginX=200;
    PhongThanSummonOwnerAttacked(1,5); // Owner has not cast an attack.
    assert(PhongThanSummonSetLock(1,1));assert(PhongThanSummonGetLock(1)==1);
    s_PtSummonTargets[1].targetId=0;PhongThanSummonOwnerAttacked(1,5);assert(s_PtSummonTargets[1].targetId==0);
    PhongThanSummonOwnerAttack(1,5);assert(s_PtSummonTargets[1].targetId==Npc[5].m_dwID);
    assert(!PhongThanSummonSetLock(1,2)&&!PhongThanSummonSetLock(0,1)&&!PhongThanSummonSetLock(5,1));
    assert(PhongThanSummonSetLock(1,0));assert(!PhongThanSummonGetLock(1));
    assert(PhongThanSummonTargetRange(1)==1024);
    assert(PhongThanSummonThink(4)==5);
    PhongThanSummonOwnerAttack(1,0); SubWorld[0].m_dwCurrentTime+=2;
    assert(PhongThanSummonThink(4)==5); // Stopping/buffing does not clear the combat target.
    Npc[5].m_dwID=106;SubWorld[0].m_dwCurrentTime+=5;
    Npc[1].m_OriginX=200;
    assert(PhongThanSummonThink(4)==0); // Reused target slot cannot inherit aggro.
    assert(Npc[4].lastCommand==do_run); assert(Npc[4].m_CurrentRunSpeed==12);
    Npc[1].m_OriginX=1000;
    int oldRemove=Npc[4].removePackets,oldX=Npc[4].m_OriginX;PhongThanSummonOwnerTick(1); assert(Npc[4].m_OriginX==1096);assert(Npc[4].removePackets==oldRemove+1&&Npc[4].removedFromX==oldX);
    assert(PhongThanSummonTracked(4));
    Npc[1].m_dwID=2;assert(PhongThanSummonThink(4)==0);
    assert(!PhongThanSummonTracked(4));assert(SubWorld[0].m_Region[0].refs==0);
    Npc[1].m_SkillList.next=0;Npc[1].m_CurrentMana=100;
    Player[1].trading=true;assert(!PhongThanCastSummon(1,450));Player[1].trading=false;
    Npc[1].m_SilentState.nTime=10;assert(!PhongThanCastSummon(1,450));Npc[1].m_SilentState.nTime=0;
    Npc[1].m_FightMode=false;assert(!PhongThanCastSummon(1,450));Npc[1].m_FightMode=true;
    NpcSet.next=6;assert(PhongThanCastSummon(1,450));Npc[1].m_SubWorldIndex=1;
    PhongThanSummonOwnerTick(1);
    assert(PhongThanSummonTracked(6)); assert(Npc[6].m_SubWorldIndex==1);
    assert(SubWorld[0].m_Region[0].refs==0 && SubWorld[1].m_Region[0].refs==1);
    assert(Npc[6].m_CurrentLife==100 && Npc[1].m_CurrentMana==70);
    assert(PhongThanSummonThink(6)==0);
    Npc[1].m_SubWorldIndex=0;Npc[1].m_SkillList.next=0;
    assert(PhongThanCastSummon(1,450));Npc[1].m_CurrentLife=0;
    assert(PhongThanSummonThink(7)==0);assert(!PhongThanSummonTracked(7));
    Npc[1].m_CurrentLife=100;Npc[1].m_SkillList.next=0;
    assert(PhongThanCastSummon(1,450));NpcSet.Remove(1);
    assert(!PhongThanSummonTracked(8));assert(SubWorld[0].m_Region[0].refs==0);
    Npc[1].m_dwID=77;Npc[1].m_Level=90;Npc[1].m_CurrentLife=100;Npc[1].m_CurrentMana=1000;
    for(int summonId=450;summonId<=460;++summonId) {
        Npc[1].m_SkillList.id=summonId;Npc[1].m_SkillList.level=10;Npc[1].m_SkillList.next=0;
        assert(PhongThanCastSummon(1,summonId));int active=s_PtOwnerPet[1],live=0;
        assert(Npc[active].m_Level==90 && Npc[active].m_SkillList.level==10);
        for(int slot=1;slot<MAX_NPC;++slot)if(PhongThanSummonTracked(slot)&&s_PtSummons[slot].index==1)++live;
        assert(live==1);
    }
    puts("PASS: all eleven Di Nhan summons rank10, owner90 combat level, recast replaces previous pet (one live summon)");
    int active=s_PtOwnerPet[1];Npc[1].m_OriginX=0;Npc[active].m_OriginX=0;
    Npc[20].m_dwID=9000;Npc[20].m_CurrentLife=100;Npc[20].m_Kind=kind_normal;
    Npc[20].m_SubWorldIndex=0;Npc[20].m_RegionIndex=0;Npc[20].m_OriginX=900;
    PhongThanSummonTakingHit(active,20);
    assert(s_PtSummonTargets[1].targetId==9000);
    SubWorld[0].m_dwCurrentTime+=5;
    assert(PhongThanSummonThink(active)==0&&Npc[active].lastCommand==do_run);
    Npc[active].m_OriginX=600;PhongThanSummonOwnerTick(1);assert(Npc[active].m_OriginX==600);
    Npc[21].m_dwID=9001;Npc[21].m_CurrentLife=100;Npc[21].m_Kind=kind_player;
    Npc[21].m_SubWorldIndex=0;Npc[21].m_RegionIndex=0;Npc[21].m_OriginX=500;
    PhongThanSummonTakingHit(1,21);assert(s_PtSummonTargets[1].targetId==9001);
    Npc[active].m_OriginX=1300;PhongThanSummonOwnerTick(1);
    assert(NpcSet.GetDistance(1,active)<=1280&&s_PtSummonTargets[1].targetId==0);
    Npc[20].m_OriginX=1300;PhongThanSummonOwnerAttack(1,20);assert(s_PtSummonTargets[1].targetId==0);
    assert(PhongThanSummonShownLevel(90,10)==10&&PhongThanSummonShownLevel(200,1)==1);
    assert(PhongThanSummonShownLevel(90,0)==90&&PhongThanSummonShownLevel(90,64)==90);
    assert(Npc[active].m_CurrentAttackSpeed==90&&Npc[active].m_CurrentCastSpeed==90);
    assert(PhongThanSummonSpeedBonus(1)==0&&PhongThanSummonSpeedBonus(10)==90&&PhongThanSummonSpeedBonus(63)==100);
    assert(PhongThanSummonSetLock(1,1));
    PhongThanSummonTakingHit(1,21);assert(s_PtSummonTargets[1].targetId==0);
    PhongThanSummonOwnerAttack(1,21);assert(s_PtSummonTargets[1].targetId==9001);
    assert(PhongThanSummonSetLock(1,0));Npc[20].m_OriginX=900;
    PhongThanSummonTakingHit(active,20);assert(s_PtSummonTargets[1].targetId==9000);
    puts("PASS original button modes: normal retaliates; locked accepts owner's attack target only");
    puts("PASS: rank badge 10, retaliate for owner/pet, chase at 900, no 384 recall in combat, hard leash 1280, reject distant targets");
    puts("PASS: registry, cost/rollback, cooldown, owner names, cast packets, running, teleport/map transfer, reference counts and stale IDs");
    return 0;
}
