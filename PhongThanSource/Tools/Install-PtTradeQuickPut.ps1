$ErrorActionPreference='Stop'
$root=Split-Path $PSScriptRoot -Parent
function Hook($relative,$old,$new) {
    $path=Join-Path $root $relative
    $raw=[Text.Encoding]::ASCII.GetString([IO.File]::ReadAllBytes($path))
    if($raw.Contains($new) -or $raw.Contains($new.Replace("`r`n","`n"))) { return }
    if(!$raw.Contains($old)) { $old=$old.Replace("`r`n","`n"); $new=$new.Replace("`r`n","`n") }
    & (Join-Path $PSScriptRoot 'Invoke-LegacyAsciiHook.ps1') -Path $path -ExpectedSha256 (Get-FileHash -LiteralPath $path).Hash -Old $old -New $new -Apply
}
Hook 'Sources/GameClient/Ui/Elem/WndObjContainer.h' '    int GetObjectNum();' "    int GetObjectNum();`r`n    bool FindFreePlace(int width, int height, int& left, int& top) const;"
Hook 'Sources/GameClient/Ui/Elem/WndObjContainer.cpp' 'int KWndObjectMatrix::GetObjectNum() {' "#include `"WndObjMatrixFreePlace.inl`"`r`n`r`nint KWndObjectMatrix::GetObjectNum() {"
Hook 'Sources/GameClient/Ui/UiCase/UiTrade.h' '    void UpdateTradeEnd(int bTraded);' "    bool PutInventoryItem(const KUiDraggedObject* item);`r`n    void UpdateTradeEnd(int bTraded);"
Hook 'Sources/GameClient/Ui/UiCase/UiTrade.cpp' 'void KUiTrade::OnItemPickDrop(ITEM_PICKDROP_PLACE *pPickPos, ITEM_PICKDROP_PLACE *pDropPos) {' "#include `"UiTradeQuickPut.inl`"`r`n`r`nvoid KUiTrade::OnItemPickDrop(ITEM_PICKDROP_PLACE *pPickPos, ITEM_PICKDROP_PLACE *pDropPos) {"
$old='    Obj.eContainer = GetInventoryContainer();'
# This snippet occurs in multiple handlers, so anchor to the next shop guard.
$old+="`r`n`r`n    if (KUiPlayerShop::GetIfVisible())"
$new="    Obj.eContainer = GetInventoryContainer();`r`n`r`n    if (g_UiBase.GetStatus() == UIS_S_TRADE_PLAYER) {`r`n        KUiTrade* trade = KUiTrade::GetIfVisible();`r`n        if (bDoImmed && m_nInventoryPage == 0 && trade) trade->PutInventoryItem(pItem);`r`n        return;`r`n    }`r`n`r`n    if (KUiPlayerShop::GetIfVisible())"
Hook 'Sources/GameClient/Ui/UiCase/UiItem.cpp' $old $new
$old=@'
                    } else {
                        KUiItem *pItemsBar = KUiItem::GetIfVisible();
                        if (pItemsBar)
                            pItemsBar->UpdateItem((KUiObjAtRegion *) uParam, nParam);
                    }
'@ -replace "`r?`n","`r`n"
$new=@'
                    }
                    // F4 and the legacy embedded trade inventory may coexist.
                    // Deliver authoritative add/remove events to both views.
                    KUiItem *pItemsBar = KUiItem::GetIfVisible();
                    if (pItemsBar)
                        pItemsBar->UpdateItem((KUiObjAtRegion *) uParam, nParam);
'@ -replace "`r?`n","`r`n"
Hook 'Sources/GameClient/Ui/GameSpaceChangedNotify.cpp' $old $new
$old='        case GDCNI_NPC_TRADE:'
$new=@'
        case GDCNI_CONTAINER_OBJECT_CHANGED:
            if (uParam == UOC_ITEM_TAKE_WITH) {
                KUiItem* inventory = KUiItem::GetIfVisible();
                if (inventory) inventory->UpdateItem(NULL, 0);
                KUiTrade* trade = KUiTrade::GetIfVisible();
                if (trade) trade->OnChangedTakewithItem(NULL, 0);
            }
            break;
        case GDCNI_NPC_TRADE:
'@ -replace "`r?`n","`r`n"
Hook 'Sources/GameClient/Ui/GameSpaceChangedNotify.cpp' $old $new
