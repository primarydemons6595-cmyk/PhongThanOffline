[CmdletBinding()]
param()
$ErrorActionPreference='Stop'
$workspace=Split-Path (Split-Path $PSScriptRoot -Parent) -Parent
$source=Join-Path $workspace 'PhongThanSource'
$loose=Join-Path $workspace 'PhongThanRuntime-Loose'
$report=Join-Path $workspace 'analysis/quiet_logs_20261031'
if((Get-Content (Join-Path $source 'Build/native-runtime-profile.txt') -Raw).Trim() -ne 'Dev'){throw 'Expected Dev artifacts'}
$running=@(Get-CimInstance Win32_Process | Where-Object {$_.ExecutablePath -and $_.ExecutablePath.StartsWith($loose+'\',[StringComparison]::OrdinalIgnoreCase) -and $_.Name -match '^(PhongThanStudio|PhongThanCharacterEditor|PhongThanConsoleHost|PhongThan.Event|Game|GameServer|Goddess|Bishop|PhongThanAccountServer|PhongThanRelay)\.exe$'})
if($running.Count){throw ('Close Loose applications before publishing: '+($running.Name -join ', '))}
$backup=Join-Path $report ('runtime_backup_'+(Get-Date -Format yyyyMMdd_HHmmss))
New-Item -ItemType Directory -Force $backup | Out-Null
$receipt=@()
function Publish([string]$from,[string]$relative){
 $target=[IO.Path]::GetFullPath((Join-Path $loose $relative))
 if(!$target.StartsWith($loose+'\',[StringComparison]::OrdinalIgnoreCase)){throw 'Target escapes Loose'}
 $hash=(Get-FileHash -LiteralPath $from).Hash
 $saved=Join-Path $backup $relative
 New-Item -ItemType Directory -Force (Split-Path $target),(Split-Path $saved) | Out-Null
 $before=$null
 if(Test-Path -LiteralPath $target){$before=(Get-FileHash -LiteralPath $target).Hash;Copy-Item -LiteralPath $target -Destination $saved}
 $temp=$target+'.update-'+[guid]::NewGuid().ToString('N')
 try{
  Copy-Item -LiteralPath $from -Destination $temp
  if((Get-FileHash -LiteralPath $temp).Hash -ne $hash){throw 'Staging hash mismatch'}
  if(Test-Path -LiteralPath $target){[IO.File]::Replace($temp,$target,$saved+'.atomic')}else{[IO.File]::Move($temp,$target)}
 }finally{if(Test-Path -LiteralPath $temp){Remove-Item -LiteralPath $temp}}
 if((Get-FileHash -LiteralPath $target).Hash -ne $hash){throw 'Publication hash mismatch'}
 $script:receipt+=@{Path=$relative;Before=$before;After=$hash}
}
foreach($role in 'Server','Client'){
 if(!(Test-Path (Join-Path $loose "$role/loose_only.flag"))){throw 'Missing Loose marker'}
 $files=if($role -eq 'Server'){@('Engine.dll','CoreServer.dll','GameServer.exe','PhongThanAccountServer.exe','PhongThanRelay.exe','Bishop.exe','Goddess.exe','PhongThan.Event.exe','PhongThan.Event.dll','PhongThan.Event.deps.json','PhongThan.Event.runtimeconfig.json','PhongThan.Event.Runtime.dll')}else{@('Engine.dll','CoreClient.dll','Game.exe','Represent2.dll')}
 foreach($name in $files){Publish (Join-Path $source "Output/$role/$name") "$role/$name"}
 Publish (Join-Path $PSScriptRoot 'ProjectContent/settings/phongthan/Diagnostics.ini') "$role/settings/phongthan/Diagnostics.ini"
}
$package=Join-Path $report 'studio-package'
foreach($file in Get-ChildItem -LiteralPath $package -File -Recurse){
 $relative=$file.FullName.Substring($package.Length+1)
 Publish $file.FullName (Join-Path 'ManagerStudio/Release' $relative)
}
@{Scope='LOOSE_ONLY';Result='PASS';Backup=$backup;Files=$receipt}|ConvertTo-Json -Depth 5|Set-Content (Join-Path $report 'publication.json') -Encoding UTF8
Write-Output "PASS: $($receipt.Count) files published; backup $backup"
