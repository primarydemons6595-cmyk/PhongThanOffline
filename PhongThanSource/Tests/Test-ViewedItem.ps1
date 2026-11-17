$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
$vc='C:\Program Files (x86)\Microsoft Visual Studio\VC98'
$env:PATH="$vc\Bin;$(Split-Path $vc)\Common\MSDev98\Bin;$(Split-Path $root)\PhongThanRuntime-Loose\Client;$env:PATH"
$env:INCLUDE="$vc\Include;$env:INCLUDE"
$env:LIB="$vc\Lib;$env:LIB"
$out=Join-Path $root 'Output/Tools'
Push-Location $root
try {
 & "$vc/Bin/cl.exe" /nologo /W3 /GX /MT /O2 /DWIN32 /D_SERVER /D_STANDALONE /FIIClient.h /IHeaders /ISources/Core/Src /ISources/Engine/Src /ISources/Engine/Include "/Fo$out/ViewedItemTests.obj" "/Fe$out/ViewedItemTests.exe" Tests/Native/ViewedItemTests.cpp Lib/release/engine.lib Lib/release/common.lib /link kernel32.lib user32.lib
 if($LASTEXITCODE){throw 'Viewed item test compile failed'}
 & "$out/ViewedItemTests.exe" (Join-Path (Split-Path $root) 'PhongThanRuntime-Loose/Server')
 if($LASTEXITCODE){throw 'Viewed item test failed'}
} finally {Pop-Location}
