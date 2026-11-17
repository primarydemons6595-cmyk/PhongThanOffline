$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
$vc='C:\Program Files (x86)\Microsoft Visual Studio\VC98'
$env:PATH="$vc\Bin;$(Split-Path $vc)\Common\MSDev98\Bin;$env:PATH"
$env:INCLUDE="$vc\Include;$env:INCLUDE";$env:LIB="$vc\Lib;$env:LIB"
$out=Join-Path $root 'Output/Tools'
& "$vc/Bin/cl.exe" /nologo /MT /O2 "/Fo$out/ChatSystemMenuTests.obj" "/Fe$out/ChatSystemMenuTests.exe" (Join-Path $PSScriptRoot 'Native/ChatSystemMenuTests.cpp')
if($LASTEXITCODE){throw 'System chat/menu test compile failed'}
& "$out/ChatSystemMenuTests.exe"
if($LASTEXITCODE){throw 'System chat/menu test failed'}
