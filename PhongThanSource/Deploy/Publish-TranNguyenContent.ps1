[CmdletBinding()]
param([Parameter(Mandatory=$true)][string]$RoleRoot,[string]$ProjectRoot='')
$ErrorActionPreference='Stop'
if(!$ProjectRoot){$ProjectRoot=Split-Path -Parent $PSScriptRoot}
$role=Split-Path -Leaf $RoleRoot
if($role -notin @('Client','Server')){throw 'Expected runtime role'}
$RoleRoot=[IO.Path]::GetFullPath($RoleRoot).TrimEnd('\')
$workspace=Split-Path -Parent ([IO.Path]::GetFullPath($ProjectRoot))
if(!$RoleRoot.StartsWith($workspace+'\',[StringComparison]::OrdinalIgnoreCase)){throw 'Destination outside workspace'}
$package=Join-Path $ProjectRoot 'Deploy\ProjectContent\TranNguyen20261025'
if(!(Test-Path -LiteralPath (Join-Path $package 'manifest.json'))){return}
$m=Get-Content -LiteralPath (Join-Path $package 'manifest.json') -Raw -Encoding UTF8|ConvertFrom-Json
if($m.schema -ne 1 -or $m.npc -ne 'TranNguyen' -or $m.recipes -ne 135){throw 'Unexpected Tran Nguyen manifest'}
if(@(Get-CimInstance Win32_Process|Where-Object {$_.ExecutablePath -and $_.ExecutablePath.StartsWith($RoleRoot+'\',[StringComparison]::OrdinalIgnoreCase)}).Count){throw 'Save and stop this role before publishing content'}
$plans=@()
foreach($e in $m.files){
 if($e.role -ne $role -or $e.relative -in @('CoreServer.dll','CoreClient.dll','Game.exe')){continue}
 $relative=[string]$e.relative
 if($relative.Contains('..') -or $relative -notmatch '^(script/|Ui/ui[34]/taskinfo.ini$|settings/item/001/magicscript.txt$)'){throw 'Unsafe Tran Nguyen content path'}
 $source=Join-Path $package ('compiled/'+$role+'/'+$relative)
 if((Get-FileHash -LiteralPath $source -Algorithm SHA256).Hash -ine $e.sha256){throw 'Tran Nguyen source hash changed'}
 $plans+=@{Source=$source;Target=Join-Path $RoleRoot $relative;Hash=$e.sha256}
}
foreach($p in $plans){New-Item -ItemType Directory -Force (Split-Path $p.Target)|Out-Null;Copy-Item -LiteralPath $p.Source -Destination $p.Target -Force;if((Get-FileHash -LiteralPath $p.Target -Algorithm SHA256).Hash -ine $p.Hash){throw 'Tran Nguyen readback mismatch'}}
[pscustomobject]@{Result='PASS';Role=$role;TranNguyenFiles=$plans.Count}
