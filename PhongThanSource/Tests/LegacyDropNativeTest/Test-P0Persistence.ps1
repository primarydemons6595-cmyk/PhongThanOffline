$ErrorActionPreference='Stop'
$source=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
$workspace=Split-Path -Parent $source
$report=Join-Path $workspace 'analysis/blue_magic_p0_20261018'
$output=Join-Path $source 'Output/LegacyDropNativeTest/P0Persistence'
$isolation=Join-Path $workspace ('PhongThanRuntime-Loose/P0-BlueMagic-Isolated/'+[Guid]::NewGuid().ToString('N'))
New-Item -ItemType Directory -Force $report,$output,$isolation | Out-Null
[IO.File]::WriteAllText((Join-Path $isolation 'p0_isolated.flag'),'ISOLATED P0 NATIVE CHARACTER DATA ONLY')
$vc='C:\Program Files (x86)\Microsoft Visual Studio\VC98'
$oldLib=$env:LIB;$oldPath=$env:PATH
Push-Location $source
try{
 $env:LIB="$vc/Lib;$env:LIB";$env:PATH="$source/Output/Server;$vc/Bin;$(Split-Path $vc)/Common/MSDev98/Bin;$env:PATH"
 & node Tests/LegacyDropNativeTest/prepare-p0-bodies.mjs
 if($LASTEXITCODE){throw 'Production body extraction failed'}
 & node Tests/LegacyDropNativeTest/prepare-loose-fixture.mjs Server
 if($LASTEXITCODE){throw 'Native fixture preparation failed'}
 & node Tests/LegacyDropNativeTest/prepare-item-units.mjs
 if($LASTEXITCODE){throw 'Native units failed'}
 Copy-Item -LiteralPath "$workspace/PhongThanRuntime-Loose/Server/Engine.dll" -Destination "$output/Engine.dll" -Force
 $compile=@('/nologo','/W3','/GX','/Gy','/MT','/O2','/DWIN32','/D_SERVER','/D_STANDALONE','/FIIClient.h',"/I$vc/Include",'/IHeaders','/ISources/Core/Src','/ISources/Engine/Src','/ISources/Engine/Include')
 $objects=@()
 foreach($unit in @('Sources/Core/Src/KBasPropTbl.CPP','Output/LegacyDropNativeTest/KItem.cpp','Sources/Core/Src/KItemGenerator.CPP','Sources/Core/Src/KItemSet.cpp','Output/LegacyDropNativeTest/KSubWorldSet.cpp','Sources/MultiServer/Goddess/PhongThanCharacterStore.cpp','Tests/LegacyDropNativeTest/P0PersistenceTests.cpp')){
  $obj=Join-Path $output (([IO.Path]::GetFileNameWithoutExtension($unit))+'.obj')
  & "$vc/Bin/cl.exe" @compile "/Fo$obj" /c $unit
  if($LASTEXITCODE){throw "Compile failed $unit"};$objects+=$obj
 }
 & "$vc/Bin/link.exe" /nologo /OPT:REF "/OUT:$output/P0PersistenceTests.exe" @objects Lib/release/engine.lib Lib/release/common.lib kernel32.lib user32.lib
 if($LASTEXITCODE){throw 'P0 native link failed'}
 foreach($phase in @('write','read')){
  & "$output/P0PersistenceTests.exe" $phase "$output/../LooseFixture" $isolation $report 2>&1 | Tee-Object -FilePath "$report/$phase.log"
  if($LASTEXITCODE){throw "P0 $phase tests failed"}
 }
 [IO.File]::WriteAllText((Join-Path $report 'isolation.json'),(@{Root=$isolation;Executable="$output/P0PersistenceTests.exe";Scope='ACTUAL_SAVE_LOAD_BODIES_NATIVE_ITEMSET_REAL_CHARACTERSTORE_TWO_PROCESSES';ProductionDataTouched=$false}|ConvertTo-Json),[Text.UTF8Encoding]::new($false))
}finally{Pop-Location;$env:LIB=$oldLib;$env:PATH=$oldPath}
