[CmdletBinding()]
param([Parameter(Mandatory=$true)][string]$RoleRoot,[string]$ProjectRoot='')
$ErrorActionPreference='Stop'
if(!$ProjectRoot){$ProjectRoot=Split-Path -Parent $PSScriptRoot}
$role=Split-Path -Leaf $RoleRoot
if($role -notin @('Client','Server')){throw 'Expected Client or Server role root'}
$RoleRoot=[IO.Path]::GetFullPath($RoleRoot).TrimEnd('\')
$workspace=Split-Path -Parent ([IO.Path]::GetFullPath($ProjectRoot))
if(!$RoleRoot.StartsWith($workspace+'\',[StringComparison]::OrdinalIgnoreCase)){throw 'Destination outside workspace'}
$package=Join-Path $ProjectRoot 'Deploy\ProjectContent\SungVtt20261024'
if(!(Test-Path -LiteralPath (Join-Path $package 'manifest.json'))){return}
$manifest=Get-Content -LiteralPath (Join-Path $package 'manifest.json') -Raw -Encoding UTF8 | ConvertFrom-Json
if($manifest.schema -ne 1 -or $manifest.npcs.Count -ne 7){throw 'Unexpected Sung/VTT manifest'}
if(@(Get-CimInstance Win32_Process | Where-Object {$_.ExecutablePath -and $_.ExecutablePath.StartsWith($RoleRoot+'\',[StringComparison]::OrdinalIgnoreCase)}).Count){throw 'Save and stop this runtime role before publishing content'}
$plans=@()
foreach($entry in $manifest.files){
 if($entry.role -ne $role){continue}
 $relative=[string]$entry.relative
 if(($role+'/'+$relative) -notin $manifest.persistentOverrides){continue}
 # Native binaries always come from the current build publisher, never a frozen content package.
 if($relative -in @('Game.exe','CoreClient.dll','CoreServer.dll')){continue}
 if($relative.Contains('..') -or $relative -notmatch '^(script/|settings/|Ui/ui[34]/)'){throw 'Unsafe Sung/VTT content path'}
 $source=Join-Path $package ('compiled\'+$role+'\'+$relative)
 if((Get-FileHash -LiteralPath $source -Algorithm SHA256).Hash -ine $entry.sha256){throw 'Sung/VTT source changed'}
 $plans+=@{Source=$source;Target=(Join-Path $RoleRoot $relative);Hash=$entry.sha256}
}
foreach($plan in $plans){
 New-Item -ItemType Directory -Force -Path (Split-Path -Parent $plan.Target) | Out-Null
 Copy-Item -LiteralPath $plan.Source -Destination $plan.Target -Force
 if((Get-FileHash -LiteralPath $plan.Target -Algorithm SHA256).Hash -ine $plan.Hash){throw 'Sung/VTT publish readback mismatch'}
}
[pscustomobject]@{Result='PASS';Role=$role;Files=$plans.Count;OriginalPakModified=$false}
& (Join-Path $PSScriptRoot 'Publish-VttCollisionContent.ps1') -RoleRoot $RoleRoot -ProjectRoot $ProjectRoot
