[CmdletBinding()]
param([Parameter(Mandatory=$true)][ValidateSet('OFF','SHADOW','LIVE_TEST')][string]$Mode,[switch]$HitDetails)
$ErrorActionPreference='Stop'
$source=Split-Path -Parent $PSScriptRoot
$workspace=Split-Path -Parent $source
$server=Join-Path $workspace 'PhongThanRuntime-Loose/Server'
$config=Join-Path $server 'settings/phongthan/LegacyDropExperiment.ini'
if(!(Test-Path -LiteralPath (Join-Path $server 'loose_only.flag'))){throw 'Loose marker required'}
if($Mode -ne 'OFF'){
 $game=@(Get-Process -Name GameServer -ErrorAction SilentlyContinue | Where-Object {$_.Path -ieq (Join-Path $server 'GameServer.exe')})
 if($game.Count -ne 1){throw 'One running Loose GameServer required'}
 $deployment=Get-Content -Raw -LiteralPath (Join-Path $workspace 'analysis/legacy_drop_20261018_live/deployment.json') | ConvertFrom-Json
 if($deployment.ExperimentalHooksPresent -eq $false){throw 'Rollback binary has no experimental hooks'}
 if((Get-FileHash -LiteralPath (Join-Path $server 'CoreServer.dll')).Hash -ne $deployment.Sha256){throw 'Experimental binary differs from deployment receipt'}
 $ready=Join-Path $server 'native_world_ready.log'
 if(!(Test-Path -LiteralPath $ready) -or (Get-Item -LiteralPath $ready).LastWriteTimeUtc -lt $game[0].StartTime.ToUniversalTime()){throw 'Wait for fresh Loose WORLD_READY'}
}
$text=[IO.File]::ReadAllText($config,[Text.Encoding]::ASCII)
if([regex]::Matches($text,'(?m)^Mode=').Count -ne 1){throw 'Expected exactly one Mode field'}
$text=[regex]::Replace($text,'(?m)^Mode=[^\r\n]*',"Mode=$Mode")
$text=[regex]::Replace($text,'(?m)^SelfTestEnabled=[^\r\n]*','SelfTestEnabled=0')
$text=[regex]::Replace($text,'(?m)^SelfTestEquipment18=[^\r\n]*','SelfTestEquipment18=0')
$text=[regex]::Replace($text,'(?m)^SelfTestRepetitions=[^\r\n]*','SelfTestRepetitions=1')
$text=[regex]::Replace($text,'(?m)^SelfTestCustomScript=[^\r\n]*','SelfTestCustomScript=0')
$text=[regex]::Replace($text,'(?m)^SelfTestGeneratorFailAfter=[^\r\n]*','SelfTestGeneratorFailAfter=0')
$text=[regex]::Replace($text,'(?m)^HitDetails=[^\r\n]*',('HitDetails='+[int]$HitDetails.IsPresent))
$temporary=$config+'.mode.tmp'
[IO.File]::WriteAllText($temporary,$text,[Text.Encoding]::ASCII)
[IO.File]::Replace($temporary,$config,$config+'.previous')
Write-Output "LegacyDrop Mode=$Mode; EXPERIMENTAL; KINGSOFT_PARITY_UNVERIFIED; NPC allowlist remains in $config"
