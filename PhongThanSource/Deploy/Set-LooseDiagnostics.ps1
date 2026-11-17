[CmdletBinding()]
param([ValidateSet('Enable','Disable')][string]$Mode='Disable')
$ErrorActionPreference='Stop'
$workspace=Split-Path (Split-Path $PSScriptRoot -Parent) -Parent
$enabled=if($Mode -eq 'Enable'){1}else{0}
foreach($role in 'Server','Client'){
 $root=Join-Path $workspace "PhongThanRuntime-Loose/$role"
 if(!(Test-Path (Join-Path $root 'loose_only.flag'))){throw 'Missing Loose marker'}
 $config=Join-Path $root 'settings/phongthan/Diagnostics.ini'
 New-Item -ItemType Directory -Force (Split-Path $config)|Out-Null
 [IO.File]::WriteAllText($config,"[Diagnostics]`r`nEnabled=$enabled`r`nMaxFileBytes=1048576`r`n",[Text.Encoding]::ASCII)
}
Write-Output "Loose diagnostics: $Mode. Restart affected game/services/Studio to apply."
