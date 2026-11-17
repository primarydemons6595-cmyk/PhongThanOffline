[CmdletBinding()]
param([Parameter(Mandatory=$true)][string]$RoleRoot,[string]$ProjectRoot='')
$ErrorActionPreference='Stop'
if(!$ProjectRoot){$ProjectRoot=Split-Path -Parent $PSScriptRoot}
$workspace=Split-Path -Parent ([IO.Path]::GetFullPath($ProjectRoot))
$RoleRoot=[IO.Path]::GetFullPath($RoleRoot).TrimEnd('\');$role=Split-Path -Leaf $RoleRoot
if($role -notin @('Client','Server') -or !$RoleRoot.StartsWith($workspace+'\',[StringComparison]::OrdinalIgnoreCase)){throw 'Expected workspace Client/Server role'}
if(@(Get-CimInstance Win32_Process | Where-Object {$_.ExecutablePath -and $_.ExecutablePath.StartsWith($RoleRoot+'\',[StringComparison]::OrdinalIgnoreCase)}).Count){throw 'Save and close role first'}
$package=Join-Path $ProjectRoot 'Deploy/ProjectContent/VttWeightLife20261029'
$manifest=Get-Content (Join-Path $package 'manifest.json') -Raw -Encoding UTF8|ConvertFrom-Json
foreach($entry in $manifest.files){
 if($entry.role -ne $role -or $entry.relative -in @('CoreServer.dll','CoreClient.dll','Game.exe','Engine.dll')){continue}
 $relative=[string]$entry.relative
 if($relative.Contains('..') -or $relative -notmatch '^(script/|settings/|spr/phongthan/vtt/|Ui/ui[34]/taskinfo\.ini$)'){throw 'Unsafe content path'}
 $source=Join-Path $package ('compiled/'+$role+'/'+$relative);$target=Join-Path $RoleRoot $relative
 if((Get-FileHash -LiteralPath $source -Algorithm SHA256).Hash -ine $entry.sha256){throw 'Source mismatch'}
 New-Item -ItemType Directory -Force (Split-Path $target)|Out-Null;Copy-Item -LiteralPath $source -Destination $target -Force
 if((Get-FileHash -LiteralPath $target -Algorithm SHA256).Hash -ine $entry.sha256){throw 'Readback mismatch'}
}
[pscustomobject]@{Result='PASS';Role=$role;Package='VttWeightLife20261029'}
if($RoleRoot -ieq (Join-Path (Join-Path $workspace 'PhongThanRuntime-Loose') $role)){
 & (Join-Path $PSScriptRoot 'Publish-ThreeFactionQuestContent.ps1') -RoleRoot $RoleRoot -ProjectRoot $ProjectRoot
 & (Join-Path $PSScriptRoot 'Publish-LevelGiftContent.ps1') -RoleRoot $RoleRoot -ProjectRoot $ProjectRoot
}
