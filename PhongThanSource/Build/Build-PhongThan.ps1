[CmdletBinding()]
param(
    [ValidateSet('Release', 'Debug')]
    [string]$Configuration = 'Release',
    [string]$MsDevPath,
    [ValidateSet('Dev','SecureRelease')]
    [string]$RuntimeProfile = 'Dev',
    [string[]]$Targets = @(
        'Engine', 'LuaLibDll', 'Common', 'ExpandPackageStaticLib',
        'FilterTextStatic', 'CoreServer', 'CoreClient', 'ExpandPackage',
        'FilterText', 'Heaven', 'Rainbow', 'Represent2', 'Represent3',
        'GameClient', 'AccountServer', 'PhongThanRelay', 'Bishop', 'Goddess',
        'GameServer'
    ),
    [switch]$NoRebuild
)

$ErrorActionPreference = 'Stop'
$root = [IO.Path]::GetFullPath((Split-Path -Parent $PSScriptRoot))
$outputClient = Join-Path $root 'Output\Client'
$outputServer = Join-Path $root 'Output\Server'
$logRoot = Join-Path $root 'Build\Logs'
$profileStamp = Join-Path $root 'Build\native-runtime-profile.txt'
# MSDEV does not track CL environment changes in its dependency database.
# Never reuse secure OBJ/PCH in a subsequent Dev incremental build (or vice versa).
$previousProfile = if (Test-Path -LiteralPath $profileStamp) { [IO.File]::ReadAllText($profileStamp).Trim() } else { '' }
if ($previousProfile -ne $RuntimeProfile) {
    $NoRebuild = $false
    if ($previousProfile) {
        $Targets = @('Engine','LuaLibDll','Common','ExpandPackageStaticLib','FilterTextStatic','CoreServer','CoreClient','ExpandPackage','FilterText','Heaven','Rainbow','Represent2','Represent3','GameClient','AccountServer','PhongThanRelay','Bishop','Goddess','GameServer')
    }
}
if ($RuntimeProfile -eq 'Dev' -and $env:CL -match 'PT_SECURE_RELEASE_PROFILE') { throw 'Secure compiler flags cannot be used for a Dev build.' }
if ($RuntimeProfile -eq 'SecureRelease' -and $env:CL -notmatch 'PT_SECURE_RELEASE_PROFILE') { throw 'SecureRelease requires the secure compiler profile.' }
New-Item -ItemType Directory -Path $outputClient, $outputServer, $logRoot -Force | Out-Null
$gdiInclude = Join-Path $root 'ThirdParty\WindowsSDK\GDIPlus\Include'
$gdiLib = Join-Path $root 'ThirdParty\WindowsSDK\GDIPlus\Lib\x86'
$dxInclude = Join-Path $root 'ThirdParty\dx9csdk\Include'
$dxLib = Join-Path $root 'ThirdParty\dx9csdk\Lib'
if (-not (Test-Path -LiteralPath (Join-Path $gdiInclude 'Gdiplus.h'))) {
    throw 'Thieu GDI+ headers. Chay Tools\Import-WindowsSdkGdiPlus.ps1.'
}
if (-not (Test-Path -LiteralPath (Join-Path $gdiLib 'GdiPlus.lib'))) {
    throw 'Thieu GDI+ x86 import library. Chay Tools\Import-WindowsSdkGdiPlus.ps1.'
}
foreach ($required in 'd3d9types.h', 'd3dx9.h') {
    if (-not (Test-Path -LiteralPath (Join-Path $dxInclude $required))) {
        throw "Thieu DirectX 9 SDK header: $required"
    }
}
foreach ($required in 'd3dx9dt.lib', 'd3d9.lib', 'dxguid.lib', 'ddraw.lib') {
    if (-not (Test-Path -LiteralPath (Join-Path $dxLib $required))) {
        throw "Thieu DirectX 9 SDK library: $required"
    }
}
$env:INCLUDE = "$gdiInclude;$dxInclude;$($env:INCLUDE)"
$env:LIB = "$gdiLib;$dxLib;$($env:LIB)"

function Resolve-MsDev([string]$ExplicitPath) {
    $candidates = New-Object System.Collections.Generic.List[string]
    if ($ExplicitPath) { $candidates.Add($ExplicitPath) }
    if ($env:PHONGTHAN_MSDEV) { $candidates.Add($env:PHONGTHAN_MSDEV) }
    foreach ($name in 'MSDEV.COM', 'MSDEV.EXE') {
        $command = Get-Command $name -ErrorAction SilentlyContinue
        if ($command) { $candidates.Add($command.Source) }
    }
    foreach ($path in @(
        'D:\VisualStudio6\VisualStudio6portable-langman.congdongcviet\Microsoft Visual Studio\COMMON\MSDev98\Bin\MSDEV.COM',
        'C:\Program Files (x86)\Microsoft Visual Studio\Common\MSDev98\Bin\MSDEV.COM',
        'C:\Program Files\Microsoft Visual Studio\Common\MSDev98\Bin\MSDEV.COM',
        'D:\Microsoft Visual Studio\Common\MSDev98\Bin\MSDEV.COM'
    )) { $candidates.Add($path) }

    foreach ($candidate in $candidates) {
        if (Test-Path -LiteralPath $candidate) {
            $resolved = (Resolve-Path -LiteralPath $candidate).Path
            if ([IO.Path]::GetExtension($resolved) -ieq '.exe') {
                $console = [IO.Path]::ChangeExtension($resolved, '.com')
                if (Test-Path -LiteralPath $console) { return $console }
            }
            return $resolved
        }
    }
    throw 'Khong tim thay MSDEV.COM/MSDEV.EXE. Dat bien PHONGTHAN_MSDEV hoac truyen -MsDevPath.'
}

