$ErrorActionPreference='Stop'
$project=Split-Path -Parent $PSScriptRoot
$workspace=Split-Path -Parent $project
$vc='C:\Program Files (x86)\Microsoft Visual Studio\VC98'
$out=Join-Path $project 'Output\Tools\RequestedNpcServiceTests.exe'
$env:PATH="$vc\Bin;$project\Output\Tools;$project\Output\Client;$env:PATH"
$env:INCLUDE="$vc\Include;$project\Headers;$project\Sources\Engine\Src;$project\Sources\Engine\Include"
$env:LIB="$vc\Lib"
& "$vc\Bin\CL.EXE" /nologo /W3 /GX /MT /O2 /DWIN32 "/Fo$project\Output\Tools\RequestedNpcServiceTests.obj" "/Fe$out" "$project\Tests\Native\RequestedNpcServiceTests.cpp" "$project\Lib\release\engine.lib" "$project\Lib\LuaLibDll.lib" /link kernel32.lib user32.lib
if($LASTEXITCODE){throw 'Requested NPC test compile failed'}
& 'C:\Program Files (x86)\Windows Kits\10\bin\10.0.26100.0\x86\mt.exe' -nologo -manifest "$project\Tests\Native\UnitTest.manifest" "-outputresource:$out;#1"
if($LASTEXITCODE){throw 'Test manifest failed'}
$stage=Join-Path $workspace 'analysis\monster_ui_20261017\npc_services\staged\Server\script\phongthan\npc_recovered_batch'
& $out "$stage\1021_b5deef29.lua" "$stage\1021_828485f4.lua"
if($LASTEXITCODE){throw 'Requested NPC regression failed'}
