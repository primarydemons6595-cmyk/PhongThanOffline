$ErrorActionPreference='Stop'
$root=Split-Path $PSScriptRoot -Parent
$p=Join-Path $root 'Sources/Core/Src/KItemList.cpp'
$hook=Join-Path $PSScriptRoot 'Invoke-LegacyAsciiHook.ps1'
$old="`tmemcpy(m_Items, m_sBackItems, sizeof(PlayerItem) * MAX_PLAYER_ITEM);`r`n`tm_Hand = m_nBackHand;"
$new="`tmemcpy(m_Items, m_sBackItems, sizeof(PlayerItem) * MAX_PLAYER_ITEM);`r`n`r`n`t// Rebuild linked free/use indexes after restoring the trade snapshot.`r`n`t// The old memcpy restored slots but left indexes describing room_trade.`r`n`t m_FreeIdx.Init(MAX_PLAYER_ITEM);`r`n`t m_UseIdx.Init(MAX_PLAYER_ITEM);`r`n`t for (int i = MAX_PLAYER_ITEM - 1; i > 0; --i)`r`n`t {`r`n`t`t if (m_Items[i].nIdx) m_UseIdx.Insert(i);`r`n`t`t else m_FreeIdx.Insert(i);`r`n`t }`r`n`t m_Hand = m_nBackHand;"
$hash=(Get-FileHash -LiteralPath $p).Hash
& $hook -Path $p -ExpectedSha256 $hash -Old $old -New $new -Apply
