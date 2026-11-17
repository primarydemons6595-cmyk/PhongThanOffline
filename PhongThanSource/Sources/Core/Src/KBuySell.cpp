#include <vector>
#include "KEngine.h"
#include "KCore.h"
#include "../../../Headers/PhongThanF2Balance.h"
#include "PhongThanSkillBookKey.h"
#include <vector>
#include "KTabFile.h"
#include "KPlayer.h"
#include "KPlayerSet.h"
#include "KTongData.h"
#include "KNpc.h"
#include "KItemGenerator.h"
#include "KSubWorldSet.h"
#include "KItemSet.h"
#include "KBuySell.h"
#include "PhongThanConsumables.h"
#include "ItemActionDiag.h"
#include "KObjSet.h"
#include "KTongProtocol.h"
#ifndef _STANDALONE
#include "CoreShell.h"
#include "crtdbg.h"
#endif
#include "CoreUseNameDef.h"
#ifdef _SERVER
//#include "../MultiServer/Heaven/Interface/iServer.h"
#endif

#include	<time.h>

#define	SHOP_BOX_WIDTH		6
#define	SHOP_BOX_HEIGHT		10
#define	SSHOP_BOX_WIDTH		12
#define	SSHOP_BOX_HEIGHT	10

#define PHONGTHAN_IBSHOP_GOODS_FILE "\\settings\\ibshop\\ibshopgoods.txt"

static char* g_PhongThanIBShopCategoryFiles[PHONGTHAN_IBSHOP_TAB_COUNT] =
{
	"\\settings\\ibshop\\suggestgoods.txt",
	"\\settings\\ibshop\\potiongoods.txt",
	"\\settings\\ibshop\\charmgoods.txt",
	"\\settings\\ibshop\\upgradegoods.txt",
	"\\settings\\ibshop\\promisegoods.txt",
	"\\settings\\ibshop\\micsgoods.txt",
	"\\settings\\ibshop\\pendantgoods.txt",
	"\\settings\\ibshop\\specialgoods.txt"
};

// Physical unlocked Xu held in F4; shared by shop validation and client display.
int PhongThanXuCountForShop(int player) {
    if(player<=0 || player>=MAX_PLAYER)return 0;
    int total=0;KItemList& bag=Player[player].m_ItemList;
    for(PlayerItem* p=bag.GetFirstItem();p;p=bag.GetNextItem()) {
        int i=p->nIdx;
        if((p->nPlace==pos_equiproom || p->nPlace==pos_equiproomex || p->nPlace==pos_ptbag2 || p->nPlace==pos_ptbag3) &&
           i>0 && i<MAX_ITEM && Item[i].GetID() &&
           Item[i].GetGenre()==item_task && Item[i].GetDetailType()==47 && !Item[i].GetLock()->IsLock()) {
            int count=Item[i].GetStackNum();if(count>0 && total<=2147483647-count)total+=count;
        }
    }
    return total;
}

#include "PhongThanF2Balance.inl"
static PTF2Currency ptF2Currencies[2];
static void PTLoadF2Currencies(){
    KIniFile ini;ini.Load("\\settings\\phongthan\\F2Display.ini");
    for(int i=0;i<2;++i){char section[20];sprintf(section,"Slot%d",i+1);PTF2Currency& c=ptF2Currencies[i];memset(&c,0,sizeof(c));
        int* values=&c.Query.Type;const char* keys[6]={"Type","Genre","Detail","Particular","Level","Series"};
        for(int f=0;f<6;++f)ini.GetInteger(section,keys[f],f==0?(i==0?0:3):0,&values[f]);
        ini.GetInteger(section,"Enabled",1,&c.Enabled);ini.GetString(section,"Label",i==0?"Xu":"Bound",c.Label,sizeof(c.Label));
    }
}
const PTF2Currency* PhongThanF2CurrencyConfig(int slot){return slot>=1&&slot<=2?&ptF2Currencies[slot-1]:0;}
int PhongThanF2WalletCount(int player,const PTF2BalanceQuery& q){return PhongThanF2BalanceValue(player,q);}
int PhongThanF2CurrencyCount(int player,int slot){const PTF2Currency* c=PhongThanF2CurrencyConfig(slot);return c?PhongThanF2BalanceValue(player,c->Query):0;}
int KBuySell::GetPaymentSlot(const KItem* item){int i=item-m_Item;return i>=0&&i<(int)m_PaymentSlots.size()?m_PaymentSlots[i]:1;}
static BOOL IsPhongThanIBShop(int nShop)
{
	return nShop >= PHONGTHAN_IBSHOP_FIRST_SHOP_INDEX &&
		nShop < PHONGTHAN_IBSHOP_FIRST_SHOP_INDEX + PHONGTHAN_IBSHOP_TAB_COUNT;
}

