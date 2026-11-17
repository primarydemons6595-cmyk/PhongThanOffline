[CmdletBinding()]
param([string]$CompilerPath)
$ErrorActionPreference = 'Stop'
$taskRoot = Split-Path -Parent $PSScriptRoot
$taskTools = Join-Path $taskRoot 'Tools'
if (-not $CompilerPath) {
    $CompilerPath = Get-ChildItem 'C:\Program Files\Microsoft Visual Studio' -Recurse -Filter cl.exe |
        Where-Object FullName -like '*\Hostx64\x64\cl.exe' |
        Sort-Object FullName -Descending | Select-Object -First 1 -ExpandProperty FullName
}
if (-not $CompilerPath) { throw 'Need an x64 MSVC compiler; exporter can still use Python fallback.' }
& $CompilerPath /nologo /O2 /GS- /LD (Join-Path $taskTools 'offline_nrv2b.c') `
    /Fo"$(Join-Path $taskTools 'offline_nrv2b.obj')" `
    /Fe"$(Join-Path $taskTools 'offline_nrv2b.dll')" `
    /link /NOENTRY /NODEFAULTLIB /MACHINE:X64
if ($LASTEXITCODE -ne 0) { throw "Offline decoder build failed: $LASTEXITCODE" }
Get-FileHash -LiteralPath (Join-Path $taskTools 'offline_nrv2b.dll') -Algorithm SHA256
