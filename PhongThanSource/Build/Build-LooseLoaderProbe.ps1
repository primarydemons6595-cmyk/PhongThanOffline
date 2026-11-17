[CmdletBinding()]
param([string]$ProjectRoot)
$ErrorActionPreference='Stop'
if (-not $ProjectRoot) { $ProjectRoot=Split-Path -Parent $PSScriptRoot }
$ProjectRoot=[IO.Path]::GetFullPath($ProjectRoot)
$vcRoot='C:\Program Files (x86)\Microsoft Visual Studio\VC98'
$compiler=Join-Path $vcRoot 'Bin\CL.EXE'
$engine=Join-Path $ProjectRoot 'Sources\Engine\Src'
$lib=Join-Path $ProjectRoot 'Sources\Engine\Release\Engine.lib'
$out=Join-Path $ProjectRoot 'Output\Tools'
New-Item -ItemType Directory -Path $out -Force | Out-Null
$env:INCLUDE="$(Join-Path $ProjectRoot 'ThirdParty\dx9csdk\Include');$(Join-Path $vcRoot 'Include');$engine"
$env:LIB=Join-Path $vcRoot 'Lib'
$env:PATH="$(Join-Path $vcRoot 'Bin');C:\Program Files (x86)\Microsoft Visual Studio\Common\MSDev98\Bin;$env:PATH"
& $compiler /nologo /MT /W3 /O2 /D WIN32 /D NDEBUG "/Fo$out\LooseLoaderProbe.obj" "/Fe$out\LooseLoaderProbe.exe" `
    (Join-Path $ProjectRoot 'Tests\Native\LooseLoaderProbe.cpp') $lib /link /subsystem:console /machine:I386
if ($LASTEXITCODE -ne 0) { throw 'LooseLoaderProbe build failed' }
Copy-Item -LiteralPath (Join-Path $ProjectRoot 'Output\Client\Engine.dll') -Destination $out -Force
Copy-Item -LiteralPath (Join-Path $ProjectRoot 'Output\Client\LuaLibDll.dll') -Destination $out -Force
Write-Output 'LooseLoaderProbe build PASS'