static BOOL GenerateShopItem(const ItemGenerate& itemKey, KItem* pItem)
{
	if (!pItem)
		return FALSE;

	switch (itemKey.nGenre)
	{
	case item_equip:
		return ItemGen.Gen_Equipment(itemKey.nDetailType,
			itemKey.nParticularType, itemKey.nSeriesReq, itemKey.nLevel,
			NULL, 0, g_SubWorldSet.GetGameVersion(), pItem);
	case item_medicine:
		return ItemGen.Gen_Medicine(itemKey.nDetailType, itemKey.nLevel,
			g_SubWorldSet.GetGameVersion(), pItem);
	case item_event:
		return ItemGen.Gen_Event(itemKey.nDetailType, pItem);
	case item_materials:
		return ItemGen.Gen_Material(itemKey.nDetailType, pItem);
	case item_task:
		return ItemGen.Gen_Quest(itemKey.nDetailType, pItem);
	case item_townportal:
		return ItemGen.Gen_TownPortal(itemKey.nDetailType, pItem);
	case item_magicscript:
        // VNG Goods stores script identity in ParticularType. Passing its
        // class marker (1) generated the same broken Thai Ng pill everywhere.
        if((itemKey.nParticularType>0?itemKey.nParticularType:itemKey.nDetailType)==1)return FALSE;
        return ItemGen.Gen_MagicScript(itemKey.nParticularType>0?itemKey.nParticularType:itemKey.nDetailType, pItem,
			itemKey.nLevel, itemKey.nSeriesReq, 0);
	case item_skillbook:
		return ItemGen.Gen_SkillBook(itemKey.nDetailType, PhongThanCurrentSkillBookKey(itemKey.nDetailType,itemKey.nParticularType), itemKey.nLevel, pItem);
	case item_ibitem:
		return ItemGen.Gen_IBItem(itemKey.nDetailType, itemKey.nParticularType, pItem);
	default:
		return FALSE;
	}
}

KBuySell	BuySell;

KBuySell::KBuySell()
{
#ifndef _SERVER
	m_pShopRoom = NULL;
	m_pSShopRoom = NULL;
#endif
	m_Item = NULL;
	m_SellItem = NULL;
	m_Width = 0;
	m_Height = 0;
	m_ItemNum = 0;
}

KBuySell::~KBuySell()
{
#ifndef _SERVER
	if (m_pShopRoom)
	{
		delete m_pShopRoom;
		m_pShopRoom = NULL;
	}
	if (m_pSShopRoom)
	{
		delete m_pSShopRoom;
		m_pSShopRoom = NULL;
	}
#endif
	if (m_Item)
	{
		delete [] m_Item;
		m_Item = NULL;
	}
	if (m_SellItem)
	{
		for (int i = 0; i < m_Height; i++)
		{
			if (m_SellItem[i])
			{
				delete[] m_SellItem[i];
				m_SellItem[i] = NULL;
			}
		}
		delete[] m_SellItem;
		m_SellItem = NULL;
	}
	m_Width = 0;
	m_Height = 0;
	m_ItemNum = 0;
}

