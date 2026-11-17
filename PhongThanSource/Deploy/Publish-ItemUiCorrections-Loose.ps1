[CmdletBinding()]
param()
$ErrorActionPreference='Stop'
$source=Split-Path $PSScriptRoot -Parent
$workspace=Split-Path $source -Parent
$runtime=Join-Path $workspace 'PhongThanRuntime-Loose'
$package=Join-Path $PSScriptRoot 'ProjectContent/ItemUiCorrections20261031'
$manifest=Get-Content (Join-Path $package 'manifest.json') -Raw -Encoding UTF8|ConvertFrom-Json
if($manifest.scope -ne 'LOOSE_ONLY'){throw 'Unexpected scope'}
$running=@(Get-CimInstance Win32_Process|Where-Object {$_.ExecutablePath -and $_.ExecutablePath.StartsWith($runtime+'\',[StringComparison]::OrdinalIgnoreCase) -and $_.Name -in 'Game.exe','GameServer.exe'})
if($running.Count){throw 'Stop Loose client and world safely before publishing'}
$plans=@()
foreach($file in $manifest.files){$plans+=@{Path=$file.path;Source=(Join-Path $package ('compiled/'+$file.path));Before=$file.before;After=$file.after}}
foreach($file in $manifest.native){$plans+=@{Path=$file.path;Source=(Join-Path $package ('native/'+$file.path));Before=$file.before;After=$file.after}}
foreach($plan in $plans){
 $target=[IO.Path]::GetFullPath((Join-Path $runtime $plan.Path))
 if(!$target.StartsWith($runtime+'\',[StringComparison]::OrdinalIgnoreCase)){throw 'Target escaped Loose'}
 if((Get-FileHash -LiteralPath $plan.Source).Hash -ine $plan.After){throw ('Staged hash mismatch '+$plan.Path)}
 if((Get-FileHash -LiteralPath $target).Hash -ine $plan.Before){throw ('Live file changed '+$plan.Path)}
 $plan.Target=$target
}
$backup=Join-Path $workspace ('analysis/item_ui_corrections_20261031/runtime_backup_'+(Get-Date -Format yyyyMMdd_HHmmss))
foreach($plan in $plans){
 $saved=Join-Path $backup $plan.Path
 New-Item -ItemType Directory -Force (Split-Path $saved)|Out-Null
 Copy-Item -LiteralPath $plan.Target -Destination $saved
 $temp=$plan.Target+'.update-'+[guid]::NewGuid().ToString('N')
 try{
  Copy-Item -LiteralPath $plan.Source -Destination $temp
  if((Get-FileHash -LiteralPath $temp).Hash -ine $plan.After){throw 'Temporary hash mismatch'}
  [IO.File]::Replace($temp,$plan.Target,$saved+'.atomic')
 }finally{if(Test-Path -LiteralPath $temp){Remove-Item -LiteralPath $temp}}
 if((Get-FileHash -LiteralPath $plan.Target).Hash -ine $plan.After){throw 'Published hash mismatch'}
}
$receipt=@{Scope='LOOSE_ONLY';Result='PASS';Backup=$backup;Files=$plans|Select-Object Path,Before,After}
$receipt|ConvertTo-Json -Depth 5|Set-Content (Join-Path $package 'publication.json') -Encoding UTF8
$receipt|ConvertTo-Json -Depth 5|Set-Content (Join-Path $workspace 'analysis/item_ui_corrections_20261031/publication.json') -Encoding UTF8
Write-Output "PASS: $($plans.Count) Loose files published; backup $backup"
