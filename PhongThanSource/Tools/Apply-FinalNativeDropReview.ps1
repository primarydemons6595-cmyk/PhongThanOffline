param([switch]$Rollback)
$ErrorActionPreference='Stop'
$workspace=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
$server=Join-Path $workspace 'PhongThanRuntime-Loose/Server'
$out=Join-Path $workspace 'analysis/native_drop_final_20261018/gameplay_review'
$backup=Join-Path $out 'backup'
if(!$Rollback -and (Test-Path (Join-Path $out 'deployment.json'))){throw 'Review already deployed'}
if(!$Rollback){
 if((Get-Content -Raw (Join-Path $out 'cloak_policy_tests.log')) -notmatch 'checks=64 failures=0'){throw 'Cloak regression required'}
 if((Get-Content -Raw (Join-Path $out 'real_cold_restart_read.log')) -notmatch 'failures=0'){throw 'Real gameplay cold-copy regression required'}
 $expected=(Get-FileHash (Join-Path $backup 'CoreServer.dll')).Hash
}else{$previous=Get-Content -Raw (Join-Path $out 'deployment.json')|ConvertFrom-Json;$expected=$previous.AfterSha256}
if((Get-FileHash (Join-Path $server 'CoreServer.dll')).Hash -ne $expected){throw 'Main binary changed; refuse swap'}
$world=@(Get-Process GameServer -ErrorAction SilentlyContinue|Where-Object Path -IEQ (Join-Path $server 'GameServer.exe'))
if($world.Count -gt 1){throw 'Multiple Main worlds'}
if($world.Count){
 $proc=$world[0];[void]$proc.Handle;$event=[Threading.EventWaitHandle]::OpenExisting("Local\PhongThan.Stop.$($proc.Id)");try{[void]$event.Set()}finally{$event.Dispose()}
 if(!$proc.WaitForExit(45000) -or $proc.ExitCode -ne 0){throw 'Native save/stop failed; no forced stop'}
}else{
 $stop=Get-Content -Raw (Join-Path $out 'clean_stop.json')|ConvertFrom-Json
 if($stop.ExitCode -ne 0){throw 'Verified clean stop required'}
}
# Never restore CharacterStore, companion snapshots or reservation counters.
$source=if($Rollback){Join-Path $backup 'CoreServer.dll'}else{Join-Path $workspace 'PhongThanSource/Output/Server/CoreServer.dll'}
$target=Join-Path $server 'CoreServer.dll';Copy-Item -LiteralPath $source -Destination ($target+'.review.tmp');[IO.File]::Replace(($target+'.review.tmp'),$target,($target+'.before-review'))
$bishop=@(Get-Process Bishop -ErrorAction SilentlyContinue|Where-Object Path -IEQ (Join-Path $server 'Bishop.exe'))
if($bishop.Count -ne 1){throw 'Exactly one Main Bishop required'}
[void]$bishop[0].Handle;Stop-Process -Id $bishop[0].Id;if(!$bishop[0].WaitForExit(10000)){throw 'Bishop still running'}
$stamp=Get-Date -Format 'yyyyMMdd_HHmmss'
$old0=$env:PHONGTHAN_P0_MAGIC_ROOT;$old1=$env:PHONGTHAN_P1_BLUE_ROOT;$old2=$env:PHONGTHAN_P2_BLUE_ROOT
try{
 $env:PHONGTHAN_P0_MAGIC_ROOT=$null;$env:PHONGTHAN_P1_BLUE_ROOT=$null;$env:PHONGTHAN_P2_BLUE_ROOT=$null
 $nextBishop=Start-Process -FilePath (Join-Path $server 'Bishop.exe') -WorkingDirectory $server -WindowStyle Hidden -PassThru -RedirectStandardOutput (Join-Path $out "$stamp.Bishop.stdout.log") -RedirectStandardError (Join-Path $out "$stamp.Bishop.stderr.log")
 Start-Sleep -Seconds 2
 $next=Start-Process -FilePath (Join-Path $server 'GameServer.exe') -WorkingDirectory $server -WindowStyle Hidden -PassThru -RedirectStandardOutput (Join-Path $out "$stamp.GameServer.stdout.log") -RedirectStandardError (Join-Path $out "$stamp.GameServer.stderr.log")
}finally{$env:PHONGTHAN_P0_MAGIC_ROOT=$old0;$env:PHONGTHAN_P1_BLUE_ROOT=$old1;$env:PHONGTHAN_P2_BLUE_ROOT=$old2}
$receipt=@{Rollback=[bool]$Rollback;GameServerPid=$next.Id;BishopPid=$nextBishop.Id;BeforeSha256=$expected;AfterSha256=(Get-FileHash $target).Hash;Utc=[DateTime]::UtcNow.ToString('o');Status='WAIT_WORLD_READY';CompanionVersion=1;CounterReset=$false;CharacterStoreRestored=$false;CompanionRestored=$false;Changes='Detail9 original phi phong -> original cloak cap row only'}
$file=if($Rollback){'rollback.json'}else{'deployment.json'}
[IO.File]::WriteAllText((Join-Path $out $file),($receipt|ConvertTo-Json),[Text.UTF8Encoding]::new($false));$receipt|ConvertTo-Json
