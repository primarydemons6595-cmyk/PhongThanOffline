#!/usr/bin/env python3
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]


def patch_ansi(relative, changes):
    path = ROOT / relative
    data = path.read_bytes()
    for before, after in changes:
        old = before.encode("latin1")
        new = after.encode("latin1")
        count = data.count(old)
        if count == 0:
            lf_old = old.replace(b"\r\n", b"\n")
            lf_new = new.replace(b"\r\n", b"\n")
            lf_count = data.count(lf_old)
            if lf_count == 1:
                old, new, count = lf_old, lf_new, lf_count
        if count == 0 and not new:
            continue
        if count == 0 and (data.count(new) == 1 or
                           data.count(new.replace(b"\r\n", b"\n")) == 1):
            continue
        if count != 1:
            raise RuntimeError(f"{relative}: expected one match, got {count}: {before[:80]!r}")
        data = data.replace(old, new, 1)
    path.write_bytes(data)


patch_ansi("Sources/Core/Src/KItemList.h", [
    ("\tvoid\t\tExecuteScript(int nIndex);", "\tBOOL\t\tExecuteScript(int nIndex);"),
])

patch_ansi("Sources/Core/Src/KItemList.cpp", [
    ("#ifdef _SERVER\r\n\t\tExecuteScript(nIdx);\r\n#endif\r\n\t\treturn TRUE;",
     "#ifdef _SERVER\r\n\t\tif (!ExecuteScript(nIdx))\r\n\t\t\treturn FALSE;\r\n\r\n"
     "\t\t// PT magic-script items are charged only after main() loads.\r\n"
     "\t\t// IB-item scripts retain ownership of success-time consumption.\r\n"
     "\t\tif (nItemGenre == item_magicscript && nDetailType != 61000 &&\r\n"
     "\t\t\tFindSame(nIdx) > 0 && !RemoveItem(nIdx, 1))\r\n"
     "\t\t\treturn FALSE;\r\n#endif\r\n\t\treturn TRUE;"),
    ("void KItemList::ExecuteScript(int nIdx)", "BOOL KItemList::ExecuteScript(int nIdx)"),
    ("\tif (nIdx <= 0 || nIdx >= MAX_ITEM || m_PlayerIdx <= 0)\r\n\t\treturn;",
     "\tif (nIdx <= 0 || nIdx >= MAX_ITEM || m_PlayerIdx <= 0)\r\n\t\treturn FALSE;"),
    ("\t\tPhongThanOpenStarterBag(Player[m_PlayerIdx], nIdx);\n\t\treturn;",
     "\t\treturn PhongThanOpenStarterBag(Player[m_PlayerIdx], nIdx) ? TRUE : FALSE;"),
    ("\tif (nGenre == item_equip)\r\n\t\treturn;",
     "\tif (nGenre == item_equip)\r\n\t\treturn FALSE;"),
    ("\t\tItem[nIdx].ApplyMagicAttribToNPC(&Npc[Player[m_PlayerIdx].m_nIndex], MAX_ITEM_MAGICATTRIB / 2);\r\n\t\treturn;",
     "\t\tItem[nIdx].ApplyMagicAttribToNPC(&Npc[Player[m_PlayerIdx].m_nIndex], MAX_ITEM_MAGICATTRIB / 2);\r\n\t\treturn TRUE;"),
    ("\t\tnGenre != item_magicscript &&\n\t\tnGenre != item_ibitem)\n\t\treturn;",
     "\t\tnGenre != item_magicscript &&\n\t\tnGenre != item_ibitem)\n\t\treturn FALSE;"),
    ("\tif (!szScriptFile || !szScriptFile[0])\r\n\t\treturn;",
     "\tif (!szScriptFile || !szScriptFile[0])\r\n\t\treturn FALSE;"),
    ("\tif (nScriptLen <= 4 || nScriptLen >= 128)\r\n\t\treturn;",
     "\tif (nScriptLen <= 4 || nScriptLen >= 128)\r\n\t\treturn FALSE;"),
    ("\t\tif (ch < 32 || ch == ':')\r\n\t\t\treturn;",
     "\t\tif (ch < 32 || ch == ':')\r\n\t\t\treturn FALSE;"),
    ("\tif (strstr(szScriptFile, \"..\"))\r\n\t\treturn;",
     "\tif (strstr(szScriptFile, \"..\"))\r\n\t\treturn FALSE;"),
    ("\tif (!pszExt || _stricmp(pszExt, \".lua\") != 0)\r\n\t\treturn;",
     "\tif (!pszExt || _stricmp(pszExt, \".lua\") != 0)\r\n\t\treturn FALSE;"),
    ("\tBOOL bScriptRet = Player[m_PlayerIdx].ExecuteScript(szScriptFile, NORMAL_FUNCTION_NAME, nIdx);\n\tSO_ItemActionDiagText(\"ExecuteScript_RESULT\", szScriptFile, nIdx, bScriptRet);\r\n}",
     "\tBOOL bScriptRet = Player[m_PlayerIdx].ExecuteScript(szScriptFile, NORMAL_FUNCTION_NAME, nIdx);\n\tSO_ItemActionDiagText(\"ExecuteScript_RESULT\", szScriptFile, nIdx, bScriptRet);\r\n\treturn bScriptRet;\r\n}"),
])

