$ErrorActionPreference='Stop'
$root=Split-Path $PSScriptRoot -Parent
function Hook($relative,$old,$new) {
    $p=Join-Path $root $relative
    $raw=[Text.Encoding]::ASCII.GetString([IO.File]::ReadAllBytes($p))
    if($raw.Contains($new) -or $raw.Contains($new.Replace("`r`n","`n"))) { return }
    if(!$raw.Contains($old)) { $old=$old.Replace("`r`n","`n"); $new=$new.Replace("`r`n","`n") }
    & (Join-Path $PSScriptRoot 'Invoke-LegacyAsciiHook.ps1') -Path $p -ExpectedSha256 (Get-FileHash $p).Hash -Old $old -New $new -Apply
}
Hook 'Sources/GameClient/Ui/UiCase/UiTrade.h' '#include "../Elem/WndShowAnimate.h"' "#include `"../Elem/WndShowAnimate.h`"`r`n#include `"../Elem/WndImage.h`""
Hook 'Sources/GameClient/Ui/UiCase/UiTrade.h' '    KWndButton m_RpsChoice[3];' "    KWndButton m_RpsChoice[3];`r`n    KWndImage m_RpsImage[2];"
Hook 'Sources/GameClient/Ui/UiCase/UiTrade.cpp' '    for (int rps = 0; rps < 3; ++rps) AddChild(&m_RpsChoice[rps]);' "    for (int rps = 0; rps < 3; ++rps) AddChild(&m_RpsChoice[rps]);`r`n    for (int side = 0; side < 2; ++side) AddChild(&m_RpsImage[side]);"
Hook 'Sources/Core/Src/KProtocolProcess.cpp' '#include "PhongThanConsumables.h"' "#include `"PhongThanConsumables.h`"`r`n#include `"ItemActionDiag.h`""
$old="`tint nIndex = 0;`r`n`tif (pItem->IsNew)"
$new="`tint nIndex = 0;`r`n`tif (Player[CLIENT_PLAYER_INDEX].CheckTrading(true))`r`n`t`tSO_ItemActionDiag(`"TradeSnapshot_RECEIVE`", pItem->Genre, pItem->ItemId, pItem->SlotX, pItem->SlotY, pItem->TemplateRow, pItem->GeneratorVersion, pItem->Container, pItem->IsNew);`r`n`tif (pItem->IsNew)"
Hook 'Sources/Core/Src/KProtocolProcess.cpp' $old $new
$old="`tif (nIndex <= 0 || nIndex >= MAX_ITEM)`r`n`t`treturn;`r`n`r`n`tKLockItem Lock;"
$new="`tif (nIndex <= 0 || nIndex >= MAX_ITEM)`r`n`t{`r`n`t`tif (Player[CLIENT_PLAYER_INDEX].CheckTrading(true))`r`n`t`t`tSO_ItemActionDiagText(`"TradeSnapshot_GENERATE_FAIL`", `"item`", pItem->ItemId, pItem->TemplateRow);`r`n`t`treturn;`r`n`t}`r`n`r`n`tKLockItem Lock;"
Hook 'Sources/Core/Src/KProtocolProcess.cpp' $old $new
$old="`t`tPlayer[CLIENT_PLAYER_INDEX].m_ItemList.Add(nIndex,`r`n`t`t`tpItem->Container, pItem->SlotX, pItem->SlotY);"
$new="`t`tint added = Player[CLIENT_PLAYER_INDEX].m_ItemList.Add(nIndex,`r`n`t`t`tpItem->Container, pItem->SlotX, pItem->SlotY);`r`n`t`tif (Player[CLIENT_PLAYER_INDEX].CheckTrading(true))`r`n`t`t`tSO_ItemActionDiag(`"TradeSnapshot_ADD`", pItem->Genre, pItem->ItemId, pItem->SlotX, pItem->SlotY, nIndex, added, pItem->Container, pItem->TemplateRow);"
Hook 'Sources/Core/Src/KProtocolProcess.cpp' $old $new
