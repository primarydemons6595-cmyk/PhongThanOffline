$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
$workspace=Split-Path -Parent $root
$out=Join-Path $root 'Output\Tools'
& python (Join-Path $PSScriptRoot 'prepare-summon-combat.py')
if($LASTEXITCODE){throw 'Probe preparation failed'}
$compiler='C:\Program Files (x86)\Microsoft Visual Studio\VC98\Bin\cl.exe'
& $compiler /nologo /MT /W3 /O2 /DWIN32 "/I$out" "/I$root\Sources\Engine\Src" "/I$root\Sources\Engine\Include" "/Fo$out\SummonCombatStatsTests.obj" "/Fe$out\SummonCombatStatsTests.exe" (Join-Path $PSScriptRoot 'Native\SummonCombatStatsTests.cpp') "$root\Lib\release\engine.lib" "$root\Lib\LuaLibDll.lib" /link kernel32.lib user32.lib
if($LASTEXITCODE){throw 'Summon stats probe compilation failed'}
Push-Location (Join-Path $root 'Output\Server')
try {
    & (Join-Path $out 'SummonCombatStatsTests.exe') (Join-Path $workspace 'PhongThanRuntime-Loose\Server')
    if($LASTEXITCODE){throw 'Summon stats probe failed'}
} finally {Pop-Location}
