[CmdletBinding()]
param()
$ErrorActionPreference='Stop'
$project=Split-Path $PSScriptRoot -Parent
$workspace=Split-Path $project -Parent
$runtime=Join-Path $workspace 'PhongThanRuntime-Loose'
$content=Join-Path $PSScriptRoot 'ProjectContent\VttPopulation20261031'
$manifest=Get-Content (Join-Path $content 'manifest.json') -Raw -Encoding UTF8|ConvertFrom-Json
if($manifest.scope -ne 'LOOSE_ONLY'){throw 'Unexpected scope'}
if(!(Test-Path (Join-Path $runtime 'Server\loose_only.flag'))){throw 'Loose-only marker missing'}
if(@(Get-CimInstance Win32_Process|Where-Object {$_.Name -eq 'GameServer.exe' -and $_.ExecutablePath -eq (Join-Path $runtime 'Server\GameServer.exe')}).Count){throw 'Stop the Loose world safely before publishing'}
$plans=@()
foreach($entry in $manifest.files){
 if(!$entry.path.StartsWith('Server/')){throw 'Unexpected role'}
 $target=[IO.Path]::GetFullPath((Join-Path $runtime $entry.path))
 if(!$target.StartsWith($runtime+'\',[StringComparison]::OrdinalIgnoreCase)){throw 'Target escaped Loose'}
 $source=Join-Path $content ('compiled\'+$entry.path)
 if((Get-FileHash $source).Hash -ine $entry.sha256){throw 'Staged hash mismatch'}
 if(Test-Path $target){
  $current=(Get-FileHash $target).Hash
  if($current -ine $entry.sha256 -and $current -ine $entry.before_sha256){throw ('Live file changed: '+$entry.path)}
 }elseif($entry.before_sha256){throw 'Live file unexpectedly missing'}
 $plans+=@{Source=$source;Target=$target;Entry=$entry}
}
$backup=Join-Path $workspace ('analysis\vtt_repair_20261031\backup_'+(Get-Date -Format yyyyMMdd_HHmmss))
foreach($plan in $plans){
 if(Test-Path $plan.Target){
  $saved=Join-Path $backup $plan.Entry.path
  New-Item -ItemType Directory -Force (Split-Path $saved -Parent)|Out-Null
  Copy-Item -LiteralPath $plan.Target -Destination $saved
 }
 New-Item -ItemType Directory -Force (Split-Path $plan.Target -Parent)|Out-Null
 Copy-Item -LiteralPath $plan.Source -Destination $plan.Target -Force
 if((Get-FileHash $plan.Target).Hash -ine $plan.Entry.sha256){throw 'Published hash mismatch'}
}
$manifest|ConvertTo-Json -Depth 10|Set-Content (Join-Path $backup 'published.json') -Encoding UTF8
[pscustomobject]@{Scope='LOOSE_ONLY';Files=$plans.Count;Backup=$backup;Result='PASS'}
