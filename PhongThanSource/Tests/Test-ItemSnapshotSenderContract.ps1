$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
$source=[IO.File]::ReadAllText((Join-Path $root 'Sources/Core/Src/KItemList.cpp'),[Text.Encoding]::GetEncoding(28591))
if($source -notmatch 'PHONGTHAN_MSG_INVENTORY_ITEM_SNAPSHOT,\s*sizeof\(sItem\),\s*PHONGTHAN_WIRE_FLAG_RESPONSE'){
 throw 'Actual item snapshot sender does not match response-only receiver'
}
$receiver=[IO.File]::ReadAllText((Join-Path $root 'Headers/PhongThanProtocol.h'))
if($receiver -notmatch 'item->Header.Flags == PHONGTHAN_WIRE_FLAG_RESPONSE'){throw 'Receiver contract changed'}
$protocol=[IO.File]::ReadAllText((Join-Path $root 'Sources/Core/Src/KProtocolProcess.cpp'),[Text.Encoding]::GetEncoding(28591))
$itemList=[IO.File]::ReadAllText((Join-Path $root 'Sources/Core/Src/KItemList.cpp'),[Text.Encoding]::GetEncoding(28591))
if($protocol -notmatch 'else\s*\{\s*Player\[CLIENT_PLAYER_INDEX\]\.m_ItemList\.RefreshItem\(nIndex\);\s*\}'){
 throw 'Existing item snapshots still attempt to Add into an occupied slot.'
}
if($itemList -notmatch '(?s)void KItemList::RefreshItem\(int nGameIdx\).*?FindSame\(nGameIdx\).*?pos_equiproom:\s*Info\.eContainer = UOC_ITEM_TAKE_WITH.*?CoreDataChanged\(GDCNI_OBJECT_CHANGED, \(DWORD\)&Info, 1\)'){
 throw 'Existing inventory item refresh does not notify the owned F4 slot.'
}
'PASS: snapshot flags match and existing stack updates refresh the owned client slot without remove/add.'
