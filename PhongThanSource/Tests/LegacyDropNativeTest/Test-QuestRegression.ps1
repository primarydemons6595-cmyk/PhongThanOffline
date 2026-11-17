$ErrorActionPreference='Stop'
$source=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
$vc='C:\Program Files (x86)\Microsoft Visual Studio\VC98'
$output=Join-Path $source 'Output/LegacyDropNativeTest/QuestRegression'
New-Item -ItemType Directory -Force $output | Out-Null
$oldLib=$env:LIB;$oldPath=$env:PATH
Push-Location $source
try {
 $env:LIB="$vc/Lib;$env:LIB";$env:PATH="$source/Output/Server;$vc/Bin;$(Split-Path $vc)/Common/MSDev98/Bin;$env:PATH"
 foreach($name in @('QuestLuaContextTests','QuestExchangeTests','OriginalNpcQuestTests','QuestDeathProgressTests','EscortQuestTests')){
  & "$vc/Bin/cl.exe" /nologo /W3 /GX /MT /O2 /DWIN32 "/I$vc/Include" /ISources/Engine/Src /ISources/Engine/Include "/Fo$output/$name.obj" "/Fe$output/$name.exe" "Tests/Native/$name.cpp" Lib/release/engine.lib Lib/LuaLibDll.lib /link kernel32.lib user32.lib
  if($LASTEXITCODE){throw "Compile $name failed"}
  $testArgs=@();if($name -eq 'OriginalNpcQuestTests'){$testArgs+=Join-Path $source 'Tests/fixtures/NpcQuestOriginal'}
  if($name -eq 'QuestDeathProgressTests'){$testArgs+=Join-Path $source 'Deploy/ProjectContent/NpcQuests/compiled'}
  if($name -eq 'EscortQuestTests'){$testArgs+=Join-Path (Split-Path $source) 'analysis/event_service_20261019/staged/Server'}
  & "$output/$name.exe" @testArgs
  if($LASTEXITCODE){throw "$name regression failed"}
 }
}finally{Pop-Location;$env:LIB=$oldLib;$env:PATH=$oldPath}
