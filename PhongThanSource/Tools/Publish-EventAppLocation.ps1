$ErrorActionPreference='Stop'
$workspace=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
$runtime=Join-Path $workspace 'PhongThanRuntime-Loose';$server=Join-Path $runtime 'Server';$out=Join-Path $workspace 'analysis/event_service_20261019'
$receipt=Get-Content -Raw -Encoding UTF8 (Join-Path $out 'deployment.json')|ConvertFrom-Json
$manifestPath=Join-Path $out 'publish_manifest.json';$manifest=Get-Content -Raw -Encoding UTF8 $manifestPath|ConvertFrom-Json
$app=Get-Process -Id $receipt.EventServicePid -ErrorAction Stop
if($app.Path -ine (Join-Path $runtime 'EventService/bin/PhongThan.EventService.exe')){throw 'Unexpected service path; no relocation'}
$main=Get-Process -Id $receipt.GameServerPid -ErrorAction Stop
if($main.Path -ine (Join-Path $server 'GameServer.exe')){throw 'Unexpected Main path'}
$worldHash=(Get-FileHash -LiteralPath (Join-Path $server 'CoreServer.dll')).Hash
$configHash=(Get-FileHash -LiteralPath (Join-Path $runtime 'EventService/events.json')).Hash
$build=Join-Path $out 'service-build'
$files=@(Get-ChildItem -LiteralPath $build -File|Where-Object {$_.Name -like 'PhongThan.EventService.*' -or $_.Name -like 'PhongThanStudio.Core.*'})
if($files.Count -lt 6){throw 'Incomplete GUI build'}
$backup=Join-Path $out ('app_location_backup_'+(Get-Date -Format 'yyyyMMdd_HHmmss'));[IO.Directory]::CreateDirectory($backup)|Out-Null
$rows=@();foreach($file in $files){foreach($folder in @('Server','EventService/bin')){
 $relative=$folder+'/'+$file.Name;$target=Join-Path $runtime $relative;$before=$null
 if(Test-Path -LiteralPath $target){$before=(Get-FileHash -LiteralPath $target).Hash.ToLowerInvariant();$saved=Join-Path $backup $relative;[IO.Directory]::CreateDirectory((Split-Path $saved))|Out-Null;Copy-Item -LiteralPath $target -Destination $saved}
 $rows+=@{relative=$relative;before_sha256=$before;sha256=(Get-FileHash -LiteralPath $file.FullName).Hash.ToLowerInvariant();source=$file.FullName}
}}
[void]$app.Handle
if(!$app.CloseMainWindow()){throw 'Cannot close old service GUI cleanly'}
if(!$app.WaitForExit(20000)){throw 'Old service GUI still running; no forced stop'}
$app.Refresh();if($app.ExitCode -ne 0){throw 'Old service did not exit cleanly'}
foreach($row in $rows){
 $target=Join-Path $runtime $row.relative;$temp=$target+'.location.tmp'
 [IO.Directory]::CreateDirectory((Split-Path $target))|Out-Null
 [IO.File]::WriteAllBytes($temp,[IO.File]::ReadAllBytes($row.source))
 if(Test-Path -LiteralPath $target){[IO.File]::Replace($temp,$target,$target+'.before-location')}else{[IO.File]::Move($temp,$target)}
 $staged=Join-Path (Join-Path $out 'staged') $row.relative;[IO.Directory]::CreateDirectory((Split-Path $staged))|Out-Null;Copy-Item -LiteralPath $row.source -Destination $staged -Force
 $old=@($manifest.files|Where-Object relative -CEQ $row.relative)
 if($old.Count){$old[0].sha256=$row.sha256}else{$manifest.files+=@{relative=$row.relative;before_sha256=$row.before_sha256;sha256=$row.sha256}}
}
$new=Start-Process -FilePath (Join-Path $server 'PhongThan.EventService.exe') -ArgumentList ('--root "'+$runtime+'"') -WorkingDirectory $server -PassThru
$shortcutShell=New-Object -ComObject WScript.Shell;$shortcut=$shortcutShell.CreateShortcut((Join-Path $runtime 'Event Phong Than.lnk'))
$shortcut.TargetPath=Join-Path $server 'PhongThan.EventService.exe';$shortcut.Arguments='--root "'+$runtime+'"';$shortcut.WorkingDirectory=$server;$shortcut.Description='Event Phong Than - Server GUI companion';$shortcut.Save()
if((Get-FileHash -LiteralPath (Join-Path $server 'CoreServer.dll')).Hash -cne $worldHash){throw 'Unexpected CoreServer change'}
if((Get-FileHash -LiteralPath (Join-Path $runtime 'EventService/events.json')).Hash -cne $configHash){throw 'Unexpected configuration change'}
if(!(Get-Process -Id $main.Id -ErrorAction Stop)){throw 'Main world must remain running'}
$receipt.EventServicePid=$new.Id;$receipt.Files=$manifest.files.Count
$receipt|Add-Member -NotePropertyName EventServiceExecutable -NotePropertyValue (Join-Path $server 'PhongThan.EventService.exe') -Force
[IO.File]::WriteAllText((Join-Path $out 'deployment.json'),($receipt|ConvertTo-Json -Depth 8),[Text.UTF8Encoding]::new($false))
[IO.File]::WriteAllText($manifestPath,($manifest|ConvertTo-Json -Depth 10),[Text.UTF8Encoding]::new($false))
$result=@{PreviousServicePid=$app.Id;ServicePid=$new.Id;Executable=$receipt.EventServiceExecutable;GameServerPid=$main.Id;GameServerRestarted=$false;ConfigurationChanged=$false;Backup=$backup;LegacyStudioPath='Redirects to Server GUI executable';Utc=[DateTime]::UtcNow.ToString('o')}
[IO.File]::WriteAllText((Join-Path $out 'app_location_deployment.json'),($result|ConvertTo-Json),[Text.UTF8Encoding]::new($false));$result|ConvertTo-Json
