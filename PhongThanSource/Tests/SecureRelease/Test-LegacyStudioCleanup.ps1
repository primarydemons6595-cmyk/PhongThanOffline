$ErrorActionPreference='Stop'
$source=Join-Path (Split-Path -Parent (Split-Path -Parent $PSScriptRoot)) 'Deploy\Release\Remove-LegacyStudio.ps1'
$fixture=Join-Path ([IO.Path]::GetTempPath()) ('PtRemoveStudio_'+[Guid]::NewGuid().ToString('N'))
New-Item -ItemType Directory -Path $fixture | Out-Null
$script=Join-Path $fixture 'Remove-LegacyStudio.ps1';Copy-Item -LiteralPath $source -Destination $script
$deps=Join-Path $fixture 'PhongThan.Event.deps.json';$old=Join-Path $fixture 'PhongThanStudio.Core.dll';$state=Join-Path $fixture 'player-state.keep'
[IO.File]::WriteAllText($old,'old publisher code');[IO.File]::WriteAllText($state,'player state')
[IO.File]::WriteAllText($deps,'{"dependencies":{"PhongThanStudio.Core":"1"}}')
$rejected=$false;try { & $script } catch { $rejected=$true }
if(!$rejected -or !(Test-Path -LiteralPath $old)){throw 'Cleanup must reject a dependent Event host.'}
[IO.File]::WriteAllText($deps,'{"dependencies":{"PhongThan.Event.Runtime":"1"}}')
& $script
if(Test-Path -LiteralPath $old){throw 'Obsolete Studio dependency remained.'}
if([IO.File]::ReadAllText($state) -ne 'player state'){throw 'Unrelated server state changed.'}
& $script
Write-Output 'LEGACY_STUDIO_CLEANUP_PASS checks=3'
