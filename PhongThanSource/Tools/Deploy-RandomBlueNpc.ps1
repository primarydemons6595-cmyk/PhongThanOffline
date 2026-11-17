param([switch]$Rollback,[switch]$Update)
$ErrorActionPreference='Stop'
$workspace=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
$server=Join-Path $workspace 'PhongThanRuntime-Loose/Server'
$out=Join-Path $workspace 'analysis/random_blue_npc_20261018'
$settings=Join-Path $server 'settings/phongthan'
$receiptPath=Join-Path $out 'deployment.json'
if(!(Test-Path (Join-Path $server 'native_drop_official_v1.required'))){throw 'Official Native Drop required'}
if(!$Rollback){
 if((Test-Path $receiptPath) -and !$Update){throw 'Random blue already deployed'}
 if((Get-Content -Raw (Join-Path $out 'tests.log')) -notmatch 'checks=56 failures=0'){throw 'Random blue tests required'}
 if((Get-Content -Raw (Join-Path $out 'native_preflight.log')) -notmatch 'checks=(19|22) failures=0'){throw 'Selected-profile preflight required'}
 $expected=(Get-FileHash (Join-Path $out 'backup/CoreServer.dll')).Hash
 $scope=Get-Content -Raw (Join-Path $out 'scope.json')|ConvertFrom-Json
 $qexpected=$scope.before_sha256
 if($Update){$previous=Get-Content -Raw $receiptPath|ConvertFrom-Json;$expected=$previous.CoreServerSha256;$qexpected=$previous.QuarantineSha256}
 if((Get-FileHash (Join-Path $settings 'NativeDropQuarantine.txt')).Hash -ine $qexpected){throw 'Quarantine changed after preflight'}
}else{
 $previous=Get-Content -Raw $receiptPath|ConvertFrom-Json;$expected=$previous.CoreServerSha256
 if((Get-FileHash (Join-Path $settings 'NativeDropQuarantine.txt')).Hash -ine $previous.QuarantineSha256){throw 'Quarantine changed after deployment'}
}
if((Get-FileHash (Join-Path $server 'CoreServer.dll')).Hash -ine $expected){throw 'Main binary changed; refuse overwrite'}
$world=@(Get-Process GameServer -ErrorAction SilentlyContinue|Where-Object Path -IEQ (Join-Path $server 'GameServer.exe'))
$bishop=@(Get-Process Bishop -ErrorAction SilentlyContinue|Where-Object Path -IEQ (Join-Path $server 'Bishop.exe'))
if($world.Count -ne 1 -or $bishop.Count -ne 1){throw 'Exactly one Main world/Bishop required'}
$proc=$world[0];[void]$proc.Handle;$event=[Threading.EventWaitHandle]::OpenExisting("Local\PhongThan.Stop.$($proc.Id)")
try{[void]$event.Set()}finally{$event.Dispose()}
if(!$proc.WaitForExit(45000) -or $proc.ExitCode -ne 0){throw 'Native save/stop not confirmed; no forced stop'}
$saved=Join-Path $out ('saved_'+(Get-Date -Format 'yyyyMMdd_HHmmss'));New-Item -ItemType Directory -Path $saved|Out-Null
Copy-Item -LiteralPath (Join-Path $server 'CharacterStore') -Destination (Join-Path $saved 'CharacterStore') -Recurse
Copy-Item -LiteralPath (Join-Path $server 'NativeDropPersistenceV1') -Destination (Join-Path $saved 'NativeDropPersistenceV1') -Recurse
[IO.File]::WriteAllText((Join-Path $saved 'stop.json'),(@{Pid=$proc.Id;ExitCode=$proc.ExitCode}|ConvertTo-Json))
$core=if($Rollback){Join-Path $out 'backup/CoreServer.dll'}else{Join-Path $workspace 'PhongThanSource/Output/Server/CoreServer.dll'}
Copy-Item -LiteralPath $core -Destination (Join-Path $server 'CoreServer.dll.random.tmp')
[IO.File]::Replace((Join-Path $server 'CoreServer.dll.random.tmp'),(Join-Path $server 'CoreServer.dll'),(Join-Path $saved 'CoreServer.before.dll'))
$quarantine=if($Rollback){Join-Path $out 'backup/NativeDropQuarantine.txt'}else{Join-Path $out 'NativeDropQuarantine.txt'}
Copy-Item -LiteralPath $quarantine -Destination (Join-Path $settings 'NativeDropQuarantine.txt.random.tmp')
[IO.File]::Replace((Join-Path $settings 'NativeDropQuarantine.txt.random.tmp'),(Join-Path $settings 'NativeDropQuarantine.txt'),(Join-Path $saved 'NativeDropQuarantine.before.txt'))
if(!$Rollback -and !$Update){Copy-Item -LiteralPath (Join-Path $out 'RandomBlueNpc.ini') -Destination (Join-Path $settings 'RandomBlueNpc.ini')}
if($Rollback){Move-Item -LiteralPath (Join-Path $settings 'RandomBlueNpc.ini') -Destination (Join-Path $saved 'RandomBlueNpc.retired.ini')}
Stop-Process -Id $bishop[0].Id
$old0=$env:PHONGTHAN_P0_MAGIC_ROOT;$old1=$env:PHONGTHAN_P1_BLUE_ROOT;$old2=$env:PHONGTHAN_P2_BLUE_ROOT
try{
 $env:PHONGTHAN_P0_MAGIC_ROOT=$null;$env:PHONGTHAN_P1_BLUE_ROOT=$null;$env:PHONGTHAN_P2_BLUE_ROOT=$null
 $nextBishop=Start-Process -FilePath (Join-Path $server 'Bishop.exe') -WorkingDirectory $server -WindowStyle Hidden -PassThru -RedirectStandardOutput (Join-Path $saved 'Bishop.stdout.log') -RedirectStandardError (Join-Path $saved 'Bishop.stderr.log')
 Start-Sleep -Seconds 2
 $next=Start-Process -FilePath (Join-Path $server 'GameServer.exe') -WorkingDirectory $server -WindowStyle Hidden -PassThru -RedirectStandardOutput (Join-Path $saved 'GameServer.stdout.log') -RedirectStandardError (Join-Path $saved 'GameServer.stderr.log')
}finally{$env:PHONGTHAN_P0_MAGIC_ROOT=$old0;$env:PHONGTHAN_P1_BLUE_ROOT=$old1;$env:PHONGTHAN_P2_BLUE_ROOT=$old2}
$result=@{Rollback=[bool]$Rollback;GameServerPid=$next.Id;BishopPid=$nextBishop.Id;PreviousPid=$proc.Id;Utc=[DateTime]::UtcNow.ToString('o');CleanExitCode=0;State='WAIT_WORLD_READY';EvidenceDirectory=$saved;CoreServerSha256=(Get-FileHash (Join-Path $server 'CoreServer.dll')).Hash;QuarantineSha256=(Get-FileHash (Join-Path $settings 'NativeDropQuarantine.txt')).Hash;CompanionVersion=1;CharacterDataRestored=$false;CounterReset=$false}
$file=if($Rollback){'rollback.json'}else{'deployment.json'}
if($Update -and !(Test-Path (Join-Path $out 'deployment.initial.json'))){Copy-Item -LiteralPath $receiptPath -Destination (Join-Path $out 'deployment.initial.json')}
[IO.File]::WriteAllText((Join-Path $out $file),($result|ConvertTo-Json),[Text.UTF8Encoding]::new($false));$result|ConvertTo-Json
