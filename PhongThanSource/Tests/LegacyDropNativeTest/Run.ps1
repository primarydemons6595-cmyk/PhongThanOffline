[CmdletBinding()]
param([ValidateSet(10000,100000,1000000)][int]$Deaths=10000)
$ErrorActionPreference='Stop'
$source=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
$workspace=Split-Path -Parent $source
$server=Join-Path $workspace 'PhongThanRuntime-Loose/Server'
$report=Join-Path $workspace 'analysis/legacy_drop_20261017'
$output=Join-Path $source 'Output/LegacyDropNativeTest'
New-Item -ItemType Directory -Force $output,$report | Out-Null
$compiler=(Get-Command cl.exe -ErrorAction Stop).Source
$vc=Split-Path -Parent (Split-Path -Parent $compiler)
$oldLib=$env:LIB;$oldPath=$env:PATH
try {
    $env:LIB="$vc/Lib;$env:LIB"
    $env:PATH="$source/Output/Server;$vc/Bin;$(Split-Path $vc)/Common/MSDev98/Bin;$env:PATH"
    & node (Join-Path $PSScriptRoot 'audit.mjs') | Out-File -FilePath (Join-Path $report 'audit.log') -Encoding utf8
    if($LASTEXITCODE){throw 'Audit failed'}
    & $compiler /nologo /W3 /GX /MT /O2 "/I$vc/Include" "/I$source/Sources/Core/Src" "/Fo$output/LegacyDropNativeTest.obj" "/Fe$output/LegacyDropNativeTest.exe" (Join-Path $PSScriptRoot 'LegacyDropNativeTest.cpp') 2>&1 | Tee-Object -FilePath (Join-Path $report 'build.log')
    if($LASTEXITCODE){throw 'Native test compile failed'}
    & "$output/LegacyDropNativeTest.exe" "$server/settings/item/droprate" "$report/probability_$Deaths.csv" $Deaths 2>&1 | Tee-Object -FilePath "$report/native_$Deaths.log"
    if($LASTEXITCODE){throw 'Native test failed'}
} finally {$env:LIB=$oldLib;$env:PATH=$oldPath}
