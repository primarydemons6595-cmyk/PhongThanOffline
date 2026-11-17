param([string]$ProjectRoot=(Split-Path -Parent $PSScriptRoot))
$ErrorActionPreference='Stop'
$workspace=Split-Path -Parent $ProjectRoot
$vcRoot='C:\Program Files (x86)\Microsoft Visual Studio\VC98'
$oldPath=$env:PATH;$oldInclude=$env:INCLUDE;$oldLib=$env:LIB
Push-Location $workspace
try {
    $env:PATH="$vcRoot\Bin;$ProjectRoot\Output\Tools;$env:PATH"
    $env:INCLUDE="$vcRoot\Include;$ProjectRoot\Headers;$ProjectRoot\Sources\Engine\Include;$ProjectRoot\Sources\Engine\Src"
    $env:LIB="$vcRoot\Lib"
    foreach($name in @('EventLuaCompatTests','ConnectedEventServiceTests')) {
        $exe=Join-Path $ProjectRoot "Output\Tools\$name.exe"
        $obj=[IO.Path]::ChangeExtension($exe,'.obj')
        & "$vcRoot\Bin\CL.EXE" /nologo /W3 /GX /MT /O2 /DWIN32 "/Fo$obj" "/Fe$exe" `
            "$ProjectRoot\Tests\Native\$name.cpp" "$ProjectRoot\Lib\LuaLibDll.lib" "$ProjectRoot\Lib\release\engine.lib" /link kernel32.lib user32.lib
        if($LASTEXITCODE){throw "Compile $name failed"}
        & 'C:\Program Files (x86)\Windows Kits\10\bin\10.0.26100.0\x86\mt.exe' -nologo `
            -manifest "$ProjectRoot\Tests\Native\UnitTest.manifest" "-outputresource:$exe;#1"
        if($LASTEXITCODE){throw 'Manifest failed'}
        if($name -eq 'ConnectedEventServiceTests'){& $exe 'analysis/restoration_20261016/city_service_repair/service_ui_fixtures.txt'}else{& $exe}
        if($LASTEXITCODE){throw "$name failed"}
    }
} finally {Pop-Location;$env:PATH=$oldPath;$env:INCLUDE=$oldInclude;$env:LIB=$oldLib}