patch_ansi("Sources/Core/Src/KNpc.cpp", [
    ("#include \"PhongThanNpcVitalRules.h\"\r\n#include \"PhongThanNpcVitalRules.h\"\n",
     "#include \"PhongThanNpcVitalRules.h\"\n"),
    ("#include \"PhongThanNpcVitalRules.h\"\r\n#include \"PhongThanNpcVitalRules.h\"",
     "#include \"PhongThanNpcVitalRules.h\""),
    ("\t\tif (m_StunState.nTime <= 0)\r\n\t\t{\r\n\t\t\tm_CurrentLife += m_CurrentLifeReplenish + (m_CurrentLifeReplenish * m_CurrentLifeReplenishPercent / MAX_PERCENT);\r\n\t\t\tif (m_CurrentLife > m_CurrentLifeMax)\r\n\t\t\t\tm_CurrentLife = m_CurrentLifeMax;",
     "\t\tif (m_StunState.nTime <= 0 && m_CurrentLife > 0 &&\r\n\t\t\tm_Doing != do_death && m_Doing != do_revive)\r\n\t\t{\r\n\t\t\tm_CurrentLife = PhongThanApplyNpcLifeRegen(\r\n\t\t\t\tm_CurrentLife, m_CurrentLifeMax, m_CurrentLifeReplenish,\r\n\t\t\t\tm_CurrentLifeReplenishPercent, MAX_PERCENT, TRUE);"),
    ("\tNpcSync.MaxLife\t\t= (int)m_CurrentLifeMax;\r\n\tif (m_CurrentLifeMax > 0)\r\n\t\tNpcSync.Life\t\t= (int)m_CurrentLife;",
     "\tNpcSync.MaxLife\t\t= m_CurrentLifeMax > 0 ? (int)m_CurrentLifeMax : 0;\r\n\tif (NpcSync.MaxLife > 0)\r\n\t\tNpcSync.Life\t\t= PhongThanClampLife((int)m_CurrentLife, NpcSync.MaxLife);"),
    ("\tNpcSync.MaxLife\t\t= (int)m_CurrentLifeMax;\r\n\tif (m_CurrentLifeMax > 0)\r\n\t\tNpcSync.Life\t\t\t= (int)m_CurrentLife;",
     "\tNpcSync.MaxLife\t\t= m_CurrentLifeMax > 0 ? (int)m_CurrentLifeMax : 0;\r\n\tif (NpcSync.MaxLife > 0)\r\n\t\tNpcSync.Life\t\t\t= PhongThanClampLife((int)m_CurrentLife, NpcSync.MaxLife);"),
])

