param([ValidateSet('Server','Client')][string]$Role='Server')
$ErrorActionPreference='Stop'
$source=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
$workspace=Split-Path -Parent $source
$report=Join-Path $workspace 'analysis/legacy_drop_20261017'
$output=Join-Path $source 'Output/LegacyDropNativeTest'
$compiler=(Get-Command cl.exe -ErrorAction Stop).Source
$vc=Split-Path -Parent (Split-Path -Parent $compiler)
$oldLib=$env:LIB;$oldPath=$env:PATH
Push-Location $source
try {
    $env:LIB="$vc/Lib;$env:LIB";$env:PATH="$source/Output/Server;$vc/Bin;$(Split-Path $vc)/Common/MSDev98/Bin;$env:PATH"
    $args=@('/nologo','/W3','/GX','/Gy','/MT','/O2','/DWIN32','/D_SERVER','/D_STANDALONE','/FIIClient.h',"/I$vc/Include",'/IHeaders','/ISources/Core/Src','/ISources/Engine/Src','/ISources/Engine/Include')
    & node Tests/LegacyDropNativeTest/prepare-item-units.mjs
    if($LASTEXITCODE){throw 'Production subset preparation failed'}
    & node Tests/LegacyDropNativeTest/prepare-loose-fixture.mjs $Role
    if($LASTEXITCODE){throw 'Loose fixture preparation failed'}
    $objects=@()
    foreach($unit in @('Sources/Core/Src/KBasPropTbl.CPP','Output/LegacyDropNativeTest/KItem.cpp','Sources/Core/Src/KItemGenerator.CPP','Sources/Core/Src/KItemSet.cpp','Output/LegacyDropNativeTest/KSubWorldSet.cpp','Tests/LegacyDropNativeTest/NativeItemProbe.cpp')){
        $obj=Join-Path $output (([IO.Path]::GetFileNameWithoutExtension($unit))+'.obj')
        & $compiler @args "/Fo$obj" /c $unit
        if($LASTEXITCODE){throw "Compile failed: $unit"};$objects+=$obj
    }
    & "$vc/Bin/link.exe" /nologo /OPT:REF "/OUT:$output/NativeItemProbe.exe" @objects Lib/release/engine.lib Lib/release/common.lib kernel32.lib user32.lib
    if($LASTEXITCODE){throw 'Link native item probe failed'}
    $fixtureName=if($Role -eq 'Server'){'LooseFixture'}else{'LooseFixture-Client'}
    $itemReport=if($Role -eq 'Server'){"$report/item_probe.csv"}else{"$report/phase2/item_probe_client.csv"}
    $referenceReport=if($Role -eq 'Server'){"$report/native_references.csv"}else{"$report/phase2/native_references_client.csv"}
    Push-Location (Join-Path $output $fixtureName)
    try {
        & "$output/NativeItemProbe.exe" (Get-Location).Path "$report/tuples.txt" $itemReport "$report/references.hex" $referenceReport "$report/phase2/quarantine.txt"
        $nativeExit=$LASTEXITCODE
        $npcReport=if($Role -eq 'Server'){"$report/phase2/native_npc_preflight.csv"}else{"$report/phase2/native_npc_preflight_client.csv"}
        Copy-Item -LiteralPath 'npc_preflight.csv' -Destination $npcReport -Force
        if($nativeExit){throw 'Native item probe failed'}
    }finally{Pop-Location}
    if($LASTEXITCODE){throw 'Native item probe failed'}
}finally{Pop-Location;$env:LIB=$oldLib;$env:PATH=$oldPath}