$projects = @{
    Engine = @{ Project='Sources\Engine\Engine.dsp'; Config="Engine - Win32 $Configuration"; Artifacts=@(@{Path='Sources\Engine\Release\Engine.dll';Role='Client'},@{Path='Sources\Engine\Release\Engine.dll';Role='Server'}) }
    LuaLibDll = @{ Project='Sources\Library\LuaLib\LuaLibDll.dsp'; Config="LuaLibDll - Win32 $Configuration"; Artifacts=@(@{Path='Sources\Library\LuaLib\Release\LuaLibDll.dll';Role='Client'},@{Path='Sources\Library\LuaLib\Release\LuaLibDll.dll';Role='Server'}) }
    Common = @{ Project='Sources\MultiServer\Common\Common.dsp'; Config="Common - Win32 $Configuration"; Artifacts=@() }
    ExpandPackageStaticLib = @{ Project='Sources\ExpandPackageStaticLib\ExpandPackageStaticLib.dsp'; Config="ExpandPackageStaticLib - Win32 $Configuration"; Artifacts=@() }
    FilterTextStatic = @{ Project='Sources\FilterText\FilterText_StaticLib.dsp'; Config="FilterText_StaticLib - Win32 $Configuration"; Artifacts=@() }
    CoreServer = @{ Project='Sources\Core\Core.dsp'; Config="Core - Win32 Server $Configuration"; Artifacts=@(@{Path="Sources\Core\Server$Configuration\CoreServer.dll";Role='Server'}) }
    CoreClient = @{ Project='Sources\Core\Core.dsp'; Config="Core - Win32 Client $Configuration"; Artifacts=@(@{Path="Sources\Core\Client$Configuration\CoreClient.dll";Role='Client'}) }
    ExpandPackage = @{ Project='Sources\ExpandPackage2.0\ExpandPackage.dsp'; Config="ExpandPackage - Win32 $Configuration"; Artifacts=@(@{Path="Sources\ExpandPackage2.0\$Configuration\ExpandPackage.dll";Role='Client'},@{Path="Sources\ExpandPackage2.0\$Configuration\ExpandPackage.dll";Role='Server'}) }
    FilterText = @{ Project='Sources\FilterText\FilterText.dsp'; Config="FilterText - Win32 $Configuration"; Artifacts=@(@{Path="Sources\FilterText\$Configuration\FilterText.dll";Role='Client'},@{Path="Sources\FilterText\$Configuration\FilterText.dll";Role='Server'}) }
    Heaven = @{ Project='Sources\MultiServer\Heaven\Heaven.dsp'; Config="Heaven - Win32 $Configuration"; Artifacts=@(@{Path="Sources\MultiServer\Heaven\$Configuration\Heaven.dll";Role='Client'},@{Path="Sources\MultiServer\Heaven\$Configuration\Heaven.dll";Role='Server'}) }
    Rainbow = @{ Project='Sources\MultiServer\Rainbow\Rainbow.dsp'; Config="Rainbow - Win32 $Configuration"; Artifacts=@(@{Path="Sources\MultiServer\Rainbow\$Configuration\Rainbow.dll";Role='Client'},@{Path="Sources\MultiServer\Rainbow\$Configuration\Rainbow.dll";Role='Server'}) }
    Represent2 = @{ Project='Sources\Represent\Represent2\Represent2.dsp'; Config="Represent2 - Win32 $Configuration"; Artifacts=@(@{Path="Sources\Represent\Represent2\$Configuration\Represent2.dll";Role='Client'}) }
    Represent3 = @{ Project='Sources\Represent\Represent3\Represent3.dsp'; Config="Represent3 - Win32 $Configuration"; Artifacts=@(@{Path="Sources\Represent\Represent3\$Configuration\Represent3.dll";Role='Client'}) }
    GameClient = @{ Project='Sources\GameClient\PhongThanClient.dsp'; Config="PhongThanClient - Win32 $Configuration"; Artifacts=@(@{Path="Sources\GameClient\$Configuration\Game.exe";Role='Client'}) }
    AccountServer = @{ Project='Sources\AccountServices\AccountServer\PhongThanAccountServer.dsp'; Config="PhongThanAccountServer - Win32 $Configuration"; Artifacts=@(@{Path="Sources\AccountServices\AccountServer\$Configuration\PhongThanAccountServer.exe";Role='Server'}) }
    PhongThanRelay = @{ Project='Sources\MultiServer\PhongThanRelay\PhongThanRelay.dsp'; Config="PhongThanRelay - Win32 $Configuration"; Artifacts=@(@{Path="Sources\MultiServer\PhongThanRelay\$Configuration\PhongThanRelay.exe";Role='Server'}) }
    Bishop = @{ Project='Sources\MultiServer\Bishop\Bishop.dsp'; Config="Bishop - Win32 $Configuration"; Artifacts=@(@{Path="Sources\MultiServer\Bishop\$Configuration\Bishop.exe";Role='Server'}) }
    Goddess = @{ Project='Sources\MultiServer\Goddess\Goddess.dsp'; Config="Goddess - Win32 $Configuration"; Artifacts=@(@{Path="Sources\MultiServer\Goddess\$Configuration\Goddess.exe";Role='Server'}) }
    GameServer = @{ Project='Sources\MultiServer\GameServer\GameServer.dsp'; Config="GameServer - Win32 $Configuration"; Artifacts=@(@{Path="Sources\MultiServer\GameServer\$Configuration\GameServer.exe";Role='Server'}) }
}

