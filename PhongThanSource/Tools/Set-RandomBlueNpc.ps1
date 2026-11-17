param([ValidateSet('On','Off','Rate')][string]$Action='Off',[ValidateRange(0,1000000)][int]$ChancePerMillion=10000)
$ErrorActionPreference='Stop'
$workspace=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
$server=Join-Path $workspace 'PhongThanRuntime-Loose/Server'
if(!(Test-Path (Join-Path $server 'native_drop_official_v1.required'))){throw 'Official Loose required'}
$path=Join-Path $server 'settings/phongthan/RandomBlueNpc.ini'
$text=[IO.File]::ReadAllText($path)
$key=if($Action -eq 'Rate'){'ChancePerMillion'}else{'Enabled'}
$value=if($Action -eq 'Rate'){$ChancePerMillion}elseif($Action -eq 'On'){1}else{0}
$pattern='(?m)^'+$key+'=\d+\r?$'
if([regex]::Matches($text,$pattern).Count -ne 1){throw 'Invalid random blue config'}
$next=[regex]::Replace($text,$pattern,($key+'='+$value))
$history=Join-Path $workspace 'analysis/random_blue_npc_20261018/config_history'
New-Item -ItemType Directory -Force $history|Out-Null
$backup=Join-Path $history ((Get-Date -Format 'yyyyMMdd_HHmmss_fff')+'.ini')
[IO.File]::WriteAllText(($path+'.control.tmp'),$next,[Text.Encoding]::ASCII)
[IO.File]::Replace(($path+'.control.tmp'),$path,$backup)
Write-Output "Random blue $key=$value. Applies to new spawn activations/respawns; existing lives keep their current state. Native Drop remains official."
