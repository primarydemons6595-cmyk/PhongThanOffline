$ErrorActionPreference='Stop'
$source=Split-Path $PSScriptRoot -Parent
$hook=Join-Path $PSScriptRoot 'Invoke-LegacyAsciiHook.ps1'
function Hook($relative,$old,$new) {
    $path=Join-Path $source $relative
    $raw=[Text.Encoding]::GetEncoding(28591).GetString([IO.File]::ReadAllBytes($path))
    if ($new.Length -ge $old.Length -and ($raw.Contains($new) -or $raw.Contains($new.Replace("`r`n","`n")))) { return }
    if (!$raw.Contains($old) -and $raw.Contains($old.Replace("`r`n","`n"))) {
        $old=$old.Replace("`r`n","`n"); $new=$new.Replace("`r`n","`n")
    }
    if (!$raw.Contains($old) -and ($raw.Contains($new) -or $raw.Contains($new.Replace("`r`n","`n")))) { return }
    & $hook -Path $path -ExpectedSha256 (Get-FileHash -LiteralPath $path).Hash -Old $old -New $new -Apply
}
$trade='Sources/GameClient/Ui/UiCase/UiTrade.cpp'
$short='Sources/GameClient/Ui/ShortcutKey.cpp'
Hook $trade '"UiTrade.ini"' '"\xCD\xE6\xBC\xD2\xBC\xE4\xBD\xBB\xD2\xD7.ini"'
Hook 'Sources/GameClient/Ui/GameSpaceChangedNotify.cpp' "                UiCloseWndsInGame(false);`r`n                KUiTrade::OpenWindow((KUiPlayerItem *) uParam, (BOOL) nParam);" "                KUiTrade::OpenWindow((KUiPlayerItem *) uParam, (BOOL) nParam);`r`n                KUiItem::OpenWindow();"
Hook $short "                    if (KUiItem::GetIfVisible())`r`n                        KUiItem::CloseWindow(false);`r`n                    KUiStatus::OpenWindow();" '                    KUiStatus::OpenWindow();'
Hook $short "                    if (KUiStatus::GetIfVisible())`r`n                        KUiStatus::CloseWindow(false);`r`n                    KUiItem::OpenWindow();" '                    KUiItem::OpenWindow();'
Hook $short "    if (g_UiBase.GetStatus() != UIS_S_IDLE || !ms_Enable)`r`n        return FALSE;" "    if (!ms_Enable) return FALSE;`r`n    if (g_UiBase.GetStatus() != UIS_S_IDLE &&`r`n        !(g_UiBase.GetStatus() == UIS_S_TRADE_PLAYER && ScriptCommand &&`r`n          (!strcmp(ScriptCommand, SCK_SHORTCUT_ITEMS) ||`r`n           !strcmp(ScriptCommand, SCK_SHORTCUT_STATUS))))`r`n        return FALSE;"
Hook $trade 'if (!pOther || !g_pCoreShell || g_UiBase.GetStatus() != UIS_S_IDLE)' 'if (!pOther || !g_pCoreShell || (g_UiBase.GetStatus() != UIS_S_IDLE && !(m_pSelf && g_UiBase.GetStatus() == UIS_S_TRADE_PLAYER)))'
Hook $trade "        LoadScheme(rpsScheme);`r`n        m_pSelf->UpdateTakewithItems();" "        if (!m_pSelf->IsVisible()) LoadScheme(rpsScheme);`r`n        m_pSelf->UpdateTakewithItems();"
Hook $trade "void KUiTrade::OnItemPickDrop(ITEM_PICKDROP_PLACE *pPickPos, ITEM_PICKDROP_PLACE *pDropPos) {`r`n    if (m_bTradeFolkGame) return;" 'void KUiTrade::OnItemPickDrop(ITEM_PICKDROP_PLACE *pPickPos, ITEM_PICKDROP_PLACE *pDropPos) {'
Hook $trade "void KUiTrade::OnModififyMoney() {`r`n    if (m_bTradeFolkGame) return;" 'void KUiTrade::OnModififyMoney() {'
Hook $trade "void KUiTrade::UpdateTradeEnd(int bTraded) {`r`n    CloseWindow();" "void KUiTrade::UpdateTradeEnd(int bTraded) {`r`n    if (m_bTradeFolkGame && bTraded == 2) {`r`n        m_nRpsChoice = -1;`r`n        m_nAdjustingMoney = m_nSelfTradeMoney = 0;`r`n        m_SelfItemsBox.Clear(); m_OtherItemsBox.Clear();`r`n        m_SelfMoney.SetIntText(0); m_OtherMoney.SetIntText(0);`r`n        UpdateTakewithItems(); UpdateOperData();`r`n        return;`r`n    }`r`n    CloseWindow();"
Hook $trade "    m_SelfItemsBox.EnableTracePutPos(true);`r`n    m_TakewithItemsBox.EnableTracePutPos(true);" "    m_SelfItemsBox.EnableTracePutPos(true);`r`n    m_TakewithItemsBox.EnableTracePutPos(true);`r`n    m_TakewithItemsBox.Hide(); m_TakewithMoney.Hide();`r`n    m_AddMoney.Hide(); m_ReduceMoney.Hide();"
