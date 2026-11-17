$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
$vc='C:\Program Files (x86)\Microsoft Visual Studio\VC98'
$out=Join-Path $root 'Output\Tools\ConsumableTests'
New-Item -ItemType Directory -Force $out | Out-Null
$oldPath=$env:PATH;$oldLib=$env:LIB
Push-Location $out
try {
 $env:PATH="$vc\Bin;C:\Program Files (x86)\Microsoft Visual Studio\Common\MSDev98\Bin;$oldPath"
 $env:LIB="$vc\Lib;$oldLib"
 & "$vc\Bin\cl.exe" /nologo /W3 /GX /MT /O2 "/I$vc\Include" "/I$root\Headers" "/I$root\Sources\Engine\Src" "/Fo$out\test.obj" "/Fe$out\test.exe" (Join-Path $PSScriptRoot 'ConsumableContractTests.cpp') /link kernel32.lib
 if($LASTEXITCODE -ne 0){throw 'Consumable test harness compile failed'}
 & "$out\test.exe" "$root\Deploy\ProjectContent\settings\phongthan\ConsumableEffects.txt"
 if($LASTEXITCODE -ne 0){throw 'Consumable native contract test failed'}
 & "$vc\Bin\cl.exe" /nologo /W3 /GX /MT /O2 /DPT_TEST_CLIENT "/I$vc\Include" "/I$root\Headers" "/I$root\Sources\Engine\Src" "/Fo$out\client.obj" "/Fe$out\client.exe" (Join-Path $PSScriptRoot 'ConsumableContractTests.cpp') /link kernel32.lib
 if($LASTEXITCODE -ne 0){throw 'Consumable client harness compile failed'}
 & "$out\client.exe" "$root\Deploy\ProjectContent\settings\phongthan\ConsumableEffects.txt"
 if($LASTEXITCODE -ne 0){throw 'Consumable client contract failed'}
} finally {Pop-Location;$env:PATH=$oldPath;$env:LIB=$oldLib}
