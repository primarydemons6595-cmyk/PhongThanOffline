#include "KCore.h"
#include "../../../Headers/PhongThanTooltipCopy.h"
#include "../../../Headers/PhongThanThanKyBookPresentation.h"
#include "../../../Headers/PhongThanTooltipWrap.h"
#include "../../../Headers/PhongThanQuickUse.h"
#include "../../../Headers/PhongThanQuickUse.h"
#include "MyAssert.H"
#include "KTabFile.h"
#include "KNpc.h"
#include "KItem.h"
#include "PhongThanConsumables.h"
#include "KItemSet.h"
#include "KItemGenerator.h"
#include "KPhongThanAppearance.h"
#include "KTaskFuns.h"
#include "KSortScript.h"
#include "KBuySell.h"
#include	<time.h>
#ifndef _SERVER
#include "ImgRef.h"
#include "KPlayer.h"
#include "../../Represent/iRepresent/iRepresentshell.h"
#include "KMagicDesc.h"
#include "PhongThanReservoirTooltip.inl"
#endif
#include "PhongThanCustomItemText.inl"
#include "PhongThanQuestSealText.inl"
KItem	Item[MAX_ITEM];
#include "PhongThanItemUpgrade.inl"
#include "PhongThanGemRules.h"
#include "../../../Headers/PhongThanFabaoComboData.h"
#include "PhongThanSocketRegistry.inl"
#include "PhongThanSoulRegistry.inl"
#include "PhongThanSoulPresentation.inl"
#include "PhongThanSocketPresentation.inl"
#include "../../../Headers/PhongThanShopSale.h"
int GetRandomNumber(int nMin, int nMax);

KItem::KItem()
{
	::memset(&m_CommonAttrib,    0, sizeof(m_CommonAttrib));
	::memset(m_aryBaseAttrib,    0, sizeof(m_aryBaseAttrib));
	::memset(m_aryRequireAttrib, 0, sizeof(m_aryRequireAttrib));
	::memset(m_aryMagicAttrib,   0, sizeof(m_aryMagicAttrib));
	::memset(&m_GeneratorParam,	 0, sizeof(m_GeneratorParam));
	m_nCurrentDur = -1;
#ifdef _SERVER
    m_LastInventoryOwner=0;
#endif
#ifndef _SERVER
	m_NewArrivalTick = 0;
	::memset(&m_Image,   0, sizeof(KRUImage));
#endif
	m_nIndex = 0;
}

KItem::~KItem()
{
}

void* KItem::GetRequirement(IN int nReq)
{
	int i = sizeof(m_aryRequireAttrib)/sizeof(m_aryRequireAttrib[0]);
	if (nReq >= i)
		return NULL;

	return &m_aryRequireAttrib[nReq];
}
/******************************************************************************
¹¦ÄÜ:	½«itemÉÏµÄÄ§·¨Ó¦ÓÃµ½NPCÉíÉÏ
Èë¿Ú£º	pNPC: Ö¸ÏòNPCµÄÖ¸Õë£¬nMagicAcive£º´ò¿ªµÄÒþ²ØÊôÐÔÊýÄ¿
³ö¿Ú:	Ä§·¨±»Ó¦ÓÃ¡£
		¾ßÌå¹¤×÷ÓÉKNpcµÄ³ÉÔ±º¯ÊýÍê³É¡£
		KItem ¶ÔÏó±¾ÉíÃ»ÓÐ³ÉÔ±±äÁ¿±»ÐÞ¸Ä
******************************************************************************/
void KItem::ApplyMagicAttribToNPC(IN KNpc* pNPC, IN int nMagicActive, IN int nSetActiveLines) const
{
	_ASSERT(this != NULL);
	if (!pNPC)
		return;
	(void)nMagicActive;

	int i;
	for (i = 0; i < MAX_ITEM_BASEATTRIB; ++i)
	{
		const KItemNormalAttrib* pAttrib = &m_aryBaseAttrib[i];
		if (pAttrib->nAttribType > 0)
			pNPC->ModifyAttrib(pNPC->m_Index, (void *)pAttrib);
	}

    PT_SOUL_STATE soul;if(GetGenre()==item_equip&&PtSoulFromParam(m_CommonAttrib.nParam,soul)&&soul.MagicType){KItemNormalAttrib bonus;ZeroMemory(&bonus,sizeof(bonus));bonus.nAttribType=soul.MagicType;bonus.nValue[0]=soul.MagicValue;pNPC->ModifyAttrib(pNPC->m_Index,&bonus);}
	PT_SOCKET_STATE sockets;
	if(GetGenre()==item_equip && PtSocketRead(m_CommonAttrib.nParam,sockets))for(int si=0;si<4;++si)if(sockets.Attributes[si].Type){
	 KItemNormalAttrib a;memcpy(&a,&sockets.Attributes[si],sizeof(a));pNPC->ModifyAttrib(pNPC->m_Index,&a);
	}
    KItemNormalAttrib combo[10];int comboCount=PhongThanFabaoCombo(GetDetailType(),GetParticularType(),m_CommonAttrib.nUpgradeLvl,combo,10);
    for(int c=0;c<comboCount;++c)pNPC->ModifyAttrib(pNPC->m_Index,&combo[c]);
	int nRemainingSetLines = nSetActiveLines;
	const BOOL bSetEquipment = m_CommonAttrib.nSetID > 0;
	for (i = 0; i < MAX_ITEM_MAGICATTRIB; ++i)
	{
		const KItemNormalAttrib* pAttrib = &m_aryMagicAttrib[i];
		if (pAttrib->nAttribType <= 0)
			continue;
		if (bSetEquipment && i >= MAX_ITEM_NORMAL_MAGICATTRIB)
		{
			if (nRemainingSetLines <= 0)
				continue;
			--nRemainingSetLines;
		}
		pNPC->ModifyAttrib(pNPC->m_Index, (void *)pAttrib);
	}
}

void KItem::RemoveMagicAttribFromNPC(IN KNpc* pNPC, IN int nMagicActive, IN int nSetActiveLines) const
{
	_ASSERT(this != NULL);
	if (!pNPC)
		return;
	(void)nMagicActive;

	int i;
	for (i = 0; i < MAX_ITEM_BASEATTRIB; ++i)
	{
		const KItemNormalAttrib* pAttrib = &m_aryBaseAttrib[i];
		if (pAttrib->nAttribType <= 0)
			continue;
		KItemNormalAttrib removeAttrib = *pAttrib;
		removeAttrib.nValue[0] = -removeAttrib.nValue[0];
		removeAttrib.nValue[1] = -removeAttrib.nValue[1];
		removeAttrib.nValue[2] = -removeAttrib.nValue[2];
		pNPC->ModifyAttrib(pNPC->m_Index, &removeAttrib);
	}

    PT_SOUL_STATE soul;if(GetGenre()==item_equip&&PtSoulFromParam(m_CommonAttrib.nParam,soul)&&soul.MagicType){KItemNormalAttrib bonus;ZeroMemory(&bonus,sizeof(bonus));bonus.nAttribType=soul.MagicType;bonus.nValue[0]=-soul.MagicValue;pNPC->ModifyAttrib(pNPC->m_Index,&bonus);}
	PT_SOCKET_STATE sockets;
	if(GetGenre()==item_equip && PtSocketRead(m_CommonAttrib.nParam,sockets))for(int si=0;si<4;++si)if(sockets.Attributes[si].Type){
	 KItemNormalAttrib a;memcpy(&a,&sockets.Attributes[si],sizeof(a));for(int sv=0;sv<3;++sv)a.nValue[sv]=-a.nValue[sv];pNPC->ModifyAttrib(pNPC->m_Index,&a);
	}
    KItemNormalAttrib combo[10];int comboCount=PhongThanFabaoCombo(GetDetailType(),GetParticularType(),m_CommonAttrib.nUpgradeLvl,combo,10);
    for(int c=0;c<comboCount;++c){combo[c].nValue[0]=-combo[c].nValue[0];pNPC->ModifyAttrib(pNPC->m_Index,&combo[c]);}
	int nRemainingSetLines = nSetActiveLines;
	const BOOL bSetEquipment = m_CommonAttrib.nSetID > 0;
	for (i = 0; i < MAX_ITEM_MAGICATTRIB; ++i)
	{
		const KItemNormalAttrib* pAttrib = &m_aryMagicAttrib[i];
		if (pAttrib->nAttribType <= 0)
			continue;
		if (bSetEquipment && i >= MAX_ITEM_NORMAL_MAGICATTRIB)
		{
			if (nRemainingSetLines <= 0)
				continue;
			--nRemainingSetLines;
		}
		KItemNormalAttrib removeAttrib = *pAttrib;
		removeAttrib.nValue[0] = -removeAttrib.nValue[0];
		removeAttrib.nValue[1] = -removeAttrib.nValue[1];
		removeAttrib.nValue[2] = -removeAttrib.nValue[2];
		pNPC->ModifyAttrib(pNPC->m_Index, &removeAttrib);
	}
}

