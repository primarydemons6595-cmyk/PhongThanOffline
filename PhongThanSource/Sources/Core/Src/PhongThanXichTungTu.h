#ifndef PHONGTHAN_XICH_TUNG_TU_H
#define PHONGTHAN_XICH_TUNG_TU_H

// Original PAK Region_S: Dieu Tri, passerby058, script entry D02B148D.
// Template 206 alone is an appearance, not a unique NPC service identity.
#define PT_XICH_TUNG_TU_SCRIPT "\\script\\phongthan\\npc_services\\xich_tung_tu.lua"
inline bool PhongThanIsXichTungTu(int world, int npcTemplate, DWORD scriptId)
{
    return world == 1052 && npcTemplate == 206 &&
        (scriptId == 0xD02B148DUL || scriptId == g_FileName2Id(PT_XICH_TUNG_TU_SCRIPT));
}

inline bool PhongThanIsLeiZhenZi(int world,int npcTemplate,DWORD scriptId)
{ return world==1052 && (scriptId==0xC821F1CDUL || scriptId==0xB89373F6UL); }
inline bool PhongThanIsTaoBao(int world,int npcTemplate,DWORD scriptId)
{ return world==1073 && npcTemplate==759 && scriptId==g_FileName2Id("\\script\\phongthan\\npc_restore\\1073_06.lua"); }
inline bool PhongThanIsComposeService(int world, int npcTemplate, DWORD scriptId)
{
    return PhongThanIsXichTungTu(world,npcTemplate,scriptId) || PhongThanIsLeiZhenZi(world,npcTemplate,scriptId) || PhongThanIsTaoBao(world,npcTemplate,scriptId);
}
#endif
