[CmdletBinding()]
param([string]$ProjectRoot = (Split-Path -Parent $PSScriptRoot))
$ErrorActionPreference = 'Stop'
$ProjectRoot = [IO.Path]::GetFullPath($ProjectRoot).TrimEnd('\')
$runtime = Join-Path (Split-Path -Parent $ProjectRoot) 'PhongThanRuntime-Loose'
$client = Join-Path $runtime 'Client'
$game = Join-Path $client 'Game.exe'
$running = @(Get-CimInstance Win32_Process -Filter "Name='Game.exe'" | Where-Object {
    $_.ExecutablePath -and [IO.Path]::GetFullPath($_.ExecutablePath) -ieq $game
})
if ($running.Count) { throw 'Close the loose client before publishing client binaries.' }
$names = @('Game.exe', 'CoreClient.dll', 'Represent2.dll')
foreach ($name in $names) {
    foreach ($path in @((Join-Path $ProjectRoot "Output\Client\$name"), (Join-Path $client $name))) {
        if (!(Test-Path -LiteralPath $path -PathType Leaf)) { throw "Missing artifact: $path" }
    }
}
$receiptPath = Join-Path $runtime 'LOOSE_DEPLOYMENT.json'
$receipt = Get-Content -LiteralPath $receiptPath -Raw | ConvertFrom-Json
$backup = Join-Path $runtime ('completion\backups\engine-smooth-' + (Get-Date -Format yyyyMMdd-HHmmss))
New-Item -ItemType Directory -Path $backup | Out-Null
Copy-Item -LiteralPath $receiptPath -Destination (Join-Path $backup 'LOOSE_DEPLOYMENT.json')
$records = @()
foreach ($name in $names) {
    $source = Join-Path $ProjectRoot "Output\Client\$name"
    $target = Join-Path $client $name
    $before = (Get-FileHash -LiteralPath $target -Algorithm SHA256).Hash
    Copy-Item -LiteralPath $target -Destination (Join-Path $backup $name)
    Copy-Item -LiteralPath $source -Destination $target -Force
    $after = (Get-FileHash -LiteralPath $source -Algorithm SHA256).Hash
    if ((Get-FileHash -LiteralPath $target -Algorithm SHA256).Hash -ne $after) {
        throw "Publish hash mismatch: $name; backup retained at $backup"
    }
    $entry = @($receipt.Artifacts | Where-Object { $_.Role -eq 'Client' -and $_.Name -eq $name })
    if ($entry.Count -ne 1) { throw "Expected one client receipt entry for $name" }
    $entry[0].Sha256 = $after
    $records += [pscustomobject]@{ Name=$name; Before=$before; Sha256=$after }
}
# This receipt records only these client artifacts; server/config/data are untouched.
$publication = [pscustomobject]@{
    Scope='CLIENT_ENGINE_SMOOTH'; CreatedAtUtc=[DateTime]::UtcNow.ToString('o')
    Runtime=$client; Artifacts=$records; Backup=$backup
}
[IO.File]::WriteAllText((Join-Path $backup 'publication.json'),
    ($publication | ConvertTo-Json -Depth 6), [Text.UTF8Encoding]::new($false))
[IO.File]::WriteAllText($receiptPath,
    ($receipt | ConvertTo-Json -Depth 8), [Text.UTF8Encoding]::new($false))
$publication
