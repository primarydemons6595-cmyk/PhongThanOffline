$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
$vc='C:\Program Files (x86)\Microsoft Visual Studio\VC98'
$out=Join-Path $root 'Output\Tools\TeamOptionsTests'
New-Item -ItemType Directory -Force $out | Out-Null
$source=[Text.Encoding]::ASCII.GetString([IO.File]::ReadAllBytes((Join-Path $root 'Sources\Core\Src\KPlayerTeam.cpp')))
$begin=$source.IndexOf('unsigned int PhongThanTeamOptions(')
$finish=$source.IndexOf('#endif',$begin)
if($begin -lt 0 -or $finish -lt 0){throw 'Production function missing'}
[IO.File]::WriteAllText((Join-Path $out 'TeamOptionsProduction.inl'),$source.Substring($begin,$finish-$begin),[Text.Encoding]::ASCII)
$oldPath=$env:PATH;$oldLib=$env:LIB
Push-Location $out
try {
 $env:PATH="$vc\Bin;C:\Program Files (x86)\Microsoft Visual Studio\Common\MSDev98\Bin;$oldPath"
 $env:LIB="$vc\Lib;$oldLib"
 & "$vc\Bin\cl.exe" /nologo /W3 /GX /MT /O2 "/I$out" "/I$vc\Include" "/Fo$out\test.obj" "/Fe$out\test.exe" (Join-Path $PSScriptRoot 'TeamOptionsTests.cpp') /link kernel32.lib user32.lib
 if($LASTEXITCODE -ne 0){throw 'Team options test compile failed'}
 & "$out\test.exe"
 if($LASTEXITCODE -ne 0){throw 'Team options tests failed'}
} finally {Pop-Location;$env:PATH=$oldPath;$env:LIB=$oldLib}
