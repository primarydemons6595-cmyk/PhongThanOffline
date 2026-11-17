$ErrorActionPreference = 'Stop'
$root = Split-Path -Parent $PSScriptRoot
$encoding = [Text.Encoding]::Default

function Patch-Text([string]$relative, [string]$old, [string]$new) {
    $path = Join-Path $root $relative
    $text = [IO.File]::ReadAllText($path, $encoding)
    if (-not $text.Contains($old)) {
        if ($text.Contains($new)) { return }
        throw "Patch anchor missing: $relative"
    }
    $text = $text.Replace($old, $new)
    [IO.File]::WriteAllText($path, $text, $encoding)
}

# Upgrade an earlier in-progress R2 patch to the final table-native types.
Patch-Text 'Sources\Core\Src\KBasPropTbl.h' 'KItemNormalAttrib m_aryAttrib[2];' 'KMEDATTRIB m_aryAttrib[2];'
Patch-Text 'Sources\Core\Src\KBasPropTbl.cpp' @'
	pTF->GetInteger(nRow, nImageCol + 9, 0, &pBuf->m_aryAttrib[0].nAttribType);
	pTF->GetInteger(nRow, nImageCol + 10, 0, &pBuf->m_aryAttrib[0].nValue[0]);
	pTF->GetInteger(nRow, nImageCol + 11, 0, &pBuf->m_aryAttrib[1].nAttribType);
	pTF->GetInteger(nRow, nImageCol + 12, 0, &pBuf->m_aryAttrib[1].nValue[0]);
	int nEffectTime = 0;
	pTF->GetInteger(nRow, nImageCol + 13, 0, &nEffectTime);
	pBuf->m_aryAttrib[0].nValue[1] = nEffectTime;
	pBuf->m_aryAttrib[1].nValue[1] = nEffectTime;
'@ @'
	pTF->GetInteger(nRow, nImageCol + 9, 0, &pBuf->m_aryAttrib[0].nAttrib);
	pTF->GetInteger(nRow, nImageCol + 10, 0, &pBuf->m_aryAttrib[0].nValue);
	pTF->GetInteger(nRow, nImageCol + 11, 0, &pBuf->m_aryAttrib[1].nAttrib);
	pTF->GetInteger(nRow, nImageCol + 12, 0, &pBuf->m_aryAttrib[1].nValue);
	int nEffectTime = 0;
	pTF->GetInteger(nRow, nImageCol + 13, 0, &nEffectTime);
	pBuf->m_aryAttrib[0].nTime = nEffectTime;
	pBuf->m_aryAttrib[1].nTime = nEffectTime;
'@
Patch-Text 'Sources\Core\Src\KBasPropTbl.h' @'
	char		m_szScript[SZBUFLEN_1];
	BOOL		m_bCanPick;
'@ @'
	char		m_szScript[SZBUFLEN_1];
	// PT IB rows support native effects as well as Lua callbacks. Preserve the
	// typed effect columns so native recovery items do not fall into script NONE.
	KMEDATTRIB m_aryAttrib[2];
	BOOL		m_bCanPick;
'@

Patch-Text 'Sources\Core\Src\KBasPropTbl.cpp' @'
	pTF->GetString(nRow, nImageCol + 8, "", pBuf->m_szScript, sizeof(pBuf->m_szScript));
	pTF->GetInteger(nRow, nImageCol + 16, 0, &pBuf->m_bCanPick);
'@ @'
	pTF->GetString(nRow, nImageCol + 8, "", pBuf->m_szScript, sizeof(pBuf->m_szScript));
	pTF->GetInteger(nRow, nImageCol + 9, 0, &pBuf->m_aryAttrib[0].nAttrib);
	pTF->GetInteger(nRow, nImageCol + 10, 0, &pBuf->m_aryAttrib[0].nValue);
	pTF->GetInteger(nRow, nImageCol + 11, 0, &pBuf->m_aryAttrib[1].nAttrib);
	pTF->GetInteger(nRow, nImageCol + 12, 0, &pBuf->m_aryAttrib[1].nValue);
	int nEffectTime = 0;
	pTF->GetInteger(nRow, nImageCol + 13, 0, &nEffectTime);
	pBuf->m_aryAttrib[0].nTime = nEffectTime;
	pBuf->m_aryAttrib[1].nTime = nEffectTime;
	pTF->GetInteger(nRow, nImageCol + 16, 0, &pBuf->m_bCanPick);