#include "../../../Headers/PhongThanPowerExpression.h"
#include "PhongThanForgePower.inl"
#include "PhongThanEquipmentPower.inl"
#include "../../../Headers/PhongThanUpgradePower.h"
BOOL KItem::GetUpgradePower(int& nPower) const
{
	nPower = 0;
	if (GetGenre() != item_equip) return FALSE;
	if (m_CommonAttrib.nUpgradeLvl == 0) return TRUE;
	static KTabFile table;
	static BOOL loaded = FALSE;
	if (!loaded)
		loaded = table.Load("\\settings\\powervalue\\\xD7\xB0\xB1\xB8\xC9\xFD\xBC\xB6\xB9\xA6\xC1\xA6\xB1\xED.txt");
	if (!loaded || table.GetWidth() < 2) return FALSE;
	const int level = m_CommonAttrib.nUpgradeLvl;
	if (level < 0 || level > 32 || level >= table.GetWidth()) return FALSE;
	for (int row = 2; row <= table.GetHeight(); ++row)
	{
		int equipId = 0;
		table.GetInteger(row, 1, 0, &equipId);
		if (equipId != GetEquipId()) continue;
		int steps[32];
		for (int n = 0; n < level; ++n)
			if (!table.GetInteger(row, n + 2, -1, &steps[n]) || steps[n] < 0) return FALSE;
		return PhongThanSumUpgradePower(steps, level, level, nPower) ? TRUE : FALSE;
	}
	return FALSE;
}

BOOL KItem::SetAttrib_CBR(IN const KBASICPROP_EQUIPMENT* pData)
{
	_ASSERT(pData != NULL);

	BOOL bEC = FALSE;
	if (pData)
	{
		// This object may previously have represented another equipment item.
		// Rebuild only from the new template; later explicit layers reapply their
		// own effects. Non-set templates must not inherit old set/magic slots.
		ZeroMemory(m_aryMagicAttrib, sizeof(m_aryMagicAttrib));
		//SetAttrib_Common(pData);
		*this = *pData;		// ÔËËã·ûÖØÔØ
		SetAttrib_Base(pData->m_aryPropBasic);
		SetAttrib_Req(pData->m_aryPropReq);
		bEC = TRUE;
	}
	return bEC;
}

BOOL KItem::SetAttrib_Base(const KEQCP_BASIC* pBasic)
{
	for (int i = 0;
		 i < sizeof(m_aryBaseAttrib)/sizeof(m_aryBaseAttrib[0]); i++)
	{
		KItemNormalAttrib* pDst;
		const KEQCP_BASIC* pSrc;
		pDst = &(m_aryBaseAttrib[i]);
		pSrc = &(pBasic[i]);
		pDst->nAttribType = pSrc->nType;
		pDst->nValue[0] = ::GetRandomNumber(pSrc->sRange.nMin, pSrc->sRange.nMax);
		pDst->nValue[1] = 0;	// RESERVED
		pDst->nValue[2] = 0;	// RESERVED
		if (pDst->nAttribType == magic_durability_v)
			SetDurability(pDst->nValue[0]);
	}
	//if (m_nCurrentDur == 0)	// ËµÃ÷Ã»ÓÐÄÍ¾Ã¶ÈÊôÐÔ
	//	m_nCurrentDur = -1;
	return TRUE;
}

BOOL KItem::SetAttrib_Req(const KEQCP_REQ* pReq)
{
	for (int i = 0;
		 i < sizeof(m_aryRequireAttrib)/sizeof(m_aryRequireAttrib[0]); i++)
	{
		KItemNormalAttrib* pDst;
		pDst = &(m_aryRequireAttrib[i]);
		pDst->nAttribType = pReq[i].nType;
		pDst->nValue[0] = pReq[i].nPara;
		pDst->nValue[1] = 0;	// RESERVED
		pDst->nValue[2] = 0;	// RESERVED
	}
	return TRUE;
}

/******************************************************************************
¹¦ÄÜ:	¸ù¾Ý´«ÈëµÄÊý¾Ý, ÎªitemµÄÄ§·¨ÊôÐÔ¸³³õÖµ
Èë¿Ú£º	pMA: ¸ø³öÊý¾Ý
³ö¿Ú:	³É¹¦Ê±·µ»Ø·ÇÁã, ÒÔÏÂ³ÉÔ±±äÁ¿±»Öµ:
			m_aryMagicAttrib
		Ê§°ÜÊ±·µ»ØÁã
******************************************************************************/
BOOL KItem::SetAttrib_MA(IN const KItemNormalAttrib* pMA)
{
	if (NULL == pMA)
		{ _ASSERT(FALSE); return FALSE; }
	for (int i = 0; i < sizeof(m_aryMagicAttrib) / sizeof(m_aryMagicAttrib[0]); i++)
	{
		m_aryMagicAttrib[i] = pMA[i];
		if (m_aryMagicAttrib[i].nAttribType == magic_indestructible_b)
		{
			SetDurability(-1);
		}
	}
	return TRUE;
}

/******************************************************************************
¹¦ÄÜ:	¸ù¾Ý´«ÈëµÄÊý¾Ý, ÎªitemµÄÄ§·¨ÊôÐÔ¸³³õÖµ
Èë¿Ú£º	pMA: ¸ø³öÊý¾Ý
³ö¿Ú:	³É¹¦Ê±·µ»Ø·ÇÁã, ÒÔÏÂ³ÉÔ±±äÁ¿±»Öµ:
			m_aryMagicAttrib
		Ê§°ÜÊ±·µ»ØÁã
******************************************************************************/
BOOL KItem::SetAttrib_MA(IN const KMACP* pMA)
{
	if (NULL == pMA)
		{ _ASSERT(FALSE); return FALSE; }

	for (int i = 0; i < sizeof(m_aryMagicAttrib) / sizeof(m_aryMagicAttrib[0]); i++)
	{
		const KMACP* pSrc;
		KItemNormalAttrib* pDst;
		pSrc = &(pMA[i]);
		pDst = &(m_aryMagicAttrib[i]);

		pDst->nAttribType = pSrc->nPropKind;
		pDst->nValue[0] =  ::GetRandomNumber(pSrc->aryRange[0].nMin, pSrc->aryRange[0].nMax);
		pDst->nValue[1] =  ::GetRandomNumber(pSrc->aryRange[1].nMin, pSrc->aryRange[1].nMax);
		pDst->nValue[2] =  ::GetRandomNumber(pSrc->aryRange[2].nMin, pSrc->aryRange[2].nMax);
	}
	return TRUE;
}

void KItem::operator = (const KBASICPROP_EQUIPMENT& sData)
{
	m_CommonAttrib.nUpgradeRule=0;
	m_CommonAttrib.nUpgradeAddedBaseMask=m_CommonAttrib.nUpgradeAddedRequireMask=0;
	KItemCommonAttrib* pCA	= &m_CommonAttrib;
	//TamLTM Code kham nam xanh
	//End code
	pCA->bTemp				= FALSE;
	pCA->BackLocal.Release();
	pCA->nItemGenre			= sData.m_nItemGenre;
	pCA->nDetailType		= sData.m_nDetailType;
	pCA->nParticularType	= sData.m_nParticularType;
	pCA->nObjIdx			= sData.m_nObjIdx;
	pCA->nWidth				= sData.m_nWidth;
	pCA->nHeight			= sData.m_nHeight;
	pCA->nPrice				= sData.m_nPrice;
	pCA->nNewPrice			= sData.m_nPrice;
	pCA->bNewArrival		= FALSE;
	pCA->nLevel				= sData.m_nLevel;
	pCA->nSeries			= sData.m_nSeries;
	pCA->bShortKey			= FALSE;
	pCA->nStackNum			= 1;
	pCA->nMaxStack			= 1;
	pCA->nExpirePoint		= 0;
	pCA->nRow				= -1;
	pCA->nGroup				= sData.m_nSetId;
	pCA->nSetID				= sData.m_nSetId;
	pCA->nNeedToActive1		= sData.m_nSetId ? 3 : 0;
	pCA->nNeedToActive2		= sData.m_nSetId ? 5 : 0;
	pCA->uFlash				= 0;
	pCA->nUpgradeLvl		= 0;
	pCA->nPhysicVal			= sData.m_nBasePower;
	pCA->nMagicVal			= sData.m_nMagicPower;
	pCA->nWeight			= sData.m_nWeight;
	pCA->nEquipId			= sData.m_nEquipId;
	pCA->bLockSell			= !sData.m_nCanSell;
	pCA->bLockTrade			= !sData.m_nCanTrade;
	pCA->bLockDrop			= !sData.m_nCanDrop;
	pCA->nParam				= -1;
	pCA->nFortune			= 0;
	pCA->nExpireTime		= 0;
	pCA->LockItem.Clear();
	::strcpy(pCA->szItemName,  sData.m_szName);
	::memset(pCA->szScript, 0, sizeof(pCA->szScript));
#ifndef _SERVER
	::strcpy(pCA->szImageName, sData.m_szImageName);
	::strcpy(pCA->szIntro,	   sData.m_szIntro);
	m_Image.Color.Color_b.a = 255;
	m_Image.nFrame = 0;
	m_Image.nISPosition = IMAGE_IS_POSITION_INIT;
	m_Image.nType = ISI_T_SPR;
	::strcpy(m_Image.szImage, pCA->szImageName);
	m_Image.uImage = 0;
#endif
}

