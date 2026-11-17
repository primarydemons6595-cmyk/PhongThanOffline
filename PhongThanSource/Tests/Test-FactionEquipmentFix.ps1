$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
$out=Join-Path $root 'Output\Tools\ThreeFactionSmoke'
New-Item -ItemType Directory -Force $out|Out-Null
$compiler='C:\Program Files (x86)\Microsoft Visual Studio\VC98\Bin\cl.exe'
& node (Join-Path $PSScriptRoot 'prepare-three-faction-probes.mjs')
if($LASTEXITCODE -ne 0){throw 'Production extraction failed'}
Push-Location $out
try {
 foreach($test in @('FactionEquipmentFixTests','EquipmentAppearanceR2Tests','VuSiLoginProductionTests')){
  $exe=Join-Path $out ($test+'.exe')
  & $compiler /nologo /MT /W3 /O2 "/I$out" "/Fo$out\$test.obj" "/Fe$exe" (Join-Path $PSScriptRoot "Native\$test.cpp")
  if($LASTEXITCODE -ne 0){throw "$test compilation failed"}
  & $exe
  if($LASTEXITCODE -ne 0){throw "$test failed"}
 }
} finally {Pop-Location}
