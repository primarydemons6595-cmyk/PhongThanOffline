$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
$vc='C:\Program Files (x86)\Microsoft Visual Studio\VC98'
$out=Join-Path $root 'Output\Tools\NativeMapRouteTests'
New-Item -ItemType Directory -Force $out | Out-Null
$oldPath=$env:PATH;$oldLib=$env:LIB
Push-Location $out
try {
 $env:PATH="$vc\Bin;C:\Program Files (x86)\Microsoft Visual Studio\Common\MSDev98\Bin;$oldPath"
 $env:LIB="$vc\Lib;$oldLib"
 & "$vc\Bin\cl.exe" /nologo /W3 /GX /MT /O2 "/I$PSScriptRoot\RouteStub" "/I$vc\Include" "/I$root\Headers" "/Fo$out\test.obj" "/Fe$out\test.exe" (Join-Path $PSScriptRoot 'NativeMapRouteTests.cpp') /link kernel32.lib user32.lib
 if($LASTEXITCODE -ne 0){throw 'Native map route harness compile failed'}
 & "$out\test.exe" (Join-Path (Split-Path -Parent $root) 'PhongThanRuntime-Loose\Client')
 if($LASTEXITCODE -ne 0){throw "Native map route failed $LASTEXITCODE"}
} finally {Pop-Location;$env:PATH=$oldPath;$env:LIB=$oldLib}