void KItem::operator = (const KBASICPROP_EVENTITEM& sData)
{
	KItemCommonAttrib* pCA  = &m_CommonAttrib;
	pCA->bTemp				= FALSE;
	pCA->BackLocal.Release();
	pCA->nItemGenre			= sData.m_nItemGenre;
	pCA->nDetailType		= sData.m_nDetailType;
	pCA->nParticularType	= 0;
	pCA->nObjIdx			= sData.m_nObjIdx;
	pCA->nWidth				= sData.m_nWidth;
	pCA->nHeight			= sData.m_nHeight;
	pCA->nPrice				= sData.m_nPrice;
	pCA->nNewPrice			= sData.m_nPrice;
	pCA->bNewArrival		= FALSE;
	pCA->nLevel				= 0;
	pCA->nSeries			= series_num;
	pCA->bShortKey			= sData.m_bShortKey;
	pCA->nStackNum			= 1;
	pCA->nMaxStack			= sData.m_nMaxStack;
	// Preserve the original event-item short-key permission above.
	pCA->nExpirePoint		= 0;
	pCA->nRow				= -1;
	pCA->nGroup				= 0;
	pCA->nSetID				= 0;
	pCA->nNeedToActive1		= 0;
	pCA->nNeedToActive2		= 0;
	pCA->uFlash				= 0;
	pCA->nUpgradeLvl		= 0;
	pCA->nPhysicVal			= 0;
	pCA->nMagicVal			= 0;
	pCA->nWeight = sData.m_nWeight;
	pCA->nEquipId			= 0;
	pCA->bLockSell			= FALSE;
	pCA->bLockTrade			= FALSE;
	pCA->bLockDrop			= FALSE;
	pCA->nParam				= -1;
	pCA->nFortune			= 0;
	pCA->nExpireTime		= 0;
	pCA->LockItem.Clear();
	::strcpy(pCA->szItemName,  sData.m_szName);
	::strcpy(pCA->szScript,  sData.m_szScript);
#ifndef _SERVER
	::strcpy(pCA->szImageName, sData.m_szImageName);
	::strcpy(pCA->szIntro,	   sData.m_szIntro);
#endif
	ZeroMemory(m_aryBaseAttrib, sizeof(m_aryBaseAttrib));
	ZeroMemory(m_aryRequireAttrib, sizeof(m_aryRequireAttrib));
	ZeroMemory(m_aryMagicAttrib, sizeof(m_aryMagicAttrib));
#ifndef _SERVER
	m_Image.Color.Color_b.a = 255;
	m_Image.nFrame = 0;
	m_Image.nISPosition = IMAGE_IS_POSITION_INIT;
	m_Image.nType = ISI_T_SPR;
	::strcpy(m_Image.szImage, pCA->szImageName);
	m_Image.uImage = 0;
#endif
}

void KItem::operator = (const KBASICPROP_QUEST& sData)
{
	// ¸³Öµ: ¹²Í¬ÊôÐÔ²¿·Ö
	KItemCommonAttrib* pCA = &m_CommonAttrib;
	//TamLTM Code kham nam xanh
	//End code
	pCA->bTemp				= FALSE;
	pCA->BackLocal.Release();
	pCA->nItemGenre			= sData.m_nItemGenre;
	pCA->nDetailType		= sData.m_nDetailType;
	pCA->nParticularType	= 0;
	pCA->nObjIdx			= sData.m_nObjIdx;
	pCA->nWidth				= sData.m_nWidth;
	pCA->nHeight			= sData.m_nHeight;
	pCA->nPrice				= sData.m_nPrice;
	pCA->nNewPrice			= sData.m_nPrice;
	pCA->bNewArrival		= FALSE;
	pCA->nLevel				= 0;
	pCA->nSeries			= series_num;
	pCA->bShortKey			= sData.m_bShortKey;
	pCA->nStackNum			= 1;
	pCA->nMaxStack			= sData.m_nMaxStack;
	pCA->nExpirePoint		= 0;
	pCA->nRow				= -1;
	pCA->nGroup				= 0;
	pCA->nSetID				= 0;
	pCA->nNeedToActive1		= 0;
	pCA->nNeedToActive2		= 0;
	pCA->uFlash				= 0;
	pCA->nUpgradeLvl		= 0;
	pCA->nPhysicVal			= 0;
	pCA->nMagicVal			= 0;
	pCA->nWeight = sData.m_nWeight;
	pCA->nEquipId			= 0;
	pCA->bLockSell			= FALSE;
	pCA->bLockTrade			= FALSE;
	pCA->bLockDrop			= FALSE;
	pCA->nParam				= -1;
	pCA->nFortune			= 0;
	pCA->nExpireTime		= 0;
	pCA->LockItem.Clear();
	::strcpy(pCA->szItemName,  sData.m_szName);
	::memset(pCA->szScript, 0, sizeof(pCA->szScript));
#ifndef _SERVER
	::strcpy(pCA->szImageName, sData.m_szImageName);
	::strcpy(pCA->szIntro,	   sData.m_szIntro);
#endif
	ZeroMemory(m_aryBaseAttrib, sizeof(m_aryBaseAttrib));
	// ¸³Öµ: ÐèÇóÊôÐÔ²¿·Ö: ÎÞ
	ZeroMemory(m_aryRequireAttrib, sizeof(m_aryRequireAttrib));
	// ¸³Öµ: Ä§·¨ÊôÐÔ²¿·Ö: ÎÞ
	ZeroMemory(m_aryMagicAttrib, sizeof(m_aryMagicAttrib));
#ifndef _SERVER
	m_Image.Color.Color_b.a = 255;
	m_Image.nFrame = 0;
	m_Image.nISPosition = IMAGE_IS_POSITION_INIT;
	m_Image.nType = ISI_T_SPR;
	::strcpy(m_Image.szImage, pCA->szImageName);
	m_Image.uImage = 0;
#endif
}

void KItem::operator = (const KBASICPROP_TOWNPORTAL& sData)
{
	// ¸³Öµ: ¹²Í¬ÊôÐÔ²¿·Ö
	KItemCommonAttrib* pCA = &m_CommonAttrib;
	//TamLTM Code kham nam xanh
	//End code
	pCA->bTemp				= FALSE;
	pCA->BackLocal.Release();
	pCA->nItemGenre			= sData.m_nItemGenre;
	pCA->nDetailType		= sData.m_nDetailType;
	pCA->nParticularType	= 0;
	pCA->nObjIdx			= sData.m_nObjIdx;
	pCA->nWidth				= sData.m_nWidth;
	pCA->nHeight			= sData.m_nHeight;
	pCA->nPrice				= sData.m_nPrice;
	pCA->nLevel				= 0;
	pCA->nSeries			= series_num;
	pCA->bShortKey			= sData.m_bShortKey;
	pCA->nStackNum			= 1;
	pCA->nMaxStack			= sData.m_nMaxStack;
	pCA->nExpirePoint		= 0;
	pCA->nRow				= -1;
	pCA->nGroup				= 0;
	pCA->nSetID				= 0;
	pCA->nNeedToActive1		= 0;
	pCA->nNeedToActive2		= 0;
	pCA->uFlash				= 0;
	pCA->nUpgradeLvl		= 0;
	pCA->nPhysicVal			= 0;
	pCA->nMagicVal			= 0;
	pCA->nWeight = sData.m_nWeight;
	pCA->nEquipId			= 0;
	pCA->bLockSell			= FALSE;
	pCA->bLockTrade			= FALSE;
	pCA->bLockDrop			= FALSE;
	pCA->nParam				= -1;
	pCA->nFortune			= 0;
	pCA->nExpireTime		= 0;
	pCA->LockItem.Clear();
	::strcpy(pCA->szItemName,  sData.m_szName);
	::strcpy(pCA->szScript,  sData.m_szScript);
#ifndef _SERVER
	::strcpy(pCA->szImageName, sData.m_szImageName);
	::strcpy(pCA->szIntro,	   sData.m_szIntro);
#endif
	ZeroMemory(m_aryBaseAttrib, sizeof(m_aryBaseAttrib));
	// ¸³Öµ: ÐèÇóÊôÐÔ²¿·Ö: ÎÞ
	ZeroMemory(m_aryRequireAttrib, sizeof(m_aryRequireAttrib));
	// ¸³Öµ: Ä§·¨ÊôÐÔ²¿·Ö: ÎÞ
	ZeroMemory(m_aryMagicAttrib, sizeof(m_aryMagicAttrib));
#ifndef _SERVER
	m_Image.Color.Color_b.a = 255;
	m_Image.nFrame = 0;
	m_Image.nISPosition = IMAGE_IS_POSITION_INIT;
	m_Image.nType = ISI_T_SPR;
	::strcpy(m_Image.szImage, pCA->szImageName);
	m_Image.uImage = 0;
#endif
}

void KItem::operator = (const KBASICPROP_MAGICSCRIPT& sData)
{
	// ¸³Öµ: ¹²Í¬ÊôÐÔ²¿·Ö
	KItemCommonAttrib* pCA	= &(m_CommonAttrib);
	pCA->bTemp				= FALSE;
	pCA->BackLocal.Release();
	pCA->nItemGenre			= sData.m_nItemGenre;
	pCA->nDetailType		= sData.m_nDetailType;
	pCA->nParticularType	= 0;
	pCA->nObjIdx			= sData.m_nObjIdx;
	pCA->nWidth				= sData.m_nWidth;
	pCA->nHeight			= sData.m_nHeight;
	pCA->nPrice				= sData.m_nPrice;
	pCA->nNewPrice			= sData.m_nPrice;
	pCA->bNewArrival		= FALSE;
	pCA->nLevel				= 0;
	pCA->nSeries			= series_num;
	pCA->bShortKey			= sData.m_bShortKey;
	pCA->nStackNum			= 1;
	pCA->nMaxStack			= sData.m_nMaxStack;
	pCA->nExpirePoint		= 0;
	pCA->nRow				= -1;
	pCA->nGroup				= 0;
	pCA->nSetID				= 0;
	pCA->nNeedToActive1		= 0;
	pCA->nNeedToActive2		= 0;
	pCA->uFlash				= 0;
	pCA->nUpgradeLvl		= 0;
	pCA->nPhysicVal			= 0;
	pCA->nMagicVal			= 0;
	pCA->nWeight = sData.m_nWeight;
	pCA->nEquipId			= 0;
	pCA->bLockSell			= FALSE;
	pCA->bLockTrade			= FALSE;
	pCA->bLockDrop			= FALSE;
	pCA->nParam				= -1;
	pCA->nFortune			= 0;
	pCA->nExpireTime		= 0;
	pCA->LockItem.Clear();
	::strncpy(pCA->szItemName, sData.m_szName, sizeof(pCA->szItemName) - 1);
	pCA->szItemName[sizeof(pCA->szItemName) - 1] = 0;
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
	m_Image.Color.Color_b.a = 255;
	m_Image.nFrame = 0;
	m_Image.nISPosition = IMAGE_IS_POSITION_INIT;
	m_Image.nType = ISI_T_SPR;
	::strncpy(m_Image.szImage, pCA->szImageName, sizeof(m_Image.szImage) - 1);
	m_Image.szImage[sizeof(m_Image.szImage) - 1] = 0;
	m_Image.uImage = 0;
#endif
}

