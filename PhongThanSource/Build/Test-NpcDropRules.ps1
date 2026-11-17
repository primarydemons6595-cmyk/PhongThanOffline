$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
$vc=Split-Path -Parent (Split-Path -Parent (Get-Command cl.exe -ErrorAction Stop).Source)
$output=Join-Path $root 'Output/NpcDropTests'
New-Item -ItemType Directory -Force -Path $output | Out-Null
$priorPath=$env:PATH; $priorLib=$env:LIB
try {
    $env:PATH="$vc/Bin;$(Split-Path $vc)/Common/MSDev98/Bin;$env:PATH"
    $env:LIB="$vc/Lib;$env:LIB"
    & "$vc/Bin/cl.exe" /nologo /W3 /GX /MT /O2 "/I$vc/Include" "/I$root/Sources/Core/Src" "/Fo$output/Rules.obj" "/Fe$output/Rules.exe" "$root/Tests/Native/NpcDropRulesTests.cpp"
    if($LASTEXITCODE -ne 0){throw 'Compile failed; stop'}
    & "$output/Rules.exe"
    if($LASTEXITCODE -ne 0){throw 'Test failed'}
    & "$vc/Bin/cl.exe" /nologo /W3 /GX /MT /O2 "/I$vc/Include" "/I$root/Sources/Core/Src" "/Fo$output/Integration.obj" "/Fe$output/Integration.exe" "$root/Tests/Native/NpcDropIntegrationTests.cpp"
    if($LASTEXITCODE -ne 0){throw 'Integration compile failed; stop'}
    $fixture=Join-Path ([IO.Path]::GetTempPath()) ('PtDrop_' + [guid]::NewGuid().ToString('N'))
    New-Item -ItemType Directory -Path $fixture | Out-Null
    & "$output/Integration.exe" "$root/Deploy/ProjectContent/NpcDrop/settings/phongthan/NpcDropPolicy.ini" $fixture
    if($LASTEXITCODE -ne 0){throw 'Production inl integration failed'}
} finally {$env:PATH=$priorPath; $env:LIB=$priorLib}