'@

# Remove an earlier in-progress insertion from the magic-script operator. Only
# KBASICPROP_IBITEM owns the newly recovered native-effect columns.
$itemPath = Join-Path $root 'Sources\Core\Src\KItem.cpp'
$itemText = [IO.File]::ReadAllText($itemPath, $encoding)
$magicStart = $itemText.IndexOf('void KItem::operator = (const KBASICPROP_MAGICSCRIPT& sData)')
$skillStart = $itemText.IndexOf('void KItem::operator = (const KBASICPROP_SKILLBOOK& sData)', $magicStart)
$attribBlock = @'
	m_aryBaseAttrib[0].nAttribType = sData.m_aryAttrib[0].nAttrib;
	m_aryBaseAttrib[0].nValue[0] = sData.m_aryAttrib[0].nValue;
	m_aryBaseAttrib[0].nValue[1] = sData.m_aryAttrib[0].nTime;
	m_aryBaseAttrib[1].nAttribType = sData.m_aryAttrib[1].nAttrib;
	m_aryBaseAttrib[1].nValue[0] = sData.m_aryAttrib[1].nValue;
	m_aryBaseAttrib[1].nValue[1] = sData.m_aryAttrib[1].nTime;
'@
if ($magicStart -ge 0 -and $skillStart -gt $magicStart) {
    $segment = $itemText.Substring($magicStart, $skillStart - $magicStart)
    if ($segment.Contains($attribBlock)) {
        $segment = $segment.Replace($attribBlock, '')
        $itemText = $itemText.Substring(0, $magicStart) + $segment + $itemText.Substring($skillStart)
        [IO.File]::WriteAllText($itemPath, $itemText, $encoding)
    }
}

Patch-Text 'Sources\Core\Src\KItem.cpp' @'
	::strncpy(pCA->szScript, sData.m_szScript, sizeof(pCA->szScript) - 1);
	pCA->szScript[sizeof(pCA->szScript) - 1] = 0;
#ifndef _SERVER
	::strncpy(pCA->szImageName, sData.m_szImageName, sizeof(pCA->szImageName) - 1);
	pCA->szImageName[sizeof(pCA->szImageName) - 1] = 0;
	::strncpy(pCA->szIntro, sData.m_szIntro, sizeof(pCA->szIntro) - 1);
	pCA->szIntro[sizeof(pCA->szIntro) - 1] = 0;
#endif
	ZeroMemory(m_aryBaseAttrib, sizeof(m_aryBaseAttrib));
	ZeroMemory(m_aryRequireAttrib, sizeof(m_aryRequireAttrib));
	ZeroMemory(m_aryMagicAttrib, sizeof(m_aryMagicAttrib));
#ifndef _SERVER
'@ @'
	::strncpy(pCA->szScript, sData.m_szScript, sizeof(pCA->szScript) - 1);
	pCA->szScript[sizeof(pCA->szScript) - 1] = 0;
#ifndef _SERVER
	::strncpy(pCA->szImageName, sData.m_szImageName, sizeof(pCA->szImageName) - 1);
	pCA->szImageName[sizeof(pCA->szImageName) - 1] = 0;
	::strncpy(pCA->szIntro, sData.m_szIntro, sizeof(pCA->szIntro) - 1);
	pCA->szIntro[sizeof(pCA->szIntro) - 1] = 0;
