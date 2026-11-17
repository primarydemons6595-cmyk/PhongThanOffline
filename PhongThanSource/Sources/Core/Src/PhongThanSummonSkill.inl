// KSkill is retained as the ABI host because current UI callers cast ISkill
// to KSkill. Summons never enter the legacy JX CreateNpc implementation.
BOOL KSkill::LoadPhongThanSummon(int id, int level)
{
    if (!PhongThanIsSummon(id) || level < 1 || level >= MAX_SKILLLEVEL)
        return FALSE;
    KTabFile table;
    if (!table.Load("\\settings\\summonskill.txt")) return FALSE;
    int row = 0;
    for (int i = 2; i <= table.GetHeight(); ++i)
    {
        int found = 0;
        table.GetInteger(i, "id", 0, &found);
        if (found == id) { if (row) return FALSE; row = i; }
    }
    if (!row) return FALSE;
    int npc = -1, type = -1, base = -1, param = -1, delay = -1;
    table.GetInteger(row, "summonnpcid", -1, &npc);
    table.GetInteger(row, "costtype", -1, &type);
    table.GetInteger(row, "costvalue", -1, &base);
    table.GetInteger(row, "costparam", -1, &param);
    table.GetInteger(row, "casttime", -1, &delay);
    int cost = PhongThanSummonCost(base, param, level);
    if (npc != PhongThanSummonTemplate(id) || type != 0 || cost < 0 ||
        delay <= 0 || delay > 36000) return FALSE;

    // All members in this span are scalar/POD storage. Preserve the ISkill
    // base/vptr; initialize only this new summon instance, never a loaded skill.
    memset(&m_ulLevel, 0, (char*)m_szMagicSkillDesc +
        sizeof(m_szMagicSkillDesc) - (char*)&m_ulLevel);
    m_ulLevel = level;
    m_nId = id;
    m_eSkillStyle = SKILL_SS_Reserved14;
    m_nAttrib = 2;
    m_bHaveLoad = TRUE;
    m_bClientSend = TRUE;
    m_nIsExpSkill = TRUE;
    m_bTargetSelf = TRUE;
    m_bTargetNoNpc = TRUE;
    m_nEquiptLimited = -2;
    m_nSkillCostType = attrib_mana_v;
    m_nCost = cost;
    m_nMinTimePerCast = m_nMinTimePerCastOnHorse = delay;
    m_nAttackRadius = 128;
    m_nCharActionId = cdo_magic;
    m_nValue1 = npc;
    table.GetString(row, "name", "", m_szName, sizeof(m_szName));
#ifndef _SERVER
    memset(&m_RUIconImage, 0, sizeof(m_RUIconImage));
    memset(m_szSkillDesc, 0, sizeof(m_szSkillDesc));
    m_eLRSkillInfo = RightOnlySkill;
    table.GetString(row, "icon", "", m_szSkillIcon, sizeof(m_szSkillIcon));
    table.GetString(row, "castani", "", m_szPreCastEffectFile, sizeof(m_szPreCastEffectFile));
    table.GetString(row, "desc", "", m_szSkillDesc, sizeof(m_szSkillDesc));
#endif
    return m_szName[0] != 0;
}
