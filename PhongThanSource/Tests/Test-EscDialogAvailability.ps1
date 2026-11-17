[CmdletBinding()]
param([string]$ProjectRoot = (Split-Path -Parent $PSScriptRoot))

$ErrorActionPreference = 'Stop'
$ProjectRoot = [IO.Path]::GetFullPath($ProjectRoot).TrimEnd('\')
$latin1 = [Text.Encoding]::GetEncoding(28591)
$shortcut = [IO.File]::ReadAllText((Join-Path $ProjectRoot 'Sources\GameClient\Ui\ShortcutKey.cpp'), $latin1)
$dialog = [IO.File]::ReadAllText((Join-Path $ProjectRoot 'Sources\GameClient\Ui\UiCase\UiESCDlg.cpp'), $latin1)
$header = [IO.File]::ReadAllText((Join-Path $ProjectRoot 'Sources\GameClient\Ui\UiCase\UiESCDlg.h'), $latin1)
$schemePath = Join-Path $ProjectRoot 'Deploy\ProjectContent\Ui\ui3\UiESCDlg.ini'
if (-not (Test-Path -LiteralPath $schemePath -PathType Leaf)) { throw "Missing ESC dialog scheme: $schemePath" }
$scheme = [IO.File]::ReadAllText($schemePath, [Text.Encoding]::ASCII)

if ($shortcut -notmatch 'case 5:[\s\S]{0,180}KUiESCDlg::GetIfVisible\(\)[\s\S]{0,160}KUiESCDlg::CloseWindow\(false\)[\s\S]{0,180}UiCloseWndsInGame\(false\)[\s\S]{0,140}KUiESCDlg::OpenWindow\(\)') {
    throw 'System shortcut still hides panels without opening or toggling the ESC dialog.'
}
if ($shortcut -notmatch '\{"Esc"\s*,\s*MASK_VKEY\s*,\s*VK_ESCAPE\}') {
    throw 'The title-case Esc spelling used by the shipped PT autoexec is not registered as a physical Escape key.'
}
foreach ($section in 'Main','BackGround','ContiumeGame','Options','OffLine','GameHelp','ExitGame') {
    if ($scheme -notmatch ('(?m)^\[' + [regex]::Escape($section) + '\]\r?$')) { throw "ESC scheme is missing section: $section" }
}
foreach ($button in 'ContiumeGame','Options','OffLine','GameHelp','ExitGame') {
    if ($scheme -notmatch ('(?s)\[' + [regex]::Escape($button) + '\][^\[]*Label=')) { throw "ESC button has no visible label: $button" }
}
if ($header -notmatch 'KWndLabeledButton m_ContinueGameBtn' -or
    $header -notmatch 'KWndImage m_ESCBg' -or
    $dialog -notmatch 'sprintf\(Buff, "%s\\\\%s", pScheme, SCHEME_INI_INIT\)') {
    throw 'ESC dialog controls are not backed by a visible labeled-button scheme.'
}

[pscustomobject]@{Result='PASS';Scheme=$schemePath;Buttons=5;Shortcut='TOGGLE_ESC_DIALOG'}
