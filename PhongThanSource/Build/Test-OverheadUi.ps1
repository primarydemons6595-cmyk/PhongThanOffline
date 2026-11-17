$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
$vc='C:\Program Files (x86)\Microsoft Visual Studio\VC98'
$out=Join-Path $root 'Output\OverheadUiTests'
New-Item -ItemType Directory -Force -Path $out | Out-Null
$env:INCLUDE="$vc\Include;$root\Headers;$root\Sources\Represent\iRepresent"
$env:LIB="$vc\Lib"
$env:PATH="$vc\Bin;$env:PATH"
& "$vc\Bin\cl.exe" /nologo /W3 /GX /MT /O2 "/Fo$out\test.obj" "/Fe$out\OverheadUiTests.exe" "$root\Tests\Native\PhongThanOverheadUiTests.cpp" /link kernel32.lib
if($LASTEXITCODE){throw 'Overhead UI test compile failed'}
& "$out\OverheadUiTests.exe"
if($LASTEXITCODE){throw 'Overhead UI regression failed'}
