param([switch]$ResetIsolatedGateway,[switch]$DeployCore,[switch]$DeployGameServer,[switch]$DeployBishop)
$ErrorActionPreference='Stop'
if($DeployBishop -and !$ResetIsolatedGateway){throw 'DeployBishop requires stopped isolated gateway'}
$workspace=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
$report=Join-Path $workspace 'analysis/blue_magic_p1_20261018'
$iso=Get-Content -Raw (Join-Path $report 'isolation.json')|ConvertFrom-Json
$exe=Join-Path $iso.Server 'GameServer.exe'
$world=@(Get-Process GameServer -ErrorAction SilentlyContinue|Where-Object {$_.Path -ieq $exe})
if($world.Count -ne 1){throw 'Exactly one isolated world required'}
$stamp=Get-Date -Format 'yyyyMMdd_HHmmss';$evidence=Join-Path $report ('restart_'+$stamp)
New-Item -ItemType Directory -Path $evidence | Out-Null
Copy-Item -LiteralPath (Join-Path $iso.Server 'CharacterStore') -Destination (Join-Path $evidence 'CharacterStore.before') -Recurse
$proc=$world[0];[void]$proc.Handle
$event=[Threading.EventWaitHandle]::OpenExisting("Local\PhongThan.Stop.$($proc.Id)")
try{[void]$event.Set()}finally{$event.Dispose()}
if(!$proc.WaitForExit(45000)){throw 'Isolated world clean stop pending; no force kill or restart'}
$exit=$proc.ExitCode
[IO.File]::WriteAllText((Join-Path $evidence 'stop.json'),(@{Pid=$proc.Id;ExitCode=$exit;Utc=[DateTime]::UtcNow.ToString('o')}|ConvertTo-Json),[Text.UTF8Encoding]::new($false))
if($exit -ne 0){throw 'Isolated world did not exit cleanly; restart withheld'}
if($DeployCore){
 Copy-Item -LiteralPath (Join-Path $iso.Server 'CoreServer.dll') -Destination (Join-Path $evidence 'CoreServer.before.dll')
 Copy-Item -LiteralPath (Join-Path $workspace 'PhongThanSource/Output/Server/CoreServer.dll') -Destination (Join-Path $iso.Server 'CoreServer.dll')
}
if($DeployGameServer){
 Copy-Item -LiteralPath $exe -Destination (Join-Path $evidence 'GameServer.before.exe')
 Copy-Item -LiteralPath (Join-Path $workspace 'PhongThanSource/Output/Server/GameServer.exe') -Destination $exe
}
if($ResetIsolatedGateway){
 foreach($name in 'Bishop','PhongThanAccountServer'){
  $target=Join-Path $iso.Server ($name+'.exe');$services=@(Get-Process $name -ErrorAction SilentlyContinue|Where-Object {$_.Path -ieq $target})
  foreach($service in $services){$service.Kill();if(!$service.WaitForExit(10000)){throw 'Isolated support service stop pending'}}
 }
 if($DeployBishop){
  Copy-Item -LiteralPath (Join-Path $iso.Server 'Bishop.exe') -Destination (Join-Path $evidence 'Bishop.before.exe')
  Copy-Item -LiteralPath (Join-Path $workspace 'PhongThanSource/Output/Server/Bishop.exe') -Destination (Join-Path $iso.Server 'Bishop.exe')
 }
 foreach($name in 'PhongThanAccountServer','Bishop'){
  $service=Start-Process -FilePath (Join-Path $iso.Server ($name+'.exe')) -WorkingDirectory $iso.Server -WindowStyle Hidden -PassThru -RedirectStandardOutput (Join-Path $evidence ($name+'.stdout.log')) -RedirectStandardError (Join-Path $evidence ($name+'.stderr.log'))
  Start-Sleep -Seconds 1;$service.Refresh();if($service.HasExited){throw 'Isolated support service exited'}
 }
}
$oldP0=$env:PHONGTHAN_P0_MAGIC_ROOT;$oldP1=$env:PHONGTHAN_P1_BLUE_ROOT
try{$env:PHONGTHAN_P0_MAGIC_ROOT=$iso.Root;$env:PHONGTHAN_P1_BLUE_ROOT=$iso.Root;$next=Start-Process -FilePath $exe -WorkingDirectory $iso.Server -WindowStyle Hidden -PassThru -RedirectStandardOutput (Join-Path $evidence 'GameServer.stdout.log') -RedirectStandardError (Join-Path $evidence 'GameServer.stderr.log')}
finally{$env:PHONGTHAN_P0_MAGIC_ROOT=$oldP0;$env:PHONGTHAN_P1_BLUE_ROOT=$oldP1}
$receipt=@{PreviousPid=$proc.Id;ExitCode=$exit;NextPid=$next.Id;GatewayReset=$ResetIsolatedGateway.IsPresent;StartedUtc=$next.StartTime.ToUniversalTime().ToString('o');Evidence=$evidence;Scope='P1_ISOLATED_ONLY'}
[IO.File]::WriteAllText((Join-Path $evidence 'restart.json'),($receipt|ConvertTo-Json),[Text.UTF8Encoding]::new($false))
$receipt|ConvertTo-Json
