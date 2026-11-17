// Included after GetNpcKeyData so summons share the actual Lua-backed loader.
#ifdef _SERVER
#include "../../../Headers/PhongThanSummonStats.h"
struct PtSummonStatReader {
    KLuaScript* script;int row;
    PtSummonStatReader(int npc):script(0),row(npc+2) {
        if(g_pNpcTemplate[npc])script=(KLuaScript*)g_GetScript(g_pNpcTemplate[npc]->m_LevelScriptID);
    }
    int Read(int level,const char* style,const char* column) {
        return GetNpcKeyData(script,row,level,style,column);
    }
};
static bool PtConfigureSummonCombatLevel(int pet,int ownerLevel,int rank) {
    int npc=Npc[pet].m_NpcSettingIdx;
    if(npc<0 || npc>=MAX_NPCSTYLE || !g_pNpcTemplate[npc])return false;
    PtSummonStatReader reader(npc);
    return PhongThanApplySummonCombatLevel(Npc[pet],ownerLevel,reader,rank);
}
#endif