void KItem::operator = (const KBASICPROP_SKILLBOOK& sData)
{
	KItemCommonAttrib* pCA = &m_CommonAttrib;
	pCA->bTemp = FALSE;
	pCA->BackLocal.Release();
	pCA->nItemGenre = sData.m_nItemGenre;
	pCA->nDetailType = sData.m_nDetailType;
	pCA->nParticularType = sData.m_nParticularType;
	pCA->nObjIdx = sData.m_nObjIdx;
	pCA->nWidth = sData.m_nWidth;
	pCA->nHeight = sData.m_nHeight;
	pCA->nPrice = sData.m_nPrice;
	pCA->nNewPrice = sData.m_nPrice;
	pCA->bNewArrival = FALSE;
	pCA->nLevel = 0;
	pCA->nSeries = series_num;
	pCA->bShortKey = FALSE;
	pCA->nStackNum = 1;
	pCA->nMaxStack = sData.m_nMaxStack;
	pCA->nExpirePoint = 0;
	pCA->nRow = -1;
	pCA->nGroup = 0;
	pCA->nSetID = 0;
	pCA->nNeedToActive1 = 0;
	pCA->nNeedToActive2 = 0;
	pCA->uFlash = 0;
	pCA->nUpgradeLvl = 0;
	pCA->nPhysicVal = 0;
	pCA->nMagicVal = 0;
	pCA->nWeight = sData.m_nWeight;
	pCA->nEquipId = 0;
	pCA->bLockSell = !sData.m_bCanSell;
	pCA->bLockTrade = !sData.m_bCanTrade;
	pCA->bLockDrop = !sData.m_bCanDrop;
	pCA->nParam = -1;
	pCA->nFortune = 0;
	pCA->nExpireTime = 0;
	pCA->LockItem.Clear();
	::strncpy(pCA->szItemName, sData.m_szName, sizeof(pCA->szItemName) - 1);
	pCA->szItemName[sizeof(pCA->szItemName) - 1] = 0;
	::memset(pCA->szScript, 0, sizeof(pCA->szScript));
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
	m_Image.Color.Color_b.a = 255;
	m_Image.nFrame = 0;
	m_Image.nISPosition = IMAGE_IS_POSITION_INIT;
	m_Image.nType = ISI_T_SPR;
	::strncpy(m_Image.szImage, pCA->szImageName, sizeof(m_Image.szImage) - 1);
	m_Image.szImage[sizeof(m_Image.szImage) - 1] = 0;
	m_Image.uImage = 0;
#endif
}

void KItem::operator = (const KBASICPROP_IBITEM& sData)
{
	KItemCommonAttrib* pCA = &m_CommonAttrib;
	pCA->bTemp = FALSE;
	pCA->BackLocal.Release();
	pCA->nItemGenre = item_ibitem;
	pCA->nDetailType = sData.m_nDetailType;
	pCA->nParticularType = sData.m_nParticularType;
	pCA->nObjIdx = sData.m_nObjIdx;
	pCA->nWidth = sData.m_nWidth;
	pCA->nHeight = sData.m_nHeight;
	pCA->nPrice = sData.m_nPrice;
	pCA->nNewPrice = sData.m_nPrice;
	pCA->bNewArrival = FALSE;
	pCA->nLevel = 0;
	pCA->nSeries = series_num;
	pCA->bShortKey = FALSE;
	pCA->nStackNum = sData.m_nUseCount;
	pCA->nMaxStack = sData.m_nUseCount > 1 ? sData.m_nUseCount : 1;
	pCA->nExpirePoint = 0;
	pCA->nRow = -1;
	pCA->nGroup = 0;
	pCA->nSetID = 0;
	pCA->nNeedToActive1 = 0;
	pCA->nNeedToActive2 = 0;
	pCA->uFlash = 0;
	pCA->nUpgradeLvl = 0;
	pCA->nPhysicVal = 0;
	pCA->nMagicVal = 0;
	pCA->nWeight = sData.m_nWeight;
	pCA->nEquipId = 0;
	pCA->bLockSell = PtWeeklyDragonItem(item_ibitem,sData.m_nDetailType,sData.m_nParticularType) ? FALSE : !sData.m_bCanSell;
	pCA->bLockTrade = !sData.m_bCanTrade;
	pCA->bLockDrop = !sData.m_bCanDrop;
	pCA->nParam = -1;
	pCA->nFortune = 0;
	pCA->nExpireTime = 0;
	pCA->LockItem.Clear();
	::strncpy(pCA->szItemName, sData.m_szName, sizeof(pCA->szItemName) - 1);
	pCA->szItemName[sizeof(pCA->szItemName) - 1] = 0;
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
	m_Image.Color.Color_b.a = 255;
	m_Image.nFrame = 0;
	m_Image.nISPosition = IMAGE_IS_POSITION_INIT;
	m_Image.nType = ISI_T_SPR;
	::strncpy(m_Image.szImage, pCA->szImageName, sizeof(m_Image.szImage) - 1);
	m_Image.szImage[sizeof(m_Image.szImage) - 1] = 0;
	m_Image.uImage = 0;
#endif
}
void KItem::operator = (const KBASICPROP_MEDICINE& sData)
{
	// ¸³Öµ: ¹²Í¬ÊôÐÔ²¿·Ö
	KItemCommonAttrib* pCA	= &m_CommonAttrib;
	//TamLTM Code kham nam xanh
	//End code
	pCA->bTemp				= FALSE;
	pCA->BackLocal.Release();
	pCA->nItemGenre			= sData.m_nItemGenre;
	pCA->nDetailType		= sData.m_nDetailType;
	pCA->nParticularType	= sData.m_nParticularType;
	pCA->nObjIdx			= sData.m_nObjIdx;
	pCA->nWidth				= sData.m_nWidth;
	pCA->nHeight			= sData.m_nHeight;
	pCA->nPrice				= sData.m_nPrice;
	pCA->nNewPrice			= sData.m_nPrice;
	pCA->bNewArrival		= FALSE;
	pCA->nLevel				= sData.m_nLevel;
	pCA->nSeries			= sData.m_nSeries;
	pCA->bShortKey			= TRUE;
	pCA->nStackNum			= 1;
	pCA->nMaxStack			= sData.m_nMaxStack;
	pCA->nExpirePoint		= 0;
	pCA->nRow				= -1;
	pCA->nGroup				= 0;
	pCA->nSetID				= 0;
	pCA->nNeedToActive1		= 0;
	pCA->nNeedToActive2		= 0;
	pCA->uFlash				= 0;
	pCA->nUpgradeLvl		= 0;
	pCA->nPhysicVal			= 0;
	pCA->nMagicVal			= 0;
	pCA->nWeight = sData.m_nWeight;
	pCA->nEquipId			= 0;
	pCA->bLockSell			= FALSE;
	pCA->bLockTrade			= FALSE;
	pCA->bLockDrop			= FALSE;
	pCA->nParam				= -1;
	pCA->nFortune			= 0;
	pCA->nExpireTime		= 0;
	pCA->LockItem.Clear();
	::strcpy(pCA->szItemName,  sData.m_szName);
	::memset(pCA->szScript, 0, sizeof(pCA->szScript));
#ifndef _SERVER
	::strcpy(pCA->szImageName, sData.m_szImageName);
	::strcpy(pCA->szIntro,	   sData.m_szIntro);
#endif
	// ¸³Öµ: »ù±¾ÊôÐÔ²¿·Ö
	ZeroMemory(m_aryBaseAttrib, sizeof(m_aryBaseAttrib));
	KItemNormalAttrib* pBA = m_aryBaseAttrib;
	pBA[0].nAttribType = sData.m_aryAttrib[0].nAttrib;
	pBA[0].nValue[0]   = sData.m_aryAttrib[0].nValue;
	pBA[0].nValue[1]   = sData.m_aryAttrib[0].nTime;
	pBA[1].nAttribType = sData.m_aryAttrib[1].nAttrib;
	pBA[1].nValue[0]   = sData.m_aryAttrib[1].nValue;
	pBA[1].nValue[1]   = sData.m_aryAttrib[1].nTime;

	ZeroMemory(m_aryRequireAttrib, sizeof(m_aryRequireAttrib));
	ZeroMemory(m_aryMagicAttrib, sizeof(m_aryMagicAttrib));
#ifndef _SERVER
	m_Image.Color.Color_b.a = 255;
	m_Image.nFrame = 0;
	m_Image.nISPosition = IMAGE_IS_POSITION_INIT;
	m_Image.nType = ISI_T_SPR;
	::strcpy(m_Image.szImage, pCA->szImageName);
	m_Image.uImage = 0;
#endif

}

