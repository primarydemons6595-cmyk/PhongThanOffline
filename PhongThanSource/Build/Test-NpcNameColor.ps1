$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
$vc='C:\Program Files (x86)\Microsoft Visual Studio\VC98'
$out=Join-Path $root 'Output\NpcNameColorTests'
New-Item -ItemType Directory -Force -Path $out | Out-Null
$env:INCLUDE="$vc\Include"
$env:LIB="$vc\Lib"
$env:PATH="$vc\Bin;$env:PATH"
& "$vc\Bin\cl.exe" /nologo /W3 /GX /MT /O2 "/Fo$out\test.obj" "/Fe$out\NpcNameColorTests.exe" "$root\Tests\Native\PhongThanNpcNameColorTests.cpp" /link kernel32.lib
if($LASTEXITCODE){throw 'NPC name color test compile failed'}
& "$out\NpcNameColorTests.exe"
if($LASTEXITCODE){throw 'NPC name color regression failed'}