int KBuySell::Init()
{
    PTLoadF2Currencies();m_PaymentSlots.clear();
	KTabFile		GoodsFile;
	KTabFile		BuySellFile;
	KTabFile		IBShopGoodsFile;
//	g_SetFilePath("\\");
	if (!BuySellFile.Load(BUYSELL_FILE) || !GoodsFile.Load(GOODS_FILE))
		return 0;

	int nGoodsHeight = GoodsFile.GetHeight() - 1;
	int nGoodsWidth = GoodsFile.GetWidth() - 1;
	if (nGoodsWidth <= 0 || nGoodsHeight <= 0)
		return 0;

	BOOL bHasIBShop = IBShopGoodsFile.Load(PHONGTHAN_IBSHOP_GOODS_FILE);
	int nIBShopHeight = bHasIBShop ? IBShopGoodsFile.GetHeight() - 1 : 0;
	int nIBShopGenerated = 0;
	int nIBShopCategoryLoaded = 0;
	int nIBShopFailedRows[32];
	int nIBShopFailedCount = 0;
	int nItemCapacity = nGoodsHeight + nIBShopHeight;
	int* pIBShopItemIndex = NULL;

	m_Item = (KItem *)new KItem[nItemCapacity];
	if (!m_Item)
		return 0;
	if (nIBShopHeight > 0)
	{
		pIBShopItemIndex = new int[nIBShopHeight];
		if (!pIBShopItemIndex)
			return 0;
		for (int nIBInitRow = 0; nIBInitRow < nIBShopHeight; ++nIBInitRow)
			pIBShopItemIndex[nIBInitRow] = -1;
	}

	// BuySell references original one-based goods rows, not generated indexes.
	std::vector<int> normalGoodsIndex(nGoodsHeight, -1);
	ItemGenerate itemKey;
	for (int k = 0; k < nGoodsHeight; ++k)
	{
		GoodsFile.GetInteger(k + 2, 1, -1, &itemKey.nGenre);
		GoodsFile.GetInteger(k + 2, 2, -1, &itemKey.nDetailType);
		GoodsFile.GetInteger(k + 2, 3, -1, &itemKey.nParticularType);
		GoodsFile.GetInteger(k + 2, 4, series_nil, &itemKey.nSeriesReq);
		GoodsFile.GetInteger(k + 2, 5, 0, &itemKey.nLevel);

		if (!GenerateShopItem(itemKey, &m_Item[m_ItemNum]))
			continue;
		m_Item[m_ItemNum].GetBackLocal()->Release();
		normalGoodsIndex[k] = m_ItemNum;
		++m_ItemNum;
	}

	for (int nIBLoadRow = 0; nIBLoadRow < nIBShopHeight; ++nIBLoadRow)
	{
		int nStackCount = 1;
		int nNewArrival = 0;
		int nShowPrice = -1, nActualPrice = -1;
		IBShopGoodsFile.GetInteger(nIBLoadRow + 2, 2, -1, &itemKey.nGenre);
		if (itemKey.nGenre >= item_number)
		{
			// A small VNG tail block omits the Genre cell. Its first numeric
			// value is the IBItem detail id, so restore the declared schema.
			itemKey.nDetailType = itemKey.nGenre;
			itemKey.nGenre = item_ibitem;
			IBShopGoodsFile.GetInteger(nIBLoadRow + 2, 3, -1, &itemKey.nParticularType);
			IBShopGoodsFile.GetInteger(nIBLoadRow + 2, 4, 0, &itemKey.nLevel);
			IBShopGoodsFile.GetInteger(nIBLoadRow + 2, 5, series_nil, &itemKey.nSeriesReq);
			IBShopGoodsFile.GetInteger(nIBLoadRow + 2, 6, 1, &nStackCount);
			IBShopGoodsFile.GetInteger(nIBLoadRow + 2, 13, 0, &nNewArrival);
			IBShopGoodsFile.GetInteger(nIBLoadRow + 2, 7, -1, &nShowPrice);
			IBShopGoodsFile.GetInteger(nIBLoadRow + 2, 8, -1, &nActualPrice);
		}
		else
		{
			IBShopGoodsFile.GetInteger(nIBLoadRow + 2, 3, -1, &itemKey.nDetailType);
			IBShopGoodsFile.GetInteger(nIBLoadRow + 2, 4, -1, &itemKey.nParticularType);
			IBShopGoodsFile.GetInteger(nIBLoadRow + 2, 5, 0, &itemKey.nLevel);
			IBShopGoodsFile.GetInteger(nIBLoadRow + 2, 6, series_nil, &itemKey.nSeriesReq);
			IBShopGoodsFile.GetInteger(nIBLoadRow + 2, 7, 1, &nStackCount);
			IBShopGoodsFile.GetInteger(nIBLoadRow + 2, 14, 0, &nNewArrival);
			IBShopGoodsFile.GetInteger(nIBLoadRow + 2, 8, -1, &nShowPrice);
			IBShopGoodsFile.GetInteger(nIBLoadRow + 2, 9, -1, &nActualPrice);
		}

		if (nShowPrice <= 0 || nActualPrice <= 0 || !GenerateShopItem(itemKey, &m_Item[m_ItemNum]))
		{
			if (nIBShopFailedCount < sizeof(nIBShopFailedRows) / sizeof(nIBShopFailedRows[0]))
				nIBShopFailedRows[nIBShopFailedCount++] = nIBLoadRow + 1;
			continue;
		}
        int currencySlot=1;IBShopGoodsFile.GetInteger(nIBLoadRow+2,"PaymentSlot",1,&currencySlot);
        if(currencySlot<1||currencySlot>2)continue;
        if((int)m_PaymentSlots.size()<=m_ItemNum)m_PaymentSlots.resize(m_ItemNum+1,1);
        m_PaymentSlots[m_ItemNum]=currencySlot;
        m_Item[m_ItemNum].SetShopPrices(nShowPrice, nActualPrice);
        // PT consumable UseCount is capacity/charges, not shop bundle quantity.
        int charges = PhongThanConsumableInitialCharges(m_Item[m_ItemNum].GetGenre(),
            m_Item[m_ItemNum].GetDetailType(), m_Item[m_ItemNum].GetParticular(),
            m_Item[m_ItemNum].GetMaxStackNum(), m_Item[m_ItemNum].GetScript());
        if(PhongThanTravelHasCharges(m_Item[m_ItemNum].GetGenre(),m_Item[m_ItemNum].GetDetailType(),m_Item[m_ItemNum].GetParticular()))charges=m_Item[m_ItemNum].GetMaxStackNum();
        m_Item[m_ItemNum].SetStackNum(charges > 0 ? charges : (nStackCount > 0 ? nStackCount : 1));
		int hot=0,suggested=0;
        int shift=itemKey.nGenre==item_ibitem?0:0;
        // The original tail without Genre shifts every flag one cell left.
        int rawGenre=0;IBShopGoodsFile.GetInteger(nIBLoadRow+2,2,0,&rawGenre);
        if(rawGenre>=item_number)shift=1;
        IBShopGoodsFile.GetInteger(nIBLoadRow+2,12-shift,0,&hot);
        IBShopGoodsFile.GetInteger(nIBLoadRow+2,13-shift,0,&suggested);
        m_Item[m_ItemNum].SetNewArrival((nNewArrival>0?1:0)|(hot>0?2:0)|(suggested>0?4:0));
		m_Item[m_ItemNum].GetBackLocal()->Release();
		pIBShopItemIndex[nIBLoadRow] = m_ItemNum;
		++m_ItemNum;
		++nIBShopGenerated;
	}

	int nNormalShopHeight = BuySellFile.GetHeight() - 1;
	int nIBShopEnd = PHONGTHAN_IBSHOP_FIRST_SHOP_INDEX + PHONGTHAN_IBSHOP_TAB_COUNT;
	m_Height = nNormalShopHeight > nIBShopEnd ? nNormalShopHeight : nIBShopEnd;
	m_Width = BuySellFile.GetWidth();
	if (nIBShopHeight > m_Width)
		m_Width = nIBShopHeight;
	
	if (m_Width <= 0 || m_Height <= 0)
	{
		delete [] pIBShopItemIndex;
		return 0;
	}

	m_SellItem = (int **)new int*[m_Height];
	if (!m_SellItem)
	{
		delete [] pIBShopItemIndex;
		return 0;
	}
	for (int i = 0; i < m_Height; i++)
	{
		m_SellItem[i] = NULL;
		m_SellItem[i] = (int *)new int[m_Width];
		if (!m_SellItem[i])
		{
			delete [] pIBShopItemIndex;
			return 0;
		}
		for (int j = 0; j < m_Width; ++j)
			m_SellItem[i][j] = -1;
	}

	FILE* normalShopLog = fopen("normal_shop_loader_diag.csv", "wt");
	if(normalShopLog)fprintf(normalShopLog,"shop,slot,goods_row,item_index,genre,detail\n");
	for (int nNormalRow = 0; nNormalRow < nNormalShopHeight; ++nNormalRow)
	{
		for (int column = 0; column < BuySellFile.GetWidth(); ++column)
		{
			int rowId = -1;
			// VNG BuySell has goods in column one; it has no shop-ID column.
			BuySellFile.GetInteger(nNormalRow + 2, column + 1, -1, &rowId);
			if (rowId > 0 && rowId <= nGoodsHeight)
			{
				m_SellItem[nNormalRow][column] = normalGoodsIndex[rowId - 1];
				const int index=normalGoodsIndex[rowId - 1];
				if(normalShopLog)fprintf(normalShopLog,"%d,%d,%d,%d,%d,%d\n",nNormalRow+1,column,rowId,index,
					index>=0?m_Item[index].GetGenre():-1,index>=0?m_Item[index].GetDetailType():-1);
			}
		}
	}
	if(normalShopLog)fclose(normalShopLog);

	if (pIBShopItemIndex)
	{
		for (int nCategory = 0; nCategory < PHONGTHAN_IBSHOP_TAB_COUNT; ++nCategory)
		{
			KTabFile CategoryFile;
			if (!CategoryFile.Load(g_PhongThanIBShopCategoryFiles[nCategory]))
				continue;
			++nIBShopCategoryLoaded;
			int nCategorySlot = 0;
			for (int nRow = 2; nRow <= CategoryFile.GetHeight(); ++nRow)
			{
				int nIBIndex = -1;
				CategoryFile.GetInteger(nRow, 1, -1, &nIBIndex);
				// VNG category Index is zero-based; index 0 is a valid product.
				if (nIBIndex < 0 || nIBIndex >= nIBShopHeight)
					continue;
				int nItemIndex = pIBShopItemIndex[nIBIndex];
				if (nItemIndex >= 0 && nCategorySlot < m_Width)
					m_SellItem[PHONGTHAN_IBSHOP_FIRST_SHOP_INDEX + nCategory][nCategorySlot++] = nItemIndex;
			}
		}


	}
	delete [] pIBShopItemIndex;
#ifdef _SERVER
	FILE* pIBShopDiag = fopen("ibshop_loader_diag.log", "wt");
	if (pIBShopDiag)
	{
		fprintf(pIBShopDiag,
			"source=%s rows=%d generated=%d tabs=%d categories=%d price_source=ActualPrice result=%s\n",
			PHONGTHAN_IBSHOP_GOODS_FILE, nIBShopHeight, nIBShopGenerated,
			PHONGTHAN_IBSHOP_TAB_COUNT, nIBShopCategoryLoaded,
			(bHasIBShop && nIBShopGenerated == nIBShopHeight &&
			 nIBShopCategoryLoaded == PHONGTHAN_IBSHOP_TAB_COUNT) ? "ready" : "incomplete");
		if (nIBShopFailedCount > 0)
		{
			fprintf(pIBShopDiag, "failed_rows=");
			for (int nFailed = 0; nFailed < nIBShopFailedCount; ++nFailed)
				fprintf(pIBShopDiag, "%s%d", nFailed ? "," : "", nIBShopFailedRows[nFailed]);
			fprintf(pIBShopDiag, "\n");
		}
		fclose(pIBShopDiag);
	}
#endif
#ifndef _SERVER
	if (!m_pShopRoom)
	{
		m_pShopRoom = new KInventory;
		m_pShopRoom->Init(SHOP_BOX_WIDTH, SHOP_BOX_HEIGHT);
	}
	if (!m_pSShopRoom)
	{
		m_pSShopRoom = new KInventory;
		m_pSShopRoom->Init(SSHOP_BOX_WIDTH, SSHOP_BOX_HEIGHT);
	}
#endif
	return m_ItemNum;
}