void KItem::Remove()
{
#ifdef _SERVER
    m_LastInventoryOwner=0;
#endif
#ifndef _SERVER
    m_NewArrivalTick = 0;
#endif
	m_nIndex = 0;
}

BOOL KItem::SetBaseAttrib(IN const KItemNormalAttrib* pAttrib)
{
	if (!pAttrib)
		return FALSE;

	for (int i = 0; i < sizeof(m_aryBaseAttrib) / sizeof(m_aryBaseAttrib[0]); i++)
	{
		m_aryBaseAttrib[i] = pAttrib[i];
	}
	return TRUE;
}

BOOL KItem::SetRequireAttrib(IN const KItemNormalAttrib* pAttrib)
{
	if (!pAttrib)
		return FALSE;

	for (int i = 0; i < sizeof(m_aryRequireAttrib) / sizeof(m_aryRequireAttrib[0]); i++)
	{
		m_aryRequireAttrib[i] = pAttrib[i];
	}
	return TRUE;
}

BOOL KItem::SetMagicAttrib(IN const KItemNormalAttrib* pAttrib)
{
	if(!SetAttrib_MA(pAttrib))return FALSE;
	return ItemGen.RefreshPhongThanSetAttrib(this);
}
//------------------------------------------------------------------
//	Ä¥Ëð£¬·µ»ØÖµ±íÊ¾Ê£ÓàÄÍ¾Ã¶È Quai danh tinh hu do ben
//------------------------------------------------------------------
int KItem::Abrade(IN const int nAbradeP, IN const int nRange)
{
//	g_DebugLog("Abrade(IN const int nAbradeP, IN const int nRange)");

	if (m_nCurrentDur == -1 || nRange == 0)	// ÓÀ²»Ä¥Ëð
		return -1;

	if(nAbradeP > 0)
		m_nCurrentDur -= GetMaxDurability() * nAbradeP / MAX_PERCENT;


//	g_DebugLog("m_nCurrentDur %d", m_nCurrentDur);

	if(m_nCurrentDur > 0)
	{
//		g_DebugLog("m_nCurrentDur g_Random %d", g_Random(nRange));
		// nRandRange·ÖÖ®Ò»µÄ¸ÅÂÊ
		if (g_Random(nRange) == 0)
		{
			m_nCurrentDur--;
			//TamLTM fix hu hong do ben
			if (m_nCurrentDur == 0)
			{
				return 0;
			}
			//End code
		}
	}

	if (m_nCurrentDur <= 0)
		m_nCurrentDur = 0;

	return m_nCurrentDur;
}

#ifndef _SERVER
static void AppendItemDescription(char* pszDestination, size_t nCapacity, const char* pszSource)
{
	if (!pszDestination || !pszSource || nCapacity == 0)
		return;
	size_t nUsed = strlen(pszDestination);
	if (nUsed >= nCapacity - 1)
	{
		pszDestination[nCapacity - 1] = 0;
		return;
	}
	strncat(pszDestination, pszSource, nCapacity - nUsed - 1);
	pszDestination[nCapacity - 1] = 0;
}

static const char* SkipVngDisplayPrefix(const char* pszText)
{
	if (!pszText)
		return "";
	while (*pszText == '#' || *pszText == '$')
		++pszText;
	return pszText;
}

#include "PhongThanEquipmentArt.inl"
#include "PhongThanEquippedSocketArt.inl"
#include "PhongThanEquipmentTooltip.inl"
#include "../../../Headers/PhongThanItemLink.h"
#include "PhongThanMaximumAttribute.inl"
#include "../../../Headers/PhongThanGemText.h"
int KItem::GetTooltipImageName(char* path, int capacity)
{
    if (!path || capacity <= 0) return 0;
    path[0] = 0;
    // All item genres have an inventory icon beside their description.
    // Wearable equipment can additionally resolve its authored large art.
    if (GetGenre() != item_equip || GetDetailType() < equip_meleeweapon ||
        GetDetailType() > equip_signet) {
        g_StrCpyLen(path, m_Image.szImage, capacity);
        return path[0] ? 5 : 0;
    }
    KRUImage image = m_Image;
    PhongThanEquipLargeImage(*this, image);
    g_StrCpyLen(path, image.szImage, capacity);
    // Bit 1 means a real opposite-sex preview is available. Common-art
    // weapons/mounts must never acquire a duplicate inventory thumbnail.
    const KBASICPROP_EQUIPMENT* data = GetGenre() == item_equip ?
        ItemGen.CatalogEquipment(GetDetailType(), GetRow()) : NULL;
    if (data && data->m_nSpecialSex && strcmp(image.szImage, m_Image.szImage)) {
        char alternate[256];
        bool female = strstr(image.szImage, "_big_female.spr") != NULL;
        if (PhongThanBigItemPath(m_Image.szImage, !female, alternate, sizeof(alternate))) {
            KImageParam params;
            if (g_pRepresent->GetImageParam(alternate, &params, ISI_T_SPR) && params.nNumFrames > 0)
                return 3;
        }
    }
    return path[0] ? 1 : 0;
}

void KItem::PaintEquipmentSlot(int nX, int nY, int nWidth, int nHeight)
{
	if (!g_pRepresent || nWidth <= 4 || nHeight <= 4) return;
	KRUImage Icon = m_Image;
	PhongThanEquipLargeImage(*this,Icon);
	PhongThanPaintUpgradeAura(*this,nX,nY,nWidth,nHeight);
	// Native detail 12 is a compact jade pendant slot, not large armor art.
	if (GetGenre()==item_equip && GetDetailType()==equip_shipin)
		PhongThanDrawSlotImage(Icon,nX+2,nY+2,nWidth-4,nHeight-4,true);
	else
		PhongThanDrawSlotImage(Icon,nX,nY,nWidth,nHeight,false);
    PhongThanPaintEquippedSockets(*this,nX,nY,nWidth,nHeight);
}

void KItem::PaintInventorySlot(int nX, int nY, int nWidth, int nHeight)
{
	// Inventory keeps the small icon and never inherits equipped aura/art.
	PhongThanDrawSlotImage(m_Image,nX+2,nY+2,nWidth-4,nHeight-4,true);
    if(m_NewArrivalTick && nWidth>0 && nHeight>0) {
        DWORD elapsed=GetTickCount()-m_NewArrivalTick;
        if(elapsed>120000)m_NewArrivalTick=0;
        else {
            // Exact PC acquisition sparkle, centered in the same slot as the
            // icon. Preserve the 35x35 canvas across all ten animation frames;
            // never recenter each cropped frame or animate the quality border.
            KImageParam info;ZeroMemory(&info,sizeof(info));
            const char* path="\\spr\\pt_item\\acquisition.spr";
            if(g_pRepresent->GetImageParam(path,&info,ISI_T_SPR) && info.nNumFrames>0) {
                KRUImagePart shine;ZeroMemory(&shine,sizeof(shine));
                shine.nType=ISI_T_SPR;shine.nISPosition=IMAGE_IS_POSITION_INIT;
                shine.Color.Color_b.a=255;shine.bRenderStyle=IMAGE_RENDER_STYLE_ALPHA;
                shine.nFrame=(elapsed/(info.nInterval>0?info.nInterval:200))%info.nNumFrames;
                strcpy(shine.szImage,path);
                KRPosition2 offset,size;
                if(g_pRepresent->GetImageFrameParam(path,shine.nFrame,&offset,&size,ISI_T_SPR)) {
                    int frameX=nX+(nWidth-info.nWidth)/2+offset.nX;
                    int frameY=nY+(nHeight-info.nHeight)/2+offset.nY;
                    int left=frameX>nX?frameX:nX,top=frameY>nY?frameY:nY;
                    int right=frameX+size.nX<nX+nWidth?frameX+size.nX:nX+nWidth;
                    int bottom=frameY+size.nY<nY+nHeight?frameY+size.nY:nY+nHeight;
                    if(right>left && bottom>top) {
                        shine.bRenderFlag=RUIMAGE_RENDER_FLAG_FRAME_DRAW;
                        shine.oPosition.nX=left;shine.oPosition.nY=top;
                        shine.oImgLTPos.nX=left-frameX;shine.oImgLTPos.nY=top-frameY;
                        shine.oImgRBPos.nX=right-frameX;shine.oImgRBPos.nY=bottom-frameY;
                        g_pRepresent->DrawPrimitives(1,&shine,RU_T_IMAGE_PART,TRUE);
                    }
                }
            }
        }
    }

	if (IsStack() && nWidth >= NORMAL_FONTSIZE && nHeight >= NORMAL_FONTSIZE + 1)
	{
		char number[16];
		int length = sprintf(number, "%d", GetStackNum());
		int x = nX + nWidth - length * NORMAL_FONTSIZE / 2;
		if (x < nX) x = nX;
		g_pRepresent->OutputText(NORMAL_FONTSIZE, number, KRF_ZERO_END,
			x, nY + nHeight - NORMAL_FONTSIZE - 1, 0xFFFFCC);
	}
}

