param([string]$Workspace=(Split-Path (Split-Path $PSScriptRoot -Parent) -Parent))
$ErrorActionPreference='Stop'
$Workspace=[IO.Path]::GetFullPath($Workspace)
$runtime=Join-Path $Workspace 'PhongThanRuntime-Loose'
$project=Join-Path $Workspace 'PhongThanSource'
# Publishing paired native semantics requires a clean game/server shutdown.
$running=@(Get-Process Game,GameServer -ErrorAction SilentlyContinue | Where-Object {
    $_.Path -and $_.Path.StartsWith($runtime+'\',[StringComparison]::OrdinalIgnoreCase)
})
if($running.Count){throw 'Close Game and safely stop the Loose GameServer before publishing; active player data must be saved.'}
$content=Join-Path $project 'Deploy/ProjectContent'
$manifest=Get-Content -Raw (Join-Path $content 'settings/phongthan/ReservoirTooltips.json') | ConvertFrom-Json
if($manifest.rows.Count -ne 25){throw 'Expected 25 reservoir formatters'}
$operations=@()
foreach($role in 'Client','Server') {
    $dll=if($role -eq 'Client'){'CoreClient.dll'}else{'CoreServer.dll'}
    $operations += [pscustomobject]@{Source=(Join-Path $project "Output/$role/$dll");Relative="$role/$dll"}
    foreach($row in $manifest.rows) {
        $source=Join-Path $content $row.path
        if((Get-FileHash -LiteralPath $source -Algorithm SHA256).Hash -ne $row.sha256){throw "Lua hash mismatch: $source"}
        $operations += [pscustomobject]@{Source=$source;Relative="$role/$($row.path)"}
    }
}
foreach($op in $operations){if(!(Test-Path -LiteralPath $op.Source)){throw "Missing $($op.Source)"}}
$backup=Join-Path $runtime ('completion/backups/reservoir-'+(Get-Date -Format yyyyMMdd-HHmmss))
New-Item -ItemType Directory -Path $backup -Force | Out-Null
$records=@()
foreach($op in $operations) {
    $target=Join-Path $runtime $op.Relative
    $saved=Join-Path $backup $op.Relative
    $existed=Test-Path -LiteralPath $target
    if($existed){New-Item -ItemType Directory -Path (Split-Path $saved -Parent) -Force | Out-Null;Copy-Item -LiteralPath $target -Destination $saved}
    New-Item -ItemType Directory -Path (Split-Path $target -Parent) -Force | Out-Null
    Copy-Item -LiteralPath $op.Source -Destination $target -Force
    $sha=(Get-FileHash -LiteralPath $op.Source -Algorithm SHA256).Hash
    if((Get-FileHash -LiteralPath $target -Algorithm SHA256).Hash -ne $sha){throw "Publish hash mismatch: $target"}
    $records += [pscustomobject]@{Path=$op.Relative;Existed=$existed;Sha256=$sha}
}
# KPakFile opens these exact physical names first in Loose mode. No table,
# recovered original manifest, or global index rewrite is necessary.
$records | ConvertTo-Json | Set-Content -LiteralPath (Join-Path $backup 'receipt.json') -Encoding ASCII
"PASS published $($records.Count) files; backup=$backup"
