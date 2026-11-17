$ErrorActionPreference='Stop'
$workspace=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
$runtime=Join-Path $workspace 'PhongThanRuntime-Loose';$server=Join-Path $runtime 'Server';$out=Join-Path $workspace 'analysis/event_service_20261019'
$oldRoot=[IO.Path]::GetFullPath((Join-Path $runtime 'EventService'))
$newRoot=[IO.Path]::GetFullPath((Join-Path $server 'EventService'))
if($oldRoot -ine ($runtime+'\EventService') -or $newRoot -ine ($server+'\EventService')){throw 'Unexpected migration paths'}
$receiptPath=Join-Path $out 'deployment.json';$receipt=Get-Content -Raw -Encoding UTF8 $receiptPath|ConvertFrom-Json
$manifestPath=Join-Path $out 'publish_manifest.json';$manifest=Get-Content -Raw -Encoding UTF8 $manifestPath|ConvertFrom-Json
$main=Get-Process -Id $receipt.GameServerPid -ErrorAction Stop
if($main.Path -ine (Join-Path $server 'GameServer.exe')){throw 'Unexpected Main GameServer'}
$coreHash=(Get-FileHash -LiteralPath (Join-Path $server 'CoreServer.dll')).Hash
$eventConfigHash=(Get-FileHash -LiteralPath (Join-Path $oldRoot 'events.json')).Hash
$tokenHash=(Get-FileHash -LiteralPath (Join-Path $oldRoot 'access.token')).Hash
$app=Get-Process -Id $receipt.EventServicePid -ErrorAction Stop
if($app.Path -ine (Join-Path $server 'PhongThan.EventService.exe')){throw 'Unexpected old service path'}
$backup=Join-Path $out ('layout_backup_'+(Get-Date -Format 'yyyyMMdd_HHmmss'));[IO.Directory]::CreateDirectory($backup)|Out-Null
$rows=@()
foreach($pair in @(@('layout-build','Server'),@('layout-studio-build','ManagerStudio/Wpf/PhongThanStudio.App/bin/Release/net10.0-windows'))){
 $build=Join-Path $out $pair[0]
 foreach($file in @(Get-ChildItem -LiteralPath $build -File|Where-Object {$_.Extension -in @('.exe','.dll','.json','.pdb','.cfg')})){
  $relative=$pair[1]+'/'+$file.Name;$target=Join-Path $runtime $relative;$before=$null
  if(Test-Path -LiteralPath $target){$before=(Get-FileHash -LiteralPath $target).Hash.ToLowerInvariant();$saved=Join-Path $backup $relative;[IO.Directory]::CreateDirectory((Split-Path $saved))|Out-Null;Copy-Item -LiteralPath $target -Destination $saved}
  $rows+=@{relative=$relative;source=$file.FullName;before_sha256=$before;sha256=(Get-FileHash -LiteralPath $file.FullName).Hash.ToLowerInvariant()}
 }
}
if(!($rows|Where-Object relative -EQ 'Server/PhongThan.Event.exe') -or !($rows|Where-Object relative -EQ 'Server/PhongThan.Event.cfg')){throw 'Incomplete package'}
$studios=@(Get-Process PhongThanStudio -ErrorAction SilentlyContinue|Where-Object {$_.Path -like ($runtime+'\ManagerStudio\*')})
foreach($studio in $studios){
 [void]$studio.Handle
 if(!$studio.CloseMainWindow()){throw 'Close Studio after saving drafts; normal close was not accepted'}
 if(!$studio.WaitForExit(20000)){throw 'Studio is waiting for unsaved changes; save or cancel them before updating. No forced close.'}
}
[void]$app.Handle
if(!$app.CloseMainWindow() -or !$app.WaitForExit(20000)){throw 'Event app did not close cleanly; no forced stop'}
$app.Refresh();if($app.ExitCode -ne 0){throw 'Event app exit must be clean'}
# Migrate authoritative state without touching bridge pending/acks or resetting revisions.
foreach($name in @('catalog.json','events.json','access.token','commands','revisions','drop_drafts')){
 $from=Join-Path $oldRoot $name
 if(!(Test-Path -LiteralPath $from)){continue}
 $target=Join-Path $newRoot $name
 if(Test-Path -LiteralPath $target){throw "Canonical migration target already exists: $target"}
 Copy-Item -LiteralPath $from -Destination $target -Recurse
}
if((Get-FileHash -LiteralPath (Join-Path $newRoot 'events.json')).Hash -cne $eventConfigHash -or (Get-FileHash -LiteralPath (Join-Path $newRoot 'access.token')).Hash -cne $tokenHash){throw 'Config/token migration mismatch'}
foreach($row in $rows){
 $target=Join-Path $runtime $row.relative;$temporary=$target+'.layout.tmp'
 [IO.Directory]::CreateDirectory((Split-Path $target))|Out-Null;[IO.File]::WriteAllBytes($temporary,[IO.File]::ReadAllBytes($row.source))
 if(Test-Path -LiteralPath $target){[IO.File]::Replace($temporary,$target,$target+'.layout-before')}else{[IO.File]::Move($temporary,$target)}
 if((Get-FileHash -LiteralPath $target).Hash.ToLowerInvariant() -cne $row.sha256){throw "Package hash mismatch: $($row.relative)"}
 $staged=Join-Path (Join-Path $out 'staged') $row.relative;[IO.Directory]::CreateDirectory((Split-Path $staged))|Out-Null;Copy-Item -LiteralPath $row.source -Destination $staged -Force
 $old=@($manifest.files|Where-Object relative -CEQ $row.relative)
 if($old.Count){$old[0].sha256=$row.sha256}else{$manifest.files+=@{relative=$row.relative;before_sha256=$row.before_sha256;sha256=$row.sha256}}
}
# Checked absolute source path stays in the intended runtime. Archive rather
# than delete old state/bin, retaining the complete rollback evidence.
if([IO.Path]::GetFullPath($oldRoot) -ine ($runtime+'\EventService')){throw 'Unsafe old directory move'}
Move-Item -LiteralPath $oldRoot -Destination (Join-Path $backup 'PreviousRootEventService')
$oldBins=@(Get-ChildItem -LiteralPath $server -File -Filter 'PhongThan.EventService.*')
foreach($file in $oldBins){Move-Item -LiteralPath $file.FullName -Destination (Join-Path $backup $file.Name)}
$manifest.files=@($manifest.files|Where-Object {$_.relative -notlike 'EventService/*' -and $_.relative -notlike 'Server/PhongThan.EventService.*'})
$catalogTarget=Join-Path $newRoot 'catalog.json';$catalogRelative='Server/EventService/catalog.json'
$catalogStage=Join-Path (Join-Path $out 'staged') $catalogRelative;Copy-Item -LiteralPath $catalogTarget -Destination $catalogStage -Force
$manifest.files+=@{relative=$catalogRelative;before_sha256=$null;sha256=(Get-FileHash -LiteralPath $catalogTarget).Hash.ToLowerInvariant()}
$appNew=Start-Process -FilePath (Join-Path $server 'PhongThan.Event.exe') -ArgumentList ('--root "'+$runtime+'"') -WorkingDirectory $server -PassThru
$studioNew=Start-Process -FilePath (Join-Path $runtime 'ManagerStudio/Wpf/PhongThanStudio.App/bin/Release/net10.0-windows/PhongThanStudio.exe') -WorkingDirectory $runtime -PassThru
$shortcutShell=New-Object -ComObject WScript.Shell;$shortcut=$shortcutShell.CreateShortcut((Join-Path $runtime 'Event Phong Than.lnk'))
$shortcut.TargetPath=Join-Path $server 'PhongThan.Event.exe';$shortcut.Arguments='--root "'+$runtime+'"';$shortcut.WorkingDirectory=$server;$shortcut.Description='Event Phong Than - Server GUI companion';$shortcut.Save()
if((Get-FileHash -LiteralPath (Join-Path $server 'CoreServer.dll')).Hash -cne $coreHash){throw 'GameServer binary unexpectedly changed'}
$main.Refresh();if($main.HasExited){throw 'GameServer must remain running'}
$receipt.EventServicePid=$appNew.Id;$receipt.EventServiceExecutable=Join-Path $server 'PhongThan.Event.exe';$receipt.Files=$manifest.files.Count
$receipt|Add-Member -NotePropertyName StudioPid -NotePropertyValue $studioNew.Id -Force
[IO.File]::WriteAllText($receiptPath,($receipt|ConvertTo-Json -Depth 8),[Text.UTF8Encoding]::new($false))
[IO.File]::WriteAllText($manifestPath,($manifest|ConvertTo-Json -Depth 10),[Text.UTF8Encoding]::new($false))
$result=@{Utc=[DateTime]::UtcNow.ToString('o');Executable=$receipt.EventServiceExecutable;Config=Join-Path $server 'PhongThan.Event.cfg';EventData=$newRoot;ServicePid=$appNew.Id;StudioPid=$studioNew.Id;GameServerPid=$main.Id;GameServerRestarted=$false;ConfigurationChanged=$false;TokenPreserved=$true;Backup=$backup;LegacyFolderArchived=$true}
[IO.File]::WriteAllText((Join-Path $out 'package_layout_deployment.json'),($result|ConvertTo-Json),[Text.UTF8Encoding]::new($false));$result|ConvertTo-Json