#endif
	ZeroMemory(m_aryBaseAttrib, sizeof(m_aryBaseAttrib));
	ZeroMemory(m_aryRequireAttrib, sizeof(m_aryRequireAttrib));
	ZeroMemory(m_aryMagicAttrib, sizeof(m_aryMagicAttrib));
	m_aryBaseAttrib[0].nAttribType = sData.m_aryAttrib[0].nAttrib;
	m_aryBaseAttrib[0].nValue[0] = sData.m_aryAttrib[0].nValue;
	m_aryBaseAttrib[0].nValue[1] = sData.m_aryAttrib[0].nTime;
	m_aryBaseAttrib[1].nAttribType = sData.m_aryAttrib[1].nAttrib;
	m_aryBaseAttrib[1].nValue[0] = sData.m_aryAttrib[1].nValue;
	m_aryBaseAttrib[1].nValue[1] = sData.m_aryAttrib[1].nTime;
#ifndef _SERVER
'@

Patch-Text 'Sources\Core\Src\KItemList.cpp' "`telse if (nItemGenre == item_medicine)" ("`telse if (nItemGenre == item_medicine ||`r`n" +
    "`t`t(nItemGenre == item_ibitem &&`r`n" +
    "`t`t Item[nIdx].m_aryBaseAttrib[0].nAttribType > 0))")

$npcHeaderPath = Join-Path $root 'Sources\Core\Src\KNpc.h'
$npcHeader = [IO.File]::ReadAllText($npcHeaderPath, $encoding)
$npcHeader = [Text.RegularExpressions.Regex]::Replace($npcHeader,
    '(?m)^(\s*int GetNpcPate\(\);\r?\n)(?:\s*int GetNpcVisualPate\(\);\r?\n)*',
    "`$1`tint GetNpcVisualPate();`r`n")
[IO.File]::WriteAllText($npcHeaderPath, $npcHeader, $encoding)

$npcSourcePath = Join-Path $root 'Sources\Core\Src\KNpc.cpp'
$npcSource = [IO.File]::ReadAllText($npcSourcePath, $encoding)
$npcSource = [Text.RegularExpressions.Regex]::Replace($npcSource,
    '(?s)#ifndef _SERVER\r?\nint KNpc::GetNpcVisualPate\(\)\r?\n\{.*?\r?\n\}\r?\n#endif\r?\n\r?\n', '')
[IO.File]::WriteAllText($npcSourcePath, $npcSource, $encoding)

Patch-Text 'Sources\Core\Src\KNpc.cpp' @'
#ifndef _SERVER
int	KNpc::GetNpcPatePeopleInfo()
'@ @'
#ifndef _SERVER
int KNpc::GetNpcVisualPate()
{
	// Player composition has several independent parts; retain its established
	// authored stature. Normal NPCs have one authoritative current body SPR.
	if (m_Kind == kind_player)
		return GetNpcPate();
	char szResource[MAX_PATH];
	ZeroMemory(szResource, sizeof(szResource));
	m_DataRes.GetResFile(m_DataRes.GetAction(), szResource);
	KImageParam image;
	ZeroMemory(&image, sizeof(image));
	if (g_pRepresent && szResource[0] &&
		g_pRepresent->GetImageParam(szResource, &image, ISI_T_SPR))
	{
		// The SPR reference spot is the body-foot attachment point. Its Y value
		// is therefore the exact visual distance from the feet to the top.
		int nVisualHeight = image.nReferenceSpotY;
		if (nVisualHeight <= 0 || nVisualHeight > image.nHeight)
			nVisualHeight = image.nHeight;
		if (nVisualHeight > 0)
			return m_Height + nVisualHeight;
	}
	return GetNpcPate();
}
#endif

#ifndef _SERVER
int	KNpc::GetNpcPatePeopleInfo()
'@

Patch-Text 'Sources\Core\Src\Scene\KScenePlaceC.cpp' 'int nNameHeight = NpcObj.GetNpcPate();' 'int nNameHeight = NpcObj.GetNpcVisualPate();'
Patch-Text 'Sources\Core\Src\KNpc.cpp' @'
		int nVisualHeight = image.nReferenceSpotY;
		if (nVisualHeight <= 0 || nVisualHeight > image.nHeight)
			nVisualHeight = image.nHeight;
		if (nVisualHeight > 0)
			return m_Height + nVisualHeight;
