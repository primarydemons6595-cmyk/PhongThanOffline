[CmdletBinding()]
param()
$ErrorActionPreference='Stop'
$project=Split-Path $PSScriptRoot -Parent
$workspace=Split-Path $project -Parent
$runtime=Join-Path $workspace 'PhongThanRuntime-Loose'
$package=Join-Path $PSScriptRoot 'ProjectContent\TgptVtt20261031'
$manifest=Get-Content (Join-Path $package 'manifest.json') -Raw -Encoding UTF8|ConvertFrom-Json
if($manifest.scope -ne 'LOOSE_ONLY'){throw 'Unexpected scope'}
foreach($role in 'Server','Client'){if(!(Test-Path (Join-Path $runtime "$role\loose_only.flag"))){throw 'Loose marker missing'}}
if(@(Get-CimInstance Win32_Process|Where-Object {$_.Name -in 'Game.exe','GameServer.exe' -and $_.ExecutablePath -and $_.ExecutablePath.StartsWith($runtime+'\',[StringComparison]::OrdinalIgnoreCase)}).Count){throw 'Stop the Loose client and world safely before publishing'}
$plans=@(@{Path='Server/CoreServer.dll';Source=(Join-Path $project 'Output\Server\CoreServer.dll');CheckBefore=$false})
foreach($entry in $manifest.files){
 if(!$entry.path.StartsWith('Server/') -and !$entry.path.StartsWith('Client/')){throw 'Unexpected role path'}
 $plans+=@{Path=$entry.path;Source=(Join-Path $package ('compiled\'+$entry.path));CheckBefore=$true;Before=$entry.before_sha256;Expected=$entry.sha256}
}
foreach($plan in $plans){
 $target=[IO.Path]::GetFullPath((Join-Path $runtime $plan.Path))
 if(!$target.StartsWith($runtime+'\',[StringComparison]::OrdinalIgnoreCase)){throw 'Target escaped Loose'}
 $hash=(Get-FileHash -LiteralPath $plan.Source).Hash
 if($plan.Expected -and $hash -ine $plan.Expected){throw 'Staged hash mismatch'}
 if(Test-Path -LiteralPath $target){
  $current=(Get-FileHash -LiteralPath $target).Hash
  if($plan.CheckBefore -and $current -ine $hash -and $current -ine $plan.Before){throw ('Live file changed: '+$plan.Path)}
 }elseif($plan.Before){throw ('Live file unexpectedly missing: '+$plan.Path)}
 $plan.Target=$target;$plan.Hash=$hash
}
$backup=Join-Path $workspace ('analysis\vtt_tgpt_20261031\backup_'+(Get-Date -Format yyyyMMdd_HHmmss))
foreach($plan in $plans){
 if(Test-Path -LiteralPath $plan.Target){
  $saved=Join-Path $backup $plan.Path;New-Item -ItemType Directory -Force (Split-Path $saved -Parent)|Out-Null
  Copy-Item -LiteralPath $plan.Target -Destination $saved
 }
 New-Item -ItemType Directory -Force (Split-Path $plan.Target -Parent)|Out-Null
 $temporary=$plan.Target+'.pt-update-'+[guid]::NewGuid().ToString('N')
 try{
  Copy-Item -LiteralPath $plan.Source -Destination $temporary
  if((Get-FileHash -LiteralPath $temporary).Hash -ine $plan.Hash){throw 'Temporary hash mismatch'}
  if(Test-Path -LiteralPath $plan.Target){
   $replaceBackup=Join-Path $backup ('atomic\'+$plan.Path)
   New-Item -ItemType Directory -Force (Split-Path $replaceBackup -Parent)|Out-Null
   [IO.File]::Replace($temporary,$plan.Target,$replaceBackup)
  }else{[IO.File]::Move($temporary,$plan.Target)}
 }finally{if(Test-Path -LiteralPath $temporary){Remove-Item -LiteralPath $temporary}}
 if((Get-FileHash -LiteralPath $plan.Target).Hash -ine $plan.Hash){throw 'Published hash mismatch'}
}
$receipt=[pscustomobject]@{Scope='LOOSE_ONLY';Files=$plans.Count;Backup=$backup;CoreServerSHA256=(Get-FileHash (Join-Path $runtime 'Server\CoreServer.dll')).Hash;Result='PASS'}
$receipt|ConvertTo-Json|Set-Content (Join-Path $package 'publication.json') -Encoding UTF8
$receipt|ConvertTo-Json|Set-Content (Join-Path $backup 'publication.json') -Encoding UTF8
$receipt
