$ErrorActionPreference='Stop'
$source=Split-Path $PSScriptRoot -Parent
$hook=Join-Path $PSScriptRoot 'Invoke-LegacyAsciiHook.ps1'
function Hook($relative,$old,$new) {
    $path=Join-Path $source $relative
    $raw=[Text.Encoding]::GetEncoding(28591).GetString([IO.File]::ReadAllBytes($path))
    if ($raw.Contains($new) -or $raw.Contains($new.Replace("`r`n","`n"))) { return }
    if (!$raw.Contains($old) -and $raw.Contains($old.Replace("`r`n","`n"))) {
        $old=$old.Replace("`r`n","`n"); $new=$new.Replace("`r`n","`n")
    }
    & $hook -Path $path -ExpectedSha256 (Get-FileHash -LiteralPath $path).Hash -Old $old -New $new -Apply
}
$ui='Sources/GameClient/Ui/UiCase/UiGame.cpp'
$player='Sources/Core/Src/KPlayer.cpp'
Hook $ui '#include "../../../core/Src/KPlayerMenuState.h"' '#include "../../../core/Src/PhongThanTradeInvite.h"'
Hook $player '#include "PhongThanRpsRules.h"' "#include `"PhongThanRpsRules.h`"`r`n#include `"PhongThanTradeInvite.h`""
Hook $ui 'g_pCoreShell && pDest->nData == PLAYER_MENU_STATE_TRADEOPEN && pDest->nIndex != -1' 'g_pCoreShell && PhongThanCanReceiveTradeInvite(pDest->nData) && pDest->nIndex > 0'
Hook $ui 'g_pCoreShell && pDest->nData == PLAYER_MENU_STATE_TRADEOPEN && pDest->nIndex > 0' 'g_pCoreShell && PhongThanCanReceiveTradeInvite(pDest->nData) && pDest->nIndex > 0'
Hook $ui 'i == ACTION_TRADE && SelectPlayer.nIndex != -1 && SelectPlayer.nData == PLAYER_MENU_STATE_TRADEOPEN' 'i == ACTION_TRADE && SelectPlayer.nIndex > 0 && PhongThanCanReceiveTradeInvite(SelectPlayer.nData)'
Hook $ui 'SelectPlayer.nData == PLAYER_MENU_STATE_TRADEOPEN && !bTrade' 'PhongThanCanReceiveTradeInvite(SelectPlayer.nData) && !bTrade'
Hook $player "void`tKPlayer::TradeApplyStart(BYTE* pProtocol)`r`n{`r`n`tif (this->CheckTrading())" "void`tKPlayer::TradeApplyStart(BYTE* pProtocol)`r`n{`r`n`tif (!pProtocol || m_bForbidTrade || this->CheckTrading() ||`r`n`t`t!PhongThanCanReceiveTradeInvite(m_cMenuState.m_nState))"
Hook $player "if (nPlayer < 0)`r`n`t`treturn;`r`n`tif (Player[nPlayer].m_cMenuState.m_nState != PLAYER_MENU_STATE_TRADEOPEN)" "if (nPlayer <= 0 || nPlayer >= MAX_PLAYER || nPlayer == m_nPlayerIndex)`r`n`t`treturn;`r`n`tif (Player[nPlayer].m_nIndex <= 0 || Player[nPlayer].CheckTrading() ||`r`n`t`tPlayer[nPlayer].m_bForbidTrade ||`r`n`t`t!PhongThanCanReceiveTradeInvite(Player[nPlayer].m_cMenuState.m_nState))"
Hook $player 'if (this->m_cMenuState.m_nState != PLAYER_MENU_STATE_TRADEOPEN || this->m_cMenuState.m_nState == PLAYER_MENU_STATE_TRADING)' "if (m_bForbidTrade || Player[nPlayerIdx].CheckTrading() ||`r`n`t`t!PhongThanCanReceiveTradeInvite(m_cMenuState.m_nState) ||`r`n`t`t!PhongThanCanReceiveTradeInvite(Player[nPlayerIdx].m_cMenuState.m_nState) ||`r`n`t`tFindAroundPlayer(Npc[Player[nPlayerIdx].m_nIndex].m_dwID) != nPlayerIdx)"
