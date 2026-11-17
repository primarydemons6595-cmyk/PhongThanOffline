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
Hook 'Sources/Core/Src/KPlayer.h' "TradeDecision(BYTE* pProtocol);" "TradeDecision(BYTE* pProtocol);`r`n    bool FinishPtRpsRound(int result);"
Hook 'Sources/Core/Src/KPlayer.cpp' "void`tKPlayer::TradeDecision(BYTE* pProtocol)" "#include `"PhongThanRpsRound.inl`"`r`nvoid`tKPlayer::TradeDecision(BYTE* pProtocol)"
Hook 'Sources/Core/Src/KPlayer.cpp' "    TRADE_DECISION_SYNC`t*pSync = (TRADE_DECISION_SYNC*)pMsg;" "    TRADE_DECISION_SYNC`t*pSync = (TRADE_DECISION_SYNC*)pMsg;`r`n    #include `"PhongThanRpsRoundClient.inl`""
Hook 'Sources/Core/Src/KPlayer.cpp' 'SO_ItemActionDiag("Move_SERVER_BEGIN", DownPos.nPlace' "if (CheckTrading(true) &&`r`n        ((DownPos.nPlace != pos_hand && DownPos.nPlace != pos_equiproom && DownPos.nPlace != pos_traderoom) ||`r`n         (UpPos.nPlace != pos_hand && UpPos.nPlace != pos_equiproom && UpPos.nPlace != pos_traderoom))) return;`r`n    SO_ItemActionDiag(`"Move_SERVER_BEGIN`", DownPos.nPlace"
Hook 'Sources/GameClient/Ui/UiCase/UiTrade.h' 'KWndButton m_RpsChoice[3];' "KWndButton m_RpsChoice[3];`r`n    KWndButton m_PtClose;"
Hook 'Sources/GameClient/Ui/UiCase/UiTrade.cpp' 'AddChild(&m_Cancel);' "AddChild(&m_Cancel);`r`n    AddChild(&m_PtClose);"
Hook 'Sources/GameClient/Ui/UiCase/UiTrade.cpp' 'm_Cancel.Init(pIni, "CancelBtn");' "m_Cancel.Init(pIni, `"CancelBtn`");`r`n    m_PtClose.Init(pIni, `"BtnClose`");"
Hook 'Sources/GameClient/Ui/UiCase/UiTrade.cpp' 'if (uParam == (unsigned int) (KWndWindow *) &m_Cancel)' 'if (uParam == (unsigned int) (KWndWindow *) &m_Cancel || uParam == (unsigned int) (KWndWindow *) &m_PtClose)'
