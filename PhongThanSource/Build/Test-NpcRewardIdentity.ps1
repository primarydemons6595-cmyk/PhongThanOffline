$ErrorActionPreference='Stop'
$project=Split-Path -Parent $PSScriptRoot
$vc='C:\Program Files (x86)\Microsoft Visual Studio\VC98'
$savedPath=$env:PATH;$savedInclude=$env:INCLUDE;$savedLib=$env:LIB
Push-Location $project
try {
    $env:PATH="$vc\Bin;$project\Output\Client;$env:PATH"
    $env:INCLUDE="$vc\Include;$project\Headers;$project\Sources\Core\Src;$project\Sources\Engine\Src;$project\Sources\Engine\Include"
    $env:LIB="$vc\Lib"
    foreach($spec in @(@('Sources/Core/Src/KBasPropTbl.cpp','NpcRewardTables'),@('Tests/Native/QuestItemTupleTests.cpp','NpcRewardTuple'))) {
        & "$vc\Bin\CL.EXE" /nologo /W3 /GX /MT /O2 /DWIN32 /D_SERVER /D_STANDALONE /FIIClient.h "/FoOutput/Tools/$($spec[1]).obj" /c $spec[0]
        if($LASTEXITCODE){throw 'Compile reward registry/tuple test failed'}
    }
    & "$vc\Bin\LINK.EXE" /nologo /OUT:Output/Tools/NpcRewardIdentity.exe Output/Tools/NpcRewardTables.obj Output/Tools/NpcRewardTuple.obj Lib/release/engine.lib Lib/release/common.lib kernel32.lib user32.lib
    if($LASTEXITCODE){throw 'Link reward identity test failed'}
    & 'C:\Program Files (x86)\Windows Kits\10\bin\10.0.26100.0\x86\mt.exe' -nologo -manifest Tests/Native/UnitTest.manifest '-outputresource:Output/Tools/NpcRewardIdentity.exe;#1'
    if($LASTEXITCODE){throw 'Manifest failed'}
    & Output/Tools/NpcRewardIdentity.exe (Join-Path (Split-Path -Parent $project) 'PhongThanRuntime-Loose/Server')
    if($LASTEXITCODE){throw 'Reward identity test failed'}
} finally {Pop-Location;$env:PATH=$savedPath;$env:INCLUDE=$savedInclude;$env:LIB=$savedLib}
