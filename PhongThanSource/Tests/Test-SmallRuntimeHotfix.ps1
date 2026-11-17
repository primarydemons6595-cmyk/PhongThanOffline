[CmdletBinding()]
param([string]$ProjectRoot)
$ErrorActionPreference = 'Stop'
if (-not $ProjectRoot) { $ProjectRoot = Split-Path -Parent $PSScriptRoot }
$vcRoot = 'C:\Program Files (x86)\Microsoft Visual Studio\VC98'
$compiler = Join-Path $vcRoot 'Bin\cl.exe'
if (-not (Test-Path -LiteralPath $compiler)) { throw "Compiler missing: $compiler" }
$output = Join-Path $ProjectRoot 'Output\Tools'
New-Item -ItemType Directory -Path $output -Force | Out-Null
$exe = Join-Path $output 'SmallRuntimeHotfixTests.exe'
$obj = Join-Path $output 'SmallRuntimeHotfixTests.obj'
$oldPath = $env:PATH
$oldLib = $env:LIB
try {
    $env:PATH = (Join-Path $vcRoot 'Bin') + ';' +
        (Join-Path (Split-Path -Parent $vcRoot) 'Common\MSDev98\Bin') + ';' + $oldPath
    $env:LIB = (Join-Path $vcRoot 'Lib') + ';' + $oldLib
    & $compiler /nologo /W3 /GX /MT /O2 "/I$(Join-Path $vcRoot 'Include')" `
        "/I$(Join-Path $ProjectRoot 'Headers')" "/Fo$obj" "/Fe$exe" `
        (Join-Path $PSScriptRoot 'SmallRuntimeHotfixTests.cpp') /link kernel32.lib
    if ($LASTEXITCODE -ne 0) { throw 'Small runtime hotfix test compilation failed.' }
    & $exe
    if ($LASTEXITCODE -ne 0) { throw 'Small runtime hotfix native tests failed.' }
} finally {
    $env:PATH = $oldPath
    $env:LIB = $oldLib
}

$itemSource = Get-Content -LiteralPath (Join-Path $ProjectRoot 'Sources\Core\Src\KItemList.cpp') -Raw -Encoding Default
$sceneSource = Get-Content -LiteralPath (Join-Path $ProjectRoot 'Sources\Core\Src\Scene\KScenePlaceC.cpp') -Raw -Encoding Default
$baseTableSource = Get-Content -LiteralPath (Join-Path $ProjectRoot 'Sources\Core\Src\KBasPropTbl.cpp') -Raw -Encoding Default
$npcSource = Get-Content -LiteralPath (Join-Path $ProjectRoot 'Sources\Core\Src\KNpc.cpp') -Raw -Encoding Default
$npcResNodeSource = Get-Content -LiteralPath (Join-Path $ProjectRoot 'Sources\Core\Src\KNpcResNode.cpp') -Raw -Encoding Default
$protocolSource = Get-Content -LiteralPath (Join-Path $ProjectRoot 'Sources\Core\Src\KProtocolProcess.cpp') -Raw -Encoding Default
$roleFilterSource = Get-Content -LiteralPath (Join-Path $ProjectRoot 'Sources\MultiServer\Goddess\RoleNameFilter.cpp') -Raw -Encoding Default
$gamePlayerSource = Get-Content -LiteralPath (Join-Path $ProjectRoot 'Sources\MultiServer\Bishop\GamePlayer.cpp') -Raw -Encoding Default
foreach ($required in @('if (!ExecuteScript(nIdx))', 'return bScriptRet;', 'FindSame(nIdx) > 0 && !RemoveItem(nIdx, 1)')) {
    if (-not $itemSource.Contains($required)) { throw "Missing atomic item-use source boundary: $required" }
}
if ($sceneSource.Contains('client_npc_name_overlay_diag.log')) { throw 'Noisy overlay diagnostic remains.' }
if (-not $sceneSource.Contains('Background.bRenderFlag = 0;')) { throw 'Monster bar background is not in screen-space mode.' }
if (-not $sceneSource.Contains('GetNpcVisualPate()')) { throw 'Monster overlay does not use current visual SPR bounds.' }
if (-not $npcSource.Contains('PhongThanActorReferenceY')) { throw 'Overlay does not mirror the body SPR reference point.' }
foreach ($required in @('bPlayerNpcMorph', 'bPlayerNpcMorph ? cdo_stand : m_ClientDoing', 'if (!bPlayerNpcMorph)', 'return nVisualHeight;')) {
    if (-not $npcSource.Contains($required)) { throw "Morph template reload retains player presentation: $required" }
}
foreach ($required in @('BOOL bTemplateReloaded = FALSE;', 'bTemplateReloaded = TRUE;', 'if (bTemplateReloaded || Npc[nIdx].m_Doing != action)')) {
    if (-not $protocolSource.Contains($required)) { throw "Morph template reload does not force authoritative action: $required" }
}
foreach ($required in @('GetPhongThanNormalNpcAction', 'return GetPhongThanNormalNpcAction(nDoing);', 'cdo_run (6) select')) {
    if (-not $npcResNodeSource.Contains($required)) { throw "NormalNpc action domain is not mapped to the VNG style table: $required" }
}
if (-not $baseTableSource.Contains('nImageCol + 13')) { throw 'IB-item native effect columns are not loaded.' }
if (-not $roleFilterSource.Contains('"rolename.flt"')) { throw 'Character name-filter input changed without evidence.' }
if (($gamePlayerSource.Split(@('ZeroMemory(&RP, sizeof(RP));'), [StringSplitOptions]::None).Count - 1) -ne 1) {
    throw 'ROLEPARAM must be initialized exactly once.'
}
foreach ($required in @(
    'sizeof( RP.szName ) - 1',
    "RP.szName[nMinLen] = '\0';",
    'sizeof( RP.szAccName ) - 1',
    "RP.szAccName[nMinLen] = '\0';")) {
    if (-not $gamePlayerSource.Contains($required)) { throw "Missing bounded role identity boundary: $required" }
}

$expected = @(
    '93F8904C4B1048E00D801F8A46E9DB1A7A734F8CFFC7B6DCCC74C3CC6AF7B20D',
    '404227D73AB5FD11643AA92ACC44D7C1A53598C153383015DE0F0B741CE7092D'
)
$scriptRoot = Join-Path $ProjectRoot 'Deploy\ProjectContent\script\item'
$actual = @(Get-ChildItem -LiteralPath $scriptRoot -File | ForEach-Object {
    (Get-FileHash -LiteralPath $_.FullName -Algorithm SHA256).Hash
})
foreach ($hash in $expected) {
    if ($actual -notcontains $hash) { throw "Missing exact recovered item callback: $hash" }
}
Write-Output 'PASS focused runtime R2 source and exact-script checks=25'
