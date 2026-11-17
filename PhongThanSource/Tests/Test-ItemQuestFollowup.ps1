$ErrorActionPreference='Stop'
$source=Split-Path $PSScriptRoot -Parent
$workspace=Split-Path $source -Parent
$vc='C:\Program Files (x86)\Microsoft Visual Studio\VC98'
$env:PATH="$vc\Bin;C:\Program Files (x86)\Microsoft Visual Studio\Common\MSDev98\Bin;$source\Output\Tools;$env:PATH"
$env:LIB="$vc\Lib";$env:INCLUDE="$vc\Include"
Push-Location $workspace
try{
 foreach($name in 'TreasureItemTests','SealEquipTests','SpecialBuffTests','F2RequestedGiftTests','QuickUseTests','CoreSaveRetryTests'){
  $compile=@('/nologo','/GX','/MT','/O2','/IPhongThanSource/Headers','/IPhongThanSource/Sources/Engine/Include','/IPhongThanSource/Sources/Engine/Src','/IPhongThanSource/Output/Tools',"PhongThanSource/Tests/Native/$name.cpp","/FePhongThanSource/Output/Tools/$name.exe","/FoPhongThanSource/Output/Tools/$name.obj",'/link','PhongThanSource/Lib/release/engine.lib','PhongThanSource/Lib/release/common.lib','PhongThanSource/Lib/release/LuaLibDll.lib','kernel32.lib','user32.lib')
  & cl @compile
  if($LASTEXITCODE){throw "Compile failed: $name"}
  & (Join-Path $source "Output/Tools/$name.exe")
  if($LASTEXITCODE){throw "Test failed: $name"}
 }
}finally{Pop-Location}