void KItem::Paint(int nX, int nY, bool bResize/* = false*/, bool bPaintStack/* = false*/)
{
	if (bResize && (m_CommonAttrib.nWidth*m_CommonAttrib.nHeight > 1))
		strcpy(m_Image.szImage, RESIZEITEM_SPR);

	m_Image.oPosition.nX = nX;
	m_Image.oPosition.nY = nY;
	m_Image.bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;
	g_pRepresent->DrawPrimitives(1, &m_Image, RU_T_IMAGE, TRUE);

	if (IsStack() && bPaintStack)
	{
		char szNum[16];
		int nLen = sprintf(szNum, "%d", GetStackNum());
		g_pRepresent->OutputText(NORMAL_FONTSIZE, szNum, KRF_ZERO_END,
			nX + (GetWidth() * 27) - nLen * (NORMAL_FONTSIZE ) / 2,
			nY + (GetHeight() * 26) - (NORMAL_FONTSIZE + 1), 0xFFFFCC);

	}
}


void KItem::GetDesc(char* pszMsg, bool bShowPrice, bool bPriceScale, int nActiveAttrib, int nGoldActiveAttrib)
{
#ifndef _SERVER
    AcknowledgeArrival();
#endif
    if (!pszMsg)
        return;

    // Build in a private buffer; UI consumers expose a 2048-byte title buffer.
    // This prevents legacy unbounded strcat calls from corrupting the UI heap.
    char* pszOutput = pszMsg;
    char szSafeDesc[16384];
    szSafeDesc[0] = 0;
    pszMsg = szSafeDesc;
    // Keep every legacy append inside the private tooltip buffer. A broken
    // Lua GetDesc result can now truncate this tooltip, never the UI heap.
#define strcat(pszDestination, pszSource) AppendItemDescription((pszDestination), sizeof(szSafeDesc), (pszSource))
	char pszKeyName[6];
	char pszTemp[128];
	char pszTemp2[128];

	memset(pszKeyName, 0, sizeof(pszKeyName));
	memset(pszTemp, 0, sizeof(pszTemp));
	memset(pszTemp2, 0, sizeof(pszTemp2));

    const PTThanKyBookText* thanKyBook=PTThanKyBookPresentation(GetGenre(),GetDetailType(),GetParticular());
    if(thanKyBook)strcat(pszMsg,"<color=red>");
    else switch(this->GetPhongThanQuality())
	{
	case 1: strcat(pszMsg, "<color=100,149,255>"); break; // blue
	case 2: strcat(pszMsg, "<color=255,215,0>"); break; // gold
	case 3: strcat(pszMsg, "<color=255,160,32>"); break; // orange
	case 4: strcat(pszMsg, "<color=0,255,0>"); break; // set
	case 5: strcat(pszMsg, "<color=189,40,255>"); break; // Tinh Quan
	default: strcat(pszMsg, "<color=255,255,255>"); break;
	}
    char plainName[80];PTPlainItemName(plainName,sizeof(plainName),m_CommonAttrib.szItemName);
    const char* customName=PhongThanCustomItemRichName(m_CommonAttrib.nParam);
    const char* pszBaseName=thanKyBook?thanKyBook->name:customName?customName:plainName;
    // Star upgrades add a stat layer, not a crafting prefix. A real forged
    // item's canonical name already owns its prefix and must remain intact.
    strcat(pszMsg, pszBaseName);

	strcat(pszMsg, "<bclr=0,0,0><color>");
	if(GetGenre()==item_equip && m_CommonAttrib.nUpgradeLvl>0 && m_CommonAttrib.nUpgradeLvl<=12)
	{
		char stars[32];sprintf(stars,"\n<pic=%d>",PhongThanStarPic(GetPhongThanQuality(),m_CommonAttrib.nUpgradeLvl));
		strcat(pszMsg,stars);
	}

	char szPriceColor[moneyunit_num][32] =
	{
		"<color=255,255,255>",
		"<color=255,90,0>",
		"<color=255,219,74>",
		"<color=255,219,74>",
		"<color=0,255,0>",
		"<color=0,255,0>",
		"<color=255,90,0>",
	};
	if (bShowPrice)
	{
		int nPrice = 0;
		if (bPriceScale)
		{
			nPrice = GetSalePrice();
			strcat(pszMsg, szPriceColor[moneyunit_money]);
			sprintf(pszTemp2, "%d", moneyunit_money);
		}
		else
		{
			nPrice = GetCurPrice();
			strcat(pszMsg, szPriceColor[Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nMoneyUnit]);
			sprintf(pszTemp2, "%d", Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nMoneyUnit);
		}
		strcat(pszMsg, "\n");
		strcpy(pszTemp, "Gi¸ c¶: ");
		strcat(pszMsg, pszTemp);

		g_GameSetting.GetString("MoneyUnit", pszTemp2, "", pszTemp, sizeof(pszTemp));
		sprintf(pszTemp2, "%d %s<color=255,255,255>", nPrice, pszTemp);
		strcat(pszMsg, pszTemp2);
	}

	pszTemp2[0] = 0;
	switch (GetLock()->nState)
	{
		case LOCK_STATE_CHARACTER:
			sprintf(pszTemp2, "<color=0,255,0>VËt phÈm ®Ýnh kÌm theo nh©n vËt<color>");
			break;
		case LOCK_STATE_FOREVER:
			sprintf(pszTemp2, "<color=0,255,0>VËt phÈm nµy ®· khãa b¶o hiÓm vÜnh viÔn<color>");
			break;
		case LOCK_STATE_LOCK:
			sprintf(pszTemp2, "<color=0,255,0>VËt phÈm nµy ®· khãa b¶o hiÓm<color>");
			break;
		case LOCK_STATE_UNLOCK:
			if (m_CommonAttrib.LockItem.dwLockTime > KSG_GetCurSec())
			{
				time_t nowtime=m_CommonAttrib.LockItem.dwLockTime+1451581200;
				struct tm * timeinfo = localtime(&nowtime);
				strcpy(pszTemp, "<color=0,255,0>Thêi gian m?khãa: %H:%M:%S %d-%m-%Y<color>");
				strftime(pszTemp2, sizeof(pszTemp2), pszTemp, timeinfo);
			}
			break;
	}
	if (pszTemp2[0])
	{
		strcat(pszMsg, "\n");
		strcat(pszMsg, pszTemp2);
	}

	sprintf(pszKeyName, "%d", m_CommonAttrib.nSeries);
	g_GameSetting.GetString("Elements", pszKeyName, "", pszTemp, sizeof(pszTemp));
	if (pszTemp[0])
	{
		strcat(pszMsg, "\n");
		strcat(pszMsg, pszTemp);
	}

	const char* customEventIntro=thanKyBook?thanKyBook->intro:PhongThanQuestSealTooltip(m_CommonAttrib.nParam);if(!customEventIntro)customEventIntro=PhongThanCustomItemTooltip(m_CommonAttrib.nParam);
	if (customEventIntro || m_CommonAttrib.szIntro[0])
	{
		char szIntro[1600];
		const char* pszIntro = customEventIntro ? customEventIntro : SkipVngDisplayPrefix(m_CommonAttrib.szIntro);
		PtWrapTooltip(pszIntro,szIntro,sizeof(szIntro),48);

		strcat(pszMsg, "\n");
		strcat(pszMsg, szIntro);
	}
	if (m_CommonAttrib.nItemGenre == item_equip)
	{
		/*if (m_aryBaseAttrib[0].nAttribType > 0 ||
			m_aryRequireAttrib[0].nAttribType > 0 ||
			m_aryMagicAttrib[0].nAttribType > 0)*/
			strcat(pszMsg, "\n\n");
	}
	else
		strcat(pszMsg, "\n");


	if (m_CommonAttrib.nItemGenre == item_magicscript)
	{
		if (m_GeneratorParam.nLuck)
		{
			KTabFile MagicTab;

			MagicTab.Load(MAGICATTRIB_LVINDEX_FILE);

			sprintf(pszKeyName,"%d", m_GeneratorParam.nLuck);

			MagicTab.GetString(pszKeyName,"DESC","",pszTemp,sizeof(pszTemp));
			strcat(pszMsg, "<color=100,100,255>Thuéc tÝnh: ");
			strcat(pszMsg, pszTemp);
			strcat(pszMsg, "\n");
			MagicTab.GetString(pszKeyName,"FIT_EQUIP","",pszTemp,sizeof(pszTemp));
			strcat(pszMsg, "<color=255,219,74>Lo¹i trang b?c?th?kh¶m n¹m: ");
			strcat(pszMsg, pszTemp);
			strcat(pszMsg, "<color=255,255,255>");
			strcat(pszMsg, "\n");
		}

		if(m_CommonAttrib.nLevel)
		{
			sprintf(pszTemp, "<color=100,100,255>PhÈm chÊt thuéc tÝnh: <color=255,255,0>%d<color=255,255,255>", m_CommonAttrib.nLevel);
			strcat(pszMsg, pszTemp);
		}
	}

	(void)nActiveAttrib;
	const BOOL bSetEquipment = m_CommonAttrib.nSetID > 0;
	int i = 0;
	for (i = 0; i < MAX_ITEM_BASEATTRIB; ++i)
	{
		if (m_aryBaseAttrib[i].nAttribType <= 0)
			continue;
		if (m_aryBaseAttrib[i].nAttribType == magic_durability_v)
		{
			if (GetDurability() == -1)
				sprintf(pszTemp, "<color=255,255,0>Kh«ng thÓ ph¸ hñy<color=255,255,255>");
			else
				sprintf(pszTemp, "Do ben: %3d / %3d", GetDurability(), GetMaxDurability());
			strcat(pszMsg, pszTemp);
		}
		else
		{
			const char* pszAttrib = g_MagicDesc.GetDesc(&m_aryBaseAttrib[i]);
			if (!pszAttrib || !pszAttrib[0])
				continue;
			if(PhongThanMaximumAttribute(*this,false,i))strcat(pszMsg,"<pic=428>");
			strcat(pszMsg, pszAttrib);
		}
		strcat(pszMsg, "\n");
	}

	for (i = 0; i < 6; ++i)
	{
		if (m_aryRequireAttrib[i].nAttribType <= 0)
			continue;
		const char* pszRequirement = g_MagicDesc.GetDesc(&m_aryRequireAttrib[i]);
		if (!pszRequirement || !pszRequirement[0])
			continue;
		strcat(pszMsg, Player[CLIENT_PLAYER_INDEX].m_ItemList.EnoughAttrib(&m_aryRequireAttrib[i]) ?
			"<color=255,255,255>" : "<color=255,0,0>");
		strcat(pszMsg, pszRequirement);
		strcat(pszMsg, "\n");
	}

	int nRemainingSetLines = nGoldActiveAttrib;
	for (i = 0; i < MAX_ITEM_MAGICATTRIB; ++i)
	{
		if(bSetEquipment && i==MAX_ITEM_NORMAL_MAGICATTRIB)
		{
			char upgradeLine[256];PhongThanUpgradeDescription(*this,upgradeLine);
			strcat(pszMsg,upgradeLine);
		}
		if (m_aryMagicAttrib[i].nAttribType <= 0)
			continue;
		const char* pszAttrib = g_MagicDesc.GetDesc(&m_aryMagicAttrib[i]);
		if (!pszAttrib || !pszAttrib[0])
			continue;

		if (bSetEquipment && i >= MAX_ITEM_NORMAL_MAGICATTRIB)
		{
			if (nRemainingSetLines > 0)
			{
				strcat(pszMsg, "<color=255,255,0>");
				--nRemainingSetLines;
			}
			else
			{
				strcat(pszMsg, "<color=120,120,120>");
			}
		}
		else
		{
			strcat(pszMsg, "<color=99,101,255>");
		}
		if(PhongThanMaximumAttribute(*this,true,i)||PtGem::Maximum(*this,i))strcat(pszMsg,"<pic=428>");
		strcat(pszMsg, pszAttrib);
		strcat(pszMsg, "\n<color=255,255,255>");
	}


    if(GetGenre()==item_equip)PtSoulDescription(*this,pszMsg);
    if(GetGenre()==item_equip && PtSocketParam(GetParam())){
        PT_SOCKET_STATE sockets;
        if(PtSocketRead(GetParam(),sockets)){
            static KTabFile holeNames,gemRows,materials;static bool loaded=false;
            if(!loaded)loaded=holeNames.Load("\\settings\\item\\da_kong_xiang_qian\\kong_shu_xing_biao_sc.txt") && gemRows.Load("\\settings\\item\\da_kong_xiang_qian\\bao_shi_shu_xing_biao_sc.txt") && materials.Load("\\settings\\item\\001\\material.txt");
            int holeCount=0,filledCount=0;for(int hc=0;hc<4;++hc){if(sockets.Holes[hc])++holeCount;if(sockets.Gems[hc])++filledCount;}
            char holeCountText[96];int maximum=(GetDetailType()==5||GetDetailType()==6||GetDetailType()==7)?3:4;
            sprintf(holeCountText,"\n<color=white>Sè lç ®ôc(%d/%d)<color>\n",filledCount,holeCount);strcat(pszMsg,holeCountText);
            for(int sh=0;sh<4;++sh)if(sockets.Holes[sh]){
                char key[24],name[160],gemName[160],line[512];sprintf(key,"%d",sockets.Holes[sh]);name[0]=gemName[0]=0;
                if(loaded)holeNames.GetString(holeNames.FindRow(key),2,"",name,sizeof(name));
                if(sockets.Gems[sh] && loaded){sprintf(key,"%d",sockets.Gems[sh]);int detail=0;gemRows.GetInteger(gemRows.FindRow(key),3,0,&detail);
                    for(int mr=2;mr<=materials.GetHeight();++mr){int d=0;materials.GetInteger(mr,3,0,&d);if(d==detail){materials.GetString(mr,1,"",gemName,sizeof(gemName));break;}}
                }
                sprintf(line,"\n{pts%d}<color=green>Lç %d: %s<color> - <color=green>%s<color>\n",sh,sh+1,name[0]=='#'?name+1:name,sockets.Gems[sh]?(gemName[0]=='#'?gemName+1:gemName):"Ch­a kh¶m");strcat(pszMsg,line);
                if(sockets.Attributes[sh].Type){KItemNormalAttrib a;memcpy(&a,&sockets.Attributes[sh],sizeof(a));a.nValue[0]-=sockets.Bonus[sh];const char* desc=g_MagicDesc.GetDesc(&a);if(desc){strcat(pszMsg,"<color=yellow>");if(PtGem::MaximumValue(sockets.Gems[sh],a.nAttribType,a.nValue[0]))strcat(pszMsg,"<pic=428>");strcat(pszMsg,desc);
                    if(sockets.Bonus[sh]){char extra[80];sprintf(extra," (T¨ng thªm %d%s)",sockets.Bonus[sh],strchr(desc,'%')?"%":"");strcat(pszMsg,extra);}strcat(pszMsg,"<color>\n");}}
            }
        }else PtSocketRequest(GetParam());
    }
    KItemNormalAttrib combo[10];int comboCount=PhongThanFabaoCombo(GetDetailType(),GetParticularType(),m_CommonAttrib.nUpgradeLvl,combo,10);
    for(int c=0;c<comboCount;++c){const char* desc=g_MagicDesc.GetDesc(&combo[c]);if(desc&&*desc){strcat(pszMsg,"<color=255,255,0>");strcat(pszMsg,desc);strcat(pszMsg,"\n<color=255,255,255>");}}
	if(!bSetEquipment)
	{
		char upgradeLine[256];PhongThanUpgradeDescription(*this,upgradeLine);
		strcat(pszMsg,upgradeLine);
	}

		if (m_CommonAttrib.nExpireTime > KSG_GetCurSec())
	{
		time_t nowtime= m_CommonAttrib.nExpireTime+1451581200;
		struct tm * timeinfo = localtime(&nowtime);
		strcpy(pszTemp, "<color=255,90,0>Thêi h¹n s?dông: %H:%M:%S %d-%m-%Y<color>");
		strftime(pszTemp2, sizeof(pszTemp2), pszTemp, timeinfo);
		strcat(pszMsg, "\n");
		strcat(pszMsg,pszTemp2);
		strcat(pszMsg, "\n");
	}

	if (m_CommonAttrib.nExpirePoint)
	{
		time_t nowtime = m_CommonAttrib.nExpirePoint+(KSG_GetCurSec()+1451581200);
		struct tm * timeinfo = localtime(&nowtime);
		strcpy(pszTemp, "<color=255,90,0>Thêi h¹n s?dông: %H:%M:%S %d-%m-%Y<color>");
		strftime(pszTemp2, sizeof(pszTemp2), pszTemp, timeinfo);
		strcat(pszMsg, "\n");
		strcat(pszMsg,pszTemp2);
		strcat(pszMsg, "\n");
	}
	if(GetGenre()==item_equip)
	{
		int total=0;char line[256];
		strcat(pszMsg,"<color=255,255,0>");
		if(PhongThanKnownTooltipPower(*this,total))sprintf(line,PT_TEXT_POWER,total);
		else strcpy(line,PT_TEXT_UNKNOWN_POWER);
		strcat(pszMsg,line);strcat(pszMsg,"<color=255,255,255>\n\n");

	}
	PlayerItem m_pItems;
	memset(&m_pItems, 0, sizeof(m_pItems));
	int nItemListIdx = Player[CLIENT_PLAYER_INDEX].m_ItemList.FindSame(m_dwID);
	if (nItemListIdx > 0 && nItemListIdx < MAX_PLAYER_ITEM)
		 m_pItems = Player[CLIENT_PLAYER_INDEX].m_ItemList.m_Items[nItemListIdx];
	// Set lines above come directly from the VNG additional-attribute table
	// through m_aryMagicAttrib[6..]. The inherited GoldEquip.txt tooltip path
	// is intentionally removed to prevent Vo Lam rows and duplicate lines.
    // Only generated, allowlisted reservoir formatters run on hover. They get
    // a value snapshot, never execute main(), and do not read/mutate an item.
    const char* poolScript=PhongThanReservoirTooltip(GetGenre(),GetDetailType(),
        GetParticular(),GetMaxStackNum(),GetScript());
    if(poolScript) {
        char description[1024];
        if(PTReservoirDescribe(poolScript,GetStackNum(),description,sizeof(description))) {
            strcat(pszMsg,"\n");strcat(pszMsg,description);strcat(pszMsg,"\n");
        }
    }
    if(PhongThanTravelHasCharges(GetGenre(),GetDetailType(),GetParticular())){
        char uses[120];sprintf(uses,"\n<color=yellow>Sè lÇn sö dông cßn l¹i: %d<color>\n",GetStackNum());strcat(pszMsg,uses);
    }
	if (m_CommonAttrib.nFortune)
	{
		sprintf(pszTemp2, "<color=255,255,0>Tr?s?tµi ph?binh gi¸p:<color> <color=0,255,0>%d<color>", m_CommonAttrib.nFortune);

		strcat(pszMsg, "\n");
		strcat(pszMsg, pszTemp2);
		strcat(pszMsg, "\n<color=255,255,255>");
	}

	if ((Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_PTrade.nTrade || bShowPrice) &&
		m_CommonAttrib.nTradePrice)
	{
		strcat(pszMsg,"\n");
		strcat(pszMsg, "<color=255,255,255>Gi?niªm yÕt: <color=255,255,0>");
		if (m_CommonAttrib.nTradePrice < MONEY_FLOOR)
			sprintf(pszTemp,"%d l­îng",m_CommonAttrib.nTradePrice);
		else if ((m_CommonAttrib.nTradePrice % MONEY_FLOOR) == 0)
			sprintf(pszTemp,"%d v¹n l­îng",m_CommonAttrib.nTradePrice / MONEY_FLOOR);
		else
			sprintf(pszTemp,"%d v¹n %d l­îng",m_CommonAttrib.nTradePrice / MONEY_FLOOR, m_CommonAttrib.nTradePrice % MONEY_FLOOR);
		strcat(pszMsg, pszTemp);
	}

//Debug
#ifdef SWORDONLINE_SHOW_DBUG_INFO
	if(Player[CLIENT_PLAYER_INDEX].m_bDebugMode)
	{
		char szTemp[64];
		sprintf(szTemp, "<color=green>ItemID: [%d]<color>", m_dwID);
		strcat(pszMsg, "\n");
		strcat(pszMsg, szTemp);
		strcat(pszMsg, "\n");
	}
#endif
//end code
    // Common footer belongs to every item genre and remains below all properties.
    char footer[512];int used=0;footer[0]=0;
    if(GetGenre()==item_equip || GetWeight()>0) {
        char weightLine[80];sprintf(weightLine,PT_TEXT_WEIGHT,GetWeight());
        used=_snprintf(footer,sizeof(footer)-1,"\n<color=255,255,255>%s\n",weightLine);
        if(used<0)used=0;
    }
    _snprintf(footer+used,sizeof(footer)-used-1,"\n<color=255,255,255>%s\n",PT_TEXT_CHAT_HINT);
    footer[sizeof(footer)-1]=0;
    PTCopyTooltipWithFooter(pszOutput,GOD_MAX_OBJ_TITLE_LEN,szSafeDesc,footer);
#undef strcat
}
#endif