KItem* KBuySell::GetItem(int nIndex)
{
	if (nIndex < 0 || nIndex >= m_ItemNum || !m_Item)
		return NULL;

	return &m_Item[nIndex];
}

int KBuySell::GetItemIndex(int nShop, int nIndex)
{
	if (!m_SellItem || nShop < 0 || nShop >= m_Height || nIndex < 0 || nIndex >= m_Width)
		return -1;

	if (!m_SellItem[nShop])
		return -1;

	return m_SellItem[nShop][nIndex];
}

BOOL KBuySell::BuySellCheck(int nBuy, int nBuyIdx)
{
	if (nBuy < 0 || nBuy >= m_Height)
		return FALSE;
	if (nBuyIdx < 0 || nBuyIdx >= m_Width)
		return FALSE;
	int nIdx = m_SellItem[nBuy][nBuyIdx];
	if (nIdx < 0 || nIdx >= m_ItemNum)
		return FALSE;

	return TRUE;
}

#ifdef _SERVER
BOOL KBuySell::CanBuy(int nPlayerIdx, int nBuy, int nBuyIdx, int nBuyNumber)
{
	BOOL bShowMsg = TRUE;
	if (nPlayerIdx < 0 || nPlayerIdx >= MAX_PLAYER)
	{
		_ASSERT(0);
		return FALSE;
	}
	if (Player[nPlayerIdx].CheckTrading())
		return FALSE;

	if (BuySellCheck(nBuy, nBuyIdx) == FALSE)
		return FALSE;

	if (nBuy == -1)
	{
		printf("[error]BuySell: %s buy idx error (%d)!", Npc[Player[nPlayerIdx].m_nIndex].Name, nBuy);
		return FALSE;
	}

	if (nBuyNumber <= 0)
		return FALSE;

	int nIdx = m_SellItem[nBuy][nBuyIdx];
	int nUnitPrice = m_Item[nIdx].GetCurPrice();
	if (IsPhongThanIBShop(nBuy) && (nUnitPrice < 0 || (nUnitPrice > 0 && nBuyNumber > 2147483647 / nUnitPrice))) return FALSE;
	int nPrice = nUnitPrice * nBuyNumber;
    if(IsPhongThanIBShop(nBuy))return nUnitPrice>0&&PhongThanF2CurrencyCount(nPlayerIdx,GetPaymentSlot(&m_Item[nIdx]))>=nPrice;
	SHOW_MSG_SYNC	sMsg;
	sMsg.ProtocolType = s2c_msgshow;
	sMsg.m_wLength = sizeof(SHOW_MSG_SYNC) - 1 - sizeof(LPVOID);
    // Phong Than NPC shops also operate outside towns, in fight mode.
	{
		switch (Player[nPlayerIdx].m_BuyInfo.m_nMoneyUnit)
		{
		case moneyunit_money:
			if (Player[nPlayerIdx].m_ItemList.GetEquipmentMoney() < nPrice)
			{
				bShowMsg = FALSE;		
				sMsg.m_wMsgID = enumMSG_ID_SHOP_NO_MONEY;
			}
			break;
		case moneyunit_extpoint:
			if (Player[nPlayerIdx].GetExtPoint() < nPrice)
			{
				bShowMsg = FALSE;		
				sMsg.m_wMsgID = enumMSG_ID_SHOP_NO_EXTPOINT;
			}
			break;
		case moneyunit_fuyuan:
			if (Player[nPlayerIdx].m_cTask.GetSaveVal(TASKVALUE_STATTASK_FUYUAN) < nPrice)
			{
				bShowMsg = FALSE;		
				sMsg.m_wMsgID = enumMSG_ID_SHOP_NO_FUYUAN;
			}
			break;
		case moneyunit_repute:
			if (Player[nPlayerIdx].m_cTask.GetSaveVal(TASKVALUE_STATTASK_REPUTE) < nPrice)
			{
				bShowMsg = FALSE;		
				sMsg.m_wMsgID = enumMSG_ID_SHOP_NO_REPUTE;
			}
			break;
		case moneyunit_accum:
			if (Player[nPlayerIdx].m_cTask.GetSaveVal(TASKVALUE_STATTASK_ACCUM) < nPrice)
			{
				bShowMsg = FALSE;		
				sMsg.m_wMsgID = enumMSG_ID_SHOP_NO_ACCUM;
			}	
			break;
		case moneyunit_honor:
			if (Player[nPlayerIdx].m_cTask.GetSaveVal(TASKVALUE_STATTASK_HONOR) < nPrice)
			{
				bShowMsg = FALSE;		
				sMsg.m_wMsgID = enumMSG_ID_SHOP_NO_HONOR;
			}
			break;
		case moneyunit_respect:
			if (Player[nPlayerIdx].m_cTask.GetSaveVal(TASKVALUE_STATTASK_RESPECT) < nPrice)
			{
				bShowMsg = FALSE;		
				sMsg.m_wMsgID = enumMSG_ID_SHOP_NO_RESPECT;
			}
			break;
		default:
			return FALSE;
		}
	}

	ItemPos	Pos;
	if (Player[nPlayerIdx].m_ItemList.SearchPosition(m_Item[nIdx].GetWidth(), m_Item[nIdx].GetHeight(), &Pos, true) == FALSE)
	{
		bShowMsg = FALSE;		
		sMsg.m_wMsgID = enumMSG_ID_SHOP_NO_ROOM;
	}

	if (!bShowMsg)
	{
		SO_ItemActionDiag("ShopBuy_REJECT", nBuy, nBuyIdx, nBuyNumber, nPrice, Player[nPlayerIdx].m_ItemList.GetEquipmentMoney(), 0, Npc[Player[nPlayerIdx].m_nIndex].m_FightMode, sMsg.m_wMsgID);
		g_pServer->PackDataToClient(Player[nPlayerIdx].m_nNetConnectIdx, &sMsg, sMsg.m_wLength + 1);
	}	

	return bShowMsg;
}

