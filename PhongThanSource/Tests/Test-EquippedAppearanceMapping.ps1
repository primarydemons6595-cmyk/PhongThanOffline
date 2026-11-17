$ErrorActionPreference = 'Stop'
$root = Split-Path -Parent $PSScriptRoot
$items = [IO.File]::ReadAllText((Join-Path $root 'Sources/Core/Src/KItemList.cpp'), [Text.Encoding]::GetEncoding(28591))
if ($items -notmatch 'nAppearanceRecord = Item\[nIdx\]\.GetRow\(\) \+ 1') { throw 'Appearance must use the one-based template record' }
foreach ($part in 'Armor','Helm') {
    if ($items -notmatch "Resolve$part\(Item\[nIdx\]\.GetDetailType\(\),") { throw "Missing typed item lookup: $part" }
}
if ($items -notmatch 'ResolveHorse\(nAppearanceRecord, &Visual\)') { throw 'Incorrect horse lookup key' }
if ($items -notmatch 'ResolvePhiPhong\(Item\[nIdx\]\.GetDetailType\(\),') { throw 'Cape still uses slot EquipId as appearance' }
if ($items -notmatch 'ResolveWeapon\(Item\[nIdx\]\.GetDetailType\(\),\s*nAppearanceRecord, &Visual\)') { throw 'Incorrect weapon lookup key' }
$source = [IO.File]::ReadAllText((Join-Path $root 'Sources/Core/Src/KPhongThanAppearance.cpp'))
foreach ($part in 'Armor','Helm','MeleeWeapon','RangeWeapon') {
    if ($source -notmatch "ResolveTablePart\(m_$part, nEquipId, 1, pVisual\)") { throw "Invalid one-based part mapping: $part" }
}
$typed = [IO.File]::ReadAllText((Join-Path $root 'Headers/PhongThanEquipmentAppearance.h'))
if ($typed -notmatch 'level > 10' -or $typed -notmatch '\(level - 1\) / 2 \+ 1') { throw 'Authentic PT basic-tier boundaries missing' }
$res = [IO.File]::ReadAllText((Join-Path $root 'Sources/Core/Src/KNpcRes.cpp'), [Text.Encoding]::GetEncoding(28591))
if ($res -match 'GetImageParam\(m_cNpcImage\[i\]') { throw 'Body metadata is reloaded inside the frame loop' }
if ($res -notmatch 'SetCurFrame\(dir \* frames \+ phase\)') { throw 'Missing per-component animation phase' }
'PASS: one-based equipment mapping and per-component frame wiring (source regression).'
