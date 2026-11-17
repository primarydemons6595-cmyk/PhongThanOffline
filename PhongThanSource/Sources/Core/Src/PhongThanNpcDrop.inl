#ifndef PHONGTHAN_NPC_DROP_INL
#define PHONGTHAN_NPC_DROP_INL
#include "PhongThanDropRules.h"
#include "PhongThanQuestItemTuple.h"
#include "../../../Headers/PhongThanRewardOwner.h"
static void PtDropTrace(int npc, int owner, const char* event, int row, int result)
{
    FILE* f = fopen("npc_drop_trace.log","a");
    if (!f) return;
    SYSTEMTIME now; GetLocalTime(&now);
    fprintf(f,"%04d-%02d-%02d %02d:%02d:%02d npc=%d template=%d entity=%u owner=%d event=%s entry=%d result=%d\n",
        now.wYear,now.wMonth,now.wDay,now.wHour,now.wMinute,now.wSecond,
        npc,Npc[npc].m_NpcSettingIdx,Npc[npc].m_dwID,owner,event,row,result);
    fclose(f);
}
static void PtDropNpc(KNpc& npc, int owner)
{
    if (npc.m_Kind != kind_normal || owner <= 0 || owner >= MAX_PLAYER ||
        !PhongThanRewardOwnerAvailable(Player[owner].m_nNetConnectIdx,Player[owner].m_bIsBot!=0,Player[owner].m_PlayerDBLoad!=0) || npc.m_SubWorldIndex < 0 ||
        npc.m_SubWorldIndex >= MAX_SUBWORLD || npc.m_RegionIndex < 0) return;
    int ownerNpc = Player[owner].m_nIndex;
    if (ownerNpc <= 0 || ownerNpc >= MAX_NPC || !Npc[ownerNpc].IsPlayer() ||
        Npc[ownerNpc].m_SubWorldIndex != npc.m_SubWorldIndex) return;
    KIniFile ini;
    if (!ini.Load("\\settings\\phongthan\\NpcDropPolicy.ini")) return;
    int enabled = 0, version = 0;
    if (!PtDropInt(ini,"Main","Version",0,&version) || version != 1 ||
        !PtDropInt(ini,"Main","Enabled",0,&enabled) || enabled != 1) return;
    char section[64], tableName[64], overrideName[64];
    tableName[0] = overrideName[0] = 0;
    ini.GetString("Main","DefaultTable","",tableName,sizeof(tableName));
    sprintf(section,"Template_%d",npc.m_NpcSettingIdx);
    char enabledText[64]; enabledText[0] = 0;
    if (ini.GetString(section,"Enabled","",enabledText,sizeof(enabledText)))
    {
        if (!PtDropInt(ini,section,"Enabled",0,&enabled)) return;
    }
    else enabled = 1;
    if (enabled != 1) return;
    ini.GetString(section,"Table","",overrideName,sizeof(overrideName));
    if (overrideName[0]) g_StrCpyLen(tableName,overrideName,sizeof(tableName));
    PtDropTable table;
    if (!PtDropReadTable(ini,tableName,table))
    { PtDropTrace(npc.m_Index,owner,"CONFIG_REJECT",0,0); return; }
    // Reject the entire table before any RNG/object side effect if an identity
    // is outside the supported native tuple contract. No random magic added.
    int i;
    for (i = 0; i < table.count; ++i)
    {
        const PtDropEntry& e = table.entries[i]; PhongThanQuestItemTuple tuple;
        if (!PhongThanDecodeQuestItemTuple(e.genre,e.detail,e.particular,e.level,e.series,e.luck,tuple))
        { PtDropTrace(npc.m_Index,owner,"IDENTITY_REJECT",i+1,0); return; }
    }
    PtDropTrace(npc.m_Index,owner,"PLAN",table.count,table.draws);
    for (i = 0; i < table.draws; ++i)
    {
        int picked = PtDropPick(table,g_Random(table.range));
        if (picked < 0) { PtDropTrace(npc.m_Index,owner,"NO_DROP",0,0); continue; }
        const PtDropEntry& e = table.entries[picked]; PhongThanQuestItemTuple tuple;
        PhongThanDecodeQuestItemTuple(e.genre,e.detail,e.particular,e.level,e.series,e.luck,tuple);
        for (int q = 0; q < e.quantity; ++q)
        {
            int magic[MAX_ITEM_MAGICLEVEL]; ZeroMemory(magic,sizeof(magic));
            int item = ItemSet.Add(tuple.genre,tuple.series,tuple.level,tuple.luck,
                tuple.detail,tuple.particular,magic,g_SubWorldSet.GetGameVersion());
            if (item <= 0) { PtDropTrace(npc.m_Index,owner,"ITEM_REJECT",picked+1,0); continue; }
            if (!PhongThanQuestItemMatches(Item[item],tuple) || !Item[item].GetName()[0] ||
                Item[item].GetWidth() <= 0 || Item[item].GetHeight() <= 0)
            { ItemSet.Remove(item); PtDropTrace(npc.m_Index,owner,"ITEM_REJECT",picked+1,0); continue; }
            char objectKind[32]; objectKind[0] = 0;
            if (Item[item].GetObjIdx() <= 0 ||
                !ObjSet.m_cTabFile.GetString(Item[item].GetObjIdx()+1,3,"",objectKind,sizeof(objectKind)) ||
                strcmp(objectKind,"Item") != 0)
            { ItemSet.Remove(item); PtDropTrace(npc.m_Index,owner,"OBJECT_TEMPLATE_REJECT",picked+1,0); continue; }
            int x, y; npc.GetMpsPos(&x,&y); POINT pos; pos.x = x; pos.y = y;
            SubWorld[npc.m_SubWorldIndex].GetFreeObjPos(pos);
            KMapPos map; map.nSubWorld = npc.m_SubWorldIndex;
            SubWorld[map.nSubWorld].Mps2Map(pos.x,pos.y,&map.nRegion,&map.nMapX,&map.nMapY,&map.nOffX,&map.nOffY);
            if (map.nRegion < 0) { ItemSet.Remove(item); continue; }
            KObjItemInfo info; ZeroMemory(&info,sizeof(info));
            info.m_nItemID = item; info.m_nItemWidth = Item[item].GetWidth(); info.m_nItemHeight = Item[item].GetHeight();
            info.m_nColorID = Item[item].GetColorItem(); info.m_nGenre = Item[item].GetGenre(); info.m_nDetailType = Item[item].GetDetailType();
            info.m_nMovieFlag = info.m_nSoundFlag = 1;
            g_StrCpyLen(info.m_szName,Item[item].GetName(),sizeof(info.m_szName));
            int object = ObjSet.Add(Item[item].GetObjIdx(),map,info);
            if (object <= 0) ItemSet.Remove(item); else Object[object].SetItemBelong(owner);
            PtDropTrace(npc.m_Index,owner,"DROP_OBJECT",picked+1,object);
        }
    }
}
#endif
