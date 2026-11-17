[CmdletBinding()]
param([Parameter(Mandatory=$true)][string]$RoleRoot,[string]$ProjectRoot='')
$ErrorActionPreference='Stop'
if(!$ProjectRoot){$ProjectRoot=Split-Path -Parent $PSScriptRoot}
$role=Split-Path -Leaf $RoleRoot
if($role -notin @('Client','Server')){throw 'Expected runtime role'}
$RoleRoot=[IO.Path]::GetFullPath($RoleRoot).TrimEnd('\')
$workspace=Split-Path -Parent ([IO.Path]::GetFullPath($ProjectRoot))
if(!$RoleRoot.StartsWith($workspace+'\',[StringComparison]::OrdinalIgnoreCase)){throw 'Destination outside workspace'}
$package=Join-Path $ProjectRoot 'Deploy\ProjectContent\DailyQuests20261025'
if(!(Test-Path -LiteralPath (Join-Path $package 'manifest.json'))){return}
$m=Get-Content -LiteralPath (Join-Path $package 'manifest.json') -Raw -Encoding UTF8|ConvertFrom-Json
if($m.schema -ne 1 -or $m.quests -ne 10 -or $m.hopeCounter -ne 418){throw 'Unexpected daily quest manifest'}
if(@(Get-CimInstance Win32_Process|Where-Object {$_.ExecutablePath -and $_.ExecutablePath.StartsWith($RoleRoot+'\',[StringComparison]::OrdinalIgnoreCase)}).Count){throw 'Save and stop this role before publishing content'}
$plans=@()
foreach($e in $m.files){
 if($e.role -ne $role -or $e.relative -eq 'CoreServer.dll'){continue}
 $relative=[string]$e.relative
 if($relative.Contains('..') -or $relative -notmatch '^(script/|settings/phongthan/npc_regions/|Ui/ui[34]/taskinfo.ini$)'){throw 'Unsafe daily quest content path'}
 $source=Join-Path $package ('compiled/'+$role+'/'+$relative)
 if((Get-FileHash -LiteralPath $source -Algorithm SHA256).Hash -ine $e.sha256){throw 'Daily quest source hash changed'}
 $plans+=@{Source=$source;Target=Join-Path $RoleRoot $relative;Hash=$e.sha256}
}
foreach($p in $plans){
 New-Item -ItemType Directory -Force (Split-Path $p.Target)|Out-Null
 Copy-Item -LiteralPath $p.Source -Destination $p.Target -Force
 if((Get-FileHash -LiteralPath $p.Target -Algorithm SHA256).Hash -ine $p.Hash){throw 'Daily quest readback mismatch'}
}
[pscustomobject]@{Result='PASS';Role=$role;DailyQuestFiles=$plans.Count}
& (Join-Path $PSScriptRoot 'Publish-TranNguyenContent.ps1') -RoleRoot $RoleRoot -ProjectRoot $ProjectRoot
& (Join-Path $PSScriptRoot 'Publish-PhongAnThapContent.ps1') -RoleRoot $RoleRoot -ProjectRoot $ProjectRoot
& (Join-Path $PSScriptRoot 'Publish-OriginalQuestAuditContent.ps1') -RoleRoot $RoleRoot -ProjectRoot $ProjectRoot
& (Join-Path $PSScriptRoot 'Publish-QuestRefreshTravelStarterContent.ps1') -RoleRoot $RoleRoot -ProjectRoot $ProjectRoot
