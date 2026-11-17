$ErrorActionPreference = 'Stop'
$workspace = [IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
$encoding = [Text.Encoding]::GetEncoding(28591)
$backup = Join-Path $workspace 'analysis/repair/state_render_isolation_before'
$nodeRelative = 'PhongThanSource/Sources/Core/Src/KNpcResNode.cpp'
$renderRelative = 'PhongThanSource/Sources/Core/Src/KNpcRes.cpp'
$node = $encoding.GetString([IO.File]::ReadAllBytes((Join-Path $workspace $nodeRelative)))
$render = $encoding.GetString([IO.File]::ReadAllBytes((Join-Path $workspace $renderRelative)))
foreach ($pair in @(@('Head','\xCD\xB7\xB6\xA5'), @('Foot','\xBD\xC5\xB5\xD7'), @('Loop','\xD1\xAD\xBB\xB7'))) {
    $old = 'strcmp(szBuffer, "' + $pair[0] + '") == 0'
    $new = '(' + $old + ' || strcmp(szBuffer, "' + $pair[1] + '") == 0)'
    if (!$node.Contains($new)) { throw "Missing original PT alias: $($pair[0])" }
    $node = $node.Replace($new, $old)
}
$reset = "m_cDrawFile[nPos].bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;`r`n`t`t`tm_cDrawFile[nPos].Color.Color_dw = 0xffffffff;`r`n`t`t`t"
foreach ($sprite in @('m_cStateSpr[i].m_SprContrul.m_szName', 'this->m_cNpcShadow.m_szName', 'm_cSpecialSpr.m_szName')) {
    $old = 'strcpy(m_cDrawFile[nPos].szImage, ' + $sprite + ');'
    $new = $reset + $old
    $count = $render.Split([string[]]@($new), [StringSplitOptions]::None).Length - 1
    $expected = if ($sprite.StartsWith('m_cStateSpr')) { 4 } else { 1 }
    if ($count -ne $expected) { throw "Reset count mismatch: $sprite" }
    $render = $render.Replace($new, $old)
}
foreach ($entry in @(@($nodeRelative,$node), @($renderRelative,$render))) {
    $original = $encoding.GetString([IO.File]::ReadAllBytes((Join-Path $backup ($entry[0].Replace('/','_') + '.bin'))))
    if (!$original.Equals($entry[1])) { throw "Unrelated bytes changed: $($entry[0])" }
}
$statePath = Join-Path $workspace 'PhongThanRuntime-Staging/Client/settings/npcres/zhuang_tai_tu_xing_dui_zhao_biao.txt'
$rows = [IO.File]::ReadAllLines($statePath, $encoding)
$foot = $encoding.GetString([byte[]]@(0xbd,0xc5,0xb5,0xd7))
$loop = $encoding.GetString([byte[]]@(0xd1,0xad,0xbb,0xb7))
foreach ($state in 19,22) {
    $fields = $rows[$state].Split([char]9)
    if ($fields[2] -cne $foot -or $fields[3] -cne $loop) { throw "Unexpected original state contract: $state" }
}
'PASS: 3 PT aliases, 6 isolated draw sites, 2 exact-byte preservation checks, 2 deployed Foot/Loop states. Static/data checks only.'
