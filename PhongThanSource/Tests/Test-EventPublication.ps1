$ErrorActionPreference='Stop'
$source=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$workspace=Split-Path $source
$out=Join-Path $workspace 'analysis/event_service_20261019'
$vc='C:\Program Files (x86)\Microsoft Visual Studio\VC98'
$oldLib=$env:LIB;$oldPath=$env:PATH
Push-Location $source
try {
 $env:LIB="$vc/Lib;$env:LIB";$env:PATH="$source/Output/Server;$vc/Bin;$(Split-Path $vc)/Common/MSDev98/Bin;$env:PATH"
 foreach($name in @('VanTienLifecycleTests','LuaStageSyntaxTests','EventControlBridgeTests')) {
  & "$vc/Bin/cl.exe" /nologo /W3 /GX /MT /O2 /DWIN32 "/I$vc/Include" /ISources/Engine/Src /ISources/Engine/Include "/Fo$out/$name.obj" "/Fe$out/$name.exe" "Tests/Native/$name.cpp" Lib/release/engine.lib Lib/LuaLibDll.lib /link kernel32.lib user32.lib
  if($LASTEXITCODE){throw "Compile $name failed"}
 }
 & "$out/LuaStageSyntaxTests.exe" "$out/staged_lua_list.txt"
 if($LASTEXITCODE){throw 'Staged Lua syntax failed'}
 & "$out/VanTienLifecycleTests.exe" "$out/lifecycle_fixtures.txt" "$workspace/PhongThanRuntime-Loose/Server/script/phongthan/van_tien/doctor.lua"
 if($LASTEXITCODE){throw 'VTT lifecycle failed'}
 $bridgeFixture=Join-Path $out ('bridge_fixture_'+[guid]::NewGuid().ToString('N'))
 New-Item -ItemType Directory -Path $bridgeFixture | Out-Null
 & "$out/EventControlBridgeTests.exe" $bridgeFixture
 if($LASTEXITCODE){throw 'Native bridge failed'}
} finally {Pop-Location;$env:LIB=$oldLib;$env:PATH=$oldPath}
