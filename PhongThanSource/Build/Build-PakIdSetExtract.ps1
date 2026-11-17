[CmdletBinding()]
param(
    [string]$ProjectRoot = (Split-Path -Parent $PSScriptRoot),
    [string]$CompilerPath,
    [string]$OutputPath
)
$ErrorActionPreference = 'Stop'
$ProjectRoot = [IO.Path]::GetFullPath($ProjectRoot).TrimEnd('\')
if (-not $OutputPath) { $OutputPath = Join-Path $ProjectRoot 'Output\Tools\PakIdSetExtract.exe' }
if (-not $CompilerPath) {
    $CompilerPath = @(
        $env:PHONGTHAN_CL,
        'C:\Program Files (x86)\Microsoft Visual Studio\VC98\Bin\CL.EXE'
    ) | Where-Object { $_ -and (Test-Path -LiteralPath $_ -PathType Leaf) } | Select-Object -First 1
}
if (-not $CompilerPath) { throw 'Khong tim thay VC6 CL.EXE.' }
$vcRoot = Split-Path -Parent (Split-Path -Parent $CompilerPath)
$commonBin = Join-Path (Split-Path -Parent $vcRoot) 'Common\MSDev98\Bin'
$toolSource = Join-Path $ProjectRoot 'Tests\Native\PakIdSetExtract.cpp'
$uclSource = Join-Path $ProjectRoot 'Sources\Engine\Src\ucl\n2b_ds.c'
$toolObject = [IO.Path]::ChangeExtension($OutputPath, '.obj')
$uclObject = Join-Path (Split-Path -Parent $OutputPath) 'n2b_ds.obj'
New-Item -ItemType Directory -Path (Split-Path -Parent $OutputPath) -Force | Out-Null
$oldInclude, $oldLib, $oldPath = $env:INCLUDE, $env:LIB, $env:PATH
try {
    $env:INCLUDE = "$(Join-Path $vcRoot 'Include');$(Join-Path $ProjectRoot 'Sources\Engine\Include');$(Join-Path $ProjectRoot 'Sources\Engine\Src\ucl');$oldInclude"
    $env:LIB = "$(Join-Path $vcRoot 'Lib');$oldLib"
    $env:PATH = "$(Split-Path -Parent $CompilerPath);$commonBin;$oldPath"
    & $CompilerPath /nologo /MT /W3 /O2 /GX /c "/Fo$toolObject" $toolSource
    if ($LASTEXITCODE) { throw 'PakIdSetExtract compile failed.' }
    & $CompilerPath /nologo /MT /W3 /O2 /c "/Fo$uclObject" $uclSource
    if ($LASTEXITCODE) { throw 'UCL decoder compile failed.' }
    & (Join-Path (Split-Path -Parent $CompilerPath) 'LINK.EXE') /nologo "/out:$OutputPath" /subsystem:console /machine:I386 $toolObject $uclObject
    if ($LASTEXITCODE) { throw 'PakIdSetExtract link failed.' }
}
finally {
    $env:INCLUDE, $env:LIB, $env:PATH = $oldInclude, $oldLib, $oldPath
}
[pscustomobject]@{ Status='PASS'; Output=$OutputPath; Sha256=(Get-FileHash -LiteralPath $OutputPath -Algorithm SHA256).Hash }
