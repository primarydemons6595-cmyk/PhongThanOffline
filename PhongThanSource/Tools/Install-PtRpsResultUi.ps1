$ErrorActionPreference='Stop'
$root=Split-Path $PSScriptRoot -Parent
$hook=Join-Path $PSScriptRoot 'Invoke-LegacyAsciiHook.ps1'
function Hook($relative,$old,$new) {
 $p=Join-Path $root $relative
 $raw=[Text.Encoding]::ASCII.GetString([IO.File]::ReadAllBytes($p))
 if($raw.Contains($new) -or $raw.Contains($new.Replace("`r`n","`n"))){return}
 if(!$raw.Contains($old)){$old=$old.Replace("`r`n","`n");$new=$new.Replace("`r`n","`n")}
 & $hook -Path $p -ExpectedSha256 (Get-FileHash $p).Hash -Old $old -New $new -Apply
}
Hook 'Sources/GameClient/Ui/UiCase/UiTrade.h' '    BOOL m_bTradeFolkGame;' "    BOOL m_bTradeFolkGame;`r`n    bool m_bShowRpsResult;"
Hook 'Sources/GameClient/Ui/UiCase/UiTrade.cpp' '    m_bTradeFolkGame = FALSE;' "    m_bTradeFolkGame = FALSE;`r`n    m_bShowRpsResult = false;"
$old='void KUiTrade::UpdateTradeEnd(int bTraded) {'
$new=@'
void KUiTrade::UpdateTradeEnd(int bTraded) {
    if (m_bTradeFolkGame && bTraded >= 5 && bTraded <= 7) {
        const int result = bTraded - 6;
        const int self = m_nRpsChoice;
        int other = 3;
        if (self >= 0 && self < 3 && result >= -1 && result <= 1)
            for (int choice = 0; choice < 3; ++choice)
                if (choice != self && (((self + 2) % 3 == choice) ? 1 : -1) == result)
                    other = choice;
        m_RpsImage[0].SetFrame(self >= 0 && self < 3 ? self : 3);
        m_RpsImage[1].SetFrame(other);
        m_RpsImage[0].Show(); m_RpsImage[1].Show();
        m_bShowRpsResult = true;
        m_nRpsChoice = -1;
        m_nAdjustingMoney = m_nSelfTradeMoney = 0;
        m_SelfItemsBox.Clear(); m_OtherItemsBox.Clear();
        m_SelfMoney.SetIntText(0); m_OtherMoney.SetIntText(0);
        UpdateTakewithItems();
        return;
    }
    if (m_bTradeFolkGame && bTraded == 2) {
'@
# Replace the existing opening and preserve its original branch body by only adding a guard.
Hook 'Sources/GameClient/Ui/UiCase/UiTrade.cpp' $old $new
