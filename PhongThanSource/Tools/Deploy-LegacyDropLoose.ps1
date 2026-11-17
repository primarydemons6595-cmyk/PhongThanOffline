[CmdletBinding()]
param([switch]$Rollback,[switch]$ResumeStoppedEmptyWorld,[switch]$Monitor,[switch]$PreserveConfiguration)
$ErrorActionPreference='Stop'
$source=Split-Path -Parent $PSScriptRoot
$workspace=Split-Path -Parent $source
$server=[IO.Path]::GetFullPath((Join-Path $workspace 'PhongThanRuntime-Loose/Server'))
if((Split-Path -Leaf (Split-Path -Parent $server)) -cne 'PhongThanRuntime-Loose'){throw 'Loose path required'}
if(!(Test-Path -LiteralPath (Join-Path $server 'loose_only.flag'))){throw 'Loose marker missing'}
$report=Join-Path $workspace 'analysis/legacy_drop_20261018_live'
$backup=Join-Path $report 'runtime_before'
New-Item -ItemType Directory -Force $backup | Out-Null
$binary=Join-Path $source 'Output/Server/CoreServer.dll'
if($Rollback){$binary=Join-Path $backup 'CoreServer.dll'}
if(!(Test-Path -LiteralPath $binary)){throw 'Build CoreServer first'}
$game=@(Get-Process -Name GameServer -ErrorAction SilentlyContinue | Where-Object {$_.Path -ieq (Join-Path $server 'GameServer.exe')})
if($game.Count -gt 1 -or ($game.Count -eq 0 -and !$ResumeStoppedEmptyWorld)){throw 'Expected one running Loose GameServer; do not start a duplicate'}
$stopEvidence='NOT_STOPPED'
if($game.Count -eq 0){
 $previous=Get-Content -Raw -LiteralPath (Join-Path $report 'deployment.json') | ConvertFrom-Json
 $nativeStopped=$false
 if($previous.MonitorLog -and (Test-Path -LiteralPath $previous.MonitorLog)){
  $nativeStopped=(Get-Content -Raw -LiteralPath $previous.MonitorLog) -match ('EXIT pid='+$previous.GameServerPid+' code=0 hex=00000000')
 }
 $since=[DateTime]::Parse($previous.PublishedUtc).ToUniversalTime()
 if((Get-Item -LiteralPath (Join-Path $server 'gameserver_login_diag.log')).LastWriteTimeUtc -ge $since){throw 'Cannot recover automatically: possible player sessions since startup'}
 if(@(Get-ChildItem -LiteralPath (Join-Path $server 'CharacterStore') -File | Where-Object {$_.LastWriteTimeUtc -ge $since}).Count){throw 'Cannot recover automatically: character storage changed'}
 $stopEvidence=if($nativeStopped){'NATIVE_MONITOR_EXIT_0'}else{'PREVIOUS_EXIT_UNCONFIRMED_NO_PLAYER_LOG_OR_CHARACTER_WRITES'}
}
$expectedBinaryHash=(Get-FileHash -LiteralPath $binary -Algorithm SHA256).Hash
# Prepare rollback copies before any service mutation. No data/catalog is copied.
foreach($relative in @('CoreServer.dll','settings/phongthan/LegacyDropExperiment.ini','settings/phongthan/LegacyDropQuarantine.txt')){
 $file=Join-Path $server $relative
 if(Test-Path -LiteralPath $file){$saved=Join-Path $backup ($relative.Replace('/','__'));if(!(Test-Path -LiteralPath $saved)){Copy-Item -LiteralPath $file -Destination $saved}}
}
$previousPid=0
if($game.Count -eq 0){$previousPid=[int]$previous.GameServerPid}
if($game.Count -eq 1){
 $proc=$game[0];$previousPid=$proc.Id
 Add-Type -TypeDefinition 'using System; using System.Runtime.InteropServices; public static class LegacyDropNativeExit { [DllImport("kernel32.dll", SetLastError=true)] public static extern IntPtr OpenProcess(uint access,bool inherit,int pid); [DllImport("kernel32.dll", SetLastError=true)] public static extern bool GetExitCodeProcess(IntPtr handle,out uint code); [DllImport("kernel32.dll")] public static extern bool CloseHandle(IntPtr handle); }'
 $nativeHandle=[LegacyDropNativeExit]::OpenProcess(0x101000,$false,$proc.Id)
 if($nativeHandle -eq [IntPtr]::Zero){throw 'Cannot retain native process handle for verified stop'}
 try{
  $event=[Threading.EventWaitHandle]::OpenExisting("Local\PhongThan.Stop.$($proc.Id)")
  try{[void]$event.Set()}finally{$event.Dispose()}
  if(!$proc.WaitForExit(45000)){throw 'Save/stop still pending. Do not force kill; support services retained.'}
  [uint32]$exit=0
  if(![LegacyDropNativeExit]::GetExitCodeProcess($nativeHandle,[ref]$exit)){throw 'Cannot query native exit code; binary retained'}
  [IO.File]::WriteAllText((Join-Path $report "stop_$previousPid.json"),(@{ProcessId=$previousPid;NativeExitCode=$exit}|ConvertTo-Json))
  if($exit -ne 0){throw "World exit=$exit; binary not replaced, support services retained"}
  $stopEvidence='NATIVE_EXIT_0'
 }finally{[LegacyDropNativeExit]::CloseHandle($nativeHandle)|Out-Null}
}
Copy-Item -LiteralPath $binary -Destination (Join-Path $server 'CoreServer.dll') -Force
if(!$PreserveConfiguration){
 Copy-Item -LiteralPath (Join-Path $source 'Tests/LegacyDropNativeTest/LegacyDropExperiment.ini') -Destination (Join-Path $server 'settings/phongthan/LegacyDropExperiment.ini') -Force
 Copy-Item -LiteralPath (Join-Path $workspace 'analysis/legacy_drop_20261017/phase2/quarantine.txt') -Destination (Join-Path $server 'settings/phongthan/LegacyDropQuarantine.txt') -Force
}
if((Get-FileHash -LiteralPath (Join-Path $server 'CoreServer.dll')).Hash -ne $expectedBinaryHash){throw 'Published binary hash differs'}
$stamp=Get-Date -Format 'yyyyMMdd_HHmmss'
$monitorPid=0;$monitorLog=''
if($Monitor){
 $monitorExecutable=Join-Path $source 'Output/LegacyDropNativeTest/NativeWorldMonitor.exe'
 $monitorLog=Join-Path $report "$stamp.world_monitor.log";$stdout=Join-Path $report "$stamp.GameServer.stdout.log"
 $arguments=@(('"'+(Join-Path $server 'GameServer.exe')+'"'),('"'+$monitorLog+'"'),('"'+$stdout+'"'))
 $parent=Start-Process -FilePath $monitorExecutable -ArgumentList $arguments -WorkingDirectory $server -WindowStyle Hidden -PassThru
 $monitorPid=$parent.Id;$childPid=0
 for($attempt=0;$attempt -lt 30;$attempt++){if(Test-Path -LiteralPath $monitorLog){$m=[regex]::Match((Get-Content -Raw -LiteralPath $monitorLog),'START pid=(\d+)');if($m.Success){$childPid=[int]$m.Groups[1].Value;break}};Start-Sleep -Milliseconds 200}
 if(!$childPid){throw 'Native monitor did not report a GameServer PID; inspect monitor log'}
}else{
 $next=Start-Process -FilePath (Join-Path $server 'GameServer.exe') -WorkingDirectory $server -WindowStyle Hidden -PassThru -RedirectStandardOutput (Join-Path $report "$stamp.GameServer.stdout.log") -RedirectStandardError (Join-Path $report "$stamp.GameServer.stderr.log")
 $childPid=$next.Id
}
$receipt=[ordered]@{Scope='LOOSE_EXPERIMENTAL_ONLY';ExperimentalHooksPresent=(!$Rollback);ProductionGate='CLOSED';Parity='KINGSOFT_PARITY_UNVERIFIED';DefaultMode='OFF';PreservedConfiguration=$PreserveConfiguration.IsPresent;PreviousPid=$previousPid;StopEvidence=$stopEvidence;GameServerPid=$childPid;MonitorPid=$monitorPid;MonitorLog=$monitorLog;Binary=(Join-Path $server 'CoreServer.dll');Sha256=$expectedBinaryHash;RollbackBinary=(Join-Path $backup 'CoreServer.dll');PublishedUtc=[DateTime]::UtcNow.ToString('o')}
[IO.File]::WriteAllText((Join-Path $report 'deployment.json'),($receipt|ConvertTo-Json),[Text.UTF8Encoding]::new($false))
$receipt|ConvertTo-Json