patch_ansi("Sources/Core/Src/Scene/KScenePlaceC.cpp", [
    ("#include \"KWeatherMgr.h\"\r\n\r\n#ifdef SWORDONLINE_SHOW_DBUG_INFO",
     "#include \"KWeatherMgr.h\"\r\n#include \"PhongThanNpcVitalRules.h\"\r\n\r\n#ifdef SWORDONLINE_SHOW_DBUG_INFO"),
    ("\tnScreenX = nWorldX - nFocusX + nViewWidth / 2;\r\n"
     "\tnScreenY = (nWorldY - nFocusY) / 2 + nViewHeight / 2 -\r\n"
     "\t\t((nWorldZ * 887) >> 10);",
     "\tPhongThanProjectWorldAnchor(nWorldX, nWorldY, nWorldZ,\r\n"
     "\t\tnFocusX, nFocusY, nViewWidth, nViewHeight, nScreenX, nScreenY);"),
    ("\tint nPercent = NpcObj.m_CurrentLife * MAX_PERCENT / NpcObj.m_CurrentLifeMax;\r\n\tif (nPercent < 0)\r\n\t\tnPercent = 0;\r\n\telse if (nPercent > MAX_PERCENT)\r\n\t\tnPercent = MAX_PERCENT;",
     "\tconst int nPercent = PhongThanLifePercent(\r\n\t\tNpcObj.m_CurrentLife, NpcObj.m_CurrentLifeMax, MAX_PERCENT);"),
    ("\tBackground.bRenderFlag = RUIMAGE_RENDER_FLAG_REF_SPOT;",
     "\t// Screen-space top-left coordinates: REF_SPOT would shift only the SPR.\r\n\tBackground.bRenderFlag = 0;"),
    ("\tstatic BYTE s_bOverlayLogged[MAX_NPC] = {0};\r\n", ""),
    ("\t\t// Preserve the entity-specific sprite/body/mount visual height.\r\n"
     "\t\tif (nNameHeight <= 0)\r\n\t\t\tnNameHeight = 96;",
     "\t\t// Preserve the entity-specific sprite/body/mount visual height.\r\n"
     "\t\tnNameHeight = PhongThanVisualAnchorHeight(nNameHeight, 96);"),
    ("\t\tif (nNameHeight < 96)\r\n\t\t\tnNameHeight = 96;\r\n\t\telse if (nNameHeight > 220)\r\n\t\t\tnNameHeight = 220;",
     "\t\t// Preserve the entity-specific sprite/body/mount visual height.\r\n"
     "\t\tnNameHeight = PhongThanVisualAnchorHeight(nNameHeight, 96);"),
    ("\t\t// Preserve the entity-specific sprite/body/mount visual height.\r\n"
     "\t\tif (nNameHeight <= 0)\r\n\t\t\tnNameHeight = 96;",
     "\t\t// Preserve the entity-specific sprite/body/mount visual height.\r\n"
     "\t\tnNameHeight = PhongThanVisualAnchorHeight(nNameHeight, 96);"),
    ("\r\n\t\tif (nNpcIdx > 0 && nNpcIdx < MAX_NPC && !s_bOverlayLogged[nNpcIdx])\r\n\t\t{\r\n\t\t\ts_bOverlayLogged[nNpcIdx] = 1;\r\n\t\t\tFILE* pLog = fopen(\"client_npc_name_overlay_diag.log\", \"a\");\r\n\t\t\tif (pLog)\r\n\t\t\t{\r\n\t\t\t\tfprintf(pLog,\r\n\t\t\t\t\t\"overlay=1 index=%d kind=%d world=%d,%d focus=%d,%d screen=%d,%d view=%d,%d height=%d name=%s\\n\",\r\n\t\t\t\t\tnNpcIdx, NpcObj.m_Kind, nMpsX, nMpsY,\r\n\t\t\t\t\tnFocusX, nFocusY, nScreenX, nScreenY,\r\n\t\t\t\t\tnViewWidth, nViewHeight, nNameHeight, pszName);\r\n\t\t\t\tfclose(pLog);\r\n\t\t\t}\r\n\t\t}", ""),
])

print("Small runtime hotfix source patch applied.")
