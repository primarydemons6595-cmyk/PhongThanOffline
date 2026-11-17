[CmdletBinding()]
param([Parameter(Mandatory=$true)][string]$RoleRoot,[string]$ProjectRoot='')
$ErrorActionPreference='Stop'
if(!$ProjectRoot){$ProjectRoot=Split-Path -Parent $PSScriptRoot}
$RoleRoot=[IO.Path]::GetFullPath($RoleRoot).TrimEnd('\')
$role=Split-Path -Leaf $RoleRoot
if($role -notin @('Client','Server')){throw 'Expected Client or Server role'}
$workspace=Split-Path -Parent ([IO.Path]::GetFullPath($ProjectRoot))
if(!$RoleRoot.StartsWith($workspace+'\',[StringComparison]::OrdinalIgnoreCase)){throw 'Destination outside workspace'}
if(@(Get-CimInstance Win32_Process | Where-Object {$_.ExecutablePath -and $_.ExecutablePath.StartsWith($RoleRoot+'\',[StringComparison]::OrdinalIgnoreCase)}).Count){throw 'Save and stop role before publishing'}
$package=Join-Path $ProjectRoot 'Deploy/ProjectContent/VttBoss20261029'
$manifest=Get-Content (Join-Path $package 'manifest.json') -Raw -Encoding UTF8 | ConvertFrom-Json
$plans=@()
foreach($entry in $manifest.files){
 if($entry.role -ne $role){continue}
 $relative=[string]$entry.relative
 if($relative.Contains('..') -or $relative -notmatch '^(settings/|script/phongthan/(van_tien|npc_recovered_batch)/)'){throw 'Unsafe content path'}
 $source=Join-Path $package ('compiled/'+$role+'/'+$relative)
 if((Get-FileHash -LiteralPath $source -Algorithm SHA256).Hash -ine $entry.sha256){throw 'VTT source hash mismatch'}
 $plans+=@{Source=$source;Target=Join-Path $RoleRoot $relative;Hash=$entry.sha256}
}
$backup=Join-Path $workspace ('analysis/vtt_boss_20261029/backups/'+[DateTime]::UtcNow.ToString('yyyyMMdd_HHmmss_ffff')+'/'+$role)
foreach($p in $plans){
 $relative=$p.Target.Substring($RoleRoot.Length+1)
 if(Test-Path -LiteralPath $p.Target){$saved=Join-Path $backup $relative;New-Item -ItemType Directory -Force (Split-Path $saved)|Out-Null;Copy-Item -LiteralPath $p.Target -Destination $saved}
 New-Item -ItemType Directory -Force (Split-Path $p.Target)|Out-Null
 Copy-Item -LiteralPath $p.Source -Destination $p.Target -Force
 if((Get-FileHash -LiteralPath $p.Target -Algorithm SHA256).Hash -ine $p.Hash){throw 'VTT readback mismatch'}
}
[pscustomobject]@{Result='PASS';Role=$role;Files=$plans.Count;Backup=$backup}
& (Join-Path $PSScriptRoot 'Publish-VttWeightLifeContent.ps1') -RoleRoot $RoleRoot -ProjectRoot $ProjectRoot
