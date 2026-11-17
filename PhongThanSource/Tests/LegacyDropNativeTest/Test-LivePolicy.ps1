$ErrorActionPreference='Stop'
$source=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
$workspace=Split-Path -Parent $source
$report=Join-Path $workspace 'analysis/legacy_drop_20261018_live'
$output=Join-Path $source 'Output/LegacyDropNativeTest/LivePolicy'
New-Item -ItemType Directory -Force $output,$report | Out-Null
$compiler='C:\Program Files (x86)\Microsoft Visual Studio\VC98\Bin\CL.EXE'
$vc=Split-Path -Parent (Split-Path -Parent $compiler)
$oldLib=$env:LIB;$oldPath=$env:PATH
Push-Location $source
try {
 $env:LIB="$vc/Lib;$env:LIB";$env:PATH="$source/Output/Server;$vc/Bin;$(Split-Path $vc)/Common/MSDev98/Bin;$env:PATH"
 Copy-Item -LiteralPath "$workspace/PhongThanRuntime-Loose/Server/Engine.dll" -Destination "$output/Engine.dll" -Force
 & node Tests/LegacyDropNativeTest/prepare-loose-fixture.mjs Server
 if($LASTEXITCODE){throw 'Current native fixture failed'}
 & node Tests/LegacyDropNativeTest/prepare-item-units.mjs
 if($LASTEXITCODE){throw 'Native units failed'}
 $compileArgs=@('/nologo','/W3','/GX','/Gy','/MT','/O2','/DWIN32','/D_SERVER','/D_STANDALONE','/FIIClient.h',"/I$vc/Include",'/IHeaders','/ISources/Core/Src','/ISources/Engine/Src','/ISources/Engine/Include')
 $objects=@()
 foreach($unit in @('Sources/Core/Src/KBasPropTbl.CPP','Output/LegacyDropNativeTest/KItem.cpp','Sources/Core/Src/KItemGenerator.CPP','Sources/Core/Src/KItemSet.cpp','Output/LegacyDropNativeTest/KSubWorldSet.cpp','Tests/LegacyDropNativeTest/LivePolicyTests.cpp')){
  $obj=Join-Path $output (([IO.Path]::GetFileNameWithoutExtension($unit))+'.obj')
  & $compiler @compileArgs "/Fo$obj" /c $unit
  if($LASTEXITCODE){throw "Compile $unit failed"};$objects+=$obj
 }
 & "$vc/Bin/link.exe" /nologo /OPT:REF "/OUT:$output/LivePolicyTests.exe" @objects Lib/release/engine.lib Lib/release/common.lib kernel32.lib user32.lib
 if($LASTEXITCODE){throw 'Native live-policy link failed'}
 & "$output/LivePolicyTests.exe" "$output/../LooseFixture" "$workspace/analysis/legacy_drop_20261017/phase2/quarantine.txt" $report 2>&1 | Tee-Object -FilePath "$report/native_live_policy.log"
 if($LASTEXITCODE){throw 'Native live-policy tests failed'}
} finally {Pop-Location;$env:LIB=$oldLib;$env:PATH=$oldPath}