'@ @'
		return PhongThanSpriteVisualAnchorHeight(m_Height, image.nHeight,
			image.nReferenceSpotY, GetNpcPate());
'@

Write-Host 'Applied focused runtime hotfix R2 source patch.'

$roleFilterPath = Join-Path $root 'Sources\MultiServer\Goddess\RoleNameFilter.cpp'
$roleFilterText = [IO.File]::ReadAllText($roleFilterPath, $encoding)
$roleFilterText = $roleFilterText.Replace(
    'const char file_rolenameflt[] = "RoleFilter.ini";',
    'const char file_rolenameflt[] = "rolename.flt";')
[IO.File]::WriteAllText($roleFilterPath, $roleFilterText, $encoding)

$gamePlayerPath = Join-Path $root 'Sources\MultiServer\Bishop\GamePlayer.cpp'
$gamePlayerText = [IO.File]::ReadAllText($gamePlayerPath, $encoding)
$gamePlayerText = [Text.RegularExpressions.Regex]::Replace($gamePlayerText,
    '(?m)^(\s*CPlayerCreator::ROLEPARAM\s+RP;\r?\n)(?:\s*ZeroMemory\(&RP, sizeof\(RP\)\);\r?\n)*',
    "`$1`tZeroMemory(&RP, sizeof(RP));`r`n")
[IO.File]::WriteAllText($gamePlayerPath, $gamePlayerText, $encoding)
Patch-Text 'Sources\MultiServer\Bishop\GamePlayer.cpp' 'nMinLen = nMinLen > sizeof( RP.szName ) ? sizeof( RP.szName ) : nMinLen;' 'nMinLen = nMinLen >= sizeof( RP.szName ) ? sizeof( RP.szName ) - 1 : nMinLen;'
Patch-Text 'Sources\MultiServer\Bishop\GamePlayer.cpp' 'nMinLen = nMinLen > sizeof( RP.szAccName ) ? sizeof( RP.szAccName ) : nMinLen;' 'nMinLen = nMinLen >= sizeof( RP.szAccName ) ? sizeof( RP.szAccName ) - 1 : nMinLen;'

$baseTablePath = Join-Path $root 'Sources\Core\Src\KBasPropTbl.cpp'
$baseTableText = [IO.File]::ReadAllText($baseTablePath, $encoding)
if (-not $baseTableText.Contains('#include "../../../Headers/PhongThanIbItemEffect.h"')) {
    $baseTableText = $baseTableText.Replace(
        '#include "../../../Headers/PhongThanSetTable.h"',
        "#include `"../../../Headers/PhongThanSetTable.h`"`r`n#include `"../../../Headers/PhongThanIbItemEffect.h`"")
}
if (-not $baseTableText.Contains('PhongThanAdaptIbEffectId(pBuf->m_aryAttrib[0].nAttrib)')) {
    $anchor = "`tpTF->GetInteger(nRow, nImageCol + 12, 0, &pBuf->m_aryAttrib[1].nValue);"
    $replacement = $anchor + "`r`n" +
        "`tpBuf->m_aryAttrib[0].nAttrib =`r`n" +
        "`t`tPhongThanAdaptIbEffectId(pBuf->m_aryAttrib[0].nAttrib);`r`n" +
        "`tpBuf->m_aryAttrib[1].nAttrib =`r`n" +
        "`t`tPhongThanAdaptIbEffectId(pBuf->m_aryAttrib[1].nAttrib);"
    if (-not $baseTableText.Contains($anchor)) { throw 'IB effect adapter anchor missing.' }
    $baseTableText = $baseTableText.Replace($anchor, $replacement)
}
[IO.File]::WriteAllText($baseTablePath, $baseTableText, $encoding)
