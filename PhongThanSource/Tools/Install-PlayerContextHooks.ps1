$ErrorActionPreference = 'Stop'
$root = Join-Path $PSScriptRoot '..\Sources\GameClient\Ui'
function Hook([string]$file,[string]$old,[string]$new) {
    $path = Join-Path $root $file
    $text = [Text.Encoding]::ASCII.GetString([IO.File]::ReadAllBytes($path))
    if ($text.Contains($new) -or $text.Contains($new.Replace("`n","`r`n"))) { return }
    if (-not $text.Contains($old)) {
        $old = $old.Replace("`n","`r`n")
        $new = $new.Replace("`n","`r`n")
    }
    & (Join-Path $PSScriptRoot 'Invoke-LegacyAsciiHook.ps1') -Path $path `
        -ExpectedSha256 (Get-FileHash -LiteralPath $path -Algorithm SHA256).Hash `
        -Old $old -New $new -Apply
}
Hook 'UiCase\UiGame.cpp' '#define PREFIX_SUBWORLD 16' "#define PREFIX_SUBWORLD 16`n#include `"UiPlayerContextCompat.inl`""
Hook 'UiCase\UiGame.cpp' 'int KUiGameSpace::WndProc(unsigned int uMsg, unsigned int uParam, int nParam) {' "int KUiGameSpace::WndProc(unsigned int uMsg, unsigned int uParam, int nParam) {`n    if (PhongThanHandlePlayerContextChord(uMsg, uParam, nParam)) return 0;"
Hook 'UiCase\UiGame.cpp' 'ProcessPeople(&m_SelectPlayer, LOWORD(nParam));' 'PhongThanProcessPlayerMenuAction(&m_SelectPlayer, LOWORD(nParam));'
Hook 'UiCase\UiGame.cpp' '    if (SelectPlayer.Name[0] == 0)' '    if (!g_pCoreShell || SelectPlayer.Name[0] == 0)'
Hook 'UiCase\UiGame.cpp' @'
    KUiPlayerTeam TeamInfo;
    TeamInfo.nCaptainPower = 0;
'@ @'
    KUiPlayerTeam TeamInfo;
    memset(&TeamInfo, 0, sizeof(TeamInfo));
    TeamInfo.nTeamServerID = -1;
    TeamInfo.nCaptainPower = 0;
'@
Hook 'UiCase\UiGame.cpp' 'strcpy(pMenuData->Items[nMenuCount].szData, g_ActionName[i]);' 'strcpy(pMenuData->Items[nMenuCount].szData, PhongThanPeopleActionName(i));'
Hook 'UiCase\UiGame.cpp' 'pMenuData->Items[nMenuCount].uDataLen = strlen(g_ActionName[i]);' 'pMenuData->Items[nMenuCount].uDataLen = strlen(PhongThanPeopleActionName(i));'
Hook 'ShortcutKey.cpp' 'int Mouse_Menu(Lua_State *L) {' "extern bool PhongThanOpenPlayerContextMenu(int x, int y);`nint Mouse_Menu(Lua_State *L) {"
Hook 'ShortcutKey.cpp' @'
    PopUpContextPeopleMenu(SelectPlayer, KShortcutKeyCentre::ms_MouseX, KShortcutKeyCentre::ms_MouseY);
'@ @'
    PhongThanOpenPlayerContextMenu(KShortcutKeyCentre::ms_MouseX, KShortcutKeyCentre::ms_MouseY);
'@
$path = Join-Path $root 'ShortcutKey.cpp'
$ascii = [Text.Encoding]::ASCII.GetString([IO.File]::ReadAllBytes($path))
$start = $ascii.IndexOf('int Mouse_Menu(Lua_State *L) {')
$end = $ascii.IndexOf('int Mouse_Emote_Menu(Lua_State *L)', $start)
if ($start -lt 0 -or $end -le $start) { throw 'Mouse_Menu boundaries missing' }
Hook 'ShortcutKey.cpp' $ascii.Substring($start, $end-$start) @'
int Mouse_Menu(Lua_State *L) {
    if (KShortcutKeyCentre::ms_bMouse)
        PhongThanOpenPlayerContextMenu(KShortcutKeyCentre::ms_MouseX, KShortcutKeyCentre::ms_MouseY);
    return 0;
}


'@
