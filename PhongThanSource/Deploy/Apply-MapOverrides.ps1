[CmdletBinding()]
param(
    [string]$ProjectRoot = (Split-Path -Parent $PSScriptRoot),
    [Parameter(Mandatory = $true)]
    [string]$RuntimeRoot
)

$ErrorActionPreference = 'Stop'
$ProjectRoot = [IO.Path]::GetFullPath($ProjectRoot).TrimEnd('\')
$RuntimeRoot = [IO.Path]::GetFullPath($RuntimeRoot).TrimEnd('\')
$projectParent = Split-Path -Parent $ProjectRoot
$allowedRoots = @(
    [IO.Path]::GetFullPath((Join-Path $projectParent 'PhongThanRuntime-Content')).TrimEnd('\'),
    [IO.Path]::GetFullPath((Join-Path $projectParent 'PhongThanRuntime-Staging')).TrimEnd('\')
)
if ($RuntimeRoot -notin $allowedRoots) {
    throw "Map override target is not an approved content/staging root: $RuntimeRoot"
}

$manifestPath = Join-Path $ProjectRoot 'Deploy\ProjectContent\MapOverrides\manifest.json'
$manifest = Get-Content -LiteralPath $manifestPath -Raw | ConvertFrom-Json
if ($manifest.Schema -ne 1 -or -not $manifest.Overrides.Count) {
    throw 'Invalid or empty map override manifest.'
}

$results = New-Object System.Collections.Generic.List[object]
foreach ($override in $manifest.Overrides) {
    $role = [string]$override.Role
    $relative = ([string]$override.RelativePath).Replace('/', '\')
    if ($role -notin @('Client', 'Server') -or [IO.Path]::IsPathRooted($relative) -or
        @($relative.Split('\') | Where-Object { $_ -in @('', '.', '..') }).Count) {
        throw "Unsafe map override path: $role\$relative"
    }
    $roleRoot = [IO.Path]::GetFullPath((Join-Path $RuntimeRoot $role)).TrimEnd('\')
    $sourceRoot = [IO.Path]::GetFullPath((Join-Path $ProjectRoot "Deploy\ProjectContent\MapOverrides\$role")).TrimEnd('\')
    $source = [IO.Path]::GetFullPath((Join-Path $sourceRoot $relative))
    $target = [IO.Path]::GetFullPath((Join-Path $roleRoot $relative))
    if (-not $source.StartsWith($sourceRoot + '\', [StringComparison]::OrdinalIgnoreCase) -or
        -not $target.StartsWith($roleRoot + '\', [StringComparison]::OrdinalIgnoreCase)) {
        throw "Map override path escaped its role root: $relative"
    }
    if (-not (Test-Path -LiteralPath $source -PathType Leaf)) {
        throw "Missing canonical map override: $source"
    }
    $sourceInfo = Get-Item -LiteralPath $source
    $sourceHash = (Get-FileHash -LiteralPath $source -Algorithm SHA256).Hash
    if ($sourceInfo.Length -ne [long]$override.Length -or $sourceHash -ne [string]$override.Sha256) {
        throw "Map override source provenance mismatch: $source"
    }

    if ($RuntimeRoot -ieq $allowedRoots[1]) {
        $running = @(Get-CimInstance Win32_Process | Where-Object {
            $_.ExecutablePath -and
            ([IO.Path]::GetFullPath($_.ExecutablePath)).StartsWith($roleRoot + '\', [StringComparison]::OrdinalIgnoreCase)
        })
        if ($running) { throw "Stop $role processes before applying map override: $($running.Name -join ', ')" }
    }

    New-Item -ItemType Directory -Path (Split-Path -Parent $target) -Force | Out-Null
    Copy-Item -LiteralPath $source -Destination $target -Force
    $targetHash = (Get-FileHash -LiteralPath $target -Algorithm SHA256).Hash
    if ($targetHash -ne $sourceHash) { throw "Map override target hash mismatch: $target" }
    $results.Add([pscustomobject]@{Role=$role;Path=$relative;Length=$sourceInfo.Length;Sha256=$targetHash})
}

[pscustomobject]@{Result='PASS';RuntimeRoot=$RuntimeRoot;Overrides=$results.ToArray()}
