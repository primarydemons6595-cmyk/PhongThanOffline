$ErrorActionPreference='Stop'
$root=Split-Path $PSScriptRoot -Parent
function Read-Ascii($relative) { [Text.Encoding]::ASCII.GetString([IO.File]::ReadAllBytes((Join-Path $root $relative))) }
$notify=Read-Ascii 'Sources/GameClient/Ui/GameSpaceChangedNotify.cpp'
$start=$notify.IndexOf('if (pObject->eContainer == UOC_ITEM_TAKE_WITH)')
$end=$notify.IndexOf('} else if (pObject->eContainer == UOC_ITEM_TAKE_WITH_EX)', $start)
if($start -lt 0 -or $end -lt 0) { throw 'Missing inventory notification branch' }
$branch=$notify.Substring($start,$end-$start)
if(!$branch.Contains('pTradeBar->OnChangedTakewithItem') -or !$branch.Contains('pItemsBar->UpdateItem') -or $branch.Contains('} else {')) { throw 'Trade notification excludes F4' }
$start=$notify.IndexOf('case GDCNI_CONTAINER_OBJECT_CHANGED:')
$end=$notify.IndexOf('case GDCNI_NPC_TRADE:', $start)
$branch=$notify.Substring($start,$end-$start)
if(!$branch.Contains('inventory->UpdateItem(NULL, 0)') -or !$branch.Contains('trade->OnChangedTakewithItem(NULL, 0)')) { throw 'Snapshot restore refresh missing' }
$click=Read-Ascii 'Sources/GameClient/Ui/UiCase/UiItem.cpp'
if(!$click.Contains('if (bDoImmed && m_nInventoryPage == 0 && trade) trade->PutInventoryItem(pItem);')) { throw 'Main-bag right-click route missing' }
$rps=Read-Ascii 'Sources/Core/Src/PhongThanRpsRoundClient.inl'
if(!$rps.Contains('m_ItemList.RecoverTrade();') -or !$rps.Contains('CoreDataChanged(GDCNI_CONTAINER_OBJECT_CHANGED, UOC_ITEM_TAKE_WITH, 0);')) { throw 'RPS restore notification missing' }
foreach($pair in @(
    @('Sources/GameClient/Ui/Elem/WndObjContainer.h','bool FindFreePlace('),
    @('Sources/GameClient/Ui/Elem/WndObjContainer.cpp','#include "WndObjMatrixFreePlace.inl"'),
    @('Sources/GameClient/Ui/UiCase/UiTrade.h','bool PutInventoryItem('),
    @('Sources/GameClient/Ui/UiCase/UiTrade.cpp','#include "UiTradeQuickPut.inl"')
)) {
    if(([regex]::Matches((Read-Ascii $pair[0]),[regex]::Escape($pair[1]))).Count -ne 1) { throw 'Hook missing or duplicated' }
}
'PASS: inventory delta fanout, snapshot refresh, right-click routing, RPS restore source contracts; four hooks unique'
