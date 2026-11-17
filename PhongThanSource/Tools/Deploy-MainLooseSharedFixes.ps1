param([switch]$Rollback)
$ErrorActionPreference='Stop'
$workspace=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
$main=Join-Path $workspace 'PhongThanRuntime-Loose'
$server=Join-Path $main 'Server'
$client=Join-Path $main 'Client'
$out=Join-Path $workspace 'analysis/loose_shared_update_20261018'
$backup=Join-Path $out 'backup'
$plan=Get-Content -Raw (Join-Path $out 'binary_plan.json')|ConvertFrom-Json
if(!$Rollback -and (Test-Path (Join-Path $out 'deployment.json'))){throw 'Already deployed; preserve first receipt'}
foreach($row in $plan){
 $actual=(Get-FileHash -LiteralPath (Join-Path $main $row.path)).Hash
 if($Rollback -and $actual -ine $row.after){throw "Binary changed after deployment: $($row.path)"}
 if(!$Rollback -and $actual -ine $row.before -and $actual -ine $row.after){throw "Unreviewed target binary: $($row.path)"}
}
if($Rollback){
 $data=Get-Content -Raw (Join-Path $out 'data_applied.json')|ConvertFrom-Json
 foreach($row in $data.files){if((Get-FileHash -LiteralPath (Join-Path $server $row.path)).Hash -ine $row.after){throw "Data changed after deployment: $($row.path)"}}
 if(Test-Path (Join-Path $out 'blue_config.json')){
  $blue=Get-Content -Raw (Join-Path $out 'blue_config.json')|ConvertFrom-Json
  if((Get-FileHash -LiteralPath (Join-Path $main $blue.path)).Hash -ine $blue.after_sha256){throw 'Blue config changed after deployment'}
 }
}
$world=@(Get-Process GameServer -ErrorAction SilentlyContinue|Where-Object Path -IEQ (Join-Path $server 'GameServer.exe'))
if($world.Count -gt 1){throw 'Multiple main worlds'}
if(@(Get-Process Game -ErrorAction SilentlyContinue|Where-Object Path -IEQ (Join-Path $client 'Game.exe')).Count){throw 'Save and close main Loose Client before binary update'}
if($world.Count -eq 1){
 $proc=$world[0];[void]$proc.Handle
 $event=[Threading.EventWaitHandle]::OpenExisting("Local\PhongThan.Stop.$($proc.Id)")
 try{[void]$event.Set()}finally{$event.Dispose()}
 if(!$proc.WaitForExit(45000) -or $proc.ExitCode -ne 0){throw 'Native save/stop not confirmed; no forced world stop'}
}elseif(!(Test-Path (Join-Path $out 'main_clean_stop.json'))){throw 'No verified clean main-world stop receipt'}
$bishop=@(Get-Process Bishop -ErrorAction SilentlyContinue|Where-Object Path -IEQ (Join-Path $server 'Bishop.exe'))
if($bishop.Count -gt 1){throw 'Multiple main Bishops'}
$oldBishop=0
if($bishop.Count){
 $oldBishop=$bishop[0].Id
 # World has saved and exited. Bishop contains admission/session state, not the live inventory.
 # Other support services and the isolated stack remain running.
 Stop-Process -Id $oldBishop -ErrorAction Stop
 if(!$bishop[0].WaitForExit(10000)){throw 'Bishop still running; binaries retained'}
}
foreach($row in $plan){
 $target=Join-Path $main $row.path
 $from=Join-Path $(if($Rollback){$backup}else{Join-Path $out 'prepared'}) $row.path
 $expected=if($Rollback){$row.before}else{$row.after}
 if((Get-FileHash -LiteralPath $from).Hash -ine $expected){throw "Prepared/backup hash mismatch: $($row.path)"}
 $temp=$target+'.shared-fixes.tmp'
 Copy-Item -LiteralPath $from -Destination $temp
 [IO.File]::Replace($temp,$target,$target+'.swap_backup')
 if((Get-FileHash -LiteralPath $target).Hash -ine $expected){throw "Target hash mismatch: $($row.path)"}
}
foreach($name in 'LegacyDropExperiment.ini','LegacyDropQuarantine.txt'){
 $relative='Server/settings/phongthan/'+$name
 $from=Join-Path $(if($Rollback){$backup}else{Join-Path $out 'prepared'}) $relative
 $target=Join-Path $main $relative
 $temp=$target+'.shared-fixes.tmp';Copy-Item -LiteralPath $from -Destination $temp
 [IO.File]::Replace($temp,$target,$target+'.swap_backup')
}
if($Rollback){
 $data=Get-Content -Raw (Join-Path $out 'data_applied.json')|ConvertFrom-Json
 foreach($row in $data.files){
  $target=Join-Path $server $row.path
  if((Get-FileHash -LiteralPath $target).Hash -ine $row.after){throw "Data changed since update: $($row.path)"}
  $temp=$target+'.rollback.tmp';Copy-Item -LiteralPath (Join-Path (Join-Path $backup 'Server') $row.path) -Destination $temp
  [IO.File]::Replace($temp,$target,$target+'.swap_backup')
 }
 if($blue -and !$blue.before_exists){Remove-Item -LiteralPath (Join-Path $main $blue.path)}
}
$stamp=Get-Date -Format 'yyyyMMdd_HHmmss'
$old0=$env:PHONGTHAN_P0_MAGIC_ROOT;$old1=$env:PHONGTHAN_P1_BLUE_ROOT;$old2=$env:PHONGTHAN_P2_BLUE_ROOT;$oldAuto=$env:PHONGTHAN_AUTO_LOGIN
try{
 $env:PHONGTHAN_P0_MAGIC_ROOT=$null;$env:PHONGTHAN_P1_BLUE_ROOT=$null;$env:PHONGTHAN_P2_BLUE_ROOT=$null;$env:PHONGTHAN_AUTO_LOGIN=$null
 $nextBishop=Start-Process -FilePath (Join-Path $server 'Bishop.exe') -WorkingDirectory $server -WindowStyle Hidden -PassThru -RedirectStandardOutput (Join-Path $out "$stamp.Bishop.stdout.log") -RedirectStandardError (Join-Path $out "$stamp.Bishop.stderr.log")
 Start-Sleep -Seconds 2
 if($nextBishop.HasExited){throw "Updated Bishop exited: $($nextBishop.ExitCode)"}
 $next=Start-Process -FilePath (Join-Path $server 'GameServer.exe') -WorkingDirectory $server -WindowStyle Hidden -PassThru -RedirectStandardOutput (Join-Path $out "$stamp.GameServer.stdout.log") -RedirectStandardError (Join-Path $out "$stamp.GameServer.stderr.log")
}finally{$env:PHONGTHAN_P0_MAGIC_ROOT=$old0;$env:PHONGTHAN_P1_BLUE_ROOT=$old1;$env:PHONGTHAN_P2_BLUE_ROOT=$old2;$env:PHONGTHAN_AUTO_LOGIN=$oldAuto}
$receipt=@{Utc=[DateTime]::UtcNow.ToString('o');Rollback=[bool]$Rollback;Root=$main;GameServerPid=$next.Id;BishopPid=$nextBishop.Id;PreviousBishopPid=$oldBishop;BishopStopMethod='STOP_AFTER_NATIVE_WORLD_SAVE_EXIT_0';Evidence=$out;Stdout=(Join-Path $out "$stamp.GameServer.stdout.log");PlayerDataRestored=$false;IsolationFlagsCopied=$false;BluePersistenceActivation='OFF_ISOLATION_GUARDS_RETAINED';State='WAIT_WORLD_READY'}
$file=if($Rollback){'rollback.json'}else{'deployment.json'}
[IO.File]::WriteAllText((Join-Path $out $file),($receipt|ConvertTo-Json -Depth 5),[Text.UTF8Encoding]::new($false))
$receipt|ConvertTo-Json -Depth 5
