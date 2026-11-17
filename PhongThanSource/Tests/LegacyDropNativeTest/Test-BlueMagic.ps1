$ErrorActionPreference='Stop'
$source=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
$workspace=Split-Path -Parent $source
$report=Join-Path $workspace 'analysis/blue_magic_20261018'
$output=Join-Path $source 'Output/LegacyDropNativeTest/BlueMagic'
New-Item -ItemType Directory -Force $output,$report | Out-Null
$vc='C:\Program Files (x86)\Microsoft Visual Studio\VC98'
$oldLib=$env:LIB;$oldPath=$env:PATH
Push-Location $source
try{
 $env:LIB="$vc/Lib;$env:LIB";$env:PATH="$source/Output/Server;$vc/Bin;$(Split-Path $vc)/Common/MSDev98/Bin;$env:PATH"
 & node Tests/LegacyDropNativeTest/blue-magic-audit.mjs
 if($LASTEXITCODE){throw 'Blue source audit failed'}
 & node Tests/LegacyDropNativeTest/prepare-loose-fixture.mjs Server
 if($LASTEXITCODE){throw 'Native loose fixture failed'}
 & node Tests/LegacyDropNativeTest/prepare-item-units.mjs
 if($LASTEXITCODE){throw 'Native item source preparation failed'}
 Copy-Item -LiteralPath "$workspace/PhongThanRuntime-Loose/Server/Engine.dll" -Destination "$output/Engine.dll" -Force
 $argsCompile=@('/nologo','/W3','/GX','/Gy','/MT','/O2','/DWIN32','/D_SERVER','/D_STANDALONE','/FIIClient.h',"/I$vc/Include",'/IHeaders','/ISources/Core/Src','/ISources/Engine/Src','/ISources/Engine/Include')
 $objects=@()
 foreach($unit in @('Sources/Core/Src/KBasPropTbl.CPP','Output/LegacyDropNativeTest/KItem.cpp','Sources/Core/Src/KItemGenerator.CPP','Sources/Core/Src/KItemSet.cpp','Output/LegacyDropNativeTest/KSubWorldSet.cpp','Tests/LegacyDropNativeTest/BlueMagicNativeTests.cpp')){
  $obj=Join-Path $output (([IO.Path]::GetFileNameWithoutExtension($unit))+'.obj')
  & "$vc/Bin/cl.exe" @argsCompile "/Fo$obj" /c $unit
  if($LASTEXITCODE){throw "Compile failed $unit"};$objects+=$obj
 }
 & "$vc/Bin/link.exe" /nologo /OPT:REF "/OUT:$output/BlueMagicNativeTests.exe" @objects Lib/release/engine.lib Lib/release/common.lib kernel32.lib user32.lib
 if($LASTEXITCODE){throw 'Blue native fixture link failed'}
 & "$output/BlueMagicNativeTests.exe" "$output/../LooseFixture" "$report/table_manifest.txt" "$report/native_abi.jsonl" 2>&1 | Tee-Object -FilePath "$report/native.log"
 if($LASTEXITCODE){throw 'Blue native fixture failed'}
}finally{Pop-Location;$env:LIB=$oldLib;$env:PATH=$oldPath}
