$ErrorActionPreference='Stop'
$source=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
$report=Join-Path (Split-Path -Parent $source) 'analysis/legacy_drop_20261017'
& node "$PSScriptRoot/phase2-investigate.mjs"
if($LASTEXITCODE){throw 'Archive investigation failed'}
foreach($deaths in 10000,100000,1000000){ & "$PSScriptRoot/Run.ps1" -Deaths $deaths }
& node "$PSScriptRoot/phase2-classify.mjs"
if($LASTEXITCODE){throw 'Classification failed'}
$manifestBefore=(Get-FileHash -LiteralPath "$report/phase2/quarantine.txt").Hash
& "$PSScriptRoot/Probe-Items.ps1" *> "$report/phase2/item_build.log"
& node "$PSScriptRoot/phase2-classify.mjs"
if($LASTEXITCODE){throw 'Post-probe classification failed'}
if((Get-FileHash -LiteralPath "$report/phase2/quarantine.txt").Hash -ne $manifestBefore){
    & "$PSScriptRoot/Probe-Items.ps1" *> "$report/phase2/item_build.log"
}
& "$PSScriptRoot/Probe-Items.ps1" -Role Client *> "$report/phase2/client_item_build.log"
& "$PSScriptRoot/Test-GenreRegression.ps1" *> "$report/phase2/genre_regression.log"
& (Join-Path $source 'Build/Build-PhongThan.ps1') -Targets @('CoreServer','CoreClient') -NoRebuild *> "$report/phase2/core_build.log"
& (Join-Path $source 'Tests/Test-PhongThanItemBaseline.ps1') -ProjectRoot $source *> "$report/phase2/appearance_baseline.log"
& "$PSScriptRoot/Test-CorePreflight.ps1" *> "$report/phase2/core_shared_test.log"
& node "$PSScriptRoot/phase2-report.mjs"
if($LASTEXITCODE){exit 1}
$acceptance=Get-Content -Raw (Join-Path $report 'phase2/acceptance.json') | ConvertFrom-Json
if(-not $acceptance.phase2FunctionalReady){Write-Output 'BLOCKED: no death hook or deployment; see phase2/PHASE2_RESOLUTION_REPORT.md';exit 2}
