// RECONSTRUCTED server policy, not recovered VNG server code:
// one live summon per owner; recast replaces only after successful allocation
// and payment. Template stats/skills are loaded by the normal PT NPC loader.
// Corpse mechanics and owner-damage inheritance are deliberately not guessed.
#ifdef _SERVER
#include "../../../Headers/PhongThanSummonRules.h"
static bool PtConfigureSummonCombatLevel(int pet,int ownerLevel,int rank);
struct PT_SUMMON_OWNER { int index; DWORD ownerId; DWORD petId;int skill,rank;BOOL escortPaused; };
static PT_SUMMON_OWNER s_PtSummons[MAX_NPC];
static int s_PtOwnerPet[MAX_NPC];
struct PT_SUMMON_TARGET { DWORD ownerId; DWORD targetId;BOOL locked; };
static PT_SUMMON_TARGET s_PtSummonTargets[MAX_NPC];

int PhongThanSummonGetLock(int owner){return owner>0&&owner<MAX_NPC&&s_PtSummonTargets[owner].ownerId==Npc[owner].m_dwID&&s_PtSummonTargets[owner].locked?1:0;}
bool PhongThanSummonSetLock(int owner,int locked){
 if(owner<=0||owner>=MAX_NPC||!Npc[owner].IsPlayer()||(locked!=0&&locked!=1))return false;
 int pet=s_PtOwnerPet[owner];if(!PhongThanSummonTracked(pet)||s_PtSummons[pet].ownerId!=Npc[owner].m_dwID)return false;
 s_PtSummonTargets[owner].ownerId=Npc[owner].m_dwID;s_PtSummonTargets[owner].locked=locked;
 PhongThanSummonSyncOwner(pet,0,TRUE);return true;
}
static void PtSummonLog(const char* event, int owner, int pet, int skill);
void PhongThanSummonOwnerAttack(int owner, int target)
{
    if (owner <= 0 || owner >= MAX_NPC || !Npc[owner].IsPlayer()) return;
    int pet=s_PtOwnerPet[owner];
    if(!PhongThanSummonTracked(pet)||s_PtSummons[pet].ownerId!=Npc[owner].m_dwID)return;
    if(PhongThanEscortMounted(Npc[owner].GetPlayerIdx()))return;
    if (s_PtSummonTargets[owner].ownerId != Npc[owner].m_dwID)
    {
        memset(&s_PtSummonTargets[owner],0,sizeof(s_PtSummonTargets[owner]));
        s_PtSummonTargets[owner].ownerId = Npc[owner].m_dwID;
    }
    if (target > 0 && target < MAX_NPC && (Npc[target].m_Kind == kind_normal || Npc[target].IsPlayer()) &&
        Npc[target].IsAlive() &&
        Npc[target].m_SubWorldIndex == Npc[owner].m_SubWorldIndex &&
        NpcSet.GetDistance(owner,target)<=PT_SUMMON_ACQUIRE_RANGE &&
        (NpcSet.GetRelation(owner, target) & relation_enemy))
    {
        if (s_PtSummonTargets[owner].targetId != Npc[target].m_dwID)
            PtSummonLog("TARGET_SET", owner, s_PtOwnerPet[owner], target);
        s_PtSummonTargets[owner].targetId = Npc[target].m_dwID;
    }
}
int PhongThanSummonTargetRange(int owner)
{
    if (owner <= 0 || owner >= MAX_NPC) return 0;
    int pet=s_PtOwnerPet[owner];
    if (!PhongThanSummonTracked(pet) || s_PtSummons[pet].ownerId != Npc[owner].m_dwID) return 0;
    return PT_SUMMON_ACQUIRE_RANGE;
}
void PhongThanSummonOwnerAttacked(int owner, int attacker)
{
    if(PhongThanSummonGetLock(owner))return;
    if (owner <= 0 || owner >= MAX_NPC || !Npc[owner].IsPlayer() || !Npc[owner].m_FightMode) return;
    // An unlocked summon retaliates against the latest valid attacker.
    PhongThanSummonOwnerAttack(owner, attacker);
}

