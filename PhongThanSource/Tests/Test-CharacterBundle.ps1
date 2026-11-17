$ErrorActionPreference='Stop'
$workspace=Split-Path (Split-Path $PSScriptRoot -Parent) -Parent
$vc='C:\Program Files (x86)\Microsoft Visual Studio\VC98'
$env:PATH="$vc\Bin;C:\Program Files (x86)\Microsoft Visual Studio\Common\MSDev98\Bin;$env:PATH"
$env:LIB="$vc\Lib";$env:INCLUDE="$vc\Include"
Push-Location $workspace
try {
 & cl /nologo /GX /MT /O2 /IPhongThanSource/Headers /IPhongThanSource/Sources/MultiServer/Goddess /IPhongThanSource/Sources/Engine/Include /IPhongThanSource/Sources/Common /IPhongThanSource/Sources/MultiServer/Heaven /IPhongThanSource/Sources/MultiServer/Include /IPhongThanSource/Sources/MultiServer/Headers PhongThanSource/Tests/Native/CharacterBundleTests.cpp PhongThanSource/Sources/MultiServer/Goddess/PhongThanCharacterStore.cpp /FePhongThanSource/Output/Tools/CharacterBundleTests.exe /FoPhongThanSource/Output/Tools/ /link kernel32.lib user32.lib
 if($LASTEXITCODE){throw 'Compile CharacterBundleTests failed'}
 $fixture=Join-Path $workspace ('analysis/single_pthc_20261101/native-'+[Guid]::NewGuid().ToString('N'))
 & ./PhongThanSource/Output/Tools/CharacterBundleTests.exe $fixture
 if($LASTEXITCODE){throw 'CharacterBundleTests failed'}
}finally{Pop-Location}