void KItem::SetDurability(IN const int nDur)
{
	m_nCurrentDur = nDur;
};

int KItem::GetMaxDurability()
{
	for (int i = 0; i < MAX_ITEM_BASEATTRIB; i++)
	{
		if (m_aryBaseAttrib[i].nAttribType == magic_durability_v)
		{
			return m_aryBaseAttrib[i].nValue[0];
		}
	}
	return -1;
}

int KItem::GetTotalMagicLevel()
{
	int nRet = 0;
	for (int i = 0; i < MAX_ITEM_MAGICATTRIB; i++)
	{
		nRet += m_GeneratorParam.nGeneratorLevel[i];
	}
	return nRet;
}

BOOL KItem::IsReduce()
{
	for (int i = 0; i < MAX_ITEM_MAGICATTRIB; i++)
	{
		if (m_GeneratorParam.nGeneratorLevel[i] > MAX_ITEM_GENERATORLEVEL)
			return TRUE;
	}
	return FALSE;
}

int KItem::GetRepairPrice()
{
    const int scale=ItemSet.m_sRepairParam.nMagicScale;
    const int priceScale=ItemSet.m_sRepairParam.nPriceScale;
    if(scale<=0||priceScale<=0||m_CommonAttrib.nItemGenre!=item_equip||m_CommonAttrib.nPrice<=0||m_nCurrentDur<0)return 0;
    const int maximum=GetMaxDurability();
    if(maximum<=0||m_nCurrentDur>=maximum)return 0;
    int magic=IsReduce()?MAX_ITEM_MAGICATTRIB*MAX_ITEM_GENERATORLEVEL:GetTotalMagicLevel();
    if(magic<0)return 0;
    // Keep the original staged integer pricing, with wide intermediates.
    // Low-price gear with a small loss must not get an unusable zero quote.
    __int64 price=(__int64)m_CommonAttrib.nPrice*priceScale/100;
    const __int64 largest=9223372036854775807i64;
    int missing=maximum-m_nCurrentDur;
    if(price>largest/missing)return 2147483647;
    price=price*missing/maximum;
    __int64 multiplier=(__int64)scale+magic;
    if(price>largest/multiplier)return 2147483647;
    price=price*multiplier/scale;
    return price>2147483647?2147483647:price<1?1:(int)price;
}

