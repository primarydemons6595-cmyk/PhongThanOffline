$ErrorActionPreference='Stop'
$source=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
$workspace=Split-Path $source
$out=Join-Path $workspace 'analysis/native_drop_official_20261018'
$compiler=(Get-Command cl.exe).Source;$vc=Split-Path (Split-Path $compiler)
$env:LIB="$vc/Lib;$env:LIB"
$env:PATH="$source/Output/Server;$vc/Bin;$(Split-Path $vc)/Common/MSDev98/Bin;$env:PATH"
Push-Location $source
try{
 & $compiler /nologo /W3 /GX /Gy /MT /O2 /DWIN32 /D_SERVER /D_STANDALONE /FIIClient.h "/I$vc/Include" /IHeaders /ISources/Core/Src /ISources/Engine/Src /ISources/Engine/Include "/Fo$out/OfficialDropTests.obj" /c Tests/LegacyDropNativeTest/OfficialDropTests.cpp > "$out/test_build.log" 2>&1
 if($LASTEXITCODE){throw 'Official test compile failed'}
 $old=Join-Path $source 'Output/LegacyDropNativeTest/ExecutionPolicy'
 $objects=@('KBasPropTbl','KItem','KItemGenerator','KItemSet','KSubWorldSet')|ForEach-Object {Join-Path $old ($_.ToString()+'.obj')}
 & "$vc/Bin/link.exe" /nologo /OPT:REF "/OUT:$out/OfficialDropTests.exe" "$out/OfficialDropTests.obj" @objects Lib/release/engine.lib Lib/release/common.lib kernel32.lib user32.lib >> "$out/test_build.log" 2>&1
 if($LASTEXITCODE){throw 'Official test link failed'}
}finally{Pop-Location}
& "$out/OfficialDropTests.exe" "$workspace/PhongThanRuntime-Loose/Server" "$out/table_only_quarantine.txt" "$out/npc_cases.txt" "$out/npc_preflight.csv" > "$out/native_preflight.log" 2>&1
if($LASTEXITCODE){Get-Content "$out/native_preflight.log" -Tail 5;throw "Official tests failed: $LASTEXITCODE"}
Get-Content "$out/native_preflight.log" -Tail 1
