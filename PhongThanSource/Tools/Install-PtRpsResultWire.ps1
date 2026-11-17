$ErrorActionPreference='Stop'
$p=Join-Path (Split-Path $PSScriptRoot -Parent) 'Sources/Core/Src/PhongThanRpsRound.inl'
$hook=Join-Path $PSScriptRoot 'Invoke-LegacyAsciiHook.ps1'
$old='    marker.m_btDecision = 3;'
$new='    marker.m_btDecision = (BYTE)(6 + result); // 5=tie, 6=self win, 7=self loss; keep result in the existing sync byte'
$raw=[Text.Encoding]::ASCII.GetString([IO.File]::ReadAllBytes($p))
if(!$raw.Contains($new)) { & $hook -Path $p -ExpectedSha256 (Get-FileHash $p).Hash -Old $old -New $new -Apply }
