[CmdletBinding()]
param([switch]$Apply)
$ErrorActionPreference='Stop'
$srcRoot=Join-Path (Split-Path -Parent $PSScriptRoot) 'Sources/GameClient/Ui/UiCase'
$hook=Join-Path $PSScriptRoot 'Invoke-LegacyAsciiHook.ps1'
$edits=@(
    @{File='UiItem.h';Old='    void SetInventoryPage(int nPage);';New=@'
    void SetInventoryPage(int nPage);
    void ApplyInventoryDelta(KUiObjAtRegion* item, int add);
    bool HandleEquipRightClick(KWndWindow* source);
    void RefreshF4Availability();
'@},
    @{File='UiItem.cpp';Old="void KUiItem::Breathe() {";New="void KUiItem::Breathe() {`r`n    if (!g_pCoreShell) return;`r`n    RefreshF4Availability();"},
    @{File='UiItem.cpp';Old="void KUiItem::UpdateData() {";New="void KUiItem::UpdateData() {`r`n    if (!g_pCoreShell) return;"},
    @{File='UiItem.cpp';Old="    if (nCount == 0)`r`n        return;";New="    if (nCount <= 0 || nCount > MAX_EQUIPMENT_ITEM)`r`n        return;"},
    @{File='UiItem.cpp';Old="void KUiItem::UpdateItem(KUiObjAtRegion *pItem, int bAdd) {";New="void KUiItem::UpdateItem(KUiObjAtRegion *pItem, int bAdd) {`r`n    if (m_nInventoryPage != 0 || !g_pCoreShell) return;"},
    @{File='UiItem.cpp';Old="    if (m_nInventoryPage == 1)`r`n        UpdateItem(pItem, bAdd);";New="    if (m_nInventoryPage == 1)`r`n        ApplyInventoryDelta(pItem, bAdd);"},
    @{File='UiItem.cpp';Old='        case WND_N_RIGHT_CLICK_ITEM:';New="        case WND_N_RIGHT_CLICK_ITEM:`r`n            if (HandleEquipRightClick((KWndWindow*)nParam)) return 0;"},
    @{File='UiItem.cpp';Old='void KUiItem::OnItemPickDrop(ITEM_PICKDROP_PLACE *pPickPos, ITEM_PICKDROP_PLACE *pDropPos) {';New=@'
#include "UiItemF4Actions.inl"

void KUiItem::OnItemPickDrop(ITEM_PICKDROP_PLACE *pPickPos, ITEM_PICKDROP_PLACE *pDropPos) {
    if (!g_pCoreShell) return;
    if (pPickPos && pPickPos->pWnd != (KWndWindow*)&m_ItemBox) return;
    if (pDropPos && pDropPos->pWnd != (KWndWindow*)&m_ItemBox) return;
'@},
    @{File='UiItem.cpp';Old=@'
        ((KWndObjectMatrix *) (pPickPos->pWnd))->GetObject(
                Obj, pPickPos->h, pPickPos->v);
'@;New=@'
        if (!m_ItemBox.GetObject(Obj, pPickPos->h, pPickPos->v))
            return;
'@}
)
# Preflight every unique ASCII hook before changing any legacy file.
foreach ($edit in $edits) {
    $path=Join-Path $srcRoot $edit.File
    $bytes=[IO.File]::ReadAllBytes($path)
    $search=[Text.Encoding]::GetEncoding(28591).GetString($bytes)
    $edit.Old=$edit.Old.Replace("`r`n","`n")
    $edit.New=$edit.New.Replace("`r`n","`n")
    if ($edit.Old.Contains("`n") -and -not $search.Contains($edit.Old)) {
        $edit.Old=$edit.Old.Replace("`n","`r`n")
        $edit.New=$edit.New.Replace("`n","`r`n")
    }
    $hash=(Get-FileHash -LiteralPath $path -Algorithm SHA256).Hash
    & $hook -Path $path -ExpectedSha256 $hash -Old $edit.Old -New $edit.New | Out-Null
}
if ($Apply) {
    foreach ($edit in $edits) {
        $path=Join-Path $srcRoot $edit.File
        $hash=(Get-FileHash -LiteralPath $path -Algorithm SHA256).Hash
        & $hook -Path $path -ExpectedSha256 $hash -Old $edit.Old -New $edit.New -Apply
    }
}
'F4 ASCII hooks preflight PASS.'
