[CmdletBinding()]
param([Parameter(Mandatory=$true)][string]$RoleRoot,[string]$ProjectRoot='')
$ErrorActionPreference='Stop'
if(!$ProjectRoot){$ProjectRoot=Split-Path -Parent $PSScriptRoot}
$workspace=Split-Path -Parent ([IO.Path]::GetFullPath($ProjectRoot))
$RoleRoot=[IO.Path]::GetFullPath($RoleRoot).TrimEnd('\');$role=Split-Path -Leaf $RoleRoot
if($role -notin @('Client','Server') -or $RoleRoot -ine (Join-Path (Join-Path $workspace 'PhongThanRuntime-Loose') $role)){throw 'Loose only'}
$package=Join-Path $ProjectRoot 'Deploy/ProjectContent/MasterAlias20261030'
$manifest=Get-Content (Join-Path $package 'manifest.json') -Raw -Encoding UTF8|ConvertFrom-Json
foreach($entry in $manifest.files){
 if($entry.role -ne $role){continue}
 if($entry.relative.Contains('..') -or $entry.relative -notmatch '^script/.*\.lua$'){throw 'Unsafe path'}
 $source=Join-Path $package ('compiled/'+$role+'/'+$entry.relative);$target=Join-Path $RoleRoot $entry.relative
 if((Get-FileHash -LiteralPath $source -Algorithm SHA256).Hash -ine $entry.sha256){throw 'Source mismatch'}
 Copy-Item -LiteralPath $source -Destination $target -Force
 if((Get-FileHash -LiteralPath $target -Algorithm SHA256).Hash -ine $entry.sha256){throw 'Readback mismatch'}
}
[pscustomobject]@{Result='PASS';Package='MasterAlias20261030'}
