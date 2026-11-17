$ErrorActionPreference = 'Stop'
$root = Split-Path -Parent $PSScriptRoot
$out = Join-Path $root 'Output/Tools'
New-Item -ItemType Directory -Force -Path $out | Out-Null
$exe = Join-Path $out 'EquipmentHueTests.exe'
& 'C:/Program Files (x86)/Microsoft Visual Studio/VC98/Bin/cl.exe' /nologo /MT /W3 /O2 /DWIN32 /DNDEBUG "/Fo$out/EquipmentHueTests.obj" "/Fe$exe" (Join-Path $PSScriptRoot 'Native/EquipmentHueTests.cpp')
if ($LASTEXITCODE -ne 0) { throw 'Hue test compile failed' }
& $exe
if ($LASTEXITCODE -ne 0) { throw "Hue tests failed: $LASTEXITCODE" }
