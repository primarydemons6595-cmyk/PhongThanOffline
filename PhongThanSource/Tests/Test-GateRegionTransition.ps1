[CmdletBinding()]
param(
    [string]$ProjectRoot,
    [string]$RuntimeRoot
)

$ErrorActionPreference = 'Stop'
if (-not $ProjectRoot) { $ProjectRoot = Split-Path -Parent $PSScriptRoot }
if (-not $RuntimeRoot) { $RuntimeRoot = Join-Path (Split-Path -Parent $ProjectRoot) 'PhongThanRuntime-Staging' }
$cp936 = [Text.Encoding]::GetEncoding(936)
$latin1 = [Text.Encoding]::GetEncoding(28591)

function Assert-True([bool]$Condition, [string]$Message) {
    if (-not $Condition) { throw $Message }
}

function Get-WorldMetadata([string]$RoleRoot) {
    $names = @{}
    foreach ($line in [IO.File]::ReadAllLines((Join-Path $RoleRoot 'settings\WorldSet.ini'), $cp936)) {
        $match = [regex]::Match($line, '^\s*(?<id>\d+)\s*=\s*(?<value>[^;\r\n]+?)\s*$')
        if ($match.Success) { $names[[int]$match.Groups['id'].Value] = $match.Groups['value'].Value.Trim() }
    }
    return $names
}

function Get-MapDirectory([string]$RoleRoot, [hashtable]$Metadata, [int]$WorldId) {
    Assert-True $Metadata.ContainsKey($WorldId) "WorldSet metadata missing $WorldId"
    $physical = [string]$Metadata[$WorldId]
    $runtimeName = if ($physical -match '[^\x00-\x7f]') { $latin1.GetString($cp936.GetBytes($physical)) } else { $physical }
    if ((Split-Path -Leaf $RoleRoot) -ieq 'Server') { $runtimeName += '_S' }
    return Join-Path (Join-Path $RoleRoot 'maps') $runtimeName
}

function Test-ByteSequence([byte[]]$Data, [byte[]]$Needle) {
    for ($i = 0; $i -le $Data.Length - $Needle.Length; $i++) {
        $matched = $true
        for ($j = 0; $j -lt $Needle.Length; $j++) {
            if ($Data[$i + $j] -ne $Needle[$j]) { $matched = $false; break }
        }
        if ($matched) { return $true }
    }
    return $false
}

$source = [IO.File]::ReadAllText((Join-Path $ProjectRoot 'Sources\Core\Src\KNpc.cpp'))
Assert-True (-not $source.Contains('0xCE741D0EUL')) 'SuperTrap ID must come from the deterministic registry, not KNpc hardcoding.'
$registryPath=Join-Path $ProjectRoot 'Deploy\ProjectContent\script\phongthan\mapexit\registry.tsv'
$registry=@(Import-Csv -LiteralPath $registryPath -Delimiter "`t")
$superTrap=@($registry | Where-Object {$_.ScriptIdHex -eq '0xCE741D0E'})
Assert-True ($superTrap.Count -eq 1) 'Registry is missing or duplicates the proven SuperTrap ID binding.'
Assert-True ($superTrap[0].LoosePath -eq '\script\phongthan\gate\supertrap.lua') 'Registry has the wrong loose SuperTrap path.'
Assert-True ($superTrap[0].Classification -eq 'SUPERTRAP_OR_OTHER_TRANSITION') 'Registry has the wrong SuperTrap classification.'
$aiSource = [IO.File]::ReadAllText((Join-Path $ProjectRoot 'Sources\Core\Src\KNpcAI.cpp'))
Assert-True ($aiSource.Contains('CheckTrap(Npc[m_nIndex].m_MapX, Npc[m_nIndex].m_MapY)')) 'Map-trap AI still uses the wrong coordinate space.'

$projectScript = Join-Path $ProjectRoot 'Deploy\ProjectContent\script\phongthan\gate\supertrap.lua'
$expectedHash = '73298AB0F9825D4F8C9B6CEAB3ECC7D089F490622993E14430A10BB729A7601A'
Assert-True (Test-Path -LiteralPath $projectScript -PathType Leaf) 'Recovered loose SuperTrap script is missing.'
Assert-True ((Get-FileHash -Algorithm SHA256 -LiteralPath $projectScript).Hash -eq $expectedHash) 'Recovered SuperTrap hash mismatch.'
$provenance = [IO.File]::ReadAllText((Join-Path $ProjectRoot 'Deploy\ProjectContent\script\phongthan\gate\PROVENANCE.md'))
Assert-True ($provenance.Contains('Client_Original/data/script.pak')) 'SuperTrap provenance does not identify script.pak.'

$serverRoot = Join-Path $RuntimeRoot 'Server'
$clientRoot = Join-Path $RuntimeRoot 'Client'
$serverMetadata = Get-WorldMetadata $serverRoot
$clientMetadata = Get-WorldMetadata $clientRoot
$serverMap1 = Get-MapDirectory $serverRoot $serverMetadata 1001
$clientMap1 = Get-MapDirectory $clientRoot $clientMetadata 1001

