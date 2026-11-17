[CmdletBinding()]
param(
    [Parameter(Mandatory=$true)][string]$ProjectRoot,
    [Parameter(Mandatory=$true)][string]$RuntimeRoot
)

$ErrorActionPreference='Stop'
$ProjectRoot=[IO.Path]::GetFullPath($ProjectRoot).TrimEnd('\')
$RuntimeRoot=[IO.Path]::GetFullPath($RuntimeRoot).TrimEnd('\')
$receiptPath=Join-Path $RuntimeRoot 'NATIVE_DEPLOYMENT.json'
$receipt=Get-Content -LiteralPath $receiptPath -Raw | ConvertFrom-Json
if($receipt.Schema -ne 1 -or $receipt.Scope -ne 'NATIVE_REBUILD_UAT'){throw 'Invalid native deployment receipt'}
foreach($spec in @(@{Role='Client';Name='CoreClient.dll'},@{Role='Server';Name='CoreServer.dll'})){
    $entry=@($receipt.Artifacts | Where-Object {$_.Role -eq $spec.Role -and $_.Name -eq $spec.Name})
    if($entry.Count -ne 1){throw "Expected exactly one receipt entry: $($spec.Role)/$($spec.Name)"}
    $source=Join-Path $ProjectRoot "Output\$($spec.Role)\$($spec.Name)"
    $runtime=Join-Path $RuntimeRoot "$($spec.Role)\$($spec.Name)"
    if(!(Test-Path -LiteralPath $source) -or !(Test-Path -LiteralPath $runtime)){throw "Missing core artifact: $($spec.Name)"}
    $sourceHash=(Get-FileHash -LiteralPath $source -Algorithm SHA256).Hash
    $runtimeHash=(Get-FileHash -LiteralPath $runtime -Algorithm SHA256).Hash
    if($sourceHash -ne $runtimeHash){throw "Runtime core hash mismatch: $($spec.Name)"}
    $entry[0].Sha256=$sourceHash
}
$receipt.CreatedAtUtc=[DateTime]::UtcNow.ToString('o')
[IO.File]::WriteAllText($receiptPath,($receipt | ConvertTo-Json -Depth 6),[Text.UTF8Encoding]::new($false))
& (Join-Path $ProjectRoot 'Tests\Test-NativeRuntime.ps1') -ProjectRoot $ProjectRoot -RuntimeRoot $RuntimeRoot
