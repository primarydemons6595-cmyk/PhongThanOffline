$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
$vc='C:\Program Files (x86)\Microsoft Visual Studio\VC98'
$out=Join-Path $root 'Output\Tools\MarkupTests'
New-Item -ItemType Directory -Force $out | Out-Null
$oldPath=$env:PATH;$oldLib=$env:LIB
Push-Location (Join-Path $root 'Output\Client')
try {
 $env:PATH="$vc\Bin;C:\Program Files (x86)\Microsoft Visual Studio\Common\MSDev98\Bin;$oldPath"
 $env:LIB="$vc\Lib;$oldLib"
 & "$vc\Bin\cl.exe" /nologo /W3 /MT /O2 "/I$vc\Include" "/I$root\Sources\Engine\Src" "/Fo$out\test.obj" "/Fe$out\test.exe" (Join-Path $PSScriptRoot 'NativeMarkupTests.cpp') /link kernel32.lib
 if($LASTEXITCODE -ne 0){throw 'Native markup harness compile failed'}
 & "$out\test.exe" (Join-Path $root 'Output\Client\Engine.dll')
 if($LASTEXITCODE -ne 0){throw "Native markup failed $LASTEXITCODE"}
} finally {Pop-Location;$env:PATH=$oldPath;$env:LIB=$oldLib}
