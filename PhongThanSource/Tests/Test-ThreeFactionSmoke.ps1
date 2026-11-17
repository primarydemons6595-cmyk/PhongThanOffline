param([uint32]$Seed=20261027, [switch]$DisableAoe, [switch]$DisableBlocking, [switch]$VerifyMutations)
$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
$out=Join-Path $root 'Output\Tools\ThreeFactionSmoke'
New-Item -ItemType Directory -Force $out | Out-Null
$compiler='C:\Program Files (x86)\Microsoft Visual Studio\VC98\Bin\cl.exe'
$exe=Join-Path $out 'ThreeFactionSmoke.exe'
& node (Join-Path $PSScriptRoot 'prepare-three-faction-probes.mjs')
if($LASTEXITCODE -ne 0){throw 'Production probe preparation failed'}
Push-Location $out
try {
    $probe=Join-Path $out 'ThreeFactionProductionProbes.exe'
    & $compiler /nologo /MT /W3 /O2 "/I$out" "/Fo$out\ThreeFactionProductionProbes.obj" "/Fe$probe" (Join-Path $PSScriptRoot 'Native\ThreeFactionProductionProbes.cpp')
    if($LASTEXITCODE -ne 0){throw 'Production probe compilation failed'}
    & $probe
    if($LASTEXITCODE -ne 0){throw 'Production mechanism probes failed'}
    & $compiler /nologo /MT /W3 /O2 "/Fo$out\ThreeFactionSmoke.obj" "/Fe$exe" (Join-Path $PSScriptRoot 'Native\ThreeFactionSmoke.cpp')
    if($LASTEXITCODE -ne 0){throw 'Three-faction native smoke compilation failed'}
    $arguments=@('--seed',"$Seed")
    if($DisableAoe){$arguments+='--disable-aoe'}
    if($DisableBlocking){$arguments+='--disable-blocking'}
    & $exe @arguments
    if($LASTEXITCODE -ne 0){throw 'Three-faction smoke assertions failed'}
    if($VerifyMutations){
        foreach($mutation in @('--disable-aoe','--disable-blocking')){
            & $exe --seed $Seed $mutation
            if($LASTEXITCODE -ne 1){throw "Mutation was not detected: $mutation"}
            Write-Host "PASS: detected $mutation"
        }
    }
} finally {Pop-Location}