$msdev = Resolve-MsDev $MsDevPath
$vc6Root = Join-Path (Split-Path -Parent (Split-Path -Parent (Split-Path -Parent (Split-Path -Parent $msdev)))) 'VC98'
$results = New-Object System.Collections.Generic.List[object]
[IO.File]::WriteAllText($profileStamp,"building:$RuntimeProfile")
foreach ($target in $Targets) {
    if (-not $projects.ContainsKey($target)) { throw "Target khong hop le: $target" }
    $item = $projects[$target]
    $projectPath = Join-Path $root $item.Project
    if (-not (Test-Path -LiteralPath $projectPath)) { throw "Thieu project: $projectPath" }
    $log = Join-Path $logRoot ("{0}-{1}.log" -f $target, $Configuration)
    $arguments = @($projectPath, '/MAKE', $item.Config)
    if ($target -eq 'Engine') {
        # MSDEV's saved include directories otherwise select its old DX header
        # ahead of the imported DX9 SDK, hiding IDirectInput8 declarations.
        if (-not (Test-Path -LiteralPath (Join-Path $vc6Root 'Include\windows.h'))) {
            throw "Thieu VC6 SDK: $vc6Root"
        }
        $env:INCLUDE = "$gdiInclude;$dxInclude;$(Join-Path $vc6Root 'Include');$($env:INCLUDE)"
        $env:LIB = "$gdiLib;$dxLib;$(Join-Path $vc6Root 'Lib');$($env:LIB)"
        $arguments += '/USEENV'
    }
    if (-not $NoRebuild) { $arguments += '/REBUILD' }
    $arguments += @('/OUT', $log)
    & $msdev @arguments
    $exitCode = $LASTEXITCODE
    if ($exitCode -ne 0) { throw "Build $target that bai ($exitCode). Xem $log" }

    foreach ($artifact in $item.Artifacts) {
        $source = Join-Path $root $artifact.Path
        if (-not (Test-Path -LiteralPath $source)) { throw "Build thanh cong nhung thieu artifact: $source" }
        $destinationRoot = if ($artifact.Role -eq 'Client') { $outputClient } else { $outputServer }
        Copy-Item -LiteralPath $source -Destination (Join-Path $destinationRoot (Split-Path -Leaf $source)) -Force
    }
    if ($target -in @('Represent2', 'Represent3')) {
        $renderDll = Join-Path $outputClient ("{0}.dll" -f $target)
        & (Join-Path $root 'Tests\Test-GdiPlusAbi.ps1') -DllPath $renderDll | Out-Null
    }
    $results.Add([pscustomobject]@{ Target=$target; ExitCode=$exitCode; Log=$log })
}
if ($Targets -contains 'CoreServer' -or $Targets -contains 'CoreClient') {
    & (Join-Path $root 'Tests\Test-PhongThanItemBaseline.ps1') -ProjectRoot $root | Out-Null
    & (Join-Path $root 'Tests\Test-PhongThanProtocolBaseline.ps1') -ProjectRoot $root | Out-Null
    & (Join-Path $root 'Tests\Test-PhongThanProfessionSkills.ps1') -ProjectRoot $root | Out-Null
}
if ($Targets -contains 'AccountServer' -or
    $Targets -contains 'Bishop' -or
    $Targets -contains 'GameServer') {
    & (Join-Path $root 'Tests\Test-PhongThanServiceControlPlane.ps1') -ProjectRoot $root | Out-Null
}
[IO.File]::WriteAllText($profileStamp,$RuntimeProfile)
$results
