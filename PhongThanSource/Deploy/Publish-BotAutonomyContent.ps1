[CmdletBinding()]
param([Parameter(Mandatory=$true)][string]$RoleRoot,[string]$ProjectRoot=(Split-Path -Parent $PSScriptRoot))
$ErrorActionPreference='Stop'
$ProjectRoot=[IO.Path]::GetFullPath($ProjectRoot).TrimEnd('\');$workspace=Split-Path -Parent $ProjectRoot
$RoleRoot=[IO.Path]::GetFullPath($RoleRoot).TrimEnd('\');$role=Split-Path -Leaf $RoleRoot
if($role -notin @('Client','Server') -or $RoleRoot -ine (Join-Path $workspace "PhongThanRuntime-Loose\$role")){throw 'Only the named Loose role is supported'}
$consumer=if($role -eq 'Client'){'Game.exe'}else{'GameServer.exe'}
if(Get-CimInstance Win32_Process|Where-Object {$_.Name -eq $consumer -and $_.ExecutablePath -and $_.ExecutablePath.StartsWith($RoleRoot+'\',[StringComparison]::OrdinalIgnoreCase)}){throw 'Stop the target game/world before publishing'}
$base=Join-Path $ProjectRoot 'Deploy/ProjectContent/BotAutonomy20261109'
$manifest=Get-Content -LiteralPath (Join-Path $base 'manifest.json') -Raw -Encoding UTF8|ConvertFrom-Json
if($manifest.profile -ne 'LooseOnly'){throw 'Invalid content profile'}
foreach($entry in $manifest.files){
 if(!$entry.path.StartsWith($role+'/')){continue}
 if($entry.path -notmatch '^(Client/ui/realm_skills\.ini|Server/settings/item/droprate/(jingren|huangou|dazhongren|caoxian|xueyao|yanhuo)(-blue)?\.ini)$'){throw 'Unexpected content path'}
 $source=Join-Path $base ('compiled/'+$entry.path);$target=Join-Path $RoleRoot $entry.path.Substring($role.Length+1)
 if((Get-FileHash -LiteralPath $source).Hash -ine $entry.sha256){throw 'Source content hash mismatch'}
 if(Test-Path -LiteralPath $target){$current=(Get-FileHash -LiteralPath $target).Hash;if($current -ine $entry.before_sha256 -and $current -ine $entry.sha256){continue}}
 New-Item -ItemType Directory -Path (Split-Path -Parent $target) -Force|Out-Null
 Copy-Item -LiteralPath $source -Destination $target -Force
 if((Get-FileHash -LiteralPath $target).Hash -ine $entry.sha256){throw 'Content readback mismatch'}
}
