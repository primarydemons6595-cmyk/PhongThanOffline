[CmdletBinding()]
param([Parameter(Mandatory=$true)][string]$RoleRoot,[string]$ProjectRoot='')
$ErrorActionPreference='Stop'
if(!$ProjectRoot){$ProjectRoot=Split-Path -Parent $PSScriptRoot}
$role=Split-Path -Leaf $RoleRoot;if($role -notin @('Server','Client')){return}
$RoleRoot=[IO.Path]::GetFullPath($RoleRoot).TrimEnd('\')
$package=Join-Path $ProjectRoot 'Deploy/ProjectContent/ThuongChuTgpt20261103'
$m=Get-Content -LiteralPath (Join-Path $package 'manifest.json') -Raw -Encoding UTF8|ConvertFrom-Json
if($m.feature-ne'TGPT_THUONG_CHU_20261103'){throw 'Unexpected battlefield package'}
if(@(Get-CimInstance Win32_Process|Where-Object {$_.ExecutablePath -and $_.ExecutablePath.StartsWith($RoleRoot+'\',[StringComparison]::OrdinalIgnoreCase)}).Count){throw 'Stop the target role before publishing content'}
$plans=@()
foreach($entry in $m.files){
 if($role-eq'Client'-and$entry.relative-ne'settings/buysell.txt'){continue}
 if($entry.relative.Contains('..')){throw 'Invalid package path'}
 $source=Join-Path $package ('compiled/Server/'+$entry.relative)
 if((Get-FileHash -LiteralPath $source).Hash-ine$entry.sha256){throw 'Package hash mismatch'}
 $plans+=@{Source=$source;Target=(Join-Path $RoleRoot $entry.relative);Hash=$entry.sha256}
}
foreach($plan in $plans){New-Item -ItemType Directory -Force -Path (Split-Path $plan.Target -Parent)|Out-Null;Copy-Item -LiteralPath $plan.Source -Destination $plan.Target -Force;if((Get-FileHash -LiteralPath $plan.Target).Hash-ine$plan.Hash){throw 'Content readback failed'}}
[pscustomobject]@{Result='PASS';Feature='TGPT_THUONG_CHU_20261103';Role=$role;Files=$plans.Count}
