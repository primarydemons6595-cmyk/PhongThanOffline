[CmdletBinding()]
param([string]$ReportRoot)
$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
$workspace=Split-Path -Parent $root
$out=Join-Path $root 'Output\Tools'
if(!$ReportRoot){$ReportRoot=Join-Path $workspace 'analysis\faction_smoke_20261030'}
$ReportRoot=[IO.Path]::GetFullPath($ReportRoot)
New-Item -ItemType Directory -Force $ReportRoot | Out-Null
& (Join-Path $PSScriptRoot 'Test-PhongThanSummon.ps1') | Tee-Object (Join-Path $ReportRoot 'summon-lifecycle.log')
if(!$?){throw 'Summon lifecycle test failed'}
& (Join-Path $PSScriptRoot 'Test-SummonCombatStats.ps1') | Tee-Object (Join-Path $ReportRoot 'summon-stats.log')
if(!$?){throw 'Summon stats test failed'}
$fixturePath=Join-Path $ReportRoot 'runtime-summon-fixture.json'
Push-Location (Join-Path $root 'Output\Server')
try {
    & (Join-Path $out 'SummonCombatStatsTests.exe') (Join-Path $workspace 'PhongThanRuntime-Loose\Server') $fixturePath
    if($LASTEXITCODE){throw 'Runtime fixture extraction failed'}
}finally{Pop-Location}
$fixture=Get-Content -Raw $fixturePath | ConvertFrom-Json
$compiler='C:\Program Files (x86)\Microsoft Visual Studio\VC98\Bin\cl.exe'
foreach($name in 'SummonPvpPolicyProbe','ThreeFactionOneSummonSmoke','FactionEquipmentFixTests','SummonClientRankTests','SummonHudButtonTests','SummonHudHitTests') {
    & $compiler /nologo /MT /W3 /O2 "/I$out" "/Fo$out\$name.obj" "/Fe$out\$name.exe" (Join-Path $PSScriptRoot "Native\$name.cpp")
    if($LASTEXITCODE){throw "Compile failed: $name"}
}
& (Join-Path $out 'FactionEquipmentFixTests.exe') | Tee-Object (Join-Path $ReportRoot 'faction-matrix.log')
if($LASTEXITCODE){throw 'Faction matrix test failed'}
& (Join-Path $out 'SummonClientRankTests.exe') | Tee-Object (Join-Path $ReportRoot 'client-rank.log')
if($LASTEXITCODE){throw 'Client rank test failed'}
& (Join-Path $out 'SummonHudButtonTests.exe') | Tee-Object (Join-Path $ReportRoot 'hud-button.log')
if($LASTEXITCODE){throw 'HUD button test failed'}
& (Join-Path $out 'SummonHudHitTests.exe') | Tee-Object (Join-Path $ReportRoot 'hud-hit.log')
if($LASTEXITCODE){throw 'HUD hit routing test failed'}
& (Join-Path $out 'SummonPvpPolicyProbe.exe') $fixture.PvpPercent | Tee-Object (Join-Path $ReportRoot 'production-pvp-policy.log')
if($LASTEXITCODE){throw 'Production PvP policy probe failed'}
& (Join-Path $out 'ThreeFactionOneSummonSmoke.exe') $fixture.HP $fixture.AR $fixture.Damage $fixture.Range $fixture.Cost $fixture.RecastBeats $fixture.PvpPercent $fixture.AttackFrames |
    Tee-Object (Join-Path $ReportRoot 'one-summon-simulation.log')
if($LASTEXITCODE){throw 'One-summon simulation accounting failed'}
& node (Join-Path $PSScriptRoot 'prepare-three-faction-probes.mjs')
if($LASTEXITCODE){throw 'Production collision preparation failed'}
$probeOut=Join-Path $out 'ThreeFactionSmoke'
& $compiler /nologo /MT /W3 /O2 "/I$probeOut" "/Fo$probeOut\ThreeFactionProductionProbes.obj" "/Fe$probeOut\ThreeFactionProductionProbes.exe" (Join-Path $PSScriptRoot 'Native\ThreeFactionProductionProbes.cpp')
if($LASTEXITCODE){throw 'Production collision compilation failed'}
& (Join-Path $probeOut 'ThreeFactionProductionProbes.exe') | Tee-Object (Join-Path $ReportRoot 'production-collision.log')
if($LASTEXITCODE){throw 'Production collision test failed'}
Write-Output "REPORTS=$ReportRoot"
Write-Output 'MECHANICS_PASS; balance assessment contains known gaps, not a live PvP balance certificate.'
