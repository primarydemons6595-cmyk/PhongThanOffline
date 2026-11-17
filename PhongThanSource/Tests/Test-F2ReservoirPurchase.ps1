$ErrorActionPreference='Stop'
$project=Split-Path $PSScriptRoot -Parent
$out=Join-Path $project 'Output/Tools/F2ReservoirTests'
New-Item -ItemType Directory -Force $out | Out-Null
$source=Get-Content -Raw (Join-Path $project 'Sources/Core/Src/PhongThanXuExchange.inl')
$match=[regex]::Match($source,'(?ms)^int PhongThanBuyWithXu\([^\r\n]+\)\s*\{.*?^\}')
if(!$match.Success){throw 'Cannot extract actual F2 transaction entry point'}
[IO.File]::WriteAllText((Join-Path $out 'F2PurchaseNative.inc'),$match.Value,[Text.Encoding]::ASCII)
$manifest=Get-Content -Raw (Join-Path $project 'Deploy/ProjectContent/settings/phongthan/ReservoirTooltips.json') | ConvertFrom-Json
$rows=foreach($row in $manifest.rows){"$($row.detail)`t$($row.part)`t$($row.capacity)`t$($row.effect)`t$($row.rate)"}
$tsv=Join-Path $out 'rows.tsv';[IO.File]::WriteAllLines($tsv,[string[]]$rows,[Text.Encoding]::ASCII)
foreach($dll in 'Engine.dll','LuaLibDll.dll'){Copy-Item -LiteralPath (Join-Path $project "Output/Server/$dll") -Destination $out -Force}
$vc='C:/Program Files (x86)/Microsoft Visual Studio/VC98'
$oldPath=$env:PATH;$oldLib=$env:LIB
try {
    $env:PATH="$vc/Bin;C:/Program Files (x86)/Microsoft Visual Studio/Common/MSDev98/Bin;$oldPath";$env:LIB="$vc/Lib;$oldLib"
    & "$vc/Bin/cl.exe" /nologo /W3 /GX /MT /O2 /DWIN32 /DPT_TEST_F2 "/I$vc/Include" "/I$out" "/I$project/Sources/Engine/Src" "/Fo$out/test.obj" "/Fe$out/test.exe" "$project/Tests/Native/QuestExchangeTests.cpp" "$project/Lib/release/engine.lib" "$project/Lib/LuaLibDll.lib" /link kernel32.lib user32.lib
    if($LASTEXITCODE){throw 'F2 reservoir tests compile failed'}
    & "$out/test.exe" $tsv
    if($LASTEXITCODE){throw 'F2 reservoir tests failed'}
} finally {$env:PATH=$oldPath;$env:LIB=$oldLib}
