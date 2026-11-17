$ErrorActionPreference='Stop'
$source=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
$workspace=Split-Path -Parent $source
$output=Join-Path $source 'Output/LegacyDropNativeTest'
$report=Join-Path $workspace 'analysis/legacy_drop_20261017'
$compiler=(Get-Command cl.exe).Source
$vc=Split-Path -Parent (Split-Path -Parent $compiler)
$oldLib=$env:LIB;$oldPath=$env:PATH
try {
    $env:LIB="$vc/Lib;$env:LIB";$env:PATH="$source/Output/Server;$vc/Bin;$(Split-Path $vc)/Common/MSDev98/Bin;$env:PATH"
    & $compiler /nologo /GX /MT /O2 "/I$vc/Include" "/I$source/Sources/Core/Src" "/Fo$output/CoreDropPreflightTest.obj" "/Fe$output/CoreDropPreflightTest.exe" "$PSScriptRoot/CoreDropPreflightTest.cpp"
    if($LASTEXITCODE){throw 'Core preflight client compile failed'}
    Push-Location (Join-Path $output 'LooseFixture')
    try {
        & "$output/CoreDropPreflightTest.exe" "$source/Output/Server/CoreServer.dll" "$workspace/PhongThanRuntime-Loose/Server/settings/item/droprate" "$report/references.hex"
        if($LASTEXITCODE){throw 'Core shared preflight failed'}
    }finally{Pop-Location}
}finally{$env:LIB=$oldLib;$env:PATH=$oldPath}
