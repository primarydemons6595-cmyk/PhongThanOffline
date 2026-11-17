[CmdletBinding()]
param([Parameter(Mandatory=$true)][string]$RoleRoot,[string]$ProjectRoot='')
$ErrorActionPreference='Stop'
if(!$ProjectRoot){$ProjectRoot=Split-Path -Parent $PSScriptRoot}
$role=Split-Path -Leaf $RoleRoot
if($role -notin @('Client','Server')){throw 'Expected runtime role'}
if($role -eq 'Client'){return}
$RoleRoot=[IO.Path]::GetFullPath($RoleRoot).TrimEnd('\')
$workspace=Split-Path -Parent ([IO.Path]::GetFullPath($ProjectRoot))
if(!$RoleRoot.StartsWith($workspace+'\',[StringComparison]::OrdinalIgnoreCase)){throw 'Destination outside workspace'}
$package=Join-Path $ProjectRoot 'Deploy/ProjectContent/PhongAnThap20261025'
if(!(Test-Path (Join-Path $package 'manifest.json'))){return}
$m=Get-Content (Join-Path $package 'manifest.json') -Raw -Encoding UTF8|ConvertFrom-Json
if($m.schema -ne 1 -or $m.feature -ne 'PhongAnThap' -or $m.towers.Count -ne 6){throw 'Unexpected tower manifest'}
if(@(Get-CimInstance Win32_Process|Where-Object {$_.ExecutablePath -and $_.ExecutablePath.StartsWith($RoleRoot+'\',[StringComparison]::OrdinalIgnoreCase)}).Count){throw 'Save and stop this runtime role before publishing'}
foreach($e in $m.files){
 if($e.role -ne 'Server' -or $e.relative.Contains('..') -or $e.relative -notmatch '^(script/phongthan/.*\.lua|settings/phongthan/npc_regions/.*\.dat)$'){throw 'Unsafe tower file'}
 $source=Join-Path $package ('compiled/Server/'+$e.relative)
 if((Get-FileHash -LiteralPath $source -Algorithm SHA256).Hash -ine $e.sha256){throw 'Tower source hash mismatch'}
}
foreach($e in $m.files){
 $source=Join-Path $package ('compiled/Server/'+$e.relative);$target=Join-Path $RoleRoot $e.relative
 New-Item -ItemType Directory -Force (Split-Path $target)|Out-Null;Copy-Item -LiteralPath $source -Destination $target -Force
 if((Get-FileHash -LiteralPath $target -Algorithm SHA256).Hash -ine $e.sha256){throw 'Tower readback mismatch'}
}
[pscustomobject]@{Result='PASS';TowerFiles=$m.files.Count}