BOOL KBuySell::Buy(int nPlayerIdx, int nBuy, int nBuyIdx, BYTE nBuyNumber)
{
	KASSERT(nPlayerIdx >= 0 && nPlayerIdx < MAX_PLAYER);

	if (nPlayerIdx < 0 || nPlayerIdx >= MAX_PLAYER ||
		nBuy < 0 || nBuy >= m_Height ||
		nBuyIdx < 0 || nBuyIdx >= m_Width || nBuyNumber == 0 ||
		!m_SellItem || !m_SellItem[nBuy])
	{
		printf("BuySell: invalid purchase player=%d shop=%d slot=%d count=%u",
			nPlayerIdx, nBuy, nBuyIdx, (unsigned int)nBuyNumber);
		return FALSE;
	}

	if (m_SellItem[nBuy][nBuyIdx] < 0 || m_SellItem[nBuy][nBuyIdx] >= m_ItemNum)
		return FALSE;

	if (CanBuy(nPlayerIdx, nBuy, nBuyIdx, nBuyNumber) == FALSE)
		return FALSE;
		
	int nIdx = m_SellItem[nBuy][nBuyIdx];
    if(m_Item[nIdx].GetGenre()==item_ibitem && ((m_Item[nIdx].GetParticular()==0 && (m_Item[nIdx].GetDetailType()==330 || m_Item[nIdx].GetDetailType()==334)) || (m_Item[nIdx].GetDetailType()==7445 && m_Item[nIdx].GetParticular()==2)))m_Item[nIdx].SetExpirePoint(604800);
    if(IsPhongThanIBShop(nBuy)){extern int PhongThanBuyWithF2Currency(int,KItem&,int,int,int);return PhongThanBuyWithF2Currency(nPlayerIdx,m_Item[nIdx],nBuyNumber,m_Item[nIdx].GetCurPrice(),GetPaymentSlot(&m_Item[nIdx]));}
	int nWidth = m_Item[nIdx].GetWidth();
	int nHeight = m_Item[nIdx].GetHeight();

	if (m_Item[nIdx].GetExpirePoint())
	{
		m_Item[nIdx].SetExpireTime(KSG_GetCurSec() + m_Item[nIdx].GetExpirePoint());
	}

	int nPrice = m_Item[nIdx].GetCurPrice();

	//TamLTM Thue bang hoi chiem linh thanh thi thon
	int nMoneyMapVG = 0;
//	bool m_bIsCheckMoneyGold = false;

	if (SubWorld[Npc[Player[nPlayerIdx].m_nIndex].m_SubWorldIndex].m_bCheckTong /*&& m_bIsCheckMoneyGold*/)
	{
		int nTongVG = SubWorld[Npc[Player[nPlayerIdx].m_nIndex].m_SubWorldIndex].m_nTongVG;
		if (nTongVG >= 0 && nTongVG <= 50)
		{
			if (SubWorld[Npc[Player[nPlayerIdx].m_nIndex].m_SubWorldIndex].m_dwTongName == Player[nPlayerIdx].m_cTong.GetTongNameID())
			{
				nMoneyMapVG = nPrice * nTongVG / 200;
			}
			else
			{
				nMoneyMapVG = nPrice * nTongVG / 100;
			}

			if (nPrice <= nMoneyMapVG || nMoneyMapVG < 0)
			{
				nMoneyMapVG = 0;
			}
		}
	}
	//end code

	for(int i = 0; i < nBuyNumber; i++)
	{
		int nItemIdx = ItemSet.Add(&m_Item[nIdx]);

		if (nItemIdx <= 0)
			return FALSE;	

		ItemPos	Pos;
		if (Player[nPlayerIdx].m_ItemList.SearchPosition(nWidth, nHeight, &Pos, true))
		{
			Player[nPlayerIdx].m_ItemList.Add(nItemIdx, Pos.nPlace, Pos.nX, Pos.nY);
		}
		else
		{
			SHOW_MSG_SYNC	sMsg;
			sMsg.ProtocolType = s2c_msgshow;
			sMsg.m_wMsgID = enumMSG_ID_SHOP_NO_ROOM;
			sMsg.m_wLength = sizeof(SHOW_MSG_SYNC) - 1 - sizeof(LPVOID);
			g_pServer->PackDataToClient(Player[nPlayerIdx].m_nNetConnectIdx, &sMsg, sMsg.m_wLength + 1);
			break;
		}
	}

	if (i)
	{
		switch (Player[nPlayerIdx].m_BuyInfo.m_nMoneyUnit)
		{
			case moneyunit_money:
				Player[nPlayerIdx].Pay((nPrice * i) + nMoneyMapVG);
		//		m_bIsCheckMoneyGold = true;
				break;
			case moneyunit_extpoint:
				Player[nPlayerIdx].PayExtPoint(nPrice*i);
		//		m_bIsCheckMoneyGold = false;
				break;
			case moneyunit_fuyuan:
				Player[nPlayerIdx].m_cTask.SetSaveVal(TASKVALUE_STATTASK_FUYUAN, 
					Player[nPlayerIdx].m_cTask.GetSaveVal(TASKVALUE_STATTASK_FUYUAN)-(nPrice*i));
		//		m_bIsCheckMoneyGold = false;
				break;
			case moneyunit_repute:
				Player[nPlayerIdx].m_cTask.SetSaveVal(TASKVALUE_STATTASK_REPUTE, 
					Player[nPlayerIdx].m_cTask.GetSaveVal(TASKVALUE_STATTASK_REPUTE)-(nPrice*i));
		//		m_bIsCheckMoneyGold = false;
				break;
			case moneyunit_accum:
				Player[nPlayerIdx].m_cTask.SetSaveVal(TASKVALUE_STATTASK_ACCUM, 
					Player[nPlayerIdx].m_cTask.GetSaveVal(TASKVALUE_STATTASK_ACCUM)-(nPrice*i));
			//	m_bIsCheckMoneyGold = false;
				break;
			case moneyunit_honor:
				Player[nPlayerIdx].m_cTask.SetSaveVal(TASKVALUE_STATTASK_HONOR, 
					Player[nPlayerIdx].m_cTask.GetSaveVal(TASKVALUE_STATTASK_HONOR)-(nPrice*i));
//				m_bIsCheckMoneyGold = false;
				break;
			case moneyunit_respect:
				Player[nPlayerIdx].m_cTask.SetSaveVal(TASKVALUE_STATTASK_RESPECT, 
					Player[nPlayerIdx].m_cTask.GetSaveVal(TASKVALUE_STATTASK_RESPECT)-(nPrice*i));
			//	m_bIsCheckMoneyGold = false; //
				break;
			default:
				printf("[error]BuySell %s moneyunit (%d)", Npc[Player[nPlayerIdx].m_nIndex].Name, Player[nPlayerIdx].m_BuyInfo.m_nMoneyUnit);
				break;
		}
	}

	// TamLTM Thue bang hoi chiem linh thanh thi thon
	if (Player[nPlayerIdx].m_ItemList.GetEquipmentMoney() < (nPrice + nMoneyMapVG) /*&& m_bIsCheckMoneyGold*/)
		return FALSE;

	if (SubWorld[Npc[Player[nPlayerIdx].m_nIndex].m_SubWorldIndex].m_bCheckTong /*&& m_bIsCheckMoneyGold*/)
	{
//		m_bIsCheckMoneyGold = false;

		try
		{	
			bool bExecuteScriptMistake = true;
			KLuaScript * pScript = (KLuaScript* )g_GetScript("\\script\\item\\banghoi\\banghoi.lua");;
			if (pScript)
			{
				int nTopIndex = 0;
					
				pScript->SafeCallBegin(&nTopIndex);

				if (pScript->CallFunction("AddMoneyMain",0, "dsd", 
					SubWorld[Npc[Player[nPlayerIdx].m_nIndex].m_SubWorldIndex].m_dwTongName, 
					SubWorld[Npc[Player[nPlayerIdx].m_nIndex].m_SubWorldIndex].m_szTongNameBC, nMoneyMapVG * i));
				{
					bExecuteScriptMistake = false;
				}

				pScript->SafeCallEnd(nTopIndex);
			}
		}
		catch(...)
		{
		//	printf("Exception Have Caught When Execute Script[%d]!!!!!", g_FileName2Id("\\script\\item\\banghoi\\banghoi.lua"));
		}
	}
	//end code */

	return TRUE;
}
/*******************************************************************************
???? nIdx ????????Item??????????
*******************************************************************************/
BOOL KBuySell::Sell(int nPlayerIdx, int nBuy, int nIdx, int nBuyNumber)
{
	KASSERT(nPlayerIdx >= 0 && nPlayerIdx < MAX_PLAYER);
	KASSERT(nIdx >= 0 && nIdx < MAX_ITEM);
	
    if (nPlayerIdx <= 0 || nPlayerIdx >= MAX_PLAYER || nIdx <= 0 || nIdx >= MAX_ITEM ||
        nBuy < 0 || nBuy >= m_Height || nBuyNumber != 1 || Player[nPlayerIdx].CheckTrading())
        return FALSE;
    const int listIndex = Player[nPlayerIdx].m_ItemList.FindSame(nIdx);
    if (listIndex <= 0) return FALSE;
    const int place = Player[nPlayerIdx].m_ItemList.m_Items[listIndex].nPlace;
    if (place != pos_hand && place != pos_equiproom && place != pos_equiproomex && place != pos_ptbag2 && place != pos_ptbag3)
        return FALSE;

    // Selling to an open NPC shop does not require peace mode.

	if (nBuy == -1)
	{
		printf("BuySell: %s buy idx error!", Npc[Player[nPlayerIdx].m_nIndex].Name);
		return FALSE;
	}

	if (Item[nIdx].GetLock()->IsLock() || 
		Item[nIdx].GetLockSell())
	{
		SO_ItemActionDiag("ShopSell_LOCK_REJECT", Item[nIdx].GetGenre(), Item[nIdx].GetID(), Item[nIdx].GetRow(), Item[nIdx].GetLockSell(), Item[nIdx].GetLockTrade(), Item[nIdx].GetLockDrop(), Item[nIdx].GetLock()->nState, nBuy);
		SHOW_MSG_SYNC	sMsg;
		sMsg.ProtocolType = s2c_msgshow;
		sMsg.m_wMsgID = enumMSG_ID_LOCK_NOT_TRADE;
		sMsg.m_wLength = sizeof(SHOW_MSG_SYNC) - 1 - sizeof(LPVOID);
		if(g_pServer && Player[nPlayerIdx].m_nNetConnectIdx != -1)
			g_pServer->PackDataToClient(Player[nPlayerIdx].m_nNetConnectIdx, &sMsg, sMsg.m_wLength + 1);
		return FALSE;
	}

    const int nMoney = Item[nIdx].GetSalePrice();
    if (nMoney < 0 || (nMoney > 0 && !Player[nPlayerIdx].Earn(nMoney)))
        return FALSE; // Never destroy an item if crediting its price fails.
    if (!Player[nPlayerIdx].m_ItemList.Remove(nIdx))
    {
        if (nMoney > 0) Player[nPlayerIdx].m_ItemList.CostMoney(nMoney);
        return FALSE;
    }
    SO_ItemActionDiag("ShopSell_OK", Item[nIdx].GetGenre(), Item[nIdx].GetID(),
        Item[nIdx].GetRow(), Item[nIdx].GetStackNum(), nMoney, place, 0, nBuy);
    ItemSet.Remove(nIdx);	

	return TRUE;
}
#endif

