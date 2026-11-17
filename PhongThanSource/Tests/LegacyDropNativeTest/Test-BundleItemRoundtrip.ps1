$ErrorActionPreference='Stop'
$source=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'));$workspace=Split-Path $source -Parent
$report=Join-Path $workspace 'analysis/single_pthc_20261101';$output=Join-Path $source 'Output/LegacyDropNativeTest/BundleRoundtrip'
$vc='C:\Program Files (x86)\Microsoft Visual Studio\VC98'
$env:LIB="$vc/Lib";$env:INCLUDE="$vc/Include";$env:PATH="$source/Output/Server;$vc/Bin;$(Split-Path $vc)/Common/MSDev98/Bin;$env:PATH"
New-Item -ItemType Directory -Force $output | Out-Null
Push-Location $source
try{
 & node Tests/LegacyDropNativeTest/prepare-p0-bodies.mjs
 & node Tests/LegacyDropNativeTest/prepare-loose-fixture.mjs Server
 & node Tests/LegacyDropNativeTest/prepare-item-units.mjs
 $fixture=Join-Path $source 'Output/LegacyDropNativeTest/LooseFixture'
 Copy-Item -LiteralPath "$source/Deploy/ProjectContent/ItemQuestFollowup20261031/compiled/Server/settings/item/001/signet.txt" -Destination "$fixture/settings/item/001/signet.txt" -Force
 Copy-Item -LiteralPath "$workspace/PhongThanRuntime-Loose/Server/Engine.dll" -Destination "$output/Engine.dll" -Force
 $compile=@('/nologo','/W3','/GX','/Gy','/MT','/O2','/DWIN32','/D_SERVER','/D_STANDALONE','/FIIClient.h',"/I$vc/Include",'/IHeaders','/ISources/Core/Src','/ISources/Engine/Src','/ISources/Engine/Include')
 $objects=@()
 foreach($unit in @('Sources/Core/Src/KBasPropTbl.CPP','Output/LegacyDropNativeTest/KItem.cpp','Sources/Core/Src/KItemGenerator.CPP','Sources/Core/Src/KItemSet.cpp','Output/LegacyDropNativeTest/KSubWorldSet.cpp','Sources/MultiServer/Goddess/PhongThanCharacterStore.cpp','Tests/LegacyDropNativeTest/BundleItemRoundtripTests.cpp')){
  $obj=Join-Path $output (([IO.Path]::GetFileNameWithoutExtension($unit))+'.obj');& "$vc/Bin/cl.exe" @compile "/Fo$obj" /c $unit;if($LASTEXITCODE){throw "Compile failed $unit"};$objects+=$obj
 }
 & "$vc/Bin/link.exe" /nologo /OPT:REF "/OUT:$output/BundleItemRoundtripTests.exe" @objects Lib/release/engine.lib Lib/release/common.lib kernel32.lib user32.lib
 if($LASTEXITCODE){throw 'Link failed'}
 $data=Join-Path $report ('roundtrip-'+[guid]::NewGuid().ToString('N'));New-Item -ItemType Directory $data|Out-Null
 foreach($entry in @('settings','_loose_id','package.ini','loose_index.bin','loose_only.flag')){if(Test-Path -LiteralPath (Join-Path $fixture $entry)){Copy-Item -LiteralPath (Join-Path $fixture $entry) -Destination $data -Recurse -Force}}
 foreach($phase in @('write','read')){& "$output/BundleItemRoundtripTests.exe" $phase $data $data;if($LASTEXITCODE){throw "Failed $phase"}}
 $legacy=Join-Path $workspace 'analysis/buff_seal_ui_20261101/legacy-seals.pthc'
 if(Test-Path -LiteralPath $legacy){New-Item -ItemType Directory -Force "$data/CharacterStore/123456"|Out-Null;Copy-Item -LiteralPath $legacy -Destination "$data/CharacterStore/123456/role_7175616e6768757933.pthc"; & "$output/BundleItemRoundtripTests.exe" repair $data $data;if($LASTEXITCODE){throw 'Legacy seal repair failed'}}
}finally{Pop-Location}
