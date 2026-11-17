[CmdletBinding()]
param(
    [string]$ProjectRoot,
    [string]$RuntimeRoot,
    [string]$MsDevPath
)
$ErrorActionPreference = 'Stop'
if (-not $ProjectRoot) {
    $ProjectRoot = Split-Path -Parent $PSScriptRoot
}
if (-not $RuntimeRoot) {
    $RuntimeRoot = Join-Path (Split-Path -Parent $ProjectRoot) 'PhongThanRuntime-Staging'
}
$visualStudioRoots = New-Object System.Collections.Generic.List[string]
if ($MsDevPath) {
    $msdevBin = Split-Path -Parent ([IO.Path]::GetFullPath($MsDevPath))
    $visualStudioRoots.Add([IO.Path]::GetFullPath((Join-Path $msdevBin '..\..\..')))
}
if ($env:PHONGTHAN_MSDEV) {
    $msdevBin = Split-Path -Parent ([IO.Path]::GetFullPath($env:PHONGTHAN_MSDEV))
    $visualStudioRoots.Add([IO.Path]::GetFullPath((Join-Path $msdevBin '..\..\..')))
}
foreach ($candidate in @(
    'C:\Program Files (x86)\Microsoft Visual Studio',
    'C:\Program Files\Microsoft Visual Studio',
    'D:\VisualStudio6\VisualStudio6portable-langman.congdongcviet\Microsoft Visual Studio',
    'D:\Microsoft Visual Studio'
)) { $visualStudioRoots.Add($candidate) }
$vsRoot = $visualStudioRoots |
    Where-Object { Test-Path -LiteralPath (Join-Path $_ 'VC98\Bin\CL.EXE') } |
    Select-Object -First 1
if (-not $vsRoot) { throw 'Khong tim thay VC6 CL.EXE. Dat PHONGTHAN_MSDEV hoac truyen -MsDevPath.' }
$vc = Join-Path $vsRoot 'VC98'
$oldPath, $oldInclude, $oldLib = $env:PATH, $env:INCLUDE, $env:LIB
try {
    $env:PATH = "$vc\Bin;$(Split-Path -Parent $vc)\Common\MSDev98\Bin;$oldPath"
    $env:INCLUDE = "$vc\Include;$ProjectRoot\Headers;$oldInclude"
    $env:LIB = "$vc\Lib;$oldLib"
    $output = Join-Path $ProjectRoot 'Output\Tools'
    New-Item -ItemType Directory -Path $output -Force | Out-Null
    $exe = Join-Path $output 'CanonicalMapResolverTests.exe'
    & "$vc\Bin\cl.exe" /nologo /MT /W3 /O2 /DWIN32 /DNDEBUG `
        "/Fo$output\CanonicalMapResolverTests.obj" "/Fe$exe" `
        "$ProjectRoot\Tests\Native\CanonicalMapResolverTests.cpp" `
        /link kernel32.lib /subsystem:console /machine:I386
    if ($LASTEXITCODE) { throw 'CanonicalMapResolverTests compile failed' }
    & $exe (Join-Path $RuntimeRoot 'Server')
    if ($LASTEXITCODE) { throw 'CanonicalMapResolverTests failed' }
} finally {
    $env:PATH, $env:INCLUDE, $env:LIB = $oldPath, $oldInclude, $oldLib
}
