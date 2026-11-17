$ErrorActionPreference = 'Stop'
$root = Split-Path -Parent $PSScriptRoot
$vcRoot = 'C:\Program Files (x86)\Microsoft Visual Studio\VC98'
$compiler = Join-Path $vcRoot 'Bin\cl.exe'
$env:PATH="$vcRoot\Bin;$(Split-Path $vcRoot)\Common\MSDev98\Bin;$env:PATH"
$env:INCLUDE="$vcRoot\Include;$env:INCLUDE"
$env:LIB="$vcRoot\Lib;$env:LIB"
$out = Join-Path $root 'Output\Tools'
New-Item -ItemType Directory -Force -Path $out | Out-Null
$exe = Join-Path $out 'EquipmentWorldPresentationTests.exe'

& $compiler /nologo /MT /W3 /O2 /DWIN32 `
    "/Fo$out\EquipmentWorldPresentationTests.obj" "/Fe$exe" `
    (Join-Path $PSScriptRoot 'Native\EquipmentWorldPresentationTests.cpp')
if ($LASTEXITCODE -ne 0) { throw "Equipment world presentation compile failed: $LASTEXITCODE" }
& $exe
if ($LASTEXITCODE -ne 0) { throw "Equipment world presentation tests failed: $LASTEXITCODE" }

$protocol = Get-Content (Join-Path $root 'Headers\PhongThanWorldProtocol.h') -Raw
$snapshot = Get-Content (Join-Path $root 'Sources\Core\Src\PhongThanPlayerSnapshot.inl') -Raw
$client = Get-Content (Join-Path $root 'Sources\Core\Src\KProtocolProcess.cpp') -Raw
$render = Get-Content (Join-Path $root 'Sources\Core\Src\KNpc.cpp') -Raw
if ($protocol -notmatch 'EquipmentEffects\[4\]') { throw 'Protocol does not carry bounded equipment effects.' }
if ($snapshot -notmatch 'PhongThanResolveWeaponWorldEffect') { throw 'Server snapshot does not derive the weapon world effect.' }
if ($snapshot -notmatch 'PhongThanSelectEquipmentGlow') { throw 'Server snapshot does not derive family/set world effects.' }
if ($client -notmatch 'm_nEquipmentEffects,\s*pPlaySync->EquipmentEffects') { throw 'Client does not receive the world effects.' }
if ($render -notmatch 'PhongThanMergeEquipmentWorldEffect') { throw 'Renderer does not compose the world effect.' }

$dataRoots = @(
    (Join-Path (Split-Path $root -Parent) 'PhongThanRuntime-Loose\Client'),
    (Join-Path (Split-Path $root -Parent) 'PhongThanRuntime-Staging\Client'),
    (Join-Path (Split-Path $root -Parent) 'PhongThanRuntime-State\Client')
)
$stateTable = $null
foreach ($dataRoot in $dataRoots) {
    $candidate = Join-Path $dataRoot 'settings\npcres\zhuang_tai_tu_xing_dui_zhao_biao.txt'
    if (Test-Path $candidate) { $stateTable = $candidate; break }
}
if (-not $stateTable) { throw 'Canonical loose state-graphic table is missing.' }
$raw = [IO.File]::ReadAllText($stateTable, [Text.Encoding]::GetEncoding(936))
foreach ($state in 39..42) {
    if ($raw -notmatch "(?m)^.+$state\t") { throw "Authoritative weapon effect state $state is missing." }
}
$clientRoot = Split-Path (Split-Path $stateTable -Parent) -Parent
$clientRoot = Split-Path $clientRoot -Parent
foreach ($effect in 1..4) {
    $asset = Join-Path $clientRoot "spr\skill\magic\weaponeffect$effect.spr"
    if (-not (Test-Path $asset)) { throw "Loose world-effect asset missing: $asset" }
}
Write-Host 'PASS: mutually-exclusive weapon variants, set effects, source wiring, and loose assets.'