void PhongThanSummonTakingHit(int victim, int attacker)
{
    if (victim <= 0 || victim >= MAX_NPC) return;
    int owner = PhongThanSummonTracked(victim) ? s_PtSummons[victim].index : victim;
    PhongThanSummonOwnerAttacked(owner, attacker);
}
void PhongThanSummonSyncOwner(int pet, int client, BOOL broadcast)
{
    if (!PhongThanSummonTracked(pet)) return;
    int owner=s_PtSummons[pet].index;
    if (owner <= 0 || owner >= MAX_NPC || Npc[owner].m_dwID != s_PtSummons[pet].ownerId) return;
    PHONGTHAN_SUMMON_OWNER_SYNC packet;
    ZeroMemory(&packet,sizeof(packet));
    PhongThanInitializeWireHeader(&packet.Header,PHONGTHAN_MSG_GAMEPLAY_SUMMON_OWNER,
        sizeof(packet),PHONGTHAN_WIRE_FLAG_RESPONSE,0);
    packet.MapId=SubWorld[Npc[pet].m_SubWorldIndex].m_SubWorldID;
    packet.PetId=Npc[pet].m_dwID; packet.OwnerId=Npc[owner].m_dwID;
    packet.SkillLevel=(PHONGTHAN_U16)s_PtSummons[pet].rank;
    packet.TargetLocked=(PHONGTHAN_U16)PhongThanSummonGetLock(owner);
    if (broadcast) Npc[pet].SendDataToNearRegion(&packet,sizeof(packet));
    else g_pServer->PackDataToClient(client,(BYTE*)&packet,sizeof(packet));
}
static void PtSummonLog(const char* event, int owner, int pet, int skill)
{
    FILE* f = fopen("summon_trace.log", "a+t");
    if (!f) return;
    fprintf(f, "tick=%lu event=%s owner=%d pet=%d skill=%d\n",
        GetTickCount(), event, owner, pet, skill);
    fclose(f);
}
bool PhongThanSummonTracked(int n)
{
    return n > 0 && n < MAX_NPC && s_PtSummons[n].petId &&
        s_PtSummons[n].petId == Npc[n].m_dwID;
}
static int PtSummonOwner(int n)
{
    if (!PhongThanSummonTracked(n)) return 0;
    const PT_SUMMON_OWNER& record = s_PtSummons[n];
    int owner = record.index;
    if (owner <= 0 || owner >= MAX_NPC || !Npc[owner].IsPlayer() ||
        Npc[owner].m_dwID != record.ownerId || !Npc[owner].IsAlive() ||
        Npc[owner].m_SubWorldIndex != Npc[n].m_SubWorldIndex ||
        Npc[owner].m_RegionIndex < 0) return 0;
    return owner;
}
static int PtSummonCombatTarget(int owner,int pet) {
    if(!Npc[owner].m_FightMode)return 0;
    if(s_PtSummonTargets[owner].ownerId!=Npc[owner].m_dwID)return 0;
    int target=NpcSet.SearchID(s_PtSummonTargets[owner].targetId);
    if(target<=0||target>=MAX_NPC||!Npc[target].IsAlive()||Npc[target].m_RegionIndex<0||
       (Npc[target].m_Kind!=kind_normal&&!Npc[target].IsPlayer())||
       Npc[target].m_SubWorldIndex!=Npc[pet].m_SubWorldIndex||
       NpcSet.GetDistance(owner,target)>PT_SUMMON_LEASH_RANGE||
       NpcSet.GetDistance(owner,pet)>PT_SUMMON_LEASH_RANGE||
       !(NpcSet.GetRelation(owner,target)&relation_enemy))return 0;
    return target;
}
int PhongThanSummonTrainingOwner(int pet, int& skill)
{
    int owner = PtSummonOwner(pet);
    if (!owner) return 0;
    if(PhongThanIsSummon(s_PtSummons[pet].skill)){skill=s_PtSummons[pet].skill;return owner;}
    for (int id = 450; id <= 460; ++id)
        if (Npc[pet].m_NpcSettingIdx == PhongThanSummonTemplate(id))
        {
            skill = id;
            return owner;
        }
    return 0;
}
bool PhongThanSummonInfo(int owner,int& skill,int& type){
    if(owner<=0||owner>=MAX_NPC)return false;int pet=s_PtOwnerPet[owner];
    if(PtSummonOwner(pet)!=owner||Npc[pet].m_CurrentLife<=0)return false;
    skill=s_PtSummons[pet].skill;type=Npc[pet].m_NpcSettingIdx;return PhongThanIsSummon(skill);
}
static bool PtSummonMorphType(int type){return type==359||type==408||type==409||(type>=2651&&type<=2656);}
bool PhongThanSummonMorph(int owner,int skill,int type){
    int actual=0,old=0;if(!PhongThanSummonInfo(owner,actual,old)||actual!=skill||
        !PtSummonMorphType(type)||skill<450||skill>458)return false;
    int player=Npc[owner].GetPlayerIdx();
    if(player<=0||player>=MAX_PLAYER||Player[player].m_nIndex!=owner||Player[player].CheckTrading()||Player[player].GetLockState())return false;
    Player[player].m_cTask.SetSaveVal(TASKVALUE_PT_SUMMON_MORPH_BEGIN+skill-450,type,TRUE);
    int pet=s_PtOwnerPet[owner];Npc[pet].m_NpcSettingIdx=type;
    // Keep the original skill identity, combat stats and owner name untouched.
    Npc[pet].SendSyncData(0,TRUE);return true;
}
static void PtRemoveSummon(int pet)
{
    if (!PhongThanSummonTracked(pet)) return;
    int owner = s_PtSummons[pet].index;
    int world = Npc[pet].m_SubWorldIndex, region = Npc[pet].m_RegionIndex;
    if (world >= 0 && world < MAX_SUBWORLD && region >= 0 &&
        region < SubWorld[world].m_nTotalRegion)
    {
        SubWorld[world].m_Region[region].RemoveNpc(pet);
        SubWorld[world].m_Region[region].DecRef(Npc[pet].m_MapX, Npc[pet].m_MapY, obj_npc);
    }
    PtSummonLog("REMOVE", owner, pet, 0);
    NpcSet.Remove(pet);
}
bool PhongThanSummonEscortPaused(int pet){
    return PhongThanSummonTracked(pet) && s_PtSummons[pet].escortPaused;
}
void PhongThanSummonEscortPause(int owner,BOOL paused){
    if(owner<=0 || owner>=MAX_NPC)return;
    int pet=s_PtOwnerPet[owner];
    if(!PhongThanSummonTracked(pet) || s_PtSummons[pet].index!=owner ||
       s_PtSummons[pet].ownerId!=Npc[owner].m_dwID)return;
    if(!paused){PhongThanSummonOwnerTick(owner);return;}
    if(s_PtSummons[pet].escortPaused)return;
    KNpc& summon=Npc[pet];int world=summon.m_SubWorldIndex,region=summon.m_RegionIndex;
    summon.m_NowCommand.CmdKind=do_stand;summon.SendCommand(do_stand);
    if(world>=0 && world<MAX_SUBWORLD && region>=0 && region<SubWorld[world].m_nTotalRegion){
        PHONGTHAN_ENTITY_REFERENCE remove;
        PhongThanInitializeWireHeader(&remove.Header,PHONGTHAN_MSG_WORLD_ENTITY_REMOVE,sizeof(remove),PHONGTHAN_WIRE_FLAG_RESPONSE,0);
        remove.MapId=SubWorld[world].m_SubWorldID;remove.EntityId=summon.m_dwID;
        summon.SendDataToNearRegion(&remove,sizeof(remove));
        SubWorld[world].m_Region[region].RemoveNpc(pet);
        SubWorld[world].m_Region[region].DecRef(summon.m_MapX,summon.m_MapY,obj_npc);
    }
    summon.m_RegionIndex=-1;s_PtSummons[pet].escortPaused=TRUE;
    memset(&s_PtSummonTargets[owner],0,sizeof(s_PtSummonTargets[owner]));
}
// Runs from the owner, including while the pet's old region is inactive.
void PhongThanSummonOwnerTick(int owner)
{
    if (owner <= 0 || owner >= MAX_NPC) return;
    int pet = s_PtOwnerPet[owner];
    if (!PhongThanSummonTracked(pet) || s_PtSummons[pet].index != owner ||
        s_PtSummons[pet].ownerId != Npc[owner].m_dwID)
    { s_PtOwnerPet[owner] = 0; return; }
    KNpc& player = Npc[owner];
    KNpc& summon = Npc[pet];
    int world = player.m_SubWorldIndex;
    if (!player.IsAlive() || !summon.IsAlive()) { PtRemoveSummon(pet); return; }
    if (world < 0 || world >= MAX_SUBWORLD || player.m_RegionIndex < 0) return;
    if(PhongThanEscortMounted(player.GetPlayerIdx())){PhongThanSummonEscortPause(owner,TRUE);return;}
    bool changedWorld = world != summon.m_SubWorldIndex || summon.m_RegionIndex<0;
    int tether=(!changedWorld&&PtSummonCombatTarget(owner,pet))?PT_SUMMON_LEASH_RANGE:PT_SUMMON_IDLE_RECALL_RANGE;
    if (!changedWorld && NpcSet.GetDistance(owner, pet) <= tether) return;
    if(!changedWorld){s_PtSummonTargets[owner].targetId=0;summon.m_NowCommand.CmdKind=do_stand;summon.SendCommand(do_stand);}
    int x, y, region, mx, my, ox, oy;
    player.GetMpsPos(&x, &y);
    static const int dx[8] = {96,96,0,-96,-96,-96,0,96};
    static const int dy[8] = {0,64,64,64,0,-64,-64,-64};
    for (int i=0; i<8; ++i)
    {
        int px=x+dx[i], py=y+dy[i];
        SubWorld[world].Mps2Map(px, py, &region, &mx, &my, &ox, &oy);
        if (region < 0 || SubWorld[world].TestBarrier(px, py)) continue;
        s_PtSummons[pet].escortPaused=FALSE;
        PHONGTHAN_ENTITY_REFERENCE remove;
        PhongThanInitializeWireHeader(&remove.Header, PHONGTHAN_MSG_WORLD_ENTITY_REMOVE,
            sizeof(remove), PHONGTHAN_WIRE_FLAG_RESPONSE, 0);
        remove.MapId = SubWorld[summon.m_SubWorldIndex].m_SubWorldID;
        remove.EntityId = summon.m_dwID;
        summon.SendDataToNearRegion(&remove, sizeof(remove));
        if (!changedWorld) { summon.SetPos(px, py); summon.SendSyncData(0, TRUE); return; }
        if (summon.m_RegionIndex >= 0)
        {
            KRegion& old = SubWorld[summon.m_SubWorldIndex].m_Region[summon.m_RegionIndex];
            old.RemoveNpc(pet); old.DecRef(summon.m_MapX, summon.m_MapY, obj_npc);
        }
        summon.m_SubWorldIndex = world; summon.m_RegionIndex = region;
        summon.m_MapX=mx; summon.m_MapY=my; summon.m_OffX=ox; summon.m_OffY=oy;
        summon.m_MapZ=0; summon.m_OriginX=px; summon.m_OriginY=py;
        SubWorld[world].m_Region[region].AddNpc(pet);
        SubWorld[world].m_Region[region].AddRef(mx, my, obj_npc);
        summon.SendCommand(do_stand); summon.m_NextAITime=0;
        memset(&s_PtSummonTargets[owner], 0, sizeof(s_PtSummonTargets[owner]));
        summon.SendSyncData(0, TRUE);
        PtSummonLog("FOLLOW_MAP", owner, pet, 0);
        return;
    }
}
void PhongThanSummonRemoving(int npc)
{
    if (npc <= 0 || npc >= MAX_NPC) return;
    // Clear first: nested removal cannot recurse into the same pet.
    memset(&s_PtSummons[npc], 0, sizeof(s_PtSummons[npc]));
    memset(&s_PtSummonTargets[npc], 0, sizeof(s_PtSummonTargets[npc]));
    for (int i = 1; i < MAX_NPC; ++i)
        if (s_PtSummons[i].index == npc &&
            s_PtSummons[i].ownerId == Npc[npc].m_dwID) PtRemoveSummon(i);
}
int PhongThanSummonRelation(int a, int b)
{
    bool pa = PhongThanSummonTracked(a), pb = PhongThanSummonTracked(b);
    if (!pa && !pb) return -1;
    if (a <= 0 || a >= MAX_NPC || b <= 0 || b >= MAX_NPC) return relation_none;
    int oa = pa ? PtSummonOwner(a) : a;
    int ob = pb ? PtSummonOwner(b) : b;
    if (!oa || !ob) return relation_none;
    if (oa == ob) return relation_ally;
    // Resolve both sides to their owners before applying native PK/team rules.
    if (pa && pb) return NpcSet.GetRelation(oa, ob);
    if (pa && (Npc[b].m_Kind == kind_normal || Npc[b].IsPlayer())) return NpcSet.GetRelation(oa, b);
    if (pb && (Npc[a].m_Kind == kind_normal || Npc[a].IsPlayer())) return NpcSet.GetRelation(a, ob);
    return relation_none;
}
bool PhongThanCastSummon(int owner, int id)
{
    extern bool PhongThanThanKyCanCast(int,int,int);
    if(!PhongThanThanKyCanCast(owner,id,1))return false;
    if (!PhongThanIsSummon(id) || owner <= 0 || owner >= MAX_NPC) return false;
    KNpc& player = Npc[owner];
    int p = player.GetPlayerIdx(), world = player.m_SubWorldIndex;
    if (!player.IsPlayer() || !player.IsAlive() || !player.m_dwID ||
        p <= 0 || p >= MAX_PLAYER || Player[p].m_nIndex != owner ||
        Player[p].m_cProfession.GetProfession() != 2 || Player[p].CheckTrading() ||
        !player.m_FightMode || player.m_SilentState.nTime > 0 ||
        world < 0 || world >= MAX_SUBWORLD || player.m_RegionIndex < 0 ||
        player.m_RegionIndex >= SubWorld[world].m_nTotalRegion ||
        player.m_Level < 1 || player.m_Level > 65535) return false;
    if(PhongThanEscortMounted(p))return false;
    int level = player.m_SkillList.GetCurrentLevel(id);
    KSkill* skill = (KSkill*)g_SkillManager.GetSkill(id, level);
    DWORD now = SubWorld[world].m_dwCurrentTime;
    if (!skill || skill->GetSkillStyle() != SKILL_SS_Reserved14 ||
        !player.m_SkillList.CanCast(id, now) ||
        !player.Cost(skill->GetSkillCostType(), skill->GetSkillCost(&player), TRUE))
    { PtSummonLog("REJECT_SKILL_COST_COOLDOWN", owner, 0, id); return false; }
    int x, y, pet = 0;
    player.GetMpsPos(&x, &y);
    static const int dx[8] = {96,96,0,-96,-96,-96,0,96};
    static const int dy[8] = {0,64,64,64,0,-64,-64,-64};
    for (int i = 0; i < 8 && !pet; ++i)
        // Allocate with rank so template attack skills stay inside their
        // cache bounds. Combat stats are resolved separately below.
        pet = NpcSet.Add(MAKELONG(level, PhongThanSummonTemplate(id)),
            world, x + dx[i], y + dy[i], TRUE);
    if (pet <= 0 || pet >= MAX_NPC)
    { PtSummonLog("REJECT_SPAWN", owner, 0, id); return false; }
    s_PtSummons[pet].index = owner;
    s_PtSummons[pet].ownerId = player.m_dwID;
    s_PtSummons[pet].petId = Npc[pet].m_dwID;
    s_PtSummons[pet].rank=level;
    if(!PtConfigureSummonCombatLevel(pet,player.m_Level,level))
    { PtSummonLog("REJECT_COMBAT_STATS",owner,pet,id);PtRemoveSummon(pet);return false; }
    bool combatSkill = false;
    for (int slot = 1; slot <= MAX_NPC_USE_SKILL; ++slot)
    {
        int attackId = Npc[pet].m_SkillList.GetSkillId(slot);
        ISkill* attack = g_SkillManager.GetSkill(attackId, Npc[pet].m_SkillList.GetCurrentLevel(attackId));
        if (attack && attack->IsTargetEnemy() &&
            (attack->GetSkillStyle() == SKILL_SS_Missles ||
             attack->GetSkillStyle() == SKILL_SS_Melee ||
             attack->GetSkillStyle() == SKILL_SS_PhongThanAttack)) combatSkill = true;
    }
    // A failed/missing NPC level contract must not charge or replace a pet.
    if (!combatSkill || Npc[pet].m_Kind != kind_normal || Npc[pet].m_CurrentLifeMax <= 0 ||
        !player.Cost(skill->GetSkillCostType(), skill->GetSkillCost(&player)))
    { PtSummonLog("REJECT_TEMPLATE_OR_PAYMENT", owner, pet, id); PtRemoveSummon(pet); return false; }
    for (int old = 1; old < MAX_NPC; ++old)
        if (old != pet && s_PtSummons[old].index == owner &&
            s_PtSummons[old].ownerId == player.m_dwID) PtRemoveSummon(old);
    s_PtOwnerPet[owner] = pet;
    s_PtSummons[pet].skill=id;
    KNpc& summon = Npc[pet];
    int savedAppearance=id<=458?Player[p].m_cTask.GetSaveVal(TASKVALUE_PT_SUMMON_MORPH_BEGIN+id-450):0;
    if(PtSummonMorphType(savedAppearance))summon.m_NpcSettingIdx=savedAppearance;
    summon.m_nOwnerIdx = owner;
    summon.m_VisionRadius=summon.m_CurrentVisionRadius=PT_SUMMON_ACQUIRE_RANGE;
    const char* skillName = skill->GetSkillName();
    if (skillName[0] == '#') ++skillName;
    _snprintf(summon.Name, sizeof(summon.Name) - 1, "%s (%s)", skillName, player.Name);
    summon.Name[sizeof(summon.Name) - 1] = 0;
    summon.m_AiMode = 11; // Native owner-credit marker; custom AI bypasses JX pet AI.
    // KNpc::Init already enables AI; do not override its private lifecycle flag.
    summon.m_FightMode = player.m_FightMode;
    summon.m_CurrentCamp = player.m_CurrentCamp;
    summon.m_Camp = player.m_Camp;
    summon.m_Experience = summon.m_CurrentExperience = 0;
    summon.m_ActionScriptID = summon.m_DeathScriptID = summon.m_DropScriptID = 0;
    summon.ActionScript[0] = 0;
    memset(summon.m_DeathSkill, 0, sizeof(summon.m_DeathSkill));
    player.m_SkillList.SetNextCastTime(id, now, now + skill->GetDelayPerCast(player.m_bRideHorse));
    player.m_HideState.nTime = 0;
    PHONGTHAN_SKILL_CAST cast;
    ZeroMemory(&cast, sizeof(cast));
    PhongThanInitializeWireHeader(&cast.Header, PHONGTHAN_MSG_GAMEPLAY_SKILL_CAST,
        sizeof(cast), PHONGTHAN_WIRE_FLAG_RESPONSE, 0);
    cast.MapId = SubWorld[world].m_SubWorldID;
    cast.EntityId = player.m_dwID;
    cast.SkillId = id;
    cast.SkillLevel = level;
    cast.TargetKind = PHONGTHAN_SKILL_TARGET_POSITION;
    cast.X = x; cast.Y = y;
    player.SendDataToNearRegion(&cast, sizeof(cast));
    summon.SendSyncData(0, TRUE);
    FILE* stats=fopen("summon_combat_stats.log","a");
    if(stats){fprintf(stats,"owner=%u owner_level=%d pet=%u skill=%d rank=%d combat_level=%d template=%d hp=%d ar=%d defense=%d exdefense=%d damage=%d,%d attack=%d attack_rank=%d\n",
        player.m_dwID,player.m_Level,summon.m_dwID,id,level,summon.m_Level,PhongThanSummonTemplate(id),
        summon.m_CurrentLifeMax,summon.m_CurrentAttackRating,summon.m_CurrentDefend,summon.m_CurrentExDefend,
        summon.m_PhysicsDamage.nValue[0],summon.m_PhysicsDamage.nValue[2],
        summon.m_SkillList.GetSkillId(1),summon.m_SkillList.GetCurrentLevel(summon.m_SkillList.GetSkillId(1)));fclose(stats);}
    PtSummonLog("CAST_OK", owner, pet, id);
    return true;
}
int PhongThanSummonThink(int pet)
{
    if (!PhongThanSummonTracked(pet)) return -1;
    if(PhongThanSummonEscortPaused(pet))return 0;
    int owner = PtSummonOwner(pet);
    if (!owner || !Npc[pet].IsAlive()) { PtRemoveSummon(pet); return 0; }
    KNpc& summon = Npc[pet];
    DWORD now = SubWorld[summon.m_SubWorldIndex].m_dwCurrentTime;
    if (summon.m_NextAITime > now) return 0;
    summon.m_NextAITime = now + 2; // Reconstructed bounded AI cadence.
    summon.m_CurrentCamp = Npc[owner].m_CurrentCamp;
    summon.m_Camp = Npc[owner].m_Camp;
    summon.m_FightMode = Npc[owner].m_FightMode;
    int target=PtSummonCombatTarget(owner,pet);
    if (target>0)
    {
        // Use only skills actually supplied by the authentic NPC template.
        for (int s = 1; s <= MAX_NPC_USE_SKILL; ++s)
        {
            int id = summon.m_SkillList.GetSkillId(s);
            ISkill* skill = g_SkillManager.GetSkill(id, summon.m_SkillList.GetCurrentLevel(id));
            if (skill && skill->IsTargetEnemy() && !PhongThanIsSummon(id) &&
                (skill->GetSkillStyle() == SKILL_SS_Missles ||
                 skill->GetSkillStyle() == SKILL_SS_Melee ||
                 skill->GetSkillStyle() == SKILL_SS_PhongThanAttack) && summon.SetActiveSkill(s))
            {
                if(NpcSet.GetDistance(pet,target)>skill->GetAttackRadius()) {
                    int x,y;Npc[target].GetMpsPos(&x,&y);
                    if(summon.m_CurrentRunSpeed<Npc[owner].m_CurrentRunSpeed)summon.m_CurrentRunSpeed=Npc[owner].m_CurrentRunSpeed;
                    summon.SendCommand(do_run,x,y);return 0;
                }
                return target;
            }
        }
    }
    s_PtSummonTargets[owner].targetId=0;
    if (NpcSet.GetDistance(owner, pet) > PT_SUMMON_IDLE_FOLLOW_RANGE)
    {
        int x, y;
        Npc[owner].GetMpsPos(&x, &y);
        summon.m_CurrentRunSpeed = Npc[owner].m_CurrentRunSpeed;
        if (summon.m_CurrentRunSpeed < summon.m_RunSpeed)
            summon.m_CurrentRunSpeed = (float)summon.m_RunSpeed;
        summon.SendCommand(do_run, x + 96, y);
    }
    return 0;
}
#endif
