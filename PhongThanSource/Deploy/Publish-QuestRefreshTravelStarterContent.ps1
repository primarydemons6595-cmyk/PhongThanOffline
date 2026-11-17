[CmdletBinding()]
param([Parameter(Mandatory=$true)][string]$RoleRoot,[string]$ProjectRoot='')
$ErrorActionPreference='Stop'
if(!$ProjectRoot){$ProjectRoot=Split-Path -Parent $PSScriptRoot}
$role=Split-Path -Leaf $RoleRoot
if($role -notin @('Client','Server')){throw 'Expected runtime role'}
$RoleRoot=[IO.Path]::GetFullPath($RoleRoot).TrimEnd('\')
$workspace=Split-Path -Parent ([IO.Path]::GetFullPath($ProjectRoot))
if(!$RoleRoot.StartsWith($workspace+'\',[StringComparison]::OrdinalIgnoreCase)){throw 'Destination outside workspace'}
$package=Join-Path $ProjectRoot 'Deploy/ProjectContent/QuestRefreshTravelStarter20261025'
$m=Get-Content (Join-Path $package 'manifest.json') -Raw -Encoding UTF8|ConvertFrom-Json
if($m.schema -ne 1 -or $m.feature -ne 'QuestRefreshTravelStarter'){throw 'Unexpected manifest'}
if(@(Get-CimInstance Win32_Process|Where-Object {$_.ExecutablePath -and $_.ExecutablePath.StartsWith($RoleRoot+'\',[StringComparison]::OrdinalIgnoreCase)}).Count){throw 'Save and stop this runtime role before publishing'}
$plans=@()
foreach($e in $m.files){
 if($e.role -ne $role -or $e.relative -in @('CoreServer.dll','CoreClient.dll','Goddess.exe')){continue}
 if($e.relative.Contains('..') -or $e.relative -notmatch '^(script/.*\.lua|settings/item/001/ibitem\.txt)$'){throw 'Unsafe content path'}
 $source=Join-Path $package ('compiled/'+$role+'/'+$e.relative)
 if((Get-FileHash -LiteralPath $source -Algorithm SHA256).Hash -ine $e.sha256){throw 'Content source hash mismatch'}
 $plans+=@{Source=$source;Target=Join-Path $RoleRoot $e.relative;Hash=$e.sha256}
}
foreach($p in $plans){
 New-Item -ItemType Directory -Force (Split-Path $p.Target)|Out-Null
 Copy-Item -LiteralPath $p.Source -Destination $p.Target -Force
 if((Get-FileHash -LiteralPath $p.Target -Algorithm SHA256).Hash -ine $p.Hash){throw 'Content readback mismatch'}
}
[pscustomobject]@{Result='PASS';Role=$role;QuestRefreshTravelStarterFiles=$plans.Count}
