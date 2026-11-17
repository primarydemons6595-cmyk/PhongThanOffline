$ErrorActionPreference='Stop'
$root=Split-Path $PSScriptRoot -Parent
$hook=Join-Path $PSScriptRoot 'Invoke-LegacyAsciiHook.ps1'
function Replace-Hook($relative,$old,$new) {
    $path=Join-Path $root $relative
    $hash=(Get-FileHash -LiteralPath $path -Algorithm SHA256).Hash
    & $hook -Path $path -ExpectedSha256 $hash -Old $old -New $new -Apply
}
Replace-Hook 'Sources/Core/Src/KItem.h' '#include "../../../Headers/PhongThanUpgradeState.h"' "#include `"../../../Headers/PhongThanUpgradeState.h`"`r`n#include `"PhongThanItemLockPolicy.h`""
Replace-Hook 'Sources/Core/Src/KItem.h' 'GetLock() {return &m_CommonAttrib.LockItem;}' 'GetLock() {PhongThanExpireItemUnlock(m_CommonAttrib.LockItem, KSG_GetCurSec()); return &m_CommonAttrib.LockItem;}'
Replace-Hook 'Sources/Core/Src/KItem.h' 'm_CommonAttrib.LockItem.IsLock())' 'GetLock()->IsLock())'
Replace-Hook 'Sources/Core/Src/KItem.cpp' 'switch (m_CommonAttrib.LockItem.nState)' 'switch (GetLock()->nState)'
Replace-Hook 'Sources/Core/Src/KBuySell.cpp' '#include "PhongThanConsumables.h"' "#include `"PhongThanConsumables.h`"`r`n#include `"ItemActionDiag.h`""
$old="`tif (!bShowMsg)`r`n`t`tg_pServer->PackDataToClient(Player[nPlayerIdx].m_nNetConnectIdx, &sMsg, sMsg.m_wLength + 1);"
$new="`tif (!bShowMsg)`r`n`t{`r`n`t`tSO_ItemActionDiag(`"ShopBuy_REJECT`", nBuy, nBuyIdx, nBuyNumber, nPrice, Player[nPlayerIdx].m_ItemList.GetEquipmentMoney(), 0, Npc[Player[nPlayerIdx].m_nIndex].m_FightMode, sMsg.m_wMsgID);`r`n`t`tg_pServer->PackDataToClient(Player[nPlayerIdx].m_nNetConnectIdx, &sMsg, sMsg.m_wLength + 1);`r`n`t}"
Replace-Hook 'Sources/Core/Src/KBuySell.cpp' $old $new
$old="`tif (Item[nIdx].GetLock()->IsLock() || `r`n`t`tItem[nIdx].GetLockSell())`r`n`t{"
$new=$old+"`r`n`t`tSO_ItemActionDiag(`"ShopSell_LOCK_REJECT`", Item[nIdx].GetGenre(), Item[nIdx].GetID(), Item[nIdx].GetRow(), Item[nIdx].GetLockSell(), Item[nIdx].GetLockTrade(), Item[nIdx].GetLockDrop(), Item[nIdx].GetLock()->nState, nBuy);"
Replace-Hook 'Sources/Core/Src/KBuySell.cpp' $old $new
