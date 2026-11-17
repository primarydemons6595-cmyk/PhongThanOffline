$ErrorActionPreference = 'Stop'
# Mechanical ASCII-only splice. Never decode/re-encode legacy source bytes.
$file = Join-Path (Split-Path -Parent $PSScriptRoot) 'Sources/Core/Src/KNpc.cpp'
$original = [IO.File]::ReadAllBytes($file)
if ($original.Length -ge 3 -and $original[0] -eq 239 -and $original[1] -eq 187 -and $original[2] -eq 191) { throw 'Unexpected UTF8 BOM' }
$data = $original
$ascii = [Text.Encoding]::ASCII
$splices = @(
    @('#include "PhongThanQuestDeathContext.inl"', "#include `"PhongThanQuestDeathContext.inl`"`r`n#include `"PhongThanNpcDrop.inl`""),
    @('DeathPunish(nMode, nPlayer);', "DeathPunish(nMode, nPlayer);`r`n    PtDropNpc(*this, nPlayer); // NEW_PROJECT_RULES_V1; quest-script drops stay separate."),
    @('ObjSet.ClientAdd(0, m_CorpseSettingIdx, 0, m_Dir, 0, nTempX, nTempY, sInfo);', @'
// A corpse reference must resolve to Body, never a pickup Item template.
            // Original PT CorpseIdx=14 collides with current ObjData Item 14.
            char corpseKind[32]; corpseKind[0] = 0;
            if (m_CorpseSettingIdx > 0 &&
                ObjSet.m_cTabFile.GetString(m_CorpseSettingIdx + 1, 3, "", corpseKind, sizeof(corpseKind)) &&
                strcmp(corpseKind, "Body") == 0)
                ObjSet.ClientAdd(0, m_CorpseSettingIdx, 0, m_Dir, 0, nTempX, nTempY, sInfo);
'@)
)
$backup = Join-Path (Split-Path -Parent $PSScriptRoot) ('Build/Backups/NpcDrop_' + [guid]::NewGuid().ToString('N'))
New-Item -ItemType Directory -Path $backup | Out-Null
[IO.File]::WriteAllBytes((Join-Path $backup 'KNpc.cpp.before'),$original)
$journal = @()
foreach ($splice in $splices) {
    $old = $ascii.GetBytes($splice[0]); $new = $ascii.GetBytes(($splice[1] -replace "(?<!`r)`n","`r`n"))
    $matches = @()
    for ($i=0; $i -le $data.Length-$old.Length; $i++) {
        if ($data[$i] -ne $old[0]) { continue }
        $same=$true
        for ($j=1; $j -lt $old.Length; $j++) { if($data[$i+$j] -ne $old[$j]) {$same=$false;break} }
        if($same) {$matches += $i}
    }
    if ($matches.Count -ne 1) { throw "Expected unique ASCII hook, got $($matches.Count): $($splice[0])" }
    $at=$matches[0]; $result=New-Object byte[] ($data.Length-$old.Length+$new.Length)
    [Array]::Copy($data,0,$result,0,$at); [Array]::Copy($new,0,$result,$at,$new.Length)
    [Array]::Copy($data,$at+$old.Length,$result,$at+$new.Length,$data.Length-$at-$old.Length)
    for($i=0;$i -lt $at;$i++){if($data[$i] -ne $result[$i]){throw 'Prefix changed'}}
    for($i=$at+$old.Length;$i -lt $data.Length;$i++){if($data[$i] -ne $result[$i-$old.Length+$new.Length]){throw 'Suffix changed'}}
    $journal += @{Offset=$at;Removed=$old.Length;Inserted=$new.Length;OutsideBytesVerified=$true}
    $data=$result
}
[IO.File]::WriteAllBytes($file,$data)
@{File=$file;Before=(Get-FileHash (Join-Path $backup 'KNpc.cpp.before')).Hash;After=(Get-FileHash $file).Hash;BOM='NONE unchanged';Splices=$journal} | ConvertTo-Json -Depth 6 | Set-Content -Encoding UTF8 (Join-Path $backup 'edit.json')
Write-Output "ASCII_SPLICE_PASS: $backup"