#ifndef _SERVER
void KBuySell::PaintItem(int nIdx, int nX, int nY, BOOL bStack)
{
	int nShop = Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nShopIdx[Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nCurShop];
	if (nShop < 0 || nShop >= m_Height)
		return;
	int nItemIdx = GetItemIndex(nShop, nIdx);

	int x = nX;
	int y = nY;

	KItem* pItem = GetItem(nItemIdx);

	if (pItem)
	{
		pItem->Paint(x, y, false, false);
	}
}

void KBuySell::OpenSale(BuySellInfo *pInfo)
{
	if (pInfo->m_nShopIdx[0] < 0 || pInfo->m_nShopIdx[0] >= m_Height)
		return;
	Player[CLIENT_PLAYER_INDEX].m_BuyInfo.Clear();
	Player[CLIENT_PLAYER_INDEX].m_BuyInfo = *pInfo;
	Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nCurShop = 0;
	CoreDataChanged(GDCNI_NPC_TRADE, NULL, TRUE);
}

void KBuySell::OpenSale(int nSaleType, BuySellInfo *pInfo)
{
	if(nSaleType < 0 || nSaleType > 1)
		return;

	Player[CLIENT_PLAYER_INDEX].m_BuyInfo.Clear();
	Player[CLIENT_PLAYER_INDEX].m_BuyInfo = *pInfo;
	Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nCurShop = 0;
	CoreDataChanged(GDCNI_SUPERSHOP, (unsigned int)pInfo, nSaleType);
}
#endif

