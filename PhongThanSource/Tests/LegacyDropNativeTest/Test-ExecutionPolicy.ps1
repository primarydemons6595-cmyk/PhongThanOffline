[CmdletBinding()]
param()
$ErrorActionPreference='Stop'
$source=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
$workspace=Split-Path -Parent $source
$report=Join-Path $workspace 'analysis/legacy_drop_20261017/execution_policy'
$output=Join-Path $source 'Output/LegacyDropNativeTest/ExecutionPolicy'
New-Item -ItemType Directory -Force $output,$report | Out-Null
$compiler='C:\Program Files (x86)\Microsoft Visual Studio\VC98\Bin\CL.EXE'
$vc=Split-Path -Parent (Split-Path -Parent $compiler)
$oldLib=$env:LIB;$oldPath=$env:PATH
Push-Location $source
try {
    $env:LIB="$vc/Lib;$env:LIB"
    $env:PATH="$source/Output/Server;$vc/Bin;$(Split-Path $vc)/Common/MSDev98/Bin;$env:PATH"
    & node Tests/LegacyDropNativeTest/execution-policy-evidence.mjs
    if($LASTEXITCODE){throw 'Targeted snapshot evidence failed'}
    & node Tests/LegacyDropNativeTest/prepare-item-units.mjs
    if($LASTEXITCODE){throw 'Native production units failed'}
    $compileArgs=@('/nologo','/W3','/GX','/Gy','/MT','/O2','/DWIN32','/D_SERVER','/D_STANDALONE','/FIIClient.h',"/I$vc/Include",'/IHeaders','/ISources/Core/Src','/ISources/Engine/Src','/ISources/Engine/Include')
    $objects=@()
    foreach($unit in @('Sources/Core/Src/KBasPropTbl.CPP','Output/LegacyDropNativeTest/KItem.cpp','Sources/Core/Src/KItemGenerator.CPP','Sources/Core/Src/KItemSet.cpp','Output/LegacyDropNativeTest/KSubWorldSet.cpp','Tests/LegacyDropNativeTest/ExecutionPolicyTests.cpp')){
        $obj=Join-Path $output (([IO.Path]::GetFileNameWithoutExtension($unit))+'.obj')
        & $compiler @compileArgs "/Fo$obj" /c $unit
        if($LASTEXITCODE){throw "Compile failed: $unit"};$objects+=$obj
    }
    & "$vc/Bin/link.exe" /nologo /OPT:REF "/OUT:$output/ExecutionPolicyTests.exe" @objects Lib/release/engine.lib Lib/release/common.lib kernel32.lib user32.lib
    if($LASTEXITCODE){throw 'Link focused policy test failed'}
    foreach($role in @('Server','Client')){
        & node Tests/LegacyDropNativeTest/prepare-loose-fixture.mjs $role
        if($LASTEXITCODE){throw "Fixture failed: $role"}
        $fixtureName=if($role -eq 'Server'){'LooseFixture'}else{'LooseFixture-Client'}
        $fixture=Join-Path $source "Output/LegacyDropNativeTest/$fixtureName"
        Push-Location $fixture
        try {
            & "$output/ExecutionPolicyTests.exe" $fixture "$workspace/analysis/legacy_drop_20261017/phase2/quarantine.txt" "$report/explicit_levels.txt" "$report/npc_cases.txt" "$report/npc_results_$role.csv" 2>&1 | Tee-Object -FilePath "$report/native_$role.log"
            if($LASTEXITCODE){throw "Focused native policy tests failed: $role"}
        } finally {Pop-Location}
    }
    & node Tests/LegacyDropNativeTest/execution-policy-report.mjs
    if($LASTEXITCODE){throw 'Focused policy evidence/report validation failed'}
} finally {Pop-Location;$env:LIB=$oldLib;$env:PATH=$oldPath}
