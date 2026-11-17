$ErrorActionPreference='Stop'
$root=Join-Path $PSScriptRoot '..\Sources\GameClient\Ui\UiCase'
function Hook($file,$old,$new) {
    $path=Join-Path $root $file
    $text=[Text.Encoding]::ASCII.GetString([IO.File]::ReadAllBytes($path))
    if($text.Contains($new) -or $text.Contains($new.Replace("`n","`r`n"))){return}
    if(-not $text.Contains($old)){$old=$old.Replace("`n","`r`n");$new=$new.Replace("`n","`r`n")}
    & (Join-Path $PSScriptRoot 'Invoke-LegacyAsciiHook.ps1') -Path $path `
      -ExpectedSha256 (Get-FileHash -LiteralPath $path).Hash -Old $old -New $new -Apply
}
Hook 'UiParadeItem.h' '    KWndImage m_Avt;' "    KWndImage m_Avt;`n    KWndImage m_Background, m_EquipImage;`n    KWndImage m_EquipSlotImage[_ITEM_COUNT];"
Hook 'UiParadeItem.cpp' '#define    SCHEME_INI        "UiParadeItem.ini"' '#define    SCHEME_INI        "\xBF\xB4\xB6\xD4\xB7\xBD\xCD\xE6\xBC\xD2\xD7\xB0\xB1\xB8.ini"'
Hook 'UiParadeItem.cpp' '} CtrlItemMap[_ITEM_COUNT] =' '} LegacyCtrlItemMap[_ITEM_COUNT] ='
Hook 'UiParadeItem.cpp' 'KUiParadeItem *KUiParadeItem::GetIfVisible() {' "#include `"UiParadePtSlots.inl`"`nKUiParadeItem *KUiParadeItem::GetIfVisible() {"
Hook 'UiParadeItem.cpp' '    AddChild(&m_Name);' "    AddChild(&m_Background);`n    AddChild(&m_EquipImage);`n    AddChild(&m_Name);"
Hook 'UiParadeItem.cpp' @'
    for (int i = 0; i < _ITEM_COUNT; i++) {
            AddChild(&m_EquipExpandImg);
'@ @'
    for (int i = 0; i < PT_PARADE_SLOT_COUNT; i++) {
        AddChild(&m_EquipSlotImage[i]);
'@
# The old function body contains only ASCII; replace exactly that range.
$path=Join-Path $root 'UiParadeItem.cpp'
$text=[Text.Encoding]::ASCII.GetString([IO.File]::ReadAllBytes($path))
$start=$text.IndexOf('void KUiParadeItem::LoadScheme(class KIniFile *pIni) {')
if($start -ge 0){
 $end=$text.IndexOf("`n}",$start)+2
 if($end -le $start){throw 'Loader boundary not found'}
 Hook 'UiParadeItem.cpp' $text.Substring($start,$end-$start) '#include "UiParadePtCompat.inl"'
}
Hook 'UiParadeItem.cpp' '        m_pSelf->UpdateData(pDest);' "        m_pSelf->m_Dest = *pDest;`n        m_pSelf->UpdateData(pDest);"
Hook 'UiParadeItem.cpp' '    if (m_pSelf == NULL) {' "    if (!pDest || !g_pCoreShell) return NULL;`n    if (m_pSelf == NULL) {"
Hook 'UiParadeItem.cpp' '                Hide();' '                CloseWindow(false);'
Hook 'UiParadeItem.cpp' '    for (i = 0; i < nCount; i++) {' '    for (i = 0; i < nCount && i < _ITEM_COUNT; i++) {'
Hook 'UiParadeItem.cpp' '        for (int i = 0; i < _ITEM_COUNT; i++) {' '        for (int i = 0; i < PT_PARADE_SLOT_COUNT; i++) {'
# Replace the legacy avatar-selection block: PT uses the six Ui4 portraits.
$text=[Text.Encoding]::ASCII.GetString([IO.File]::ReadAllBytes($path))
$start=$text.IndexOf('    if (pDest->nTeamID == 1) {')
if($start -ge 0){
 $end=$text.IndexOf("`n}`n",$start)
 if($end -lt 0){throw 'Avatar block boundary not found'}
 Hook 'UiParadeItem.cpp' $text.Substring($start,$end-$start) @'
    int variant = pDest->nTeamID > 0 ? (pDest->nTeamID - 1) % 3 : 0;
    int portrait = variant * 2 +
        (g_pCoreShell->GetGameData(GDI_PLAYER_IS_MALE, 0, pDest->uId) ? 1 : 2);
    char image[128];
    sprintf(image, "\\Spr\\Ui4\\\xD0\xA4\xCF\xF1\\%03d.spr", portrait);
    m_Avt.SetImage(ISI_T_SPR, image, false);
'@
}
