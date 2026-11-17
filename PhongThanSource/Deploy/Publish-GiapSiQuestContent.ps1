[CmdletBinding()]
param([Parameter(Mandatory=$true)][string]$RoleRoot,
      [string]$ProjectRoot='')
$ErrorActionPreference='Stop'
if(!$ProjectRoot){$ProjectRoot=Split-Path -Parent $PSScriptRoot}
$role=Split-Path -Leaf $RoleRoot
if($role -notin @('Client','Server')){throw 'Expected Client or Server role root'}
$RoleRoot=[IO.Path]::GetFullPath($RoleRoot).TrimEnd('\')
$workspace=Split-Path -Parent ([IO.Path]::GetFullPath($ProjectRoot))
if(!$RoleRoot.StartsWith($workspace+'\',[StringComparison]::OrdinalIgnoreCase)){throw 'Destination outside workspace'}
$package=Join-Path $ProjectRoot 'Deploy\ProjectContent\GiapSiQuests'
if(!(Test-Path -LiteralPath (Join-Path $package 'manifest.json'))){return}
$manifest=Get-Content -LiteralPath (Join-Path $package 'manifest.json') -Raw -Encoding UTF8 | ConvertFrom-Json
if($manifest.schema -ne 1 -or $manifest.steps -ne 62){throw 'Unexpected Giap Si quest manifest'}
$running=@(Get-CimInstance Win32_Process | Where-Object {$_.ExecutablePath -and $_.ExecutablePath.StartsWith($RoleRoot+'\',[StringComparison]::OrdinalIgnoreCase)})
if($running){throw 'Save and stop this runtime role before publishing quests'}
$plans=@()
foreach($entry in $manifest.files){
 $relative=[string]$entry.relative
 if($relative.StartsWith('client/')){if($role -ne 'Client'){continue};$relative=$relative.Substring(7)}
 elseif($role -eq 'Client' -and $relative -ne 'settings/item/001/magicscript.txt'){continue}
 if($relative -notmatch '^(script/|settings/phongthan/|settings/item/(001/(magicscript|horse)|VNG_HorsePart|PhongThanHorseVisual)\.txt$|Ui/ui[34]/[A-Za-z0-9_]+\.ini$)' -or $relative.Contains('..')){throw 'Unsafe quest asset path'}
 $source=Join-Path $package ('compiled\'+$entry.relative)
 if((Get-FileHash -LiteralPath $source -Algorithm SHA256).Hash -ine $entry.sha256){throw "Quest package changed: $source"}
 $plans+=@{Source=$source;Target=(Join-Path $RoleRoot $relative);Hash=$entry.sha256}
}
foreach($plan in $plans){
 New-Item -ItemType Directory -Force -Path (Split-Path -Parent $plan.Target) | Out-Null
 Copy-Item -LiteralPath $plan.Source -Destination $plan.Target -Force
 if((Get-FileHash -LiteralPath $plan.Target -Algorithm SHA256).Hash -ine $plan.Hash){throw 'Quest publish readback mismatch'}
}
[pscustomobject]@{Result='PASS';Role=$role;Files=$plans.Count;Steps=$manifest.steps;OriginalPakModified=$false}
& (Join-Path $PSScriptRoot 'Publish-SungVttContent.ps1') -RoleRoot $RoleRoot -ProjectRoot $ProjectRoot
& (Join-Path $PSScriptRoot 'Publish-GiapSiFuturePortalContent.ps1') -RoleRoot $RoleRoot -ProjectRoot $ProjectRoot
& (Join-Path $PSScriptRoot 'Publish-ThuongChuTgptContent.ps1') -RoleRoot $RoleRoot -ProjectRoot $ProjectRoot
