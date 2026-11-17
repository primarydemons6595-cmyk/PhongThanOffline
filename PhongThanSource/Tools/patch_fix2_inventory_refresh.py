"""Apply the narrow Fix #2 client inventory-refresh patch to legacy VC6 files."""

from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]


def replace_once(relative: str, old: bytes, new: bytes) -> None:
    path = ROOT / relative
    data = path.read_bytes()
    if new in data:
        return
    count = data.count(old)
    if count != 1:
        raise RuntimeError(f"Expected one anchor in {relative}, found {count}")
    path.write_bytes(data.replace(old, new, 1))


replace_once(
    "Sources/Core/Src/KItemList.h",
    b"\tBOOL\t\tAutoMoveItem(ItemPos SrcPos,ItemPos DesPos);\r\n\tvoid\t\tMenuSetMouseItem();",
    b"\tBOOL\t\tAutoMoveItem(ItemPos SrcPos,ItemPos DesPos);\r\n"
    b"\tvoid\t\tRefreshItem(int nIdx);\r\n"
    b"\tvoid\t\tMenuSetMouseItem();",
)

replace_once(
    "Sources/Core/Src/KProtocolProcess.cpp",
    b"\telse\n\t{\n"
    b"\t\tPlayer[CLIENT_PLAYER_INDEX].m_ItemList.Add(nIndex,\n"
    b"\t\t\tPlayer[CLIENT_PLAYER_INDEX].m_ItemList.m_Items[nIndex].nPlace,\n"
    b"\t\t\tPlayer[CLIENT_PLAYER_INDEX].m_ItemList.m_Items[nIndex].nX,\n"
    b"\t\t\tPlayer[CLIENT_PLAYER_INDEX].m_ItemList.m_Items[nIndex].nY);\n"
    b"\t}\n}",
    b"\telse\n\t{\n"
    b"\t\tPlayer[CLIENT_PLAYER_INDEX].m_ItemList.RefreshItem(nIndex);\n"
    b"\t}\n}",
)

implementation = b"""void KItemList::RefreshItem(int nGameIdx)
{
\tconst int nListIdx = FindSame(nGameIdx);
\tif (nListIdx <= 0 || nGameIdx <= 0 || nGameIdx >= MAX_ITEM)
\t\treturn;

\tKUiObjAtContRegion Info;
\tZeroMemory(&Info, sizeof(Info));
\tInfo.Obj.uGenre = CGOG_ITEM;
\tInfo.Obj.uId = nGameIdx;
\tInfo.Region.Width = Item[nGameIdx].GetWidth();
\tInfo.Region.Height = Item[nGameIdx].GetHeight();
\tInfo.Region.h = m_Items[nListIdx].nX;
\tInfo.Region.v = m_Items[nListIdx].nY;

\tswitch (m_Items[nListIdx].nPlace)
\t{
\tcase pos_hand: Info.eContainer = UOC_IN_HAND; break;
\tcase pos_equiproom: Info.eContainer = UOC_ITEM_TAKE_WITH; break;
\tcase pos_equiproomex: Info.eContainer = UOC_ITEM_TAKE_WITH_EX; break;
\tcase pos_repositoryroom: Info.eContainer = UOC_STORE_BOX; break;
\tcase pos_repositoryroom1: Info.eContainer = UOC_STORE_BOX1; break;
\tcase pos_repositoryroom2: Info.eContainer = UOC_STORE_BOX2; break;
\tcase pos_repositoryroom3: Info.eContainer = UOC_STORE_BOX3; break;
\tcase pos_repositoryroom4: Info.eContainer = UOC_STORE_BOX4; break;
\tcase pos_repositoryroom5: Info.eContainer = UOC_STORE_BOX5; break;
\tcase pos_immediacy: Info.eContainer = UOC_IMMEDIA_ITEM; break;
\tcase pos_give: Info.eContainer = UOC_ITEM_GIVE; break;
\tcase pos_compoundroom: Info.eContainer = UOC_COMPOUND_BOX; break;
\tcase pos_trade1:
\t\tCoreDataChanged(GDCNI_TRADE_DESIRE_ITEM, (DWORD)&Info, 1);
\t\treturn;
\tdefault:
\t\treturn;
\t}

\tCoreDataChanged(GDCNI_OBJECT_CHANGED, (DWORD)&Info, 1);
}

"""

cpp = ROOT / "Sources/Core/Src/KItemList.cpp"
data = cpp.read_bytes()
signature = b"void\tKItemList::MenuSetMouseItem()"
if implementation not in data:
    if data.count(signature) != 1:
        raise RuntimeError("Expected one MenuSetMouseItem anchor")
    cpp.write_bytes(data.replace(signature, implementation + signature, 1))

print("FIX2_INVENTORY_REFRESH=INSTALLED")
