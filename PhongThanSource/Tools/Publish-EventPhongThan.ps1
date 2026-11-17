param([switch]$ResumeCleanStop)
$ErrorActionPreference='Stop'
$workspace=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
$runtime=Join-Path $workspace 'PhongThanRuntime-Loose'
$server=Join-Path $runtime 'Server'
$out=Join-Path $workspace 'analysis/event_service_20261019'
$manifest=Get-Content -Raw -Encoding UTF8 (Join-Path $out 'publish_manifest.json')|ConvertFrom-Json
$receipt=Join-Path $out 'deployment.json'
if(Test-Path -LiteralPath $receipt){throw 'Already published; do not overwrite first deployment receipt'}
function Hash([string]$path){if(Test-Path -LiteralPath $path){(Get-FileHash -LiteralPath $path -Algorithm SHA256).Hash.ToLowerInvariant()}else{$null}}
function Atomic-Copy([string]$from,[string]$target){
 $parent=Split-Path $target;[IO.Directory]::CreateDirectory($parent)|Out-Null
 $temporary=$target+'.event-publish.tmp';[IO.File]::WriteAllBytes($temporary,[IO.File]::ReadAllBytes($from))
 if(Test-Path -LiteralPath $target){[IO.File]::Replace($temporary,$target,$target+'.event-before-publish')}else{[IO.File]::Move($temporary,$target)}
}
foreach($row in $manifest.files){
 $target=[IO.Path]::GetFullPath((Join-Path $runtime $row.relative))
 if(!$target.StartsWith($runtime+[IO.Path]::DirectorySeparatorChar,[StringComparison]::OrdinalIgnoreCase)){throw 'Target outside runtime'}
 if((Hash $target) -cne $row.before_sha256){throw "Runtime changed after preparation: $($row.relative)"}
 if((Hash (Join-Path (Join-Path $out 'staged') $row.relative)) -cne $row.sha256){throw "Staged hash changed: $($row.relative)"}
}
foreach($row in $manifest.project_artifacts){if((Hash (Join-Path $workspace $row.target)) -cne $row.before_sha256){throw "Compiled content changed: $($row.target)"}}
foreach($row in $manifest.protected_drop_config){if((Hash (Join-Path $runtime $row.relative)) -cne $row.sha256){throw "Drop config changed: $($row.relative)"}}
if(@(Get-Process PhongThanStudio -ErrorAction SilentlyContinue|Where-Object {$_.Path -like ($runtime+'\ManagerStudio\*')}).Count){throw 'Save and close Studio before publication'}
if(@(Get-Process PhongThan.Event -ErrorAction SilentlyContinue|Where-Object {$_.Path -like ($runtime+'\EventService\*') -or $_.Path -eq (Join-Path $server 'PhongThan.Event.exe')}).Count){throw 'Event app already running; preserve its configuration'}
$world=@(Get-Process GameServer -ErrorAction SilentlyContinue|Where-Object Path -IEQ (Join-Path $server 'GameServer.exe'))
$bishop=@(Get-Process Bishop -ErrorAction SilentlyContinue|Where-Object Path -IEQ (Join-Path $server 'Bishop.exe'))
if(!$ResumeCleanStop -and ($world.Count -ne 1 -or $bishop.Count -ne 1)){throw 'Exactly one Main world and Bishop required'}
if($ResumeCleanStop){
 if($world.Count -ne 0 -or $bishop.Count -ne 0){throw 'Resume requires Main world and Bishop to remain stopped'}
 $stopped=Get-Content -Raw (Join-Path $out 'clean_stop.json')|ConvertFrom-Json
 if($stopped.ExitCode -ne 0 -or ([DateTime]::UtcNow-[DateTime]$stopped.Utc).TotalMinutes -gt 15){throw 'No recent verified clean stop'}
 $previousPid=$stopped.Pid
}
$isolated=@(Get-Process GameServer -ErrorAction SilentlyContinue|Where-Object {$_.Path -ne (Join-Path $server 'GameServer.exe')}|ForEach-Object {@{Pid=$_.Id;Path=$_.Path;CoreSha256=Hash (Join-Path (Split-Path $_.Path) 'CoreServer.dll')}})
$backup=Join-Path $out ('backup_'+(Get-Date -Format 'yyyyMMdd_HHmmss'));[IO.Directory]::CreateDirectory($backup)|Out-Null
foreach($row in $manifest.files){$target=Join-Path $runtime $row.relative;if(Test-Path -LiteralPath $target){$dest=Join-Path $backup $row.relative;[IO.Directory]::CreateDirectory((Split-Path $dest))|Out-Null;Copy-Item -LiteralPath $target -Destination $dest}}
foreach($row in $manifest.project_artifacts){$target=Join-Path $workspace $row.target;if(Test-Path -LiteralPath $target){$dest=Join-Path $backup ('Project/'+$row.target);[IO.Directory]::CreateDirectory((Split-Path $dest))|Out-Null;Copy-Item -LiteralPath $target -Destination $dest}}
if(!$ResumeCleanStop){
$proc=$world[0];[void]$proc.Handle;$previousPid=$proc.Id
$event=[Threading.EventWaitHandle]::OpenExisting("Local\PhongThan.Stop.$($proc.Id)")
try{[void]$event.Set()}finally{$event.Dispose()}
if(!$proc.WaitForExit(45000)){throw 'Main did not stop cleanly; no forced stop or file replacement'}
$proc.Refresh();if($proc.ExitCode -ne 0){throw 'Main exit was not clean; no file replacement'}
[IO.File]::WriteAllText((Join-Path $out 'clean_stop.json'),(@{Pid=$proc.Id;ExitCode=$proc.ExitCode;Utc=[DateTime]::UtcNow.ToString('o');CharacterDataRestored=$false;CountersReset=$false}|ConvertTo-Json))
# Admission state is cleared only after the world has saved and exited.
Stop-Process -Id $bishop[0].Id -ErrorAction Stop
if(!$bishop[0].WaitForExit(10000)){throw 'Main Bishop did not exit'}
}
foreach($row in $manifest.files){
 $target=Join-Path $runtime $row.relative;Atomic-Copy (Join-Path (Join-Path $out 'staged') $row.relative) $target
 if((Hash $target) -cne $row.sha256){throw "Published hash mismatch: $($row.relative)"}
}
foreach($row in $manifest.project_artifacts){Atomic-Copy (Join-Path (Join-Path $out 'staged') $row.source) (Join-Path $workspace $row.target)}
foreach($row in $manifest.protected_drop_config){if((Hash (Join-Path $runtime $row.relative)) -cne $row.sha256){throw "Drop config must remain identical: $($row.relative)"}}
$old0=$env:PHONGTHAN_P0_MAGIC_ROOT;$old1=$env:PHONGTHAN_P1_BLUE_ROOT;$old2=$env:PHONGTHAN_P2_BLUE_ROOT
try{
 $env:PHONGTHAN_P0_MAGIC_ROOT=$null;$env:PHONGTHAN_P1_BLUE_ROOT=$null;$env:PHONGTHAN_P2_BLUE_ROOT=$null
 $nextBishop=Start-Process -FilePath (Join-Path $server 'Bishop.exe') -WorkingDirectory $server -WindowStyle Hidden -PassThru -RedirectStandardOutput (Join-Path $out 'Bishop.stdout.log') -RedirectStandardError (Join-Path $out 'Bishop.stderr.log')
 Start-Sleep -Seconds 2
 $nextWorld=Start-Process -FilePath (Join-Path $server 'GameServer.exe') -WorkingDirectory $server -WindowStyle Hidden -PassThru -RedirectStandardOutput (Join-Path $out 'GameServer.stdout.log') -RedirectStandardError (Join-Path $out 'GameServer.stderr.log')
 # User explicitly requested this GUI companion app, visible like Bishop.
 $nextService=Start-Process -FilePath (Join-Path $server 'PhongThan.Event.exe') -ArgumentList ('--root "'+$runtime+'"') -WorkingDirectory $server -PassThru
}finally{$env:PHONGTHAN_P0_MAGIC_ROOT=$old0;$env:PHONGTHAN_P1_BLUE_ROOT=$old1;$env:PHONGTHAN_P2_BLUE_ROOT=$old2}
$result=@{Utc=[DateTime]::UtcNow.ToString('o');PreviousWorldPid=$previousPid;GameServerPid=$nextWorld.Id;BishopPid=$nextBishop.Id;EventServicePid=$nextService.Id;CoreServerSha256=Hash (Join-Path $server 'CoreServer.dll');Backup=$backup;State='WAIT_NATIVE_READY';NativeDropConfigChanged=$false;CharacterDataRestored=$false;CountersReset=$false;IsolatedServers=$isolated;Files=$manifest.files.Count}
[IO.File]::WriteAllText($receipt,($result|ConvertTo-Json -Depth 6),[Text.UTF8Encoding]::new($false))
$result|ConvertTo-Json -Depth 6
