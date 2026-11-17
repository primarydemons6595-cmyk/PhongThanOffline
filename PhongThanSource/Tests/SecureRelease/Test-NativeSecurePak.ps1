param([Parameter(Mandatory=$true)][string]$Fixture,[string]$Compiler='C:\Program Files (x86)\Microsoft Visual Studio\VC98\bin\CL.EXE')
$ErrorActionPreference='Stop'
$fixtureRoot=[IO.Path]::GetFullPath($Fixture)
$projectRoot=[IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
$env:LIB='C:\Program Files (x86)\Microsoft Visual Studio\VC98\Lib'
& $Compiler /nologo /O2 /EHsc /MD /DPT_RELEASE_TEST_KEYS ('/I'+$fixtureRoot) (Join-Path $projectRoot 'Tests\Native\SecurePakProbe.cpp') ('/Fo'+(Join-Path $fixtureRoot 'native.obj')) ('/Fe'+(Join-Path $fixtureRoot 'native.exe'))
if($LASTEXITCODE){throw 'Native secure reader build failed'}
$zlib=Join-Path (Split-Path -Parent $projectRoot) 'PhongThanRuntime-Loose\Client\zlib.dll'
$results=New-Object System.Collections.Generic.List[object]
foreach($case in @(
 @{Folder='.';Resource='\settings\private-test.txt';Bytes=524605;Exit=0},
 @{Folder='collision-runtime';Resource='\settings\private-test.txt';Bytes=524605;Exit=0},
 @{Folder='collision-runtime';Resource='\settings\collision-b.txt';Bytes=11;Exit=0},
 @{Folder='odd-runtime';Resource='\settings\private-tests.txt';Bytes=524605;Exit=0},
 @{Folder='deletion-runtime';Resource='\settings\private-test.txt';Bytes=0;Exit=4},
 @{Folder='forgery-runtime';Resource='\settings\private-test.txt';Bytes=0;Exit=5},
 @{Folder='tamper-runtime';Resource='\settings\private-test.txt';Bytes=0;Exit=5}
 ,@{Folder='alias-removal-runtime';Resource='\settings\private-test.txt';Bytes=524605;Exit=0}
 ,@{Folder='winner-change-runtime';Resource='\settings\private-test.txt';Bytes=524605;Exit=0}
 ,@{Folder='winner-change-runtime';Resource='\settings\collision-b.txt';Bytes=11;Exit=0}
 ,@{Folder='deleted-collision-runtime';Resource='\settings\private-test.txt';Bytes=0;Exit=4}
 ,@{Folder='deleted-collision-runtime';Resource='\settings\collision-b.txt';Bytes=11;Exit=0}
)) {
 $directory=Join-Path $fixtureRoot $case.Folder;$executable=Join-Path $directory 'native.exe'
 if($case.Folder -ne '.') { Copy-Item -LiteralPath (Join-Path $fixtureRoot 'native.exe') -Destination $executable }
 Copy-Item -LiteralPath $zlib -Destination (Join-Path $directory 'zlib.dll')
 & $executable $case.Resource ([string]$case.Bytes);$exitCode=$LASTEXITCODE
 if($exitCode -ne $case.Exit){throw "Native regression failed: $($case.Folder) exit=$exitCode expected=$($case.Exit)"}
 $results.Add(@{Case=$case.Folder;Resource=$case.Resource;Exit=$exitCode;Pass=$true})
}
$catalog=Join-Path $fixtureRoot 'Data\release.catalog';$bytes=[IO.File]::ReadAllBytes($catalog);$original=$bytes.Clone();$bytes[12]=$bytes[12] -bxor 1
try { [IO.File]::WriteAllBytes($catalog,$bytes); & (Join-Path $fixtureRoot 'native.exe') '\settings\private-test.txt';if($LASTEXITCODE -ne 3){throw 'Native catalog signature bypass'};$results.Add(@{Case='catalog-tamper';Exit=3;Pass=$true}) }
finally { [IO.File]::WriteAllBytes($catalog,$original) }
$results | ConvertTo-Json | Set-Content (Join-Path $fixtureRoot 'native-results.json') -Encoding UTF8
Write-Output ('NATIVE_ALL_PASS checks='+$results.Count)
