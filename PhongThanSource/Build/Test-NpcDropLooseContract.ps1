$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
$vc=Split-Path -Parent (Split-Path -Parent (Get-Command cl.exe).Source)
$output=Join-Path $root 'Output/NpcDropTests'
$oldLib=$env:LIB; $oldPath=$env:PATH
try{
    $env:LIB="$vc/Lib;$env:LIB"; $env:PATH="$root/Output/Server;$vc/Bin;$(Split-Path $vc)/Common/MSDev98/Bin;$env:PATH"
    Push-Location $root
    $args=@('/nologo','/W3','/GX','/MT','/O2','/DWIN32','/D_SERVER','/D_STANDALONE','/FIIClient.h',"/I$vc/Include",'/IHeaders','/ISources/Core/Src','/ISources/Engine/Src','/ISources/Engine/Include')
    & "$vc/Bin/cl.exe" @args "/Fo$output/DropBPT.obj" /c Sources/Core/Src/KBasPropTbl.CPP
    if($LASTEXITCODE -ne 0){throw 'BPT test compile failed'}
    & "$vc/Bin/cl.exe" @args "/Fo$output/LooseContract.obj" "/Fe$output/LooseContract.exe" Tests/Native/NpcDropLooseContractTests.cpp "$output/DropBPT.obj" Lib/release/engine.lib Lib/release/common.lib /link kernel32.lib user32.lib
    if($LASTEXITCODE -ne 0){throw 'Contract test compile failed'}
    & "$output/LooseContract.exe" (Join-Path (Split-Path -Parent $root) 'PhongThanRuntime-Loose/Server')
    if($LASTEXITCODE -ne 0){throw 'Native loose contract failed'}
}finally{Pop-Location; $env:LIB=$oldLib;$env:PATH=$oldPath}
