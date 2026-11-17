int P0PlayerHarness::LoadPlayerItemList(
	const PHONGTHAN_CHARACTER_STATE_HEADER* pState, unsigned int &nParam)
{
	if (!pState)
		return -1;
	const int nItemCount = (int)pState->ItemCount;
	if (!nItemCount)
		return 1;
	if (nParam >= (unsigned int)nItemCount)
		return -1;
	std::vector<P0ItemMagic::Entry> magicSnapshot;bool hasMagicSnapshot=false;std::string magicError;
	if(!P0ItemMagic::Load(pState,magicSnapshot,hasMagicSnapshot,magicError)){P0ItemMagic::Block(pState);CoreLoginDiag("P0 item magic load rejected: %s",magicError.c_str());return -1;}
	if(hasMagicSnapshot&&nParam==0)for(size_t serial=0;serial<magicSnapshot.size();++serial)if(ItemSet.SearchID(magicSnapshot[serial].serial)){P0ItemMagic::Block(pState);CoreLoginDiag("P0 persistent serial collision");return -1;}
	P0ItemMagic::LoadGuard<KItemList> magicGuard(m_ItemList,magicSnapshot,pState,hasMagicSnapshot);
	if(hasMagicSnapshot&&MAX_ITEM-1-ItemSet.GetItemNumber()<(int)magicSnapshot.size())return -1;
	const int nBegin = (int)nParam;
	int nEnd = hasMagicSnapshot?nItemCount:nBegin + DBLOADPERTIME_ITEM;
	if (nEnd > nItemCount)
		nEnd = nItemCount;
	nParam = nEnd;
	const PHONGTHAN_CHARACTER_ITEM_RECORD* pItems =
		PhongThanCharacterItems(pState);
	for (int i = nBegin; i < nEnd; ++i)
	{
		const PHONGTHAN_CHARACTER_ITEM_RECORD* pItemData = &pItems[i];
		if (pItemData->SchemaVersion != PHONGTHAN_EMBEDDED_ITEM_VERSION)
			return -1;
		if (pItemData->ExpireTime && pItemData->ExpireTime < KSG_GetCurSec())
			continue;
		KItem NewItem;
		ZeroMemory(&NewItem, sizeof(NewItem));
		NewItem.m_CommonAttrib.nDetailType = pItemData->DetailType;
		NewItem.m_CommonAttrib.nParticularType = pItemData->ParticularType;
		NewItem.m_CommonAttrib.nLevel = pItemData->Level;
		NewItem.m_CommonAttrib.nSeries = pItemData->Series;
		NewItem.m_CommonAttrib.nRow = pItemData->TemplateRow;
		memcpy(NewItem.m_GeneratorParam.nGeneratorLevel,
			pItemData->GeneratorLevel,
			sizeof(NewItem.m_GeneratorParam.nGeneratorLevel));
		NewItem.m_GeneratorParam.nVersion = pItemData->TableVersion;
		NewItem.m_GeneratorParam.uRandomSeed = pItemData->RandomSeed;
		NewItem.m_GeneratorParam.nLuck = pItemData->Luck;
		BOOL bGenerated = FALSE;
		switch(pItemData->Genre)
		{
		case item_equip:
			bGenerated = ItemGen.Gen_ExistEquipmentByTemplateRow(
				pItemData->DetailType, pItemData->TemplateRow,
				pItemData->Series, NewItem.m_GeneratorParam.nGeneratorLevel,
				pItemData->Luck, pItemData->TableVersion, &NewItem);
			break;
		case item_medicine:
			bGenerated = ItemGen.Gen_Medicine(pItemData->DetailType,
				pItemData->Level, pItemData->TableVersion, &NewItem);
			break;
		case item_event:
			bGenerated = ItemGen.Gen_Event(pItemData->DetailType, &NewItem);
			break;
		case item_materials:
			bGenerated = ItemGen.Gen_Material(pItemData->DetailType, &NewItem);
			break;
		case item_task:
			bGenerated = ItemGen.Gen_Quest(pItemData->DetailType, &NewItem);
			break;
		case item_townportal:
			bGenerated = ItemGen.Gen_TownPortal(pItemData->DetailType, &NewItem);
			break;
		case item_magicscript:
			bGenerated = ItemGen.Gen_MagicScript(pItemData->DetailType,
				&NewItem, pItemData->Level, pItemData->Series, pItemData->Luck);
			break;
		case item_skillbook:
			bGenerated = ItemGen.Gen_SkillBook(pItemData->DetailType,
				pItemData->ParticularType, pItemData->Level, &NewItem);
			break;
		case item_ibitem:
			bGenerated = ItemGen.Gen_IBItem(pItemData->DetailType,
				pItemData->ParticularType, &NewItem);
			break;
		}
		if (pItemData->TemplateRow == 59 || pItemData->TemplateRow == 249)
		{
			FILE* log = fopen("equipment_delivery.log", "a");
			if (log) { fprintf(log,"LOAD row=%d detail=%d generated=%d size=%d,%d position=%d,%d,%d\n",
				pItemData->TemplateRow,pItemData->DetailType,bGenerated,NewItem.GetWidth(),NewItem.GetHeight(),
				pItemData->Container,pItemData->SlotX,pItemData->SlotY); fclose(log); }
		}
		if (!bGenerated) { if(hasMagicSnapshot||NativeDropRollout::Active()){P0ItemMagic::Block(pState);return -1;} continue; }
		// Native medicine regeneration assigns a fresh RNG seed; retain the canonical replay metadata.
		NewItem.m_GeneratorParam.uRandomSeed = pItemData->RandomSeed;
		if (pItemData->Durability != -2)
			NewItem.SetDurability(pItemData->Durability);
		NewItem.GetBackLocal()->Release();
		NewItem.SetExpireTime(pItemData->ExpireTime);
		NewItem.SetOwner(pItemData->OwnerId);
		NewItem.SetTradePrice(pItemData->TradePrice);
		KLockItem Lock;
		Lock.nState = pItemData->LockState;
		Lock.dwLockTime = pItemData->LockUntil;
		if (Lock.nState == LOCK_STATE_UNLOCK && Lock.dwLockTime <= KSG_GetCurSec())
		{
			Lock.nState = LOCK_STATE_NORMAL;
			Lock.dwLockTime = 0;
		}
		NewItem.SetLock(&Lock);
		NewItem.SetFortune(pItemData->Fortune);
		NewItem.SetLockSell(pItemData->LockSell ? TRUE : FALSE);
		NewItem.SetLockTrade(pItemData->LockTrade ? TRUE : FALSE);
		NewItem.SetLockDrop(pItemData->LockDrop ? TRUE : FALSE);
		int nScriptParam = pItemData->ScriptParam;
		if (pItemData->Genre == item_ibitem && nScriptParam <= 0)
			nScriptParam = 0;
		NewItem.SetParam(nScriptParam);
		NewItem.SetStackNum(pItemData->StackCount);
		NewItem.SetFlash(pItemData->Flash);
		if(NewItem.GetGenre()==item_equip && !NewItem.ApplyUpgradeState(pItemData->UpgradeLevel))
		{
			CoreLoginDiag("Invalid/unresolved equipment upgrade state detail=%d row=%d state=%d",
				pItemData->DetailType,pItemData->TemplateRow,pItemData->UpgradeLevel);
			return -1;
		}
		NewItem.m_CommonAttrib.nPhysicVal = pItemData->PhysicalValue;
		NewItem.m_CommonAttrib.nMagicVal = pItemData->MagicValue;
		NewItem.SetRow(pItemData->TemplateRow);
		if(hasMagicSnapshot&&!P0ItemMagic::Restore(NewItem,magicSnapshot[i],magicError)){CoreLoginDiag("P0 item magic restore rejected: %s",magicError.c_str());return -1;}
		P1BlueTrace("SERVER_LOAD_RESTORED",NewItem);
		const int nIndex = hasMagicSnapshot?ItemSet.AddPersistent(&NewItem,magicSnapshot[i].serial):ItemSet.Add(&NewItem);
		if(NativeDropRollout::Active()&&nIndex<=0){P0ItemMagic::Block(pState);return -1;}
		if(hasMagicSnapshot){
			if(nIndex<=0||!m_ItemList.Add(nIndex,pItemData->Container,pItemData->SlotX,pItemData->SlotY,false))return -1;
		}else if (nIndex > 0){
			if(!m_ItemList.Add(nIndex, pItemData->Container,pItemData->SlotX,pItemData->SlotY,false)&&NativeDropRollout::Active()){ItemSet.Remove(nIndex);P0ItemMagic::Block(pState);return -1;}
		}
	}
	magicGuard.Commit();
	return nParam >= (unsigned int)nItemCount ? 1 : 0;
}