#ifdef _SERVER
void KBuySell::OpenSale(int nPlayerIdx, int nShop, int nShopMoneyUnit)
{
	if (nPlayerIdx <= 0 || nPlayerIdx > MAX_PLAYER)
		return;
	if (nShop < 0 || nShop >= m_Height)
		return;
	Player[nPlayerIdx].m_BuyInfo.Clear();
	Player[nPlayerIdx].m_BuyInfo.m_nShopIdx[0] = nShop;
	Player[nPlayerIdx].m_BuyInfo.m_nShopNum = 1;
	Player[nPlayerIdx].m_BuyInfo.m_nMoneyUnit = nShopMoneyUnit;
	Player[nPlayerIdx].m_BuyInfo.m_SubWorldID = Npc[Player[nPlayerIdx].m_nIndex].m_SubWorldIndex;
	Npc[Player[nPlayerIdx].m_nIndex].GetMpsPos(
		&Player[nPlayerIdx].m_BuyInfo.m_nMpsX,
		&Player[nPlayerIdx].m_BuyInfo.m_nMpsY);

	SALE_BOX_SYNC saleSync;
	saleSync.ProtocolType = s2c_opensalebox;
	memcpy(&saleSync.m_BuySellInfo, &Player[nPlayerIdx].m_BuyInfo, sizeof(BuySellInfo));
	g_pServer->PackDataToClient(Player[nPlayerIdx].m_nNetConnectIdx, &saleSync, sizeof(SALE_BOX_SYNC));
}