BOOL KItem::CanBeRepaired()
{
	if (GetGenre() != item_equip)
		return FALSE;

	if (m_nCurrentDur == -1)
		return FALSE;

	int nMaxDur = GetMaxDurability();
	if (m_nCurrentDur == nMaxDur)
		return FALSE;

	return TRUE;
}

BOOL KItem::CanShortKey()
{
	if (m_CommonAttrib.nWidth != 1 || m_CommonAttrib.nHeight != 1)
		return FALSE;

    bool effect=GetGenre()==item_ibitem && m_aryBaseAttrib[0].nAttribType>0;
    effect=effect || PhongThanConsumableInitialCharges(GetGenre(),GetDetailType(),GetParticular(),GetMaxStackNum(),GetScript())>0;
    return PhongThanQuickUse(GetGenre(),GetScript(),effect,m_CommonAttrib.bShortKey!=0);
}

int KItem::GetParticularMelee()
{	int nParticularTypeMellee = m_CommonAttrib.nParticularType;
	if (m_CommonAttrib.nObjIdx == 9)
		nParticularTypeMellee = 0;
	else if (m_CommonAttrib.nObjIdx == 10)
		nParticularTypeMellee = 1;
	else if (m_CommonAttrib.nObjIdx == 11)
		nParticularTypeMellee = 2;
	else if (m_CommonAttrib.nObjIdx == 12)
		nParticularTypeMellee = 3;
	else if (m_CommonAttrib.nObjIdx == 13)
		nParticularTypeMellee = 4;
	else if (m_CommonAttrib.nObjIdx == 14)
		nParticularTypeMellee = 5;
	else if (m_CommonAttrib.nDetailType == 1)
	{
		if (nParticularTypeMellee == 3)
			nParticularTypeMellee = 0;
		else if (nParticularTypeMellee == 4)
			nParticularTypeMellee = 1;
		else if (nParticularTypeMellee == 5)
			nParticularTypeMellee = 2;
	}
	return nParticularTypeMellee;
}

#include "../../../Headers/PhongThanForgedName.h"
#include "../../../Headers/PhongThanForgeState.h"
#include "../../../Headers/PhongThanPurpleEquipment.h"
int KItem::GetPhongThanQuality() const
{
    if (GetGenre() != item_equip) return 0;
    if(PhongThanForgeRecipe(GetUnderlyingParam()))return 3;
    if(PhongThanIsTinhQuanEquipment(GetName()))return 5; // original purple family
    if(PhongThanHasForgedName(GetName()))return 3;
    // Star upgrade rules never determine base quality.
    if (m_CommonAttrib.nSetID > 0) return 4; // set green
    // Gameplay Series is resolved to a valid element before it is sent on
    // the wire. Read the original template marker using the persisted row.
    const KBASICPROP_EQUIPMENT* source = ItemGen.CatalogEquipment(GetDetailType(), GetRow());
    const BOOL sameIdentity = source && source->m_nItemGenre == GetGenre() &&
        source->m_nDetailType == GetDetailType() &&
        source->m_nParticularType == GetParticularType() &&
        source->m_nLevel == m_CommonAttrib.nLevel;
    if (GetDetailType() == equip_amulet || GetDetailType() == equip_signet ||
        GetDetailType() == equip_shipin || GetDetailType() == equip_ptseal)
        return 2; // Phap bao, Ngoc Boi and Than An are golden equipment families.
    if (sameIdentity && source->m_nSeries == 1000 &&
        (GetDetailType() == equip_horse || GetDetailType() == equip_meleeweapon ||
         GetDetailType() == equip_rangeweapon))
        return 2;
    for (int nAttrib = 0; nAttrib < MAX_ITEM_MAGICATTRIB; ++nAttrib)
        if (m_aryMagicAttrib[nAttrib].nAttribType > 0)
            return 1; // blue attributes never promote to gold
    return 0;
}

int KItem::GetQuality()
{
	if (GetGenre() == item_equip)
	{
		if(m_nCurrentDur == 0)
			return equip_damage;
		if (m_CommonAttrib.nSetID > 0)
			return equip_set;
		// VNG set/additional attributes live after the six ordinary slots. They
		// still make a normal item green even when slot zero is intentionally
		// empty, so inspect the complete attribute payload.
		for (int nAttrib = 0; nAttrib < MAX_ITEM_MAGICATTRIB; ++nAttrib)
		{
			if (m_aryMagicAttrib[nAttrib].nAttribType > 0)
				return equip_magic;
		}
	}
	return equip_normal;
}

int KItem::GetSalePrice()
{
    // Trading and selling have independent template permissions.
    if (GetLockSell()) return 0;
    return PhongThanShopSaleQuote(m_CommonAttrib.nPrice, IsReservoir() ? 1 : GetStackNum(), BUY_SELL_SCALE,
        GetGenre() == item_equip ? 200 : 1);
}

//TamLTM kham nam
int KItem::GetColorItem()
{
    const int quality = GetPhongThanQuality();
    // Object/item name palette: 0 white, 1 blue, 2 set green, 3 gold, 5 orange.
    if (quality == 1) return 1;
    if (quality == 2) return 3;
    if (quality == 4) return 2;
    if (quality == 3) return 5; // orange forged
    if (quality == 5) return 6; // Tinh Quan purple
    // Durability does not change rarity or the ground-name palette.
    return 0;
}
//End code

#include "PhongThanForgeItem.inl"

#include "PhongThanSmithItem.inl"
