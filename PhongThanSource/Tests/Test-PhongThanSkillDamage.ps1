$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
$out=Join-Path $root 'Output\Tools'
New-Item -ItemType Directory -Force $out | Out-Null
$compiler='C:\Program Files (x86)\Microsoft Visual Studio\VC98\Bin\cl.exe'
$exe=Join-Path $out 'PhongThanSkillDamageTests.exe'
& $compiler /nologo /MT /W3 /O2 /DWIN32 "/Fo$out\PhongThanSkillDamageTests.obj" "/Fe$exe" (Join-Path $PSScriptRoot 'Native\PhongThanSkillDamageTests.cpp')
if($LASTEXITCODE -ne 0){throw 'Skill damage harness compile failed'}
& $exe
if($LASTEXITCODE -ne 0){throw 'Skill damage harness failed'}
