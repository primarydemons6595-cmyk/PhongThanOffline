[CmdletBinding()]
param(
    [string]$ProjectRoot=(Split-Path -Parent $PSScriptRoot),
    [string]$RuntimeRoot='D:\Lam game phong than\PhongThanRuntime-Staging'
)
$ErrorActionPreference='Stop'
$ProjectRoot=[IO.Path]::GetFullPath($ProjectRoot).TrimEnd('\')
$RuntimeRoot=[IO.Path]::GetFullPath($RuntimeRoot).TrimEnd('\')
if($RuntimeRoot -ine (Join-Path (Split-Path -Parent $ProjectRoot) 'PhongThanRuntime-Staging')) {
    throw 'Publish target must be the named staging runtime.'
}
$clientRoot=Join-Path $RuntimeRoot 'Client'
$running=@(Get-CimInstance Win32_Process | Where-Object {
    $_.ExecutablePath -and $_.ExecutablePath.StartsWith($clientRoot+'\',[StringComparison]::OrdinalIgnoreCase)
})
if($running){throw "Client is running: $($running.Name -join ', ')"}
$receiptPath=Join-Path $RuntimeRoot 'NATIVE_DEPLOYMENT.json'
$receipt=Get-Content -LiteralPath $receiptPath -Raw | ConvertFrom-Json
if($receipt.Schema -ne 1 -or $receipt.Scope -ne 'NATIVE_REBUILD_UAT'){throw 'Invalid native deployment receipt'}
foreach($entry in @($receipt.Artifacts | Where-Object Role -eq 'Client')) {
    $source=Join-Path $ProjectRoot ('Output\Client\'+$entry.Name)
    $destination=Join-Path $clientRoot $entry.Name
    $hash=(Get-FileHash -LiteralPath $source).Hash
    if((Get-FileHash -LiteralPath $destination).Hash -ne $hash) {
        Copy-Item -LiteralPath $source -Destination $destination -Force
        if((Get-FileHash -LiteralPath $destination).Hash -ne $hash){throw "Binary mismatch: $destination"}
        Write-Output "Updated client: $($entry.Name)"
    }
    $entry.Sha256=$hash
}
# The client reads HUD layout from loose Ui3 files before PAK entries. Keep
# authored runtime overrides in sync with the binary; otherwise UI fixes can
# build successfully but never reach Game.exe's working directory.
foreach($relativeUiPath in @(
    'Ui\FaceDetail.ini',
    'Ui\ui3\UiPlayerBar.ini',
    'Ui\ui3\UiMsgCentrePad.ini',
    'Ui\ui3\UiMiniMapSmall.ini',
    'Ui\ui3\UiShop.ini',
    'Ui\ui3\UiTradeConfirmWnd.ini',
    'Ui\ui3\UiPkConfirmation.ini',
    'Ui\ui3\UiHeaderControlBar.ini',
    'Ui\ui3\PtConsumableBuff.ini',
    'Ui\ui3\PtConsumableBuff.provenance.json',
    'Ui\ui3\UiSystemHistory.ini',
    'Ui\ui3\gong_ju_kong_zhi_tiao.ini'
)) {
    $source=Join-Path $ProjectRoot ('Deploy\ProjectContent\'+$relativeUiPath)
    $destination=Join-Path $clientRoot $relativeUiPath
    if(!(Test-Path -LiteralPath $source)){throw "Missing UI publish source: $source"}
    Copy-Item -LiteralPath $source -Destination $destination -Force
    Write-Output "Updated UI: $relativeUiPath"
}
# Publish only the seven original PT sprites referenced by the restored controls.
$uiEncoding=[Text.Encoding]::GetEncoding(28591)
$chatIni=$uiEncoding.GetString([IO.File]::ReadAllBytes((Join-Path $ProjectRoot 'Deploy\ProjectContent\Ui\ui3\UiMsgCentrePad.ini')))
$spritePaths=@()
$headerIni=$uiEncoding.GetString([IO.File]::ReadAllBytes((Join-Path $ProjectRoot 'Deploy\ProjectContent\Ui\ui3\UiHeaderControlBar.ini')))
$spritePaths+=([regex]::Match($headerIni,'(?m)^MaskPath=(.*)').Groups[1].Value.Trim())
foreach($section in 'ChkNearby','ChkTrade','ChkCity','ChkWhisper','ChkTeam','ChkFaction') {
    $sectionText=($chatIni -split ('\['+$section+'\]'))[1] -split '\[' | Select-Object -First 1
    $spritePaths+=([regex]::Match($sectionText,'(?m)^Image=(.*)').Groups[1].Value.Trim())
}
$playerIni=$uiEncoding.GetString([IO.File]::ReadAllBytes((Join-Path $ProjectRoot 'Deploy\ProjectContent\Ui\ui3\UiPlayerBar.ini')))
$spritePaths+=([regex]::Match(($playerIni -split '\[IBShop\]')[1],'(?m)^Image=(.*)').Groups[1].Value.Trim())
$historyIni=$uiEncoding.GetString([IO.File]::ReadAllBytes((Join-Path $ProjectRoot 'Deploy\ProjectContent\Ui\ui3\UiSystemHistory.ini')))
foreach($section in 'SysChannel','BattleChannel','MagicChannel') {
    $sectionText=($historyIni -split ('\['+$section+'\]'))[1] -split '\[' | Select-Object -First 1
    $spritePaths+=([regex]::Match($sectionText,'(?m)^Image=(.*)').Groups[1].Value.Trim())
}
foreach($sprite in $spritePaths) {
    if(!$sprite.StartsWith('\Spr\',[StringComparison]::OrdinalIgnoreCase) -or $sprite.Contains('..')){throw 'Invalid UI sprite path'}
    $relative=$sprite.TrimStart('\')
    $source=Join-Path $ProjectRoot ('Deploy\ProjectContent\'+$relative)
    $destination=Join-Path $clientRoot $relative
    if(!(Test-Path -LiteralPath $source)){throw "Missing UI sprite: $source"}
    New-Item -ItemType Directory -Force ([IO.Path]::GetDirectoryName($destination)) | Out-Null
    Copy-Item -LiteralPath $source -Destination $destination -Force
    if((Get-FileHash -LiteralPath $source).Hash -ne (Get-FileHash -LiteralPath $destination).Hash){throw 'UI sprite publish hash mismatch'}
    Write-Output "Updated original UI sprite: $relative"
}
$receipt.CreatedAtUtc=[DateTime]::UtcNow.ToString('o')
& (Join-Path $PSScriptRoot 'Publish-StarterBagContent.ps1') -ProjectRoot $ProjectRoot -RuntimeRoot $RuntimeRoot
[IO.File]::WriteAllText($receiptPath,($receipt | ConvertTo-Json -Depth 6),[Text.UTF8Encoding]::new($false))
& (Join-Path $ProjectRoot 'Tests\Test-NativeRuntime.ps1') -ProjectRoot $ProjectRoot -RuntimeRoot $RuntimeRoot
