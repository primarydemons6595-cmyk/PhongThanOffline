param([string]$ProjectRoot=(Split-Path -Parent $PSScriptRoot))
$ErrorActionPreference='Stop'
$workspace=Split-Path -Parent $ProjectRoot
$vcRoot='C:\Program Files (x86)\Microsoft Visual Studio\VC98'
$oldPath=$env:PATH;$oldInclude=$env:INCLUDE;$oldLib=$env:LIB
Push-Location $workspace
try {
    $env:PATH="$vcRoot\Bin;C:\Program Files (x86)\Microsoft Visual Studio\Common\MSDev98\Bin;$ProjectRoot\Output\Tools;$env:PATH"
    $env:INCLUDE="$vcRoot\Include;$ProjectRoot\Sources\Engine\Include;$ProjectRoot\Sources\Engine\Src"
    $env:LIB="$vcRoot\Lib"
    foreach($spec in @(@('OriginalWorldScriptTests','boss_fixtures.txt'),@('RestoredNpcDispatchTests','npc_dispatch_fixtures.txt'))) {
        $exe=Join-Path $ProjectRoot "Output\Tools\$($spec[0]).exe"
        $obj=[IO.Path]::ChangeExtension($exe,'.obj')
        & "$vcRoot\Bin\CL.EXE" /nologo /W3 /GX /MT /O2 /DWIN32 "/Fo$obj" "/Fe$exe" `
            "$ProjectRoot\Tests\Native\$($spec[0]).cpp" "$ProjectRoot\Lib\LuaLibDll.lib" "$ProjectRoot\Lib\release\engine.lib" /link kernel32.lib user32.lib
        if($LASTEXITCODE){throw "Compile $($spec[0]) failed"}
        # Explicitly declare these VC6 executables as ordinary unit tests.
        & 'C:\Program Files (x86)\Windows Kits\10\bin\10.0.26100.0\x86\mt.exe' -nologo `
            -manifest "$ProjectRoot\Tests\Native\UnitTest.manifest" "-outputresource:$exe;#1"
        if($LASTEXITCODE){throw 'Test manifest failed'}
        & $exe "analysis/restoration_20261016/$($spec[1])"
        if($LASTEXITCODE){throw "Test $($spec[0]) failed"}
    }
} finally { Pop-Location;$env:PATH=$oldPath;$env:INCLUDE=$oldInclude;$env:LIB=$oldLib }
