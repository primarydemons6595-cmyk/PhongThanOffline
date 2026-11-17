[CmdletBinding()]
param([Parameter(Mandatory=$true)][string]$RoleRoot,[string]$ProjectRoot='')
$ErrorActionPreference='Stop'
if(!$ProjectRoot){$ProjectRoot=Split-Path -Parent $PSScriptRoot}
if((Split-Path -Leaf $RoleRoot) -ne 'Server'){return}
$RoleRoot=[IO.Path]::GetFullPath($RoleRoot).TrimEnd('\')
$workspace=Split-Path -Parent ([IO.Path]::GetFullPath($ProjectRoot))
if(!$RoleRoot.StartsWith($workspace+'\',[StringComparison]::OrdinalIgnoreCase)){throw 'Destination outside workspace'}
if(@(Get-CimInstance Win32_Process | Where-Object {$_.ExecutablePath -and $_.ExecutablePath.StartsWith($RoleRoot+'\',[StringComparison]::OrdinalIgnoreCase)}).Count){throw 'Save and stop the target role before publishing quest content'}
$package=Join-Path $ProjectRoot 'Deploy/ProjectContent/GiapSiFuturePortal20261102'
$manifest=Get-Content -LiteralPath (Join-Path $package 'manifest.json') -Raw -Encoding UTF8 | ConvertFrom-Json
if($manifest.schema -ne 1 -or $manifest.classification -ne 'GUARDED_GIAPSI_CO_KIM_REENTRY' -or $manifest.files.Count -ne 2){throw 'Unexpected future portal package'}
$plans=@()
foreach($entry in $manifest.files){
 if($entry.relative -notin @('script/phongthan/giapsi/npc_tay.lua','script/phongthan/giapsi/npc_dac_future.lua')){throw 'Unexpected portal asset'}
 $source=Join-Path $package ('compiled/Server/'+$entry.relative)
 if((Get-FileHash -LiteralPath $source).Hash -ine $entry.sha256){throw 'Portal source hash mismatch'}
 $plans+=@{Source=$source;Target=(Join-Path $RoleRoot $entry.relative);Hash=$entry.sha256}
}
foreach($plan in $plans){
 New-Item -ItemType Directory -Force -Path (Split-Path -Parent $plan.Target) | Out-Null
 Copy-Item -LiteralPath $plan.Source -Destination $plan.Target -Force
 if((Get-FileHash -LiteralPath $plan.Target).Hash -ine $plan.Hash){throw 'Portal readback mismatch'}
}
[pscustomobject]@{Result='PASS';Role='Server';Files=$plans.Count;QuestProgressChanged=$false}
