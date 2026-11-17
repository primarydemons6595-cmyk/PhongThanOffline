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

$contentRoot=Join-Path $ProjectRoot 'Deploy\ProjectContent\Ui'
$clientRoot=Join-Path $RuntimeRoot 'Client\Ui'
$files=@(
    @{ Source=(Join-Path $contentRoot 'FaceDetail.ini'); Destination=(Join-Path $clientRoot 'FaceDetail.ini') },
    @{ Source=(Join-Path $contentRoot 'ui3\UiFaceSelector.ini'); Destination=(Join-Path $clientRoot 'ui3\UiFaceSelector.ini') },
    @{ Source=(Join-Path $contentRoot 'ui3\UiItem.ini'); Destination=(Join-Path $clientRoot 'ui3\UiItem.ini') },
    @{ Source=(Join-Path $contentRoot 'ui3\UiESCDlg.ini'); Destination=(Join-Path $clientRoot 'ui3\UiESCDlg.ini') },
    @{ Source=(Join-Path $contentRoot 'ui3\UiOptions.ini'); Destination=(Join-Path $clientRoot 'ui3\UiOptions.ini') }
)
foreach($entry in $files){
    if(-not(Test-Path -LiteralPath $entry.Source)){throw "Missing canonical UI input data: $($entry.Source)"}
    New-Item -ItemType Directory -Path (Split-Path -Parent $entry.Destination) -Force | Out-Null
    Copy-Item -LiteralPath $entry.Source -Destination $entry.Destination -Force
    $sourceHash=(Get-FileHash -LiteralPath $entry.Source -Algorithm SHA256).Hash
    $destinationHash=(Get-FileHash -LiteralPath $entry.Destination -Algorithm SHA256).Hash
    if($sourceHash -ne $destinationHash){throw "UI content hash mismatch: $($entry.Destination)"}
    Write-Output "PUBLISHED_UI_INPUT $($entry.Destination) SHA256=$sourceHash"
}
