[CmdletBinding()]
param([switch]$Apply)
$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
$hook=Join-Path $PSScriptRoot 'Invoke-LegacyAsciiHook.ps1'
$item='Sources/GameClient/Ui/UiCase/UiItem.cpp'
$shell='Sources/Core/Src/CoreShell.cpp'
$shortcuts='Sources/GameClient/Ui/ShortcutKey.cpp'
$edits=New-Object 'System.Collections.Generic.List[object]'
$edits.Add(@{File='Sources/GameClient/Ui/UiCase/UiItem.h';Old='    KWndButton m_CloseBtn;';New="    KWndButton m_CloseBtn;`n    KWndButton m_DiscardLockBtn;"})
$edits.Add(@{File=$item;Old='#include "UiItemF4Compat.inl"';New="#include `"UiItemF4Compat.inl`"`n#include `"UiItemDiscard.inl`""})
$edits.Add(@{File=$item;Old='    AddChild(&m_CloseBtn);';New="    AddChild(&m_CloseBtn);`n    AddChild(&m_DiscardLockBtn);"})
$edits.Add(@{File=$item;Old='        m_pSelf->m_CloseBtn.Init(&Ini, "CloseBtn");';New=@'
        m_pSelf->m_CloseBtn.Init(&Ini, "CloseBtn");
        m_pSelf->m_DiscardLockBtn.Init(&Ini, "BtnDiscardLock");
        m_pSelf->m_DiscardLockBtn.CheckButton(s_PtInventoryDiscard.Enabled());
'@})
$edits.Add(@{File=$item;Old='        case WND_N_BUTTON_CLICK:';New=@'
        case WND_N_BUTTON_CLICK:
            if (uParam == (unsigned int)(KWndWindow*)&m_DiscardLockBtn) {
                s_PtInventoryDiscard.SetEnabled(m_DiscardLockBtn.IsButtonChecked() != 0);
                return 0;
            }
'@})
$edits.Add(@{File=$shell;Old='#include "CoreShell.h"';New="#include `"CoreShell.h`"`n#include `"PhongThanInventoryUi.h`""})
$edits.Add(@{File=$shell;Old='case GDI_ITEM_NAME:';New="#include `"PhongThanInventoryUiQuery.inl`"`n`tcase GDI_ITEM_NAME:"})
$edits.Add(@{File=$shortcuts;Old='#include "UiCase/UiItem.h"';New="#include `"UiCase/UiItem.h`"`nextern int PhongThanRequestDiscardHeldItem();"})
$raw=[Text.Encoding]::GetEncoding(28591).GetString([IO.File]::ReadAllBytes((Join-Path $root $shortcuts)))
foreach ($name in @('Mouse_Action','Mouse_Force0','Mouse_Force1','LuaThrowAwayItem')) {
    $start=$raw.IndexOf("int $name(Lua_State *L) {")
    if ($start -lt 0) { throw "Missing function $name" }
    $call='g_pCoreShell->ThrowAwayItem()'
    $callAt=$raw.IndexOf($call,$start)
    $next=$raw.IndexOf("`nint ",$start+1)
    if ($callAt -lt 0 -or ($next -ge 0 -and $callAt -ge $next)) { throw "Missing discard call in $name" }
    $old=$raw.Substring($start,$callAt+$call.Length-$start)
    $new=$old.Substring(0,$old.Length-$call.Length)+'PhongThanRequestDiscardHeldItem()'
    $edits.Add(@{File=$shortcuts;Old=$old;New=$new})
}
foreach ($edit in $edits) {
    $path=Join-Path $root $edit.File
    $search=[Text.Encoding]::GetEncoding(28591).GetString([IO.File]::ReadAllBytes($path))
    $edit.Old=$edit.Old.Replace("`r`n","`n")
    $edit.New=$edit.New.Replace("`r`n","`n")
    if ($edit.Old.Contains("`n") -and -not $search.Contains($edit.Old)) {
        $edit.Old=$edit.Old.Replace("`n","`r`n")
        $edit.New=$edit.New.Replace("`n","`r`n")
    }
    & $hook -Path $path -ExpectedSha256 (Get-FileHash -LiteralPath $path).Hash -Old $edit.Old -New $edit.New | Out-Null
}
if ($Apply) {
    foreach ($edit in $edits) {
        $path=Join-Path $root $edit.File
        & $hook -Path $path -ExpectedSha256 (Get-FileHash -LiteralPath $path).Hash -Old $edit.Old -New $edit.New -Apply
    }
}
'F4 discard hook preflight PASS.'
