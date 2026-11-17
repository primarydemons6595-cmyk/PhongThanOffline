$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
$vc='C:\Program Files (x86)\Microsoft Visual Studio\VC98'
$env:PATH="$vc\Bin;$(Split-Path $vc)\Common\MSDev98\Bin;$env:PATH"
$env:INCLUDE="$vc\Include;$env:INCLUDE";$env:LIB="$vc\Lib;$env:LIB"
$out=Join-Path $root 'Output/Tools'
Push-Location $root
try {
 foreach($mode in @('Server','Client')){
  $extra=@(if($mode -eq 'Client'){'/DPT_REMOTE_CLIENT_TEST'})
  & "$vc/Bin/cl.exe" /nologo /W3 /GX /MT /O2 /DWIN32 /D_SERVER /D_STANDALONE /FIIClient.h @extra /IHeaders /ISources/Core/Src /ISources/Engine/Src /ISources/Engine/Include "/Fo$out/RemotePeople$mode.obj" "/Fe$out/RemotePeople$mode.exe" Tests/Native/RemotePeopleChatTests.cpp /link kernel32.lib user32.lib
  if($LASTEXITCODE){throw "Compile remote people $mode failed"}
  & "$out/RemotePeople$mode.exe"
  if($LASTEXITCODE){throw "Remote people $mode test failed"}
 }
} finally {Pop-Location}
