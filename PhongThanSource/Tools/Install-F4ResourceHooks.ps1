$ErrorActionPreference = 'Stop'
$base = Join-Path $PSScriptRoot '..\Sources\GameClient\Ui\UiCase'
function Hook([string]$file, [string]$old, [string]$new) {
    $path = Join-Path $base $file
    $bytes = [IO.File]::ReadAllBytes($path)
    $text = [Text.Encoding]::ASCII.GetString($bytes)
    if ($text.Contains($new) -or $text.Contains($new.Replace("`n", "`r`n"))) { return }
    if (-not $text.Contains($old)) {
        $old = $old.Replace("`n", "`r`n")
        $new = $new.Replace("`n", "`r`n")
    }
    & (Join-Path $PSScriptRoot 'Invoke-LegacyAsciiHook.ps1') -Path $path `
        -ExpectedSha256 (Get-FileHash -LiteralPath $path -Algorithm SHA256).Hash `
        -Old $old -New $new -Apply
}
Hook 'UiItem.h' '#define UIITEM_EQUIP_BOX_COUNT 12' "#include `"UiItemF4Resources.h`"`n#define UIITEM_EQUIP_BOX_COUNT 12"
Hook 'UiItem.h' '    void RefreshF4Availability();' "    void RefreshF4Availability();`n    bool IsInventoryPageAccessible() const;`n    bool HandleF4ResourceButton(unsigned int window);"
Hook 'UiItem.h' '    KWndButton m_DiscardLockBtn;' "    KWndButton m_DiscardLockBtn;`n    KPtF4ResourceControls m_PtF4;"
Hook 'UiItem.cpp' '#include "UiItemF4Actions.inl"' "#include `"UiItemF4Actions.inl`"`n#include `"UiItemF4ResourceActions.inl`""
Hook 'UiItem.cpp' "    m_byMark = 0;`n    m_nInventoryPage = 0;" "    m_PtF4.AddTo(this);`n    m_byMark = 0;`n    m_nInventoryPage = 0;"
Hook 'UiItem.cpp' '        m_pSelf->m_ItemBox.Init(&Ini, "ItemBox");' "        m_pSelf->m_ItemBox.Init(&Ini, `"ItemBox`");`n        m_pSelf->m_PtF4.Init(&Ini);"
Hook 'UiItem.cpp' @'
    if (nPage < 0 || nPage > 1 || !g_pCoreShell)
        return;
    if (nPage == 1 && g_pCoreShell->GetGameData(GDI_EQUIPEX_TIME, NULL, NULL) <= 0)
        return;
'@ @'
    if (nPage < 0 || nPage > 3 || !g_pCoreShell)
        return;
'@
Hook 'UiItem.cpp' '    m_nInventoryPage = nPage;' "    m_nInventoryPage = nPage;`n    m_PtF4.SetLocked(!IsInventoryPageAccessible());`n    m_ItemBox.EnablePickPut(IsInventoryPageAccessible());"
Hook 'UiItem.cpp' @'
    m_ItemBox.Clear();

    m_nMoney =
'@ @'
    m_ItemBox.Clear();
    if (!IsInventoryPageAccessible()) return;

    m_nMoney =
'@
Hook 'UiItem.cpp' '        case WND_N_BUTTON_CLICK:' "        case WND_N_BUTTON_CLICK:`n            if (HandleF4ResourceButton(uParam)) return 0;"
Hook 'UiItem.cpp' 'void KUiItem::OnClickItem(KUiDraggedObject *pItem, bool bDoImmed) {' "void KUiItem::OnClickItem(KUiDraggedObject *pItem, bool bDoImmed) {`n    if (!IsInventoryPageAccessible()) return;"
Hook 'UiItem.cpp' 'void KUiItem::OnItemPickDrop(ITEM_PICKDROP_PLACE *pPickPos, ITEM_PICKDROP_PLACE *pDropPos) {' "void KUiItem::OnItemPickDrop(ITEM_PICKDROP_PLACE *pPickPos, ITEM_PICKDROP_PLACE *pDropPos) {`n    if (!IsInventoryPageAccessible()) return;"
Hook 'UiItem.cpp' "                               ITEM_PICKDROP_PLACE *pDropPos) {" "                               ITEM_PICKDROP_PLACE *pDropPos) {`n    if (!IsInventoryPageAccessible()) return;"
