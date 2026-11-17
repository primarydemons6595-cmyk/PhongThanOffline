[CmdletBinding()]
param([string]$ProjectRoot = (Split-Path -Parent $PSScriptRoot))

$ErrorActionPreference = 'Stop'
$ProjectRoot = [IO.Path]::GetFullPath($ProjectRoot).TrimEnd('\')
$latin1 = [Text.Encoding]::GetEncoding(28591)

function Read-LegacySource([string]$RelativePath) {
    $path = Join-Path $ProjectRoot $RelativePath
    if (-not (Test-Path -LiteralPath $path -PathType Leaf)) { throw "Thieu source: $path" }
    [IO.File]::ReadAllText($path, $latin1)
}

$shell = Read-LegacySource 'Sources\Core\Src\CoreShell.cpp'
$itemList = Read-LegacySource 'Sources\Core\Src\KItemList.cpp'
$player = Read-LegacySource 'Sources\Core\Src\KPlayer.cpp'
$item = Read-LegacySource 'Sources\Core\Src\KItem.cpp'
$quickBar = Read-LegacySource 'Sources\GameClient\Ui\UiCase\UiPlayerBar.cpp'
$gameData = Read-LegacySource 'Sources\Core\Src\GameDataDef.h'
$baseTable = Read-LegacySource 'Sources\Core\Src\KBasPropTbl.CPP'
$equipmentColumns = Read-LegacySource 'Sources\Core\Src\PhongThanEquipmentColumns.h'
$setActivation = Read-LegacySource 'Headers\PhongThanSetActivation.h'
$ui = Read-LegacySource 'Sources\GameClient\Ui\UiCase\UiItem.cpp'
$wndEdit = Read-LegacySource 'Sources\GameClient\Ui\Elem\WndEdit.cpp'
$autoPlayHeader = Read-LegacySource 'Sources\GameClient\Ui\UiCase\UiAutoPlay.h'
$autoPlay = Read-LegacySource 'Sources\GameClient\Ui\UiCase\UiAutoPlay.cpp'
$scriptFuns = Read-LegacySource 'Sources\Core\Src\ScriptFuns.cpp'
$itemDiag = Read-LegacySource 'Sources\Core\Src\ItemActionDiag.h'
$taskFuns = Read-LegacySource 'Sources\Core\Src\KTaskFuns.cpp'
$msgSel = Read-LegacySource 'Sources\GameClient\Ui\UiCase\UiMsgSel.cpp'
$characterState = Read-LegacySource 'Headers\PhongThanCharacter.h'
$serverShell = Read-LegacySource 'Sources\Core\Src\CoreServerShell.cpp'
$playerDb = Read-LegacySource 'Sources\Core\Src\KPlayerDBFuns.cpp'
$projectExtensions = Get-Content -LiteralPath (Join-Path $ProjectRoot 'Deploy\Apply-ProjectGameplayExtensions.ps1') -Raw
$msgSelIni = Get-Content -LiteralPath (Join-Path $ProjectRoot 'Deploy\ProjectContent\Ui\ui3\UiMsgSel.ini') -Raw
$wildSuperTemplate = Get-Content -LiteralPath (Join-Path $ProjectRoot 'Deploy\ProjectContent\script\item\ibitem\di_ngoai_phu.lua.template') -Raw

$checks = [ordered]@{
    CoreShellInitializesMovePositions = $shell -match 'P1\.nPlace\s*=\s*P1\.nX\s*=\s*-1' -and $shell -match 'P1\.nY = P2\.nY = 0'
    EquipDestinationHasValidY = $player -match 'DesPos\.nPlace = DesPos\.nX = -1;\s*DesPos\.nY = 0;'
    InventoryEnablesPickPut = $ui -match 'm_ItemBox.Init\(&Ini, "ItemBox"\);\s*m_pSelf->m_ItemBox.EnablePickPut\(true\);'
    CoreShellChecksEquipmentUiIndex = $shell -match 'pObject1->Region\.v\s*<\s*0\s*\|\|\s*pObject1->Region\.v\s*>=\s*itempart_num'
    CoreShellChecksCompoundUiIndex = $shell -match 'pObject1->Region\.v\s*>=\s*MAX_COMPOUND_ITEM'
    CoreShellChecksBuildUiIndex = $shell -match 'pObject1->Region\.v\s*>=\s*MAX_PART_BUILD'
    CoreShellChecksUseItemId = $shell -match 'IsValidClientItemIndex\(pInfo->Obj\.uId\)\s*&&\s*Pos\.nPlace\s*!=\s*-1'
    ItemListChecksAddBounds = $itemList -match 'nIdx\s*>=\s*MAX_ITEM\s*\|\|\s*nPlace\s*<=\s*0\s*\|\|\s*nPlace\s*>=\s*pos_num'
    ItemListChecksExchangeBounds = $itemList -match '!SrcPos\s*\|\|\s*!DesPos[\s\S]{0,180}DesPos->nPlace\s*>=\s*pos_num'
    ClientUseChecksSourcePosition = $player -match 'SrcPos\.nPlace\s*<=\s*0\s*\|\|\s*SrcPos\.nPlace\s*>=\s*pos_num'
    ClientEquipUsesNamedPosition = ([regex]::Matches($player, 'DesPos\.nPlace\s*=\s*pos_equip;')).Count -eq 11
    ClientEquipUsesSingleAtomicMove = $player -match 'SendClientCmdMoveItem\(&SrcPos, &DesPos\)' -and
                                      $player -notmatch 'PLAYER_MOVE_ITEM_COMMAND'
    ServerExecutesCrossContainerMove = $itemList -match 'if \(SrcPos->nPlace != DesPos->nPlace\)[\s\S]{0,900}ExchangeItem\(&Destination, &Destination\)' -and
                                       $itemList -match 'const int nMovingItem = m_Hand'
    ServerRollsBackRejectedCrossMove = $itemList -match 'if \(m_Hand == nMovingItem\)[\s\S]{0,120}ExchangeItem\(&Source, &Source\)'
    CoreMapsAllPhongThanEquipmentSlots = ([regex]::Matches($shell, 'UIEP_HORSE,\s*UIEP_SIGNET,\s*UIEP_SHIPIN')).Count -eq 2 -and
                                          ([regex]::Matches($shell, 'itempart_horse,\s*itempart_signet,\s*itempart_shipin')).Count -eq 2
    ItemPickDropZeroInitializesPayloads = $ui -match 'OnItemPickDrop[\s\S]{0,260}memset\(&Pick, 0, sizeof\(Pick\)\)' -and
                                          $ui -match 'OnItemPickDrop[\s\S]{0,320}memset\(&Drop, 0, sizeof\(Drop\)\)' -and
                                          $ui -match 'OnItemPickDrop[\s\S]{0,380}memset\(&Obj, 0, sizeof\(Obj\)\)'
    ServerMoveChecksPacket = $player -match 'ServerMoveItem\(BYTE\* pProtocol\)[\s\S]{0,300}PHONGTHAN_ITEM_MOVE_REQUEST' -and
                             $player -match 'PHONGTHAN_MSG_INVENTORY_MOVE_REQUEST'
    ServerMoveChecksPlaces = $player -match 'DownPos\.nPlace\s*<=\s*0[\s\S]{0,140}UpPos\.nPlace\s*>=\s*pos_num'
    TooltipUsesBoundedAppend = $item -match 'static void AppendItemDescription' -and
                               $item -match 'nCapacity\s*-\s*nUsed\s*-\s*1'
    TooltipCapsUiCopy = $item -match 'strncpy\(pszOutput, szSafeDesc, GOD_MAX_OBJ_TITLE_LEN - 1\)'
    TooltipSupportsTwentyAttributes = $gameData -match 'MAX_ITEM_MAGICATTRIB\s+20' -and
                                       $gameData -match 'MAX_ITEM_BASEATTRIB\s+20' -and
                                       $gameData -match 'GOD_MAX_OBJ_TITLE_LEN\s+4096'
    VngLoaderFindsExtendedRequirementBoundary = $equipmentColumns -match 'for \(int n=0;n<MAX_ITEM_BASEATTRIB;\+\+n\)' -and
                                                $equipmentColumns -match 'for \(int r=0;r<6;\+\+r\)'
    SetEffectsFollowEquippedPieceCount = $itemList -match 'm_EquipItem\[i\]' -and
                                         $itemList -match 'nEquippedPieces < 3' -and
                                         $itemList -match 'PhongThanActiveSetEffectCount\(nEquippedPieces, effectCount\)'
    SetGameplayUsesFiveLineCap = $gameData -match 'MAX_ITEM_SET_MAGICATTRIB\s+5' -and
                                  $setActivation -match 'if\(pieces>=5\)return effects;' -and
                                  $setActivation -match 'return pieces<effects \? pieces : effects;'
    SetGameplayMatchesVngIdentity = $itemList -match 'EquippedItem\.GetGroup\(\) != nGroup \|\| EquippedItem\.GetSetID\(\) != nSetID'
    SetGameplayRemovedExpandedLegacyCounts = $itemList -notmatch 'MAX_ITEM_MAGICATTRIB\s*-\s*MAX_ITEM_NORMAL_MAGICATTRIB' -and
                                             $itemList -notmatch 'return MAX_ITEM_MAGICATTRIB\s*/\s*2'
    CharacterItemPersistenceIsCanonical = $characterState -match 'PHONGTHAN_CHARACTER_ITEM_RECORD' -and
                                          $characterState -match 'PHONGTHAN_CHARACTER_MAX_ITEMS\s*=\s*512' -and
                                          $characterState -notmatch 'TPhongThanItemRecordLegacy16' -and
                                          $serverShell -notmatch '\b(?:TRoleData|TPhongThanItemRecord)\b'
    ItemPersistenceUsesCanonicalSections = $playerDb -match 'PhongThanCharacterItems\(pState\)' -and
                                           $playerDb -match 'pState->ItemCount' -and
                                           $playerDb -match 'PHONGTHAN_CHARACTER_ITEM_RECORD' -and
                                           $playerDb -notmatch '\b(?:TPhongThanItemRecord|ExpandLegacyItemData|TRoleData)\b'
    EditTextReservesTerminator = $wndEdit -match 'nRet\s*-\s*nSkipAhead\s*<\s*nSize' -and
                                 $wndEdit -notmatch 'nRet\s*-\s*nSkipAhead\s*<=\s*nSize'
    AutoChatOwnsWritableBuffers = $autoPlayHeader -match 'char\s+m_ChatNhamInputText\[256\]' -and
                                  $autoPlay -match 'char\s+m_GetChatNhamText\[256\]\s*=\s*\{0\}'
    AutoChatRejectsLiteralWrites = $autoPlay -notmatch 'm_ChatNhamInputText\s*=\s*"' -and
                                   $autoPlay -notmatch 'char\s*\*autoChatPlayer\s*=\s*new char'
    VngIbItemExeStateRegistered = $scriptFuns -match '\{"SetExeState",\s*LuaSetExeStateCompat\}' -and
                                   $scriptFuns -match 'int\s+LuaSetExeStateCompat\s*\('
    VngIbItemExeStatePreservesDialog = $scriptFuns -match 'int\s+LuaSetExeStateCompat[\s\S]{0,300}Lua_PushNumber\(L, nState\)' -and
                                        $scriptFuns -notmatch 'int\s+LuaSetExeStateCompat[\s\S]{0,300}m_bWaitingPlayerFeedBack'
    StringResourceHonorsCallerBuffer = $taskFuns -match 'GetString\(stringid, "STRING", "", szString, nMaxLen\)' -and
                                         $taskFuns -match 'szString\[nMaxLen - 1\]\s*=\s*0'
    ScriptDialogParserChecksAnswerPointer = ([regex]::Matches($player, 'pAnswer\s*&&\s*i\s*<\s*pScriptAction->OptionCount')).Count -eq 2
    ScriptDialogRejectsInvalidUi = $msgSel -match 'pQAA->AnswerCount\s*>\s*MAX_ANSWERNUM' -and
                                   $msgSel -match 'GetCapability\(\)\s*<=\s*0'
    WildSuperUsesGeneratedPhysicalMapMenu = $projectExtensions -match '\$mapRows\.Count\s+-ne\s+91' -and
                                             $projectExtensions -match "Detail\s*=\s*35;\s*Prefix\s*=\s*'Wild'" -and
                                             $projectExtensions -match "Detail\s*=\s*159;\s*Prefix\s*=\s*'BigWild'"
    WildSuperUsesBoundedPages = $wildSuperTemplate -match '__FUNCTION_PREFIX___PER_PAGE\s*=\s*7' -and
                                $wildSuperTemplate -match 'getn\(tbSay\)' -and
                                $wildSuperTemplate -match '__FUNCTION_PREFIX__Page\('
    WildSuperConsumesOnlyAfterMove = $wildSuperTemplate -match 'local nMoved = NewRuntimeWorld\(' -and
                                      $wildSuperTemplate -match 'nMoved ~= nil and nMoved ~= 0[\s\S]{0,100}CostIBItem\(nItem\)'
    VngDialogSchemeHasSelectableCapacity = $msgSelIni -match '(?ms)\[Select_List\].*MaxMsgCount=20.*Selable=1.*HighLight=1' -and
                                           $msgSelIni -match '\\Spr\\Ui4\\fsbook\\npc_dui_hua_kuang\.spr'
    ItemDiagnosticsUseActiveRuntime = $itemDiag -match 'const char \*pszPath = "item_action_diag\.log"' -and
                                      $itemDiag -notmatch 'DEV AG v1'
    F4HasTwelveVngSlots = $ui -match 'EquipCtrlMap\[UIITEM_EQUIP_BOX_COUNT\]' -and
                           ([regex]::Matches($ui, '\{UIEP_(?:HEAD|BODY|WAIST|FOOT|HAND|HORSE|WAIST_DECOR|FINGER1|FINGER2|SHIPIN|SIGNET|NECK),')).Count -eq 12
    F4RejectsUnknownEquipmentPosition = $ui -match 'if \(nBox < 0 \|\| nBox >= UIITEM_EQUIP_BOX_COUNT\)\s*return;'
}

$checks.QuickEmptySnapshotIsNothing = $quickBar.Contains('immedItemSkillInfo.ImmediaItem[i].uId ? CGOG_IME_ITEM : CGOG_NOTHING')
$checks.QuickEmptySyncIsNothing = $quickBar.Contains('HoldObject(uId ? CGOG_IME_ITEM : CGOG_NOTHING, uId, 0, 0)')
$checks.QuickPickNormalizesRenderGenre = $quickBar.Contains('if (Pick.Obj.uGenre == CGOG_IME_ITEM)') -and $quickBar.Contains('Pick.Obj.uGenre = CGOG_ITEM;')
$checks.QuickRejectsInvalidTarget = $quickBar.Contains('if (i >= UPB_IMMEDIA_ITEM_COUNT || !g_pCoreShell)') -and $quickBar.Contains('if (!Pick.Obj.uId)')
$checks.QuickKeepsSizeAndScopedEligibility = $item.Contains('m_CommonAttrib.nWidth != 1 || m_CommonAttrib.nHeight != 1') -and $item.Contains('return m_CommonAttrib.bShortKey ||') -and $item.Contains('PhongThanConsumableInitialCharges(GetGenre(),GetDetailType(),GetParticular(),')
$failed = @($checks.GetEnumerator() | Where-Object { -not $_.Value } | ForEach-Object Key)
if ($failed.Count) { throw "Item interaction safety gate FAIL: $($failed -join ', ')" }

[pscustomobject]@{
    Result = 'PASS'
    Checks = $checks.Count
    EquipmentUiSlots = 12
    MoveProtocol = 'single Phong Than atomic request'
    BoundsPolicy = 'fail-closed'
    TooltipPolicy = 'truncate-without-heap-write'
}
