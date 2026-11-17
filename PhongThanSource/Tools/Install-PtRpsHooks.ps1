$ErrorActionPreference='Stop'
$base=Join-Path $PSScriptRoot '..\Sources'
function Hook($relative,$old,$new) {
 $path=Join-Path $base $relative
 $text=[Text.Encoding]::ASCII.GetString([IO.File]::ReadAllBytes($path))
 if($text.Contains($new) -or $text.Contains($new.Replace("`n","`r`n"))){return}
 if(-not $text.Contains($old)){$old=$old.Replace("`n","`r`n");$new=$new.Replace("`n","`r`n")}
 & (Join-Path $PSScriptRoot 'Invoke-LegacyAsciiHook.ps1') -Path $path -ExpectedSha256 (Get-FileHash $path).Hash -Old $old -New $new -Apply
}
$ui='GameClient\Ui\UiCase\'
Hook ($ui+'UiSysMsgCentre.cpp') @'
        case SMCT_UI_TRADE_FOLKGAME:
            _ASSERT(pMsg->byParamSize >= sizeof(KUiPlayerItem));
            pPlayer = (KUiPlayerItem *) (&pMsg[1]);
'@ @'
        case SMCT_UI_TRADE_FOLKGAME:
            _ASSERT(pMsg->byParamSize >= sizeof(KUiPlayerItem));
            pPlayer = (KUiPlayerItem *) (&pMsg[1]);
            sprintf(szBuf, "\x25\x73\x20\x6D\xEA\x69\x20\x62\xB9\x6E\x20\x63\x68\xAC\x69\x20\x6F\xBC\x6E\x20\x74\xEF\x20\x74\xD7\x2E", pPlayer->Name);
            if (false) // Keep the unused legacy text bytes intact.
'@
Hook ($ui+'UiTrade.cpp') '    if (g_UiBase.GetStatus() != UIS_S_IDLE)' '    if (!pOther || !g_pCoreShell || g_UiBase.GetStatus() != UIS_S_IDLE)'
Hook ($ui+'UiGame.h') '    ACTION_CHAT = 0,' "    ACTION_TRADE_FOLKGAME = 12,`n    ACTION_CHAT = 0,"
Hook ($ui+'UiGame.cpp') '    int nActionDataCount = sizeof(g_ActionName) / 32;' '    int nActionDataCount = ACTION_TRADE_FOLKGAME + 1;'
Hook ($ui+'UiGame.cpp') '            (i == ACTION_CHAT) ||' @'
            (i == ACTION_CHAT) ||
            (i == ACTION_TRADE_FOLKGAME && SelectPlayer.nIndex > 0 &&
             SelectPlayer.nData == PLAYER_MENU_STATE_TRADEOPEN && !bTrade) ||
'@
Hook 'Core\Src\KPlayer.cpp' 'if (pReply->m_bFolkGame)' 'if (false && pReply->m_bFolkGame) // No wager-capacity restriction for free RPS.'
Hook 'Core\Src\KPlayer.cpp' 'sprintf(sMsg.szMessage, MSG_TRADE_FOLKGAME_GET_APPLY, Npc[nNpcIdx].Name);' 'sprintf(sMsg.szMessage, "\x25\x73\x20\x6D\xEA\x69\x20\x62\xB9\x6E\x20\x63\x68\xAC\x69\x20\x6F\xBC\x6E\x20\x74\xEF\x20\x74\xD7\x2E", Npc[nNpcIdx].Name);'
Hook 'Core\Src\KPlayer.cpp' 'sprintf(sMsg.szMessage, MSG_TRADE_FOLKGAME_SEND_APPLY, Npc[nNpcIdx].Name);' 'sprintf(sMsg.szMessage, "\xA7\xB7\x20\x67\xF6\x69\x20\x6C\xEA\x69\x20\x6D\xEA\x69\x20\x6F\xBC\x6E\x20\x74\xEF\x20\x74\xD7\x20\xAE\xD5\x6E\x20\x25\x73\x2E", Npc[nNpcIdx].Name);'
Hook 'Core\Src\KPlayer.cpp' 'm_cTrade.StartTrade(pTrade->m_dwNpcID);' 'm_cTrade.StartTrade(pTrade->m_dwNpcID, pTrade->m_bFolkGame);'
Hook ($ui+'UiGame.cpp') '        case ACTION_CHAT:' @'
        case ACTION_TRADE_FOLKGAME:
            if (g_pCoreShell && pDest->nData == PLAYER_MENU_STATE_TRADEOPEN && pDest->nIndex > 0)
                g_pCoreShell->TradeApplyStart(pDest, true);
            break;
        case ACTION_CHAT:
'@
Hook ($ui+'UiTrade.h') '    void Clear();' @'
    KWndButton m_RpsChoice[3];
    int m_nRpsChoice;
    bool LoadRpsScheme(KIniFile*);
    bool UpdateRpsState();
    bool HandleRpsButton(unsigned int);
    void Clear();
'@
Hook ($ui+'UiTrade.cpp') '        if (m_pSelf)' @'
        if (m_pSelf) m_pSelf->m_bTradeFolkGame = bFolkGame;
        if (m_pSelf)
'@
Hook ($ui+'UiTrade.cpp') '        m_pSelf->m_bTradeFolkGame = bFolkGame;' @'
        m_pSelf->m_bTradeFolkGame = bFolkGame;
        m_pSelf->m_nRpsChoice = -1;
        char rpsScheme[256];
        g_UiBase.GetCurSchemePath(rpsScheme, sizeof(rpsScheme));
        LoadScheme(rpsScheme);
'@
Hook ($ui+'UiTrade.cpp') '    m_bTradeFolkGame = FALSE;' "    m_bTradeFolkGame = FALSE;`n    m_nRpsChoice = -1;"
Hook ($ui+'UiTrade.cpp') '    AddChild(&m_Info);' "    AddChild(&m_Info);`n    for (int rps = 0; rps < 3; ++rps) AddChild(&m_RpsChoice[rps]);"
Hook ($ui+'UiTrade.cpp') '        sprintf(Buff, "%s\\"SCHEME_INI_TRADE, pScheme);' @'
        sprintf(Buff, "%s\\%s", pScheme, m_pSelf->m_bTradeFolkGame ?
            "\xCB\xAB\xC8\xCB\xBB\xA5\xB2\xA9.ini" : SCHEME_INI_TRADE);
'@
Hook ($ui+'UiTrade.cpp') 'void KUiTrade::LoadScheme(KIniFile *pIni) {' @'
#include "UiRpsCompat.inl"
void KUiTrade::LoadScheme(KIniFile *pIni) {
    if (LoadRpsScheme(pIni)) return;
    for (int rps = 0; rps < 3; ++rps) m_RpsChoice[rps].Hide();
'@
Hook ($ui+'UiTrade.cpp') 'void KUiTrade::UpdateOperData(BOOL bSelfAsk, BOOL bDestReply) {' @'
void KUiTrade::UpdateOperData(BOOL bSelfAsk, BOOL bDestReply) {
    if (UpdateRpsState()) return;
'@
Hook ($ui+'UiTrade.cpp') '        case WND_N_BUTTON_CLICK:' @'
        case WND_N_BUTTON_CLICK:
            if (HandleRpsButton(uParam)) return 0;
'@
Hook ($ui+'UiTrade.cpp') 'void KUiTrade::OnTrade() {' @'
void KUiTrade::OnTrade() {
    if (m_bTradeFolkGame) {
        if (m_nRpsChoice >= 0 && m_nRpsChoice <= 2)
            g_pCoreShell->OperationRequest(GOI_TRADE_FOLKGAMEID, m_nRpsChoice, 0);
        return;
    }
'@
Hook ($ui+'UiTrade.cpp') 'void KUiTrade::OnModififyMoney() {' "void KUiTrade::OnModififyMoney() {`n    if (m_bTradeFolkGame) return;"
Hook ($ui+'UiTrade.cpp') 'void KUiTrade::OnItemPickDrop(ITEM_PICKDROP_PLACE *pPickPos, ITEM_PICKDROP_PLACE *pDropPos) {' @'
void KUiTrade::OnItemPickDrop(ITEM_PICKDROP_PLACE *pPickPos, ITEM_PICKDROP_PLACE *pDropPos) {
    if (m_bTradeFolkGame) return;
'@
Hook 'Core\Src\CoreShell.cpp' "`t`tif (Player[CLIENT_PLAYER_INDEX].m_cTrade.m_nTradeState)" @'
        if (Player[CLIENT_PLAYER_INDEX].m_cTrade.m_nTradeState ||
            Player[CLIENT_PLAYER_INDEX].m_cTrade.m_bTradeFolkGame)
'@
$path=Join-Path $base 'Core\Src\KPlayer.cpp'
$ascii=[Text.Encoding]::ASCII.GetString([IO.File]::ReadAllBytes($path))
$include='#include "PhongThanPlayerProtocol.h"'
Hook 'Core\Src\KPlayer.cpp' $include ($include+"`n#include `"PhongThanRpsRules.h`"")
Hook 'Core\Src\KPlayer.cpp' "TRADE_DECISION_COMMAND`t*pDecision = (TRADE_DECISION_COMMAND*)pProtocol;" @'
TRADE_DECISION_COMMAND *pDecision = (TRADE_DECISION_COMMAND*)pProtocol;
    #include "PhongThanRpsDecision.inl"
'@
