$ErrorActionPreference='Stop'
$source=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
$workspace=Split-Path $source
$out=Join-Path $workspace 'analysis/native_drop_completion_20261018'
$old=Join-Path $source 'Output/LegacyDropNativeTest/ExecutionPolicy'
$compiler=(Get-Command cl.exe).Source;$vc=Split-Path (Split-Path $compiler)
$env:LIB="$vc/Lib;$env:LIB";$env:PATH="$source/Output/Server;$vc/Bin;$(Split-Path $vc)/Common/MSDev98/Bin;$env:PATH"
Push-Location $source
try{
 & $compiler /nologo /W3 /GX /Gy /MT /O2 /DWIN32 /D_SERVER /D_STANDALONE /FIIClient.h "/I$vc/Include" /IHeaders /ISources/Core/Src /ISources/Engine/Src /ISources/Engine/Include "/Fo$out/NativeDropCompletionTests.obj" /c Tests/LegacyDropNativeTest/NativeDropCompletionTests.cpp
 if($LASTEXITCODE){throw 'Native completion compile failed'}
 $objects=@('KBasPropTbl','KItem','KItemGenerator','KItemSet','KSubWorldSet')|ForEach-Object {Join-Path $old ($_.ToString()+'.obj')}
 & "$vc/Bin/link.exe" /nologo /OPT:REF "/OUT:$out/NativeDropCompletionTests.exe" "$out/NativeDropCompletionTests.obj" @objects Lib/release/engine.lib Lib/release/common.lib kernel32.lib user32.lib
 if($LASTEXITCODE){throw 'Native completion link failed'}
 $fixture=Join-Path $source 'Output/LegacyDropNativeTest/LooseFixture'
 Push-Location $fixture
 try{& "$out/NativeDropCompletionTests.exe" $fixture "$out/parser478.csv" "$out/non_equipment_cases.txt" "$workspace/PhongThanRuntime-Loose/P0-BlueMagic-Isolated/p1_05d188ae7f02/Server/settings/phongthan/LegacyDropQuarantine.txt" "$out/npc_cases.txt" 2>&1 | Tee-Object "$out/native_tests.log";if($LASTEXITCODE){throw "Native completion checks failed exit=$LASTEXITCODE"}}finally{Pop-Location}
}finally{Pop-Location}
