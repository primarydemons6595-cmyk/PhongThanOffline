$ErrorActionPreference='Stop'
$root=Join-Path $PSScriptRoot '..\Sources\GameClient\Ui\UiCase'
function Hook($file,$old,$new) {
 $p=Join-Path $root $file
 $t=[Text.Encoding]::ASCII.GetString([IO.File]::ReadAllBytes($p))
 if($t.Contains($new) -or $t.Contains($new.Replace("`n","`r`n"))){return}
 if(!$t.Contains($old)){$old=$old.Replace("`n","`r`n");$new=$new.Replace("`n","`r`n")}
 & (Join-Path $PSScriptRoot 'Invoke-LegacyAsciiHook.ps1') -Path $p -ExpectedSha256 (Get-FileHash $p).Hash -Old $old -New $new -Apply
}
Hook 'UiGame.h' '    ACTION_TRADE_FOLKGAME = 12,' "    ACTION_TRADE_FOLKGAME = 12,`n    ACTION_CONTEXT_FACE = 13,"
Hook 'UiGame.cpp' '    int nActionDataCount = ACTION_TRADE_FOLKGAME + 1;' '    int nActionDataCount = ACTION_CONTEXT_FACE + 1;'
Hook 'UiGame.cpp' '            (i == ACTION_CHAT) ||' @'
            (i == ACTION_CHAT) ||
            (i == ACTION_CONTEXT_FACE) ||
'@
Hook 'UiGame.cpp' '        case ACTION_CHAT:' @'
        case ACTION_CONTEXT_FACE:
            KUiPlayerBar::OpenContextFaceSelector(pDest->Name);
            break;
        case ACTION_CHAT:
'@
Hook 'UiPlayerBar.h' '    static int FindRecentPlayer(const char *szName);' @'
    static void OpenContextFaceSelector(const char* recipient);
    static int FindRecentPlayer(const char *szName);
'@
Hook 'UiPlayerBar.cpp' 'extern KUiChatPhrase g_UiChatPhrase;' @'
extern KUiChatPhrase g_UiChatPhrase;
#include "UiContextFace.inl"
'@
Hook 'UiSysMsgCentre.h' '    void Clear();' @'
    void Clear();
    void SyncMessageButtons();
    int m_nIconAnchorX, m_nIconAnchorY;
'@
Hook 'UiSysMsgCentre.cpp' '    m_nPopupMsgDestX = 0;' @'
    m_nPopupMsgDestX = 0;
    m_nIconAnchorX = m_nIconAnchorY = 0;
'@
Hook 'UiSysMsgCentre.cpp' '    g_UiBase.GetCurSchemePath(Scheme, 256);' '    g_UiBase.GetCurSchemePath(Scheme, sizeof(Scheme));'
Hook 'UiSysMsgCentre.cpp' 'void KUiSysMsgCentre::LoadScheme(KIniFile *pIni) {' @'
#include "UiSysMsgInteraction.inl"
void KUiSysMsgCentre::LoadScheme(KIniFile *pIni) {
'@
Hook 'UiSysMsgCentre.cpp' '        m_MsgIconBtn[i].Init(pIni, szBuf);' @'
        if (!pIni->IsSectionExist(szBuf)) strcpy(szBuf, "MsgIcon_1");
        m_MsgIconBtn[i].Init(pIni, szBuf);
'@
Hook 'UiSysMsgCentre.cpp' '    m_SysMsgParam.cChatPrefixLen = 4;' @'
    m_SysMsgParam.cChatPrefixLen = 4;
    m_MsgIconBtn[0].GetPosition(&m_nIconAnchorX, &m_nIconAnchorY);
    SyncMessageButtons();
'@
Hook 'UiSysMsgCentre.cpp' '    KWndWindow::Init(pIni, "Main");' @'
    KWndWindow::Init(pIni, "Main");
    // The original SPR canvas is wider than the INI icon anchor's remaining
    // parent area. Hit-test the live children, including their full extents.
    m_Style |= WND_S_SIZE_WITH_ALL_CHILD;
'@
Hook 'UiSysMsgCentre.cpp' '                    DeleteMsgInHeap(i, 0, (uMsg == WND_N_BUTTON_MR_DOWN), true);' @'
                    g_DebugLog("UI_INVITE_ICON_CLICK heap=%d count=%d", i, m_MsgHeap[i].nNumValid);
                    DeleteMsgInHeap(i, 0, (uMsg == WND_N_BUTTON_MR_DOWN), true);
'@
Hook 'UiSysMsgCentre.cpp' '    switch (uParam) {' @'
    g_DebugLog("UI_INVITE_RESPONSE type=%u selection=%d pending=%p", uParam, nSelAction, m_pHandlingMsg);
    switch (uParam) {
'@
Hook 'UiSysMsgCentre.cpp' @'
    if (szBuf[0])
        UIMessageBox1(szBuf, strlen(szBuf), this, pFirstBtnText, pSecBtnText, byConfirmType, m_MsgIcon[eType].szImage);
'@ @'
    if (szBuf[0]) {
        g_DebugLog("UI_INVITE_DIALOG type=%u pending=%p", byConfirmType, m_pHandlingMsg);
        UIMessageBox(szBuf, this, pFirstBtnText, pSecBtnText, byConfirmType, -1);
    }
'@
Hook 'UiSysMsgCentre.cpp' 'void KUiSysMsgCentre::DeleteMsgInHeap(int nHeapIndex, int nMsgIndex, bool bImmedDel, bool bTobeConfirm) {' @'
void KUiSysMsgCentre::DeleteMsgInHeap(int nHeapIndex, int nMsgIndex, bool bImmedDel, bool bTobeConfirm) {
    if (nHeapIndex < 0 || nHeapIndex > POPUPING_MSG_HEAP_INDEX) return;
    if (bTobeConfirm && (m_pHandlingMsg || !g_UiInformation.PreparePtPkConfirmation()))
        return; // Keep the invitation queued until its dialog can be opened safely.
'@
Hook 'UiSysMsgCentre.cpp' @'
            *pHeap = temp;
            SetPopupMsgDest();
'@ @'
            *pHeap = temp;
            SyncMessageButtons();
            SetPopupMsgDest();
'@
Hook 'UiSysMsgCentre.cpp' @'
    pHeap->pMsgList[nIndex] = pMsg;
    pHeap->nNumValid++;
    return true;
'@ @'
    pHeap->pMsgList[nIndex] = pMsg;
    pHeap->nNumValid++;
    SyncMessageButtons();
    return true;
'@
