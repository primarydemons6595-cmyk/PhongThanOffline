[CmdletBinding()]
param([Parameter(Mandatory=$true)][string]$RoleRoot,[string]$ProjectRoot=$PSScriptRoot+'\..')
$ErrorActionPreference='Stop'
$ProjectRoot=[IO.Path]::GetFullPath($ProjectRoot)
$workspace=Split-Path $ProjectRoot -Parent
$RoleRoot=[IO.Path]::GetFullPath($RoleRoot).TrimEnd('\')
$role=Split-Path $RoleRoot -Leaf
if($role -notin 'Server','Client' -or $RoleRoot -ine (Join-Path $workspace "PhongThanRuntime-Loose\$role")){throw 'Only the main Loose role is supported'}
if(@(Get-CimInstance Win32_Process | Where-Object {$_.ExecutablePath -and $_.ExecutablePath.StartsWith($RoleRoot+'\',[StringComparison]::OrdinalIgnoreCase)}).Count){throw 'Stop the Loose role before publishing'}
$plans=@{}
foreach($pair in @(@('LifeGiaoHuan20261030','manifest.json'),@('LifeGiaoHuan20261030','life-manifest.json'),@('KhonTienBoss20261030','manifest.json'),@('JourneyRepair20261031','manifest.json'),@('SpecialBuffs20261031','manifest.json'),@('VttPopulation20261031','manifest.json'),@('HandbookLevels20261031','manifest.json'),@('TgptVtt20261031','manifest.json'),@('F2Requested20261031','manifest.json'))){
 $content=Join-Path $ProjectRoot ('Deploy\ProjectContent\'+$pair[0]);$manifest=Get-Content -LiteralPath (Join-Path $content $pair[1]) -Raw -Encoding UTF8|ConvertFrom-Json
 foreach($entry in $manifest.files){
  if(!$entry.path.StartsWith($role+'/')){continue}
  $relative=$entry.path.Substring($role.Length+1)
  $target=[IO.Path]::GetFullPath((Join-Path $RoleRoot $relative))
  if(!$target.StartsWith($RoleRoot+'\',[StringComparison]::OrdinalIgnoreCase)){throw 'Path escaped role root'}
  # Preserve later Studio schedule edits and manually tuned drop weights.
  if($pair[0] -eq 'TgptVtt20261031' -and ($entry.path -eq 'Server/EventService/events.json' -or $entry.path.StartsWith('Server/settings/item/droprate/')) -and (Test-Path -LiteralPath $target)){
   $current=(Get-FileHash -LiteralPath $target).Hash
   if($current -ine $entry.sha256 -and $current -ine $entry.before_sha256){continue}
  }
  $source=Join-Path $content ('compiled\'+$entry.path)
  if((Get-FileHash -LiteralPath $source).Hash -ine $entry.sha256){throw 'Staged content hash mismatch'}
  $plans[$relative]=@{Source=$source;Target=$target;Hash=$entry.sha256}
 }
}
foreach($plan in $plans.Values){
 New-Item -ItemType Directory -Force -Path (Split-Path $plan.Target -Parent)|Out-Null
 Copy-Item -LiteralPath $plan.Source -Destination $plan.Target -Force
 if((Get-FileHash -LiteralPath $plan.Target).Hash -ine $plan.Hash){throw 'Published content hash mismatch'}
}
[pscustomobject]@{Role=$role;Files=$plans.Count;Result='PASS'}
