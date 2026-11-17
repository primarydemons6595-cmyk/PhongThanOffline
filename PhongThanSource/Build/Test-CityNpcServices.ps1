$ErrorActionPreference='Stop'
$project=Split-Path -Parent $PSScriptRoot
$workspace=Split-Path -Parent $project
$vc='C:\Program Files (x86)\Microsoft Visual Studio\VC98'
$savedPath=$env:PATH;$savedInclude=$env:INCLUDE;$savedLib=$env:LIB
Push-Location $workspace
try {
    $env:PATH="$vc\Bin;$project\Output\Tools;$project\Output\Client;$env:PATH"
    $env:INCLUDE="$vc\Include;$project\Headers;$project\Sources\Engine\Src;$project\Sources\Engine\Include;$project\Sources\Core\Src"
    $env:LIB="$vc\Lib"
    foreach($name in @('CityNpcServiceTests','RouletteWireTests','NpcComposeRecipeTests','NpcComposeTransactionTests','EquipmentComposeTests','NpcRewardItemMemoryTests')) {
        $output=Join-Path $project "Output\Tools\$name.exe"
        $obj=[IO.Path]::ChangeExtension($output,'.obj')
        $defines=@()
        if($name -in @('EquipmentComposeTests','NpcRewardItemMemoryTests')){$defines=@('/D_SERVER','/D_STANDALONE','/FIIClient.h')}
        & "$vc\Bin\CL.EXE" /nologo /W3 /GX /MT /O2 /DWIN32 @defines "/Fo$obj" "/Fe$output" `
            "$project\Tests\Native\$name.cpp" "$project\Lib\release\engine.lib" "$project\Lib\LuaLibDll.lib" "$project\Lib\release\common.lib" /link kernel32.lib user32.lib
        if($LASTEXITCODE){throw "Compile failed: $name"}
        & 'C:\Program Files (x86)\Windows Kits\10\bin\10.0.26100.0\x86\mt.exe' -nologo -manifest "$project\Tests\Native\UnitTest.manifest" "-outputresource:$output;#1"
        if($LASTEXITCODE){throw 'Manifest failed'}
        if($name -eq 'CityNpcServiceTests'){& $output 'analysis/restoration_20261016/npc_reward_safety/dispatch_fixtures.txt'}else{& $output "$workspace\PhongThanRuntime-Loose\Server"}
        if($LASTEXITCODE){throw "Test failed: $name"}
    }
} finally {Pop-Location;$env:PATH=$savedPath;$env:INCLUDE=$savedInclude;$env:LIB=$savedLib}
