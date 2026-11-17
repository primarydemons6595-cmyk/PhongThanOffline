$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
$vc='C:\Program Files (x86)\Microsoft Visual Studio\VC98'
$out=Join-Path $root 'Output\Tools\TeamNearbyTests'
New-Item -ItemType Directory -Force $out | Out-Null
$oldPath=$env:PATH;$oldLib=$env:LIB
$source=[IO.File]::ReadAllText((Join-Path $root 'Sources/Core/Src/KNpcSet.cpp'))
$start=$source.IndexOf('int KNpcSet::GetAroundPlayerForTeamInvite(')
$end=$source.IndexOf('#endif',$start)
[IO.File]::WriteAllText((Join-Path $out 'TeamNearbyProduction.inl'),$source.Substring($start,$end-$start),[Text.Encoding]::ASCII)
Push-Location $out
try {
 $env:PATH="$vc\Bin;C:\Program Files (x86)\Microsoft Visual Studio\Common\MSDev98\Bin;$oldPath"
 $env:LIB="$vc\Lib;$oldLib"
 & "$vc\Bin\cl.exe" /nologo /W3 /GX /MT /O2 "/I$vc\Include" "/I$root\Headers" "/I$out" "/Fo$out\test.obj" "/Fe$out\test.exe" (Join-Path $PSScriptRoot 'TeamNearbyTests.cpp') /link kernel32.lib
 if($LASTEXITCODE -ne 0){throw 'Team nearby harness compile failed'}
 & "$out\test.exe"
 if($LASTEXITCODE -ne 0){throw "Team nearby failed $LASTEXITCODE"}
} finally {Pop-Location;$env:PATH=$oldPath;$env:LIB=$oldLib}
