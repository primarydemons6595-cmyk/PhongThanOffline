$ErrorActionPreference='Stop'
$source=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
$report=Join-Path (Split-Path -Parent $source) 'analysis/legacy_drop_20261017'
$output=Join-Path $source 'Output/LegacyDropNativeTest'
$compiler=(Get-Command cl.exe).Source;$vc=Split-Path -Parent (Split-Path -Parent $compiler)
$oldLib=$env:LIB;$oldPath=$env:PATH
Push-Location $source
try {
    $env:LIB="$vc/Lib;$env:LIB";$env:PATH="$source/Output/Server;$vc/Bin;$(Split-Path $vc)/Common/MSDev98/Bin;$env:PATH"
    & node Tests/LegacyDropNativeTest/prepare-genre-baseline.mjs
    if($LASTEXITCODE){throw 'Genre baseline preparation failed'}
    $args=@('/nologo','/W3','/GX','/MT','/O2','/DWIN32','/D_SERVER','/D_STANDALONE','/FIIClient.h',"/I$vc/Include",'/IHeaders','/ISources/Core/Src','/ISources/Engine/Src','/ISources/Engine/Include')
    foreach($mode in 'Before','After'){
        $unit=if($mode -eq 'Before'){'Output/LegacyDropNativeTest/BPTBefore.cpp'}else{'Sources/Core/Src/KBasPropTbl.CPP'}
        & $compiler @args "/Fo$output/Genre$mode.obj" /c $unit
        if($LASTEXITCODE){throw 'Genre catalog compile failed'}
        & $compiler @args "/Fo$output/GenreDump.obj" "/Fe$output/Genre$mode.exe" Tests/LegacyDropNativeTest/CatalogGenreRegression.cpp "$output/Genre$mode.obj" Lib/release/engine.lib Lib/release/common.lib /link kernel32.lib user32.lib
        if($LASTEXITCODE){throw 'Genre dump link failed'}
        Push-Location "$output/LooseFixture"
        try{ & "$output/Genre$mode.exe" (Get-Location).Path "$report/phase2/genre_$mode.csv";if($LASTEXITCODE){throw 'Genre dump failed'} }finally{Pop-Location}
    }
}finally{Pop-Location;$env:LIB=$oldLib;$env:PATH=$oldPath}
