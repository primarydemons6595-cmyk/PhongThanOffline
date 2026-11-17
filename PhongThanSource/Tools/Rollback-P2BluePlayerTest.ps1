$ErrorActionPreference='Stop'
$workspace=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
$report=Join-Path $workspace 'analysis/blue_magic_p2_20261018'
$iso=Get-Content -Raw (Join-Path $report 'deployment.json')|ConvertFrom-Json
if(@(Get-Process Game -ErrorAction SilentlyContinue|Where-Object {$_.Path -ieq (Join-Path $iso.Client 'Game.exe')}).Count){throw 'Logout and close isolated Game.exe before rollback'}
& (Join-Path $PSScriptRoot 'Set-P2BlueMode.ps1') -Mode ALL_OFF
$exe=Join-Path $iso.Server 'GameServer.exe'
$world=@(Get-Process GameServer -ErrorAction SilentlyContinue|Where-Object {$_.Path -ieq $exe})
foreach($proc in $world){[void]$proc.Handle;$event=[Threading.EventWaitHandle]::OpenExisting("Local\PhongThan.Stop.$($proc.Id)");try{[void]$event.Set()}finally{$event.Dispose()};if(!$proc.WaitForExit(45000) -or $proc.ExitCode -ne 0){throw 'Clean stop failed; DLL restore withheld'}}
$evidence=Join-Path $report ('rollback_'+(Get-Date -Format 'yyyyMMdd_HHmmss'));New-Item -ItemType Directory -Path $evidence|Out-Null
Copy-Item -LiteralPath (Join-Path $iso.Server 'CharacterStore') -Destination (Join-Path $evidence 'CharacterStore.preserved') -Recurse
# Restore only the P1 CoreServer (persistence v1 retained). Never roll character
# saves or companion snapshots back to pre-player-test data.
Copy-Item -LiteralPath (Join-Path $iso.Server 'CoreServer.dll') -Destination (Join-Path $evidence 'CoreServer.p2.dll')
Copy-Item -LiteralPath (Join-Path $iso.Backup 'CoreServer.dll') -Destination (Join-Path $iso.Server 'CoreServer.dll')
[IO.File]::WriteAllText((Join-Path $evidence 'result.txt'),'P2 OFF. GameServer stopped cleanly. P1 Core restored; current test role/companion data preserved. Support services remain running.')
Write-Output $evidence
