$ErrorActionPreference='Stop'
$project=Split-Path -Parent $PSScriptRoot;$workspace=Split-Path -Parent $project
$vc='C:\Program Files (x86)\Microsoft Visual Studio\VC98'
$savedPath=$env:PATH;$savedInclude=$env:INCLUDE;$savedLib=$env:LIB
Push-Location $workspace
try {
 $env:PATH="$vc\Bin;$project\Output\Tools;$env:PATH"
 $env:INCLUDE="$vc\Include;$project\Headers;$project\Sources\Engine\Src;$project\Sources\Engine\Include"
 $env:LIB="$vc\Lib"
 $output=Join-Path $project 'Output\Tools\ServerNewsTests.exe';$obj=[IO.Path]::ChangeExtension($output,'.obj')
 & "$vc\Bin\CL.EXE" /nologo /W3 /GX /MT /O2 /DWIN32 "/Fo$obj" "/Fe$output" "$project\Tests\Native\ServerNewsTests.cpp" "$project\Lib\release\engine.lib" "$project\Lib\LuaLibDll.lib" /link kernel32.lib user32.lib
 if($LASTEXITCODE){throw 'Compile server news test failed'}
 & 'C:\Program Files (x86)\Windows Kits\10\bin\10.0.26100.0\x86\mt.exe' -nologo -manifest "$project\Tests\Native\UnitTest.manifest" "-outputresource:$output;#1"
 if($LASTEXITCODE){throw 'Manifest failed'}
 & $output 'analysis/restoration_20261016/server_news/staged/Server/script/servertimer.lua'
 if($LASTEXITCODE){throw 'Server news test failed'}
} finally {Pop-Location;$env:PATH=$savedPath;$env:INCLUDE=$savedInclude;$env:LIB=$savedLib}
