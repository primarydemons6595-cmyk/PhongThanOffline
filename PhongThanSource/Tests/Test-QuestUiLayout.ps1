$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
$out=Join-Path $root 'Output\Tools'
& python (Join-Path $PSScriptRoot 'prepare-quest-ui.py')
if($LASTEXITCODE){throw 'Quest UI preparation failed'}
& 'C:\Program Files (x86)\Microsoft Visual Studio\VC98\Bin\cl.exe' /nologo /MT /GX /W3 /O2 "/I$out" "/Fo$out\QuestUiLayoutTests.obj" "/Fe$out\QuestUiLayoutTests.exe" (Join-Path $PSScriptRoot 'Native\QuestUiLayoutTests.cpp')
if($LASTEXITCODE){throw 'Quest UI compile failed'}
& (Join-Path $out 'QuestUiLayoutTests.exe')
if($LASTEXITCODE){throw 'Quest UI tests failed'}