$trapBytes = [BitConverter]::GetBytes([Convert]::ToUInt32('CE741D0E', 16))
$trapFiles = @(Get-ChildItem -LiteralPath $serverMap1 -Recurse -File -Filter '*_region_s.dat' | Where-Object {
    Test-ByteSequence ([IO.File]::ReadAllBytes($_.FullName)) $trapBytes
})
Assert-True ($trapFiles.Count -eq 2) "Expected map 1 SuperTrap data in two Region_S files, found $($trapFiles.Count)."

# CP936 bytes for the authored transition visual path. Keep the test source
# ASCII so Windows PowerShell 5.1 does not reinterpret a BOM-less UTF-8 file.
$visualBytes = [byte[]](0x5C,0xD3,0xCE,0xCF,0xB7,0xD7,0xCA,0xD4,0xB4,0x5C,0xB5,0xD8,0xCD,0xBC,0xB6,0xAF,0xBB,0xAD,0x5C,0xB9,0xE2,0xD3,0xB0,0x5C,0xB4,0xAB,0xCB,0xCD,0xB5,0xE3,0x30,0x31,0x2E,0x73,0x70,0x72)
$visualFiles = @(Get-ChildItem -LiteralPath $clientMap1 -Recurse -File -Filter '*_region_c.dat' | Where-Object {
    Test-ByteSequence ([IO.File]::ReadAllBytes($_.FullName)) $visualBytes
})
Assert-True ($visualFiles.Count -ge 1) 'Map 1 transition visual is missing from Region_C data.'

$runtimeScript = Join-Path $serverRoot 'script\phongthan\gate\supertrap.lua'
Assert-True (Test-Path -LiteralPath $runtimeScript -PathType Leaf) 'Published loose SuperTrap script is missing.'
Assert-True ((Get-FileHash -Algorithm SHA256 -LiteralPath $runtimeScript).Hash -eq $expectedHash) 'Published SuperTrap hash mismatch.'

foreach ($role in 'Server', 'Client') {
    $roleRoot = Join-Path $RuntimeRoot $role
    $metadata = if ($role -eq 'Server') { $serverMetadata } else { $clientMetadata }
    $suffix = if ($role -eq 'Server') { '_region_s.dat' } else { '_region_c.dat' }
    foreach ($fileName in 'wildsuper.lua', 'bigwildsuper.lua') {
        $path = Join-Path $roleRoot "script\item\ibitem\$fileName"
        $text = [IO.File]::ReadAllText($path, $cp936)
        $rows = @([regex]::Matches($text, '(?m)^\s*\{(?<id>\d+),(?<x>\d+),(?<y>\d+),"'))
        Assert-True ($rows.Count -eq 91) "$role/$fileName does not contain 91 generated destinations."
        foreach ($row in $rows) {
            $id = [int]$row.Groups['id'].Value
            $x = [int]$row.Groups['x'].Value
            $y = [int]$row.Groups['y'].Value
            $regionX = [Math]::Floor($x / 16)
            $regionY = [Math]::Floor($y / 32)
            $mapDir = Get-MapDirectory $roleRoot $metadata $id
            $regionPath = Join-Path (Join-Path $mapDir ("v_{0:000}" -f $regionY)) (("{0:000}" -f $regionX) + $suffix)
            Assert-True (Test-Path -LiteralPath $regionPath -PathType Leaf) "$role/$fileName map $id points outside mounted regions: ($x,$y)."
        }
        Assert-True ($text.Contains('{1103,1736,3088,"')) "$role/$fileName has the wrong map 1103 center."
    }
}

$map1103 = Get-MapDirectory $serverRoot $serverMetadata 1103
$oldInvalid = Join-Path (Join-Path $map1103 'v_108') '096_region_s.dat'
$knownValid = Join-Path (Join-Path $map1103 'v_096') '096_region_s.dat'
$correctCenter = Join-Path (Join-Path $map1103 'v_096') '108_region_s.dat'
Assert-True (-not (Test-Path -LiteralPath $oldInvalid -PathType Leaf)) 'Unexpectedly found a mounted region for legacy coordinate (1544,3472).'
Assert-True (Test-Path -LiteralPath $knownValid -PathType Leaf) 'Known valid coordinate (1544,3088) has no mounted region.'
Assert-True (Test-Path -LiteralPath $correctCenter -PathType Leaf) 'Corrected generated center (1736,3088) has no mounted region.'

[pscustomobject]@{
    Result = 'PASS_AUTOMATED'
    TrapId = '0xCE741D0E'
    TrapRegionFiles = $trapFiles.Count
    VisualRegionFiles = $visualFiles.Count
    RecoveredScriptSha256 = $expectedHash
    DiNgoaiPhuMap1103 = '1736,3088'
}
