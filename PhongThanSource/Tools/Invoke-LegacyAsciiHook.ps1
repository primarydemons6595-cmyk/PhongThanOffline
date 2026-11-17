[CmdletBinding()]
param(
    [Parameter(Mandatory=$true)][string]$Path,
    [Parameter(Mandatory=$true)][string]$ExpectedSha256,
    [Parameter(Mandatory=$true)][string]$Old,
    [Parameter(Mandatory=$true)][string]$New,
    [switch]$Apply
)
$ErrorActionPreference = 'Stop'
# Mechanical hooks only. Never decode/re-encode the legacy source file.
if ($Old.Length -eq 0 -or $Old -match '[^\x00-\x7f]' -or $New -match '[^\x00-\x7f]') {
    throw 'Hooks must be nonempty, ASCII-only source snippets.'
}
$resolved = (Resolve-Path -LiteralPath $Path).Path
$before = [IO.File]::ReadAllBytes($resolved)
$hash = (Get-FileHash -LiteralPath $resolved -Algorithm SHA256).Hash
if ($hash -ne $ExpectedSha256) { throw 'Source hash changed; refusing edit.' }
$ascii = [Text.Encoding]::ASCII
$needle = $ascii.GetBytes($Old)
$replacement = $ascii.GetBytes($New)
$matches = New-Object 'System.Collections.Generic.List[int]'
for ($i=0; $i -le $before.Length-$needle.Length; $i++) {
    if ($before[$i] -ne $needle[0]) { continue }
    $equal=$true
    for ($j=1; $j -lt $needle.Length; $j++) {
        if ($before[$i+$j] -ne $needle[$j]) { $equal=$false; break }
    }
    if ($equal) { $matches.Add($i) }
}
if ($matches.Count -ne 1) { throw "Expected exactly one hook; found $($matches.Count)." }
$offset=$matches[0]
$after=New-Object byte[] ($before.Length-$needle.Length+$replacement.Length)
[Array]::Copy($before,0,$after,0,$offset)
[Array]::Copy($replacement,0,$after,$offset,$replacement.Length)
$tail=$offset+$needle.Length
[Array]::Copy($before,$tail,$after,$offset+$replacement.Length,$before.Length-$tail)
for ($i=0; $i -lt $offset; $i++) {
    if ($before[$i] -ne $after[$i]) { throw 'Prefix verification failed.' }
}
for ($i=$tail; $i -lt $before.Length; $i++) {
    if ($before[$i] -ne $after[$i-$needle.Length+$replacement.Length]) { throw 'Suffix verification failed.' }
}
if ($offset -lt 4) { throw 'Refusing hook overlapping possible BOM.' }
if ($Apply) {
    $backupDir=Join-Path $PSScriptRoot 'LegacyByteBackups'
    New-Item -ItemType Directory -Path $backupDir -Force | Out-Null
    $backup=Join-Path $backupDir ((Split-Path -Leaf $resolved)+'.'+$hash+'.bak')
    if (-not (Test-Path -LiteralPath $backup)) { [IO.File]::WriteAllBytes($backup,$before) }
    if ((Get-FileHash -LiteralPath $resolved -Algorithm SHA256).Hash -ne $hash) { throw 'Concurrent source edit.' }
    [IO.File]::WriteAllBytes($resolved,$after)
    $readback=[IO.File]::ReadAllBytes($resolved)
    if ([Convert]::ToBase64String($readback) -cne [Convert]::ToBase64String($after)) { throw 'Readback verification failed.' }
}
[pscustomobject]@{Path=$resolved;Applied=[bool]$Apply;BeforeSha256=$hash;Offset=$offset;RemovedBytes=$needle.Length;AddedBytes=$replacement.Length;OutsideBytesUnchanged=$true;BomUnchanged=$true}
