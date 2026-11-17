param([switch]$Rollback)
$ErrorActionPreference='Stop'
$workspace=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
$report=Join-Path $workspace 'analysis/native_drop_completion_20261018'
$iso=Get-Content -Raw (Join-Path $workspace 'analysis/blue_magic_p2_20261018/deployment.json')|ConvertFrom-Json
if($iso.Root -notlike '*\PhongThanRuntime-Loose\P0-BlueMagic-Isolated\p1_*' -or !(Test-Path (Join-Path $iso.Root 'p2_player_test.flag'))){throw 'Exact approved isolated scope required'}
if(@(Get-Process Game -ErrorAction SilentlyContinue|Where-Object Path -eq (Join-Path $iso.Client 'Game.exe')).Count){throw 'Close isolated Client after saving character before swap'}
$world=@(Get-Process GameServer -ErrorAction SilentlyContinue|Where-Object Path -eq (Join-Path $iso.Server 'GameServer.exe'));if($world.Count -ne 1){throw 'Exactly one isolated world required'}
$backup=Join-Path $report 'deployment_backup'
if(!$Rollback){if(Test-Path $backup){throw 'Initial deployment backup already exists'};New-Item -ItemType Directory -Path $backup | Out-Null
 Copy-Item -LiteralPath (Join-Path $iso.Server 'CoreServer.dll') -Destination (Join-Path $backup 'CoreServer.dll')
 Copy-Item -LiteralPath (Join-Path $iso.Client 'CoreClient.dll') -Destination (Join-Path $backup 'CoreClient.dll')
 foreach($name in 'LegacyDropExperiment.ini','BlueMagicExperimental.ini','LegacyDropQuarantine.txt'){Copy-Item -LiteralPath (Join-Path $iso.Server ('settings/phongthan/'+$name)) -Destination (Join-Path $backup $name)}
}elseif(!(Test-Path (Join-Path $backup 'CoreServer.dll'))){throw 'Completion baseline backup missing'}
$evidence=Join-Path $report (('swap_'+(Get-Date -Format 'yyyyMMdd_HHmmss'))+$(if($Rollback){'_rollback'}else{''}));New-Item -ItemType Directory -Path $evidence | Out-Null
Copy-Item -LiteralPath (Join-Path $iso.Server 'CharacterStore') -Destination (Join-Path $evidence 'CharacterStore.before') -Recurse
$companions=Join-Path $evidence 'CompanionV1.before';New-Item -ItemType Directory -Path $companions | Out-Null
Get-ChildItem -LiteralPath $iso.Root -File | Where-Object {$_.Name -like '*.magic-v1' -or $_.Name -like '*.required' -or $_.Name -like '*.managed-v1'} | ForEach-Object {Copy-Item -LiteralPath $_.FullName -Destination $companions}
$proc=$world[0];[void]$proc.Handle;$event=[Threading.EventWaitHandle]::OpenExisting("Local\PhongThan.Stop.$($proc.Id)");try{[void]$event.Set()}finally{$event.Dispose()}
if(!$proc.WaitForExit(45000) -or $proc.ExitCode -ne 0){throw 'Clean native stop failed; no force kill'}
$serverCore=if($Rollback){Join-Path $backup 'CoreServer.dll'}else{Join-Path $workspace 'PhongThanSource/Output/Server/CoreServer.dll'}
$clientCore=if($Rollback){Join-Path $backup 'CoreClient.dll'}else{Join-Path $workspace 'PhongThanSource/Output/Client/CoreClient.dll'}
Copy-Item -LiteralPath $serverCore -Destination (Join-Path $iso.Server 'CoreServer.dll')
Copy-Item -LiteralPath $clientCore -Destination (Join-Path $iso.Client 'CoreClient.dll')
foreach($name in 'LegacyDropExperiment','BlueMagicExperimental'){$from=if($Rollback){Join-Path $backup ($name+'.ini')}else{Join-Path $report ($name+'.proposed.ini')};Copy-Item -LiteralPath $from -Destination (Join-Path $iso.Server ('settings/phongthan/'+$name+'.ini'))}
$old0=$env:PHONGTHAN_P0_MAGIC_ROOT;$old1=$env:PHONGTHAN_P1_BLUE_ROOT;$old2=$env:PHONGTHAN_P2_BLUE_ROOT
try{$env:PHONGTHAN_P0_MAGIC_ROOT=$iso.Root;$env:PHONGTHAN_P1_BLUE_ROOT=$iso.Root;$env:PHONGTHAN_P2_BLUE_ROOT=$iso.Root;$next=Start-Process -FilePath (Join-Path $iso.Server 'GameServer.exe') -WorkingDirectory $iso.Server -WindowStyle Hidden -PassThru -RedirectStandardOutput (Join-Path $evidence 'GameServer.stdout.log') -RedirectStandardError (Join-Path $evidence 'GameServer.stderr.log')}
finally{$env:PHONGTHAN_P0_MAGIC_ROOT=$old0;$env:PHONGTHAN_P1_BLUE_ROOT=$old1;$env:PHONGTHAN_P2_BLUE_ROOT=$old2}
$receipt=@{Status='STARTED_WAIT_WORLD_READY';Rollback=[bool]$Rollback;PreviousPid=$proc.Id;GameServerPid=$next.Id;StartedUtc=$next.StartTime.ToUniversalTime().ToString('o');CleanExitCode=$proc.ExitCode;Root=$iso.Root;Server=$iso.Server;Client=$iso.Client;Backup=$backup;Evidence=$evidence;CoreServerSha256=(Get-FileHash (Join-Path $iso.Server 'CoreServer.dll')).Hash;CoreClientSha256=(Get-FileHash (Join-Path $iso.Client 'CoreClient.dll')).Hash;QuarantineSha256=(Get-FileHash (Join-Path $iso.Server 'settings/phongthan/LegacyDropQuarantine.txt')).Hash;CharacterStoreRestored=$false;CompanionsRestored=$false}
$name=if($Rollback){'rollback.json'}else{'deployment.json'}
[IO.File]::WriteAllText((Join-Path $report $name),($receipt|ConvertTo-Json -Depth 5),[Text.UTF8Encoding]::new($false))
$receipt|ConvertTo-Json -Depth 5
