$ErrorActionPreference = 'Stop'
$root = Split-Path -Parent $PSScriptRoot
$vcRoot = 'C:\Program Files (x86)\Microsoft Visual Studio\VC98'
$compiler = Join-Path $vcRoot 'Bin\cl.exe'
$out = Join-Path $root 'Output\Tools'
New-Item -ItemType Directory -Force -Path $out | Out-Null
$exe = Join-Path $out 'EquipmentAppearanceR2Tests.exe'
& $compiler /nologo /MT /W3 /O2 /DWIN32 /DNDEBUG `
    "/Fo$out\EquipmentAppearanceR2Tests.obj" "/Fe$exe" `
    (Join-Path $PSScriptRoot 'Native\EquipmentAppearanceR2Tests.cpp')
if ($LASTEXITCODE -ne 0) { throw "Equipment appearance R2 compile failed: $LASTEXITCODE" }
& $exe
if ($LASTEXITCODE -ne 0) { throw "Equipment appearance R2 tests failed: $LASTEXITCODE" }

$client = Join-Path (Split-Path $root -Parent) 'PhongThanRuntime-Staging\Client'
$required = @('jsm01_bd_st0.spr','jsm03_bd_st0.spr','jsm04_bd_st0.spr','jsm06_bd_st0.spr')
foreach ($name in $required) {
    $path = Join-Path $client "spr\npcres\human\$name"
    if (-not (Test-Path -LiteralPath $path -PathType Leaf)) { throw "Missing canonical loose control resource: $name" }
}
$gameClient = [IO.File]::ReadAllText((Join-Path $root 'Sources\GameClient\PhongThanClient.cpp'), [Text.Encoding]::GetEncoding(28591))
if ($gameClient -notmatch 'g_SetPakFileMode\(0\)') { throw 'Development client is not loose-first' }
Write-Host 'PASS: canonical loose controls exist and client lookup is LOOSE -> PAK.'
