$ErrorActionPreference='Stop'
$project=Split-Path -Parent $PSScriptRoot;$vc='C:\Program Files (x86)\Microsoft Visual Studio\VC98'
$savedPath=$env:PATH;$savedInclude=$env:INCLUDE;$savedLib=$env:LIB
Push-Location $project
try {
 $env:PATH="$vc\Bin;$project\Output\Tools;$env:PATH"
 $env:INCLUDE="$vc\Include;$project\Headers;$project\Sources\Engine\Src;$project\Sources\Engine\Include;$project\Sources\Core\Src"
 $env:LIB="$vc\Lib"
 $output='Output\Tools\MissionLookupStackTests.exe'
 & "$vc\Bin\CL.EXE" /nologo /W3 /GX /MT /O2 /DWIN32 /D_SERVER /D_STANDALONE /FIIClient.h /FoOutput/Tools/MissionLookupStackTests.obj "/Fe$output" Tests/Native/MissionLookupStackTests.cpp Lib/release/engine.lib Lib/release/common.lib /link kernel32.lib user32.lib
 if($LASTEXITCODE){throw 'Compile mission lookup test failed'}
 & 'C:\Program Files (x86)\Windows Kits\10\bin\10.0.26100.0\x86\mt.exe' -nologo -manifest Tests/Native/UnitTest.manifest "-outputresource:$output;#1"
 if($LASTEXITCODE){throw 'Manifest failed'}
 & $output (Join-Path (Split-Path -Parent $project) 'PhongThanRuntime-Loose\Server')
 if($LASTEXITCODE){throw 'Mission lookup test failed'}
} finally {Pop-Location;$env:PATH=$savedPath;$env:INCLUDE=$savedInclude;$env:LIB=$savedLib}
