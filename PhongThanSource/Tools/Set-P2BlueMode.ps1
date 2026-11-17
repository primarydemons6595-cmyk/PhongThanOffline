param([ValidateSet('BLUE_ON','BLUE_OFF','ALL_OFF')][string]$Mode='BLUE_OFF',[ValidateRange(0,1000000)][int]$ChancePerMillion=1000000)
$ErrorActionPreference='Stop'
$workspace=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
$report=Join-Path $workspace 'analysis/blue_magic_p2_20261018'
$iso=Get-Content -Raw (Join-Path $report 'deployment.json')|ConvertFrom-Json
if(!(Test-Path (Join-Path $iso.Root 'p2_player_test.flag'))){throw 'P2 isolation marker required'}
$path=Join-Path $iso.Server 'settings/phongthan/BlueMagicExperimental.ini'
$text=[IO.File]::ReadAllText($path)
$enabled=if($Mode -eq 'BLUE_ON'){1}else{0}
$text=[regex]::Replace($text,'(?m)^Enabled=\d+',('Enabled='+$enabled))
if($Mode -eq 'BLUE_ON'){$text=[regex]::Replace($text,'(?m)^BlueChancePerMillion=\d+',('BlueChancePerMillion='+$ChancePerMillion))}
[IO.File]::WriteAllText($path,$text,[Text.Encoding]::ASCII)
$legacy=Join-Path $iso.Server 'settings/phongthan/LegacyDropExperiment.ini'
if($Mode -eq 'ALL_OFF'){$s=[IO.File]::ReadAllText($legacy);$s=[regex]::Replace($s,'(?m)^Mode=\w+','Mode=OFF');[IO.File]::WriteAllText($legacy,$s,[Text.Encoding]::ASCII)}
if($Mode -eq 'BLUE_ON'){$s=[IO.File]::ReadAllText($legacy);$s=[regex]::Replace($s,'(?m)^Mode=\w+','Mode=LIVE_TEST');[IO.File]::WriteAllText($legacy,$s,[Text.Encoding]::ASCII)}
$receipt=@{Mode=$Mode;ChancePerMillion=$ChancePerMillion;Utc=[DateTime]::UtcNow.ToString('o');Scope='P2_ISOLATED_ONLY'}
$name='mode_'+(Get-Date -Format 'yyyyMMdd_HHmmss_fff')+'.json'
[IO.File]::WriteAllText((Join-Path $report $name),($receipt|ConvertTo-Json),[Text.UTF8Encoding]::new($false))
$receipt|ConvertTo-Json