int P0PlayerHarness::SavePlayerItemList(PHONGTHAN_CHARACTER_STATE_HEADER* pState,
	unsigned int nCapacity)
{
	if (!pState)
		return -1;
	PHONGTHAN_CHARACTER_ITEM_RECORD* pItems = PhongThanCharacterItems(pState);
	int nItemCount = 0;
	std::vector<P0ItemMagic::Entry> magicSnapshot;
	int nIdx = 0;
	while ((nIdx = m_ItemList.m_UseIdx.GetNext(nIdx)) != 0)
	{
		const int nItemIndex = m_ItemList.m_Items[nIdx].nIdx;
		if (Item[nItemIndex].IsTemp())
			continue;
		if (nItemCount >= PHONGTHAN_CHARACTER_MAX_ITEMS)
			return -1;
		const unsigned __int64 nRequired =
			(unsigned __int64)((BYTE*)&pItems[nItemCount + 1] - (BYTE*)pState);
		if (nRequired > nCapacity)
			return -1;
		PHONGTHAN_CHARACTER_ITEM_RECORD* pItem = &pItems[nItemCount];
		ZeroMemory(pItem, sizeof(*pItem));
		pItem->SchemaVersion = PHONGTHAN_EMBEDDED_ITEM_VERSION;
		pItem->Genre = Item[nItemIndex].m_CommonAttrib.nItemGenre;
		pItem->DetailType = Item[nItemIndex].m_CommonAttrib.nDetailType;
		pItem->ParticularType = Item[nItemIndex].m_CommonAttrib.nParticularType;
		pItem->Level = Item[nItemIndex].m_CommonAttrib.nLevel;
		pItem->Series = Item[nItemIndex].m_CommonAttrib.nSeries;
		if (Item[nItemIndex].GetBackLocal()->nPlace >= pos_hand &&
			Item[nItemIndex].GetBackLocal()->nPlace < pos_num)
		{
			pItem->Container = Item[nItemIndex].GetBackLocal()->nPlace;
			pItem->SlotX = Item[nItemIndex].GetBackLocal()->nX;
			pItem->SlotY = Item[nItemIndex].GetBackLocal()->nY;
		}
		else
		{
			pItem->Container = m_ItemList.m_Items[nIdx].nPlace;
			pItem->SlotX = m_ItemList.m_Items[nIdx].nX;
			pItem->SlotY = m_ItemList.m_Items[nIdx].nY;
		}
		pItem->TableVersion = Item[nItemIndex].GetItemParam()->nVersion;
		pItem->RandomSeed = Item[nItemIndex].GetItemParam()->uRandomSeed;
		memcpy(pItem->GeneratorLevel,
			Item[nItemIndex].GetItemParam()->nGeneratorLevel,
			sizeof(pItem->GeneratorLevel));
		pItem->Luck = Item[nItemIndex].GetItemParam()->nLuck;
		pItem->Durability = Item[nItemIndex].GetDurability();
		pItem->StackCount = Item[nItemIndex].GetStackNum();
		pItem->ExpireTime = Item[nItemIndex].GetExpireTime();
		pItem->LockState = Item[nItemIndex].GetLock()->nState;
		pItem->LockUntil = Item[nItemIndex].GetLock()->dwLockTime;
		pItem->ScriptParam = Item[nItemIndex].GetParam();
		pItem->Fortune = Item[nItemIndex].GetFortune();
		pItem->OwnerId = Item[nItemIndex].GetOwner();
		pItem->TradePrice = Item[nItemIndex].GetTradePrice();
		pItem->LockSell = Item[nItemIndex].m_CommonAttrib.bLockSell ? 1 : 0;
		pItem->LockTrade = Item[nItemIndex].m_CommonAttrib.bLockTrade ? 1 : 0;
		pItem->LockDrop = Item[nItemIndex].m_CommonAttrib.bLockDrop ? 1 : 0;
		pItem->Flash = Item[nItemIndex].GetFlash();
		pItem->UpgradeLevel = Item[nItemIndex].GetUpgradeState();
		pItem->PhysicalValue = Item[nItemIndex].m_CommonAttrib.nPhysicVal;
		pItem->MagicValue = Item[nItemIndex].m_CommonAttrib.nMagicVal;
		pItem->TemplateRow = Item[nItemIndex].GetRow();
		P1BlueTrace("SERVER_SAVE_CAPTURE",Item[nItemIndex]);
		P0ItemMagic::Entry magic;P0ItemMagic::Capture(Item[nItemIndex],nItemCount,magic);magicSnapshot.push_back(magic);
		++nItemCount;
	}
	pState->ItemCount = nItemCount;
	std::string magicError;if(!P0ItemMagic::Save(pState,magicSnapshot,magicError)){CoreLoginDiag("P0 item magic save rejected: %s",magicError.c_str());return -1;}
	return 1;
}

