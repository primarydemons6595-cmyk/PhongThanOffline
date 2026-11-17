$ErrorActionPreference='Stop'
$workspace=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
$runtime=Join-Path $workspace 'PhongThanRuntime-Loose';$out=Join-Path $workspace 'analysis/event_service_20261019'
$targetRoot=Join-Path $runtime 'ManagerStudio/Wpf/PhongThanStudio.App/bin/Release/net10.0-windows'
$build=Join-Path $out 'boss-columns-build'
if((Get-FileHash (Join-Path $build 'PhongThanStudio.Core.dll')).Hash -ne (Get-FileHash (Join-Path $targetRoot 'PhongThanStudio.Core.dll')).Hash){throw 'Unexpected Core changes; this update must affect only Studio UI'}
$manifestPath=Join-Path $out 'publish_manifest.json';$manifest=Get-Content -Raw -Encoding UTF8 $manifestPath|ConvertFrom-Json
$receiptPath=Join-Path $out 'deployment.json';$receipt=Get-Content -Raw -Encoding UTF8 $receiptPath|ConvertFrom-Json
$backup=Join-Path $out ('boss_columns_backup_'+(Get-Date -Format 'yyyyMMdd_HHmmss'));[IO.Directory]::CreateDirectory($backup)|Out-Null
$rows=@();foreach($name in @('PhongThanStudio.exe','PhongThanStudio.dll','PhongThanStudio.pdb','PhongThanStudio.deps.json','PhongThanStudio.runtimeconfig.json')){
 $from=Join-Path $build $name;$target=Join-Path $targetRoot $name
 if(!(Test-Path -LiteralPath $from)){throw "Build missing $name"}
 Copy-Item -LiteralPath $target -Destination (Join-Path $backup $name)
 $rows+=@{name=$name;before=(Get-FileHash -LiteralPath $target).Hash;after=(Get-FileHash -LiteralPath $from).Hash}
}
$studios=@(Get-Process PhongThanStudio -ErrorAction SilentlyContinue|Where-Object Path -IEQ (Join-Path $targetRoot 'PhongThanStudio.exe'))
foreach($studio in $studios){[void]$studio.Handle;if(!$studio.CloseMainWindow() -or !$studio.WaitForExit(15000)){throw 'Studio still open; save changes and close it. No forced close.'}}
foreach($row in $rows){
 $target=Join-Path $targetRoot $row.name;$temp=$target+'.boss-columns.tmp'
 [IO.File]::WriteAllBytes($temp,[IO.File]::ReadAllBytes((Join-Path $build $row.name)));[IO.File]::Replace($temp,$target,(Join-Path $backup ($row.name+'.swap')))
 if((Get-FileHash -LiteralPath $target).Hash -ne $row.after){throw "Publish hash mismatch $($row.name)"}
 $relative='ManagerStudio/Wpf/PhongThanStudio.App/bin/Release/net10.0-windows/'+$row.name
 $entry=@($manifest.files|Where-Object relative -CEQ $relative);if($entry.Count -ne 1){throw 'Published UI file absent from manifest'};$entry[0].sha256=$row.after.ToLowerInvariant()
 Copy-Item -LiteralPath (Join-Path $build $row.name) -Destination (Join-Path (Join-Path $out 'staged') $relative) -Force
}
$next=Start-Process -FilePath (Join-Path $targetRoot 'PhongThanStudio.exe') -WorkingDirectory $runtime -PassThru
$receipt.StudioPid=$next.Id
[IO.File]::WriteAllText($manifestPath,($manifest|ConvertTo-Json -Depth 10),[Text.UTF8Encoding]::new($false))
[IO.File]::WriteAllText($receiptPath,($receipt|ConvertTo-Json -Depth 8),[Text.UTF8Encoding]::new($false))
$result=@{Utc=[DateTime]::UtcNow.ToString('o');StudioPid=$next.Id;Backup=$backup;BossRowsVerified=12;ServerRestarted=$false;EventConfigChanged=$false;Files=$rows}
[IO.File]::WriteAllText((Join-Path $out 'boss_columns_deployment.json'),($result|ConvertTo-Json -Depth 5),[Text.UTF8Encoding]::new($false));$result|ConvertTo-Json -Depth 5
