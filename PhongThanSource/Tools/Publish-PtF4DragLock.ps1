$ErrorActionPreference='Stop'
$root=Split-Path (Split-Path $PSScriptRoot -Parent) -Parent
$runtime=Join-Path $root 'PhongThanRuntime-Loose'
$running=@(Get-CimInstance Win32_Process | Where-Object {
    $_.ExecutablePath -and $_.ExecutablePath.StartsWith($runtime+'\',[StringComparison]::OrdinalIgnoreCase) -and
    $_.Name -in @('Game.exe','GameServer.exe','Bishop.exe','Goddess.exe','PhongThanRelay.exe','PhongThanAccountServer.exe')
})
if($running.Count) { throw 'Close the Loose game and safely stop server before publish.' }
$backup=Join-Path $runtime ('completion/backups/f4-drag-lock-'+(Get-Date -Format yyyyMMdd-HHmmss))
New-Item -ItemType Directory -Path $backup -Force | Out-Null
$records=@()
foreach($relative in @('Client/Game.exe','Client/CoreClient.dll','Server/CoreServer.dll')) {
    $source=Join-Path $root ('PhongThanSource/Output/'+$relative)
    $target=Join-Path $runtime $relative
    if(!(Test-Path -LiteralPath $source)) { throw "Missing build $source" }
    Copy-Item -LiteralPath $target -Destination (Join-Path $backup (Split-Path $target -Leaf))
    Copy-Item -LiteralPath $source -Destination $target -Force
    $hash=(Get-FileHash -LiteralPath $source).Hash
    if((Get-FileHash -LiteralPath $target).Hash -ne $hash) { throw 'Publish hash mismatch' }
    $records += [pscustomobject]@{Artifact=$relative;Sha256=$hash}
}
$records | ConvertTo-Json | Set-Content -LiteralPath (Join-Path $backup 'artifacts.json') -Encoding ASCII
$records
"BACKUP=$backup"
