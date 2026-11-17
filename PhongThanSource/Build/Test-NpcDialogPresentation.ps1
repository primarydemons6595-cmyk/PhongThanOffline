$ErrorActionPreference='Stop'
$project=Split-Path -Parent $PSScriptRoot
$vc='C:\Program Files (x86)\Microsoft Visual Studio\VC98'
$out=Join-Path $project 'Output\Tools\NpcDialogPresentationTests.exe'
$env:PATH="$vc\Bin;$project\Output\Tools;$project\Output\Client;$env:PATH"
$env:INCLUDE="$vc\Include;$project\Headers;$project\Sources\Engine\Src;$project\Sources\Engine\Include"
$env:LIB="$vc\Lib"
& "$vc\Bin\CL.EXE" /nologo /W3 /GX /MT /O2 /DWIN32 "/Fo$project\Output\Tools\NpcDialogPresentationTests.obj" "/Fe$out" "$project\Tests\Native\NpcDialogPresentationTests.cpp" "$project\Lib\release\engine.lib" "$project\Lib\LuaLibDll.lib" /link kernel32.lib user32.lib
if($LASTEXITCODE){throw 'Dialog presentation test compile failed'}
& 'C:\Program Files (x86)\Windows Kits\10\bin\10.0.26100.0\x86\mt.exe' -nologo -manifest "$project\Tests\Native\UnitTest.manifest" "-outputresource:$out;#1"
if($LASTEXITCODE){throw 'Test manifest failed'}
& $out
if($LASTEXITCODE){throw 'Dialog presentation regression failed'}
