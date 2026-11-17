[CmdletBinding()]
param([Parameter(Mandatory=$true)][string]$RoleRoot,[string]$ProjectRoot='')
$ErrorActionPreference='Stop'
if(!$ProjectRoot){$ProjectRoot=Split-Path -Parent $PSScriptRoot}
$role=Split-Path -Leaf $RoleRoot
if($role -notin @('Client','Server')){throw 'Expected runtime role'}
$RoleRoot=[IO.Path]::GetFullPath($RoleRoot).TrimEnd('\')
$workspace=Split-Path -Parent ([IO.Path]::GetFullPath($ProjectRoot))
if(!$RoleRoot.StartsWith($workspace+'\',[StringComparison]::OrdinalIgnoreCase)){throw 'Destination outside workspace'}
$package=Join-Path $ProjectRoot 'Deploy\ProjectContent\VttCollision20261024'
if(!(Test-Path -LiteralPath (Join-Path $package 'manifest.json'))){return}
$manifest=Get-Content -LiteralPath (Join-Path $package 'manifest.json') -Raw -Encoding UTF8 | ConvertFrom-Json
if($manifest.schema -ne 1 -or $manifest.maps.Count -ne 4){throw 'Unexpected VTT collision manifest'}
if(@(Get-CimInstance Win32_Process | Where-Object {$_.ExecutablePath -and $_.ExecutablePath.StartsWith($RoleRoot+'\',[StringComparison]::OrdinalIgnoreCase)}).Count){throw 'Save and stop this role before publishing content'}
$plans=@()
foreach($entry in $manifest.files){
 if($entry.role -ne $role -or $entry.relative -in @('CoreServer.dll','CoreClient.dll')){continue}
 $relative=[string]$entry.relative
 if($relative.Contains('..') -or $relative -notmatch '^(script/phongthan/van_tien/|settings/|spr/phongthan/vtt/)'){throw 'Unsafe VTT content path'}
 $source=Join-Path $package ('compiled\'+$role+'\'+$relative)
 if((Get-FileHash -LiteralPath $source -Algorithm SHA256).Hash -ine $entry.sha256){throw 'VTT source hash changed'}
 $plans+=@{Source=$source;Target=(Join-Path $RoleRoot $relative);Hash=$entry.sha256}
}
foreach($plan in $plans){
 New-Item -ItemType Directory -Force -Path (Split-Path -Parent $plan.Target) | Out-Null
 Copy-Item -LiteralPath $plan.Source -Destination $plan.Target -Force
 if((Get-FileHash -LiteralPath $plan.Target -Algorithm SHA256).Hash -ine $plan.Hash){throw 'VTT readback mismatch'}
}
[pscustomobject]@{Result='PASS';Role=$role;VttCollisionFiles=$plans.Count}
& (Join-Path $PSScriptRoot 'Publish-DailyQuestsContent.ps1') -RoleRoot $RoleRoot -ProjectRoot $ProjectRoot
& (Join-Path $PSScriptRoot 'Publish-VttBossContent.ps1') -RoleRoot $RoleRoot -ProjectRoot $ProjectRoot
