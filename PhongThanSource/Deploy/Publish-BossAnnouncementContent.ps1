[CmdletBinding()]
param([Parameter(Mandatory=$true)][string]$RoleRoot,[string]$ProjectRoot='')
$ErrorActionPreference='Stop'
if(!$ProjectRoot){$ProjectRoot=Split-Path -Parent $PSScriptRoot}
$workspace=Split-Path -Parent ([IO.Path]::GetFullPath($ProjectRoot))
$RoleRoot=[IO.Path]::GetFullPath($RoleRoot).TrimEnd('\');$role=Split-Path -Leaf $RoleRoot
if($role -eq 'Client'){return}
if($RoleRoot -ine (Join-Path $workspace 'PhongThanRuntime-Loose/Server')){throw 'This package targets Loose Server only'}
if(@(Get-CimInstance Win32_Process | Where-Object {$_.ExecutablePath -and $_.ExecutablePath.StartsWith($RoleRoot+'\',[StringComparison]::OrdinalIgnoreCase)}).Count){throw 'Save and close role first'}
$package=Join-Path $ProjectRoot 'Deploy/ProjectContent/BossAnnouncement20261029'
$manifest=Get-Content (Join-Path $package 'manifest.json') -Raw -Encoding UTF8|ConvertFrom-Json
foreach($entry in $manifest.files){
 if($entry.relative -notin @('EventService/catalog.json','EventService/catalog.tsv','script/servertimer.lua')){throw 'Unsafe content path'}
 $source=Join-Path $package ('compiled/Server/'+$entry.relative);$target=Join-Path $RoleRoot $entry.relative
 if((Get-FileHash -LiteralPath $source -Algorithm SHA256).Hash -ine $entry.sha256){throw 'Source mismatch'}
 Copy-Item -LiteralPath $source -Destination $target -Force
 if((Get-FileHash -LiteralPath $target -Algorithm SHA256).Hash -ine $entry.sha256){throw 'Readback mismatch'}
}
[pscustomobject]@{Result='PASS';Package='BossAnnouncement20261029'}
& (Join-Path $PSScriptRoot 'Publish-StarterStoryContent.ps1') -RoleRoot $RoleRoot -ProjectRoot $ProjectRoot