void KBuySell::OpenSale(int nPlayerIdx, int nSaleType, int nMoneyUnit, int nShopNum, int *nShopId)
{
	if (nPlayerIdx <= 0 || nPlayerIdx > MAX_PLAYER)
		return;
	if(nSaleType < 0 || nSaleType > 1)
		return;
	if(nMoneyUnit < moneyunit_money || nMoneyUnit >= moneyunit_num)
		return;
	if(nShopNum <= 0 || nShopNum >= MAX_SUPERSHOP_SHOPTAB)
		return;

	Player[nPlayerIdx].m_BuyInfo.Clear();
	memcpy(&Player[nPlayerIdx].m_BuyInfo.m_nShopIdx, nShopId, sizeof(int) * MAX_SUPERSHOP_SHOPTAB);
	Player[nPlayerIdx].m_BuyInfo.m_nShopNum = nShopNum;
	Player[nPlayerIdx].m_BuyInfo.m_nMoneyUnit = nMoneyUnit;
	Player[nPlayerIdx].m_BuyInfo.m_SubWorldID = Npc[Player[nPlayerIdx].m_nIndex].m_SubWorldIndex;
	Npc[Player[nPlayerIdx].m_nIndex].GetMpsPos(
		&Player[nPlayerIdx].m_BuyInfo.m_nMpsX,
		&Player[nPlayerIdx].m_BuyInfo.m_nMpsY);

	S2C_SUPERSHOP	sSale;
	if (nShopNum == PHONGTHAN_IBSHOP_TAB_COUNT &&
		nShopId[0] == PHONGTHAN_IBSHOP_FIRST_SHOP_INDEX)
	{
		PHONGTHAN_IBSHOP_OPEN_RESPONSE response;
		ZeroMemory(&response, sizeof(response));
		PhongThanInitializeWireHeader(&response.Header,
			PHONGTHAN_MSG_IBSHOP_OPEN_RESPONSE, sizeof(response),
			PHONGTHAN_WIRE_FLAG_RESPONSE, 0);
		response.ShopCount = nShopNum;
		memcpy(response.ShopIds, nShopId, sizeof(response.ShopIds));
		g_pServer->PackDataToClient(Player[nPlayerIdx].m_nNetConnectIdx,
			&response, sizeof(response));
		return;
	}
	sSale.ProtocolType = s2c_syncsupershop;
	sSale.m_nSaleType = nSaleType;
	memcpy(&sSale.m_BuySellInfo, &Player[nPlayerIdx].m_BuyInfo, sizeof(BuySellInfo));

//	g_DebugLog("s2c_syncsupershop %d", s2c_syncsupershop); //TamLTM Debug error packet

	g_pServer->PackDataToClient(Player[nPlayerIdx].m_nNetConnectIdx, (BYTE*)&sSale, sizeof(S2C_SUPERSHOP));
}
#endif

