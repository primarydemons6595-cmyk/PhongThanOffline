param([string]$RuntimeRoot)
$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
if(-not $RuntimeRoot){$RuntimeRoot=Join-Path (Split-Path -Parent $root) 'PhongThanRuntime-Staging\Server'}
$vcCandidates=@()
if($env:PHONGTHAN_MSDEV){$vcCandidates+=Join-Path (Split-Path -Parent $env:PHONGTHAN_MSDEV) '..\..\..\VC98'}
$vcCandidates+=@(
 'C:\Program Files (x86)\Microsoft Visual Studio\VC98',
 'C:\Program Files\Microsoft Visual Studio\VC98',
 'D:\VisualStudio6\VisualStudio6portable-langman.congdongcviet\Microsoft Visual Studio\VC98'
)
$vc=$vcCandidates | ForEach-Object {[IO.Path]::GetFullPath($_)} | Where-Object {Test-Path -LiteralPath (Join-Path $_ 'Bin\CL.EXE')} | Select-Object -First 1
if(-not $vc){throw 'VC6 CL.EXE not found.'}
$oldPath=$env:PATH;$oldInclude=$env:INCLUDE;$oldLib=$env:LIB
try {
 $env:PATH="$vc\Bin;$(Split-Path $vc)\Common\MSDev98\Bin;$oldPath"
 $env:INCLUDE="$vc\Include;$root\Sources\Engine\Src"
 $env:LIB="$vc\Lib"
 & "$vc\Bin\CL.EXE" /nologo /MT /GX /W0 /DREAL_ENGINE_TABLE "/Fo$root\Output\Tools\EquipmentEngineTests.obj" "/Fe$root\Output\Tools\EquipmentEngineTests.exe" "$root\Tests\Native\EquipmentColumnTests.cpp" "$root\Lib\release\engine.lib" /link kernel32.lib user32.lib
 if($LASTEXITCODE -ne 0){throw 'Engine column test build failed'}
 Push-Location $RuntimeRoot
 try {
  & "$root\Output\Tools\EquipmentEngineTests.exe" package.ini
  if($LASTEXITCODE -ne 0){throw 'Engine column integration failed'}
 } finally {Pop-Location}
} finally {$env:PATH=$oldPath;$env:INCLUDE=$oldInclude;$env:LIB=$oldLib}
