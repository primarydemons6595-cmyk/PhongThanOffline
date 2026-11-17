[CmdletBinding()]
param()
$ErrorActionPreference='Stop'
$project=Split-Path $PSScriptRoot -Parent
$workspace=Split-Path $project -Parent
$runtime=Join-Path $workspace 'PhongThanRuntime-Loose'
$content=Join-Path $PSScriptRoot 'ProjectContent\SpecialBuffs20261031'
$manifest=Get-Content (Join-Path $content 'manifest.json') -Raw -Encoding UTF8 | ConvertFrom-Json
if($manifest.scope -ne 'LOOSE_ONLY'){throw 'Unexpected content scope'}
foreach($role in 'Server','Client'){
 if(!(Test-Path (Join-Path $runtime "$role\loose_only.flag"))){throw 'Loose-only marker missing'}
}
$blocking=@(Get-CimInstance Win32_Process | Where-Object {
 $_.ExecutablePath -and $_.Name -in 'Game.exe','GameServer.exe' -and
 $_.ExecutablePath.StartsWith($runtime+'\',[StringComparison]::OrdinalIgnoreCase)
})
if($blocking.Count){throw 'Stop the Loose game/world before publishing'}
$plans=@(
 @{Path='Server/CoreServer.dll';Source=(Join-Path $project 'Output\Server\CoreServer.dll')},
 @{Path='Client/CoreClient.dll';Source=(Join-Path $project 'Output\Client\CoreClient.dll')}
)
foreach($entry in $manifest.files){
 $source=Join-Path $content ('compiled\'+$entry.path)
 if((Get-FileHash $source).Hash -ine $entry.sha256){throw 'Staged content hash mismatch'}
 $plans+=@{Path=$entry.path;Source=$source;Before=$entry.before_sha256;CheckBefore=$true}
}
$backup=Join-Path $workspace ('analysis\buff_fix_20261031\backup_'+(Get-Date -Format yyyyMMdd_HHmmss))
foreach($plan in $plans){
 $target=[IO.Path]::GetFullPath((Join-Path $runtime $plan.Path))
 if(!$target.StartsWith($runtime+'\',[StringComparison]::OrdinalIgnoreCase)){throw 'Target escaped Loose'}
 $hash=(Get-FileHash $plan.Source).Hash
 if(Test-Path $target){
  $current=(Get-FileHash $target).Hash
  if($plan.CheckBefore -and $current -ine $hash -and $current -ine $plan.Before){throw ('Live script changed: '+$plan.Path)}
 }elseif($plan.CheckBefore -and $plan.Before){throw ('Live script missing: '+$plan.Path)}
 $plan.Target=$target;$plan.Hash=$hash
}
foreach($plan in $plans){
 if(Test-Path $plan.Target){
  $saved=Join-Path $backup $plan.Path
  New-Item -ItemType Directory -Force (Split-Path $saved -Parent)|Out-Null
  Copy-Item -LiteralPath $plan.Target -Destination $saved
 }
 New-Item -ItemType Directory -Force (Split-Path $plan.Target -Parent)|Out-Null
 Copy-Item -LiteralPath $plan.Source -Destination $plan.Target -Force
 if((Get-FileHash $plan.Target).Hash -ine $plan.Hash){throw 'Published hash mismatch'}
}
$plans | ForEach-Object {[pscustomobject]@{Path=$_.Path;SHA256=$_.Hash}} |
 ConvertTo-Json | Set-Content (Join-Path $backup 'published.json') -Encoding UTF8
[pscustomobject]@{Scope='LOOSE_ONLY';Files=$plans.Count;Backup=$backup;Result='PASS'}
