"""Apply the narrow ASCII Fix #1D edits to legacy-encoded VC6 sources."""
from pathlib import Path


def replace_once(data: bytes, old: bytes, new: bytes, label: str) -> bytes:
    count = data.count(old)
    if count != 1:
        raise RuntimeError(f"{label}: expected one match, found {count}")
    return data.replace(old, new, 1)


root = Path(__file__).resolve().parents[1] / "Sources" / "Core" / "Src"
sort_path = root / "KSortScript.cpp"
data = sort_path.read_bytes()
if b"LoadPhongThanScriptAliasRegistry" not in data:
    data = replace_once(
        data,
        b"static unsigned int g_nScriptLoadFailures;\nchar g_szCurScriptDir[MAX_PATH];",
        b"static unsigned int g_nScriptLoadFailures;\nstatic unsigned int g_nScriptAliasCount;\nchar g_szCurScriptDir[MAX_PATH];",
        "alias counter",
    )
    data = replace_once(
        data,
        b"\r\nunsigned long g_IniScriptEngine()\n",
        b"\r\n#include \"PhongThanScriptAliasRegistry.inl\"\n\nunsigned long g_IniScriptEngine()\n",
        "registry include",
    )
    data = replace_once(
        data,
        b"\tg_nScriptLoadFailures = 0;\n\tg_ScriptBinTree.ClearList();\n\tunsigned long nLoaded = LoadAllScript(\"\\\\script\");\n\tFILE *pDiag",
        b"\tg_nScriptLoadFailures = 0;\n\tg_nScriptAliasCount = 0;\n\tg_ScriptBinTree.ClearList();\n\tunsigned long nLoaded = LoadAllScript(\"\\\\script\");\n\tBOOL bAliasRegistryValid = TRUE;\n#ifdef _SERVER\n\tbAliasRegistryValid = LoadPhongThanScriptAliasRegistry(\n\t\t\"\\\\script\\\\phongthan\\\\mapexit\\\\registry.tsv\");\n#endif\n\tFILE *pDiag",
        "registry load",
    )
    data = replace_once(
        data,
        b"fprintf(pDiag, \"loaded=%lu failed=%u capacity=%d\\n\",\n\t\t\tnLoaded, g_nScriptLoadFailures, MAX_SCRIPT_IN_SET);",
        b"fprintf(pDiag, \"loaded=%lu failed=%u aliases=%u alias_registry=%s capacity=%d\\n\",\n\t\t\tnLoaded, g_nScriptLoadFailures, g_nScriptAliasCount,\n\t\t\tbAliasRegistryValid ? \"PASS\" : \"FAIL\", MAX_SCRIPT_IN_SET);",
        "registry diagnostics",
    )
    data = replace_once(data, b"\treturn nLoaded;\n}\n", b"\treturn bAliasRegistryValid ? nLoaded : 0;\n}\n", "registry failure")
    sort_path.write_bytes(data)

npc_path = root / "KNpc.cpp"
data = npc_path.read_bytes()
start = data.find(b"\t// PT map 1's authored SuperTrap cells store the original packed-script ID.")
end_marker = b"\n\tif (m_Kind != kind_player)"
if start >= 0:
    end = data.find(end_marker, start)
    if end < 0:
        raise RuntimeError("KNpc binding block end not found")
    data = data[:start] + data[end + 1:]

start = data.find(b"\tif (m_TrapScriptID == dwPhongThanSuperTrapId)")
end_marker = b"\n\tPlayer[m_nPlayerIdx].ExecuteScript(m_TrapScriptID, NORMAL_FUNCTION_NAME, 0);"
if start >= 0:
    end = data.find(end_marker, start)
    if end < 0:
        raise RuntimeError("KNpc dispatch block end not found")
    data = data[:start] + data[end + 1:]
npc_path.write_bytes(data)
print("PATCH_FIX1D_REGISTRY=PASS")
