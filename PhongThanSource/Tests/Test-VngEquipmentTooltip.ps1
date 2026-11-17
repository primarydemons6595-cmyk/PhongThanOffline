[CmdletBinding()]
param([string]$ProjectRoot = (Split-Path -Parent $PSScriptRoot))

$ErrorActionPreference = 'Stop'
$encoding = [Text.Encoding]::GetEncoding(28591)
$itemSource = [IO.File]::ReadAllText(
    (Join-Path $ProjectRoot 'Sources\Core\Src\KItem.cpp'), $encoding)
$magicDescSource = [IO.File]::ReadAllText(
    (Join-Path $ProjectRoot 'Sources\Core\Src\KMagicDesc.cpp'), $encoding)
$generatorSource = [IO.File]::ReadAllText(
    (Join-Path $ProjectRoot 'Sources\Core\Src\KItemGenerator.CPP'), $encoding)
$baseTableSource = [IO.File]::ReadAllText(
    (Join-Path $ProjectRoot 'Sources\Core\Src\KBasPropTbl.CPP'), $encoding)
$mouseHoverSource = [IO.File]::ReadAllText(
    (Join-Path $ProjectRoot 'Sources\GameClient\Ui\Elem\MouseHover.cpp'), $encoding)
$mouseHoverIni = Get-Content -LiteralPath (
    Join-Path $ProjectRoot 'Deploy\ProjectContent\Ui\ui3\UiMouseHover.ini') -Raw
$setFunction = [regex]::Match($generatorSource,
    '(?s)BOOL KItemGenerator::ApplyPhongThanSetAttrib.*?\n}').Value

$checks = [ordered]@{
    StripsVngItemNamePrefix = $itemSource -match 'SkipVngDisplayPrefix\(m_CommonAttrib\.szItemName\)'
    StripsVngIntroPrefix = $itemSource -match 'SkipVngDisplayPrefix\(m_CommonAttrib\.szIntro\)'
    StripsMagicDescPrefix = $magicDescSource -match "while \(\*pszSource == '\$'\)"
    DoesNotAppendEquipmentLevel = $itemSource -notmatch 'sprintf\(pszTemp, " \[c'
    DoesNotAppendDebugItemId = $itemSource -notmatch 'ID:\[%d\]'
    SkipsEmptyBaseAttributes = $itemSource -match 'if \(m_aryBaseAttrib\[i\]\.nAttribType <= 0\)'
    SkipsEmptyRequirements = $itemSource -match 'if \(m_aryRequireAttrib\[i\]\.nAttribType <= 0\)'
    SkipsEmptyMagicAttributes = $itemSource -match 'if \(m_aryMagicAttrib\[i\]\.nAttribType <= 0\)'
    UsesVngSetAttributeTable = $baseTableSource -match 'TABFILE_VNG_SETATTRIB' -and
                               $baseTableSource -match 'GetVngSetAttrib'
    TreatsHiddenValuesAsFixedTriplet = $baseTableSource -match 'pOut\[nCopied\+\+\] = pSource->m_aryAttrib\[nAttrib\]' -and
                                       $setFunction -match 'memcpy\(target\.nValue, effects\[i\]\.nValue'
    UsesFixedGoldComponentColumns = $setFunction -match 'PhongThanProjectEquipmentModifiers' -and
                                    $setFunction -match 'MAX_ITEM_NORMAL_MAGICATTRIB'
    BypassesSwordOnlineMagicRollForSets = $generatorSource -match 'ApplyPhongThanSetAttrib\(pItem, pEqu\);\s*return TRUE;'
    CopiesHiddenValuesWithoutRandom = $setFunction -match 'memcpy\(target\.nValue, effects\[i\]\.nValue' -and
                                      $setFunction -notmatch 'GetRandomNumber'
    PreservesVngSetLineOrder = $setFunction -match 'MAX_ITEM_NORMAL_MAGICATTRIB \+ i' -and
                               $setFunction -match 'i < count'
    RemovesLegacyGoldTooltipRows = $itemSource -notmatch 'SetTab\.Load\(GOLD_EQUIP_FILE\)'
    GreenQualityChecksAllAttributes = $itemSource -match 'nAttrib < MAX_ITEM_MAGICATTRIB' -and
                                      $itemSource -match 'm_aryMagicAttrib\[nAttrib\]\.nAttribType > 0'
    DeduplicatesVngMagicDescNumericIds = $magicDescSource -match 'nTemplateId = nExisting' -and
                                         $magicDescSource -notmatch 'm_IniFile\.GetKeyByIndex'
    UsesVngTooltipScheme = $mouseHoverSource -match '#define SCHEME_INI\s+"UiMouseHover\.ini"' -and
                           $mouseHoverSource -match 'Ini\.GetInteger\("Main", "Font", VNG_TOOLTIP_DEFAULT_FONT_SIZE'
    UsesVngTooltipFont14 = $mouseHoverSource -match '#define VNG_TOOLTIP_DEFAULT_FONT_SIZE\s+14' -and
                               $mouseHoverIni -match '(?m)^Font=14$'
    UsesVngTooltipIndent6 = $mouseHoverSource -match '#define VNG_TOOLTIP_DEFAULT_INDENT\s+6' -and
                               $mouseHoverIni -match '(?m)^Indent=6$'
    SizesTooltipFromAllEncodedLines = $mouseHoverSource -match 'GetVngTooltipLineHeight\(m_nFontSize\) \* nNumLine' -and
                                      $mouseHoverSource -notmatch 'MAX_TOOLTIP_(HEIGHT|LINES)'
    UsesVngLineGap = $mouseHoverSource -match '#define VNG_TOOLTIP_LINE_GAP\s+1' -and
                     $mouseHoverSource -match 'Param\.nY \+= nLineHeight'
    DrawsTooltipSectionsInsideBody = $mouseHoverSource -match 'Shadow\.oEndPos\.nY = nTextY \+ nLineHeight \* m_nTitleLineNum' -and
                                      $mouseHoverSource -match 'Shadow\.oPosition\.nY = nTextY'
}

$failed = @($checks.GetEnumerator() | Where-Object { -not $_.Value } | ForEach-Object Key)
if ($failed.Count) { throw "VNG equipment tooltip gate FAIL: $($failed -join ', ')" }

[pscustomobject]@{
    Result = 'PASS'
    Checks = $checks.Count
}
