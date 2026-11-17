#include "KCore.h"
#include "KItem.h"
#include "KItemGenerator.h"
#include "KItemSet.h"
#include <time.h>
#include "../../../Headers/PhongThanVttStone.h"
#include "../../../Headers/PhongThanWeeklyDragon.h"
#ifdef _SERVER
#include "NativeDropSerialReservation.h"
#endif
//#include "MyAssert.h"

KItemSet	ItemSet;

#ifdef _SERVER
static void InitializeIBItemGenTime(KItem* pItem)
{
    if(pItem&&PtWeeklyDragonItem(pItem->GetGenre(),pItem->GetDetailType(),pItem->GetParticular())){
        if(pItem->GetParam()==-1)pItem->SetParam(0);
        if(!pItem->m_CommonAttrib.nExpireTime)pItem->SetExpireTime(KSG_GetCurSec()+604800);
        return;
    }
	if (pItem && pItem->GetGenre() == item_ibitem && pItem->GetParam() == -1)
		pItem->SetParam((int)time(NULL));
    if(pItem&&PhongThanIsVttStone(pItem->GetGenre(),pItem->GetDetailType(),pItem->GetParticular())){
        if(pItem->GetParam()<=0)pItem->SetParam((int)time(NULL));
        const int deadline=PhongThanVttStoneDeadline(pItem->GetParam());
        const int stored=pItem->m_CommonAttrib.nExpireTime;
        if(!stored||stored>deadline)pItem->SetExpireTime(deadline);
    }

}
#endif

/*!*****************************************************************************
// Function		: KItemSet::KItemSet
// Purpose		: 
// Return		: 
// Comments		:
// Author		: Spe
*****************************************************************************/
KItemSet::KItemSet()
{
	m_dwIDCreator = 100;
	ZeroMemory(&m_sRepairParam, sizeof(REPAIR_ITEM_PARAM));
#ifdef _SERVER
	m_psItemInfo = NULL;
	m_psBackItemInfo = NULL;
#endif
}

KItemSet::~KItemSet()
{
#ifdef _SERVER
	if (m_psItemInfo)
		delete [] m_psItemInfo;
	m_psItemInfo = NULL;
	if (m_psBackItemInfo)
		delete [] m_psBackItemInfo;
	m_psBackItemInfo = NULL;
#endif
}

/*!*****************************************************************************
// Function		: KItemSet::Init
// Purpose		: 
// Return		: void 
// Comments		:
// Author		: Spe
*****************************************************************************/
void KItemSet::Init()
{
	m_FreeIdx.Init(MAX_ITEM);
	m_UseIdx.Init(MAX_ITEM);

	for (int i = MAX_ITEM - 1; i > 0; i--)
	{
		m_FreeIdx.Insert(i);
	}
#ifdef _SERVER
	if (m_psItemInfo)
		delete [] m_psItemInfo;
	m_psItemInfo = NULL;
	m_psItemInfo = new TRADE_ITEM_INFO[TRADE_ROOM_WIDTH * TRADE_ROOM_HEIGHT];
	memset(this->m_psItemInfo, 0, sizeof(TRADE_ITEM_INFO) * TRADE_ROOM_WIDTH * TRADE_ROOM_HEIGHT);
	if (m_psBackItemInfo)
		delete [] m_psBackItemInfo;
	m_psBackItemInfo = NULL;
	m_psBackItemInfo = new TRADE_ITEM_INFO[TRADE_ROOM_WIDTH * TRADE_ROOM_HEIGHT];
	memset(this->m_psBackItemInfo, 0, sizeof(TRADE_ITEM_INFO) * TRADE_ROOM_WIDTH * TRADE_ROOM_HEIGHT);
#endif
	KIniFile	IniFile;
	IniFile.Load(ITEM_ABRADE_FILE);
//	维修价格
	IniFile.GetInteger("Repair", "ItemPriceScale", 100, &m_sRepairParam.nPriceScale);
	IniFile.GetInteger("Repair", "MagicPriceScale", 10, &m_sRepairParam.nMagicScale);
	IniFile.GetInteger("Repair", "WarningBaseline", 10, &m_sRepairParam.nWarningBaseline); // sua do ben trang bi
//	攻击磨损
	IniFile.GetInteger("Attack", "Weapon", 256, &m_nItemAbradeRate[enumAbradeAttack][itempart_weapon]);
	IniFile.GetInteger("Attack", "Head", 0, &m_nItemAbradeRate[enumAbradeAttack][itempart_head]);
	IniFile.GetInteger("Attack", "Body", 0, &m_nItemAbradeRate[enumAbradeAttack][itempart_body]);
	IniFile.GetInteger("Attack", "Belt", 0, &m_nItemAbradeRate[enumAbradeAttack][itempart_belt]);
	IniFile.GetInteger("Attack", "Foot", 0, &m_nItemAbradeRate[enumAbradeAttack][itempart_foot]);
	IniFile.GetInteger("Attack", "Cuff", 0, &m_nItemAbradeRate[enumAbradeAttack][itempart_cuff]);
	IniFile.GetInteger("Attack", "Amulet", 0, &m_nItemAbradeRate[enumAbradeAttack][itempart_amulet]);
	IniFile.GetInteger("Attack", "Ring1", 0, &m_nItemAbradeRate[enumAbradeAttack][itempart_ring1]);
	IniFile.GetInteger("Attack", "Ring2", 0, &m_nItemAbradeRate[enumAbradeAttack][itempart_ring2]);
	IniFile.GetInteger("Attack", "Pendant", 0, &m_nItemAbradeRate[enumAbradeAttack][itempart_pendant]);
	IniFile.GetInteger("Attack", "Horse", 0, &m_nItemAbradeRate[enumAbradeAttack][itempart_horse]);
// 防御磨损
	IniFile.GetInteger("Defend", "Weapon", 0, &m_nItemAbradeRate[enumAbradeDefend][itempart_weapon]);
	IniFile.GetInteger("Defend", "Head", 64, &m_nItemAbradeRate[enumAbradeDefend][itempart_head]);
	IniFile.GetInteger("Defend", "Body", 64, &m_nItemAbradeRate[enumAbradeDefend][itempart_body]);
	IniFile.GetInteger("Defend", "Belt", 64, &m_nItemAbradeRate[enumAbradeDefend][itempart_belt]);
	IniFile.GetInteger("Defend", "Foot", 64, &m_nItemAbradeRate[enumAbradeDefend][itempart_foot]);
	IniFile.GetInteger("Defend", "Cuff", 64, &m_nItemAbradeRate[enumAbradeDefend][itempart_cuff]);
	IniFile.GetInteger("Defend", "Amulet", 0, &m_nItemAbradeRate[enumAbradeDefend][itempart_amulet]);
	IniFile.GetInteger("Defend", "Ring1", 0, &m_nItemAbradeRate[enumAbradeDefend][itempart_ring1]);
	IniFile.GetInteger("Defend", "Ring2", 0, &m_nItemAbradeRate[enumAbradeDefend][itempart_ring2]);
	IniFile.GetInteger("Defend", "Pendant", 0, &m_nItemAbradeRate[enumAbradeDefend][itempart_pendant]);
	IniFile.GetInteger("Defend", "Horse", 0, &m_nItemAbradeRate[enumAbradeDefend][itempart_horse]);
// 移动磨损
	IniFile.GetInteger("Move", "Weapon", 0, &m_nItemAbradeRate[enumAbradeMove][itempart_weapon]);
	IniFile.GetInteger("Move", "Head", 0, &m_nItemAbradeRate[enumAbradeMove][itempart_head]);
	IniFile.GetInteger("Move", "Body", 0, &m_nItemAbradeRate[enumAbradeMove][itempart_body]);
	IniFile.GetInteger("Move", "Belt", 0, &m_nItemAbradeRate[enumAbradeMove][itempart_belt]);
	IniFile.GetInteger("Move", "Foot", 64, &m_nItemAbradeRate[enumAbradeMove][itempart_foot]);
	IniFile.GetInteger("Move", "Cuff", 0, &m_nItemAbradeRate[enumAbradeMove][itempart_cuff]);
	IniFile.GetInteger("Move", "Amulet", 0, &m_nItemAbradeRate[enumAbradeMove][itempart_amulet]);
	IniFile.GetInteger("Move", "Ring1", 0, &m_nItemAbradeRate[enumAbradeMove][itempart_ring1]);
	IniFile.GetInteger("Move", "Ring2", 0, &m_nItemAbradeRate[enumAbradeMove][itempart_ring2]);
	IniFile.GetInteger("Move", "Pendant", 0, &m_nItemAbradeRate[enumAbradeMove][itempart_pendant]);
	IniFile.GetInteger("Move", "Horse", 64, &m_nItemAbradeRate[enumAbradeMove][itempart_horse]);

}

/*!*****************************************************************************
// Function		: KItemSet::SearchID
// Purpose		: 
// Return		: int 
// Argumant		: DWORD dwID
// Comments		:
// Author		: Spe
*****************************************************************************/
int KItemSet::SearchID(DWORD dwID)
{
	int nIdx = 0;

	while(1)
	{
		nIdx = m_UseIdx.GetNext(nIdx);
		if (!nIdx)
			break;
		if (Item[nIdx].GetID() == dwID)
			break;
	}
	return nIdx;
	
}

#ifdef _SERVER
int KItemSet::AddPersistent(KItem* pItem,DWORD serial)
{
    if(!pItem||!serial||serial>=0xfffffffeUL||SearchID(serial))return 0;
    int index=FindFree();if(!index)return 0;
    Item[index]=*pItem;InitializeIBItemGenTime(&Item[index]);Item[index].SetID(serial);
    if(m_dwIDCreator<=serial)m_dwIDCreator=serial+1;
    m_FreeIdx.Remove(index);m_UseIdx.Insert(index);return index;
}
#endif

int KItemSet::Add(KItem* pItem)
{
	KASSERT(NULL != pItem);

	int i = FindFree();

	if (!i)
		return 0;

	Item[i] = *pItem;
#ifdef _SERVER
	InitializeIBItemGenTime(&Item[i]);
	SetID(i);
	if(!Item[i].GetID()){Item[i].Remove();return 0;}
#endif
	m_FreeIdx.Remove(i);
	m_UseIdx.Insert(i);
	return i;

}

/*!*****************************************************************************
// Function		: KItemSet::Add
// Purpose		: 
// Return		: int 数组编号
// Argumant		: int 道具类型（装备？药品？矿石？……）
// Argumant		: int 魔法等级（如对应于装备，就是一般装备，蓝色装备，亮金等……）
// Argumant		: int 五行属性
// Argumant		: int 等级
// Argumant		: int 幸运值
// Comments		:
// Author		: Spe
*****************************************************************************/
int KItemSet::Add(int nItemGenre, int nSeries, 
				  int nLevel, int nLuck, int nDetailType/*=-1*/, 
				  int nParticularType/*=-1*/, int* pnMagicLevel, int nVersion/*=0*/, UINT nRandomSeed, int nTemplateRow)
{
	int i = FindFree();
	
	if (i == 0)
		return 0;

	KItem*	pItem = &Item[i];
	pItem->m_GeneratorParam.nVersion = nVersion;
	pItem->m_GeneratorParam.uRandomSeed = nRandomSeed;

	BOOL bGenerated = FALSE;
	switch(nItemGenre)
	{
	case item_equip:			// 装备类
		if (nTemplateRow >= 0)
			bGenerated = ItemGen.Gen_EquipmentByTemplateRow(nDetailType, nTemplateRow, nSeries, pnMagicLevel, nLuck, nVersion, pItem);
		else
			bGenerated = ItemGen.Gen_Equipment(nDetailType, nParticularType, nSeries, nLevel, pnMagicLevel, nLuck, nVersion, pItem);
		break;
	case item_medicine:			// 药品类
		bGenerated = ItemGen.Gen_Medicine(nDetailType, nLevel, nVersion, pItem);
		break;
	case item_event:				// 矿石类
		bGenerated = ItemGen.Gen_Event(nDetailType, pItem);
		break;
	case item_materials:				// 矿石类
		bGenerated = ItemGen.Gen_Material(nDetailType, pItem);
		break;
	case item_task:				// 默认是任务类
		bGenerated = ItemGen.Gen_Quest(nDetailType, pItem);
		break;
	case item_magicscript:
		bGenerated = ItemGen.Gen_MagicScript(nDetailType, pItem, nLevel, nSeries, nLuck);
		break;
	case item_skillbook:
		bGenerated = ItemGen.Gen_SkillBook(nDetailType, nParticularType, nLevel, pItem);
		break;
	case item_ibitem:
		bGenerated = ItemGen.Gen_IBItem(nDetailType, nParticularType, pItem);
		break;
	case item_townportal:
		bGenerated = ItemGen.Gen_TownPortal(nDetailType, pItem);
		break;
	default:
		break;
	}
	if (!bGenerated)
		return 0;
	if (nTemplateRow >= 0)
		pItem->SetRow(nTemplateRow);

#ifdef _SERVER
	InitializeIBItemGenTime(pItem);
	SetID(i);
	if(!Item[i].GetID()){Item[i].Remove();return 0;}
#endif
	m_FreeIdx.Remove(i);
	m_UseIdx.Insert(i);
	return i;
}

// end
/*!*****************************************************************************
// Function		: KItemSet::FindFree
// Purpose		: 
// Return		: int 
// Comments		:
// Author		: Spe
*****************************************************************************/
int KItemSet::FindFree()
{
	return m_FreeIdx.GetNext(0);
}

void KItemSet::Remove(IN int nIdx)
{
	Item[nIdx].Remove();
	
	m_UseIdx.Remove(nIdx);
	m_FreeIdx.Insert(nIdx);
}

void KItemSet::SetID(IN int nIdx)
{
#ifdef _SERVER
    DWORD serial=0;std::string error;
    if(!NativeDropRollout::MainSerial(m_dwIDCreator,serial,error)){FILE*f=fopen("native_drop_serial_errors.log","a");if(f){fprintf(f,"serial_allocation_rejected reason=%s\n",error.c_str());fclose(f);}Item[nIdx].SetID(0);return;}
    Item[nIdx].SetID(serial);
#else
	Item[nIdx].SetID(m_dwIDCreator);
	m_dwIDCreator++;
#endif
}

#ifdef _SERVER
//---------------------------------------------------------------------------
//	功能：copy m_psItemInfo to m_psBackItemInfo
//---------------------------------------------------------------------------
void	KItemSet::BackItemInfo()
{
	_ASSERT(this->m_psItemInfo);
	_ASSERT(this->m_psBackItemInfo);
	if (!m_psItemInfo)
		return;
	if (!m_psBackItemInfo)
		m_psBackItemInfo = new TRADE_ITEM_INFO[TRADE_ROOM_WIDTH * TRADE_ROOM_HEIGHT];
	memcpy(m_psBackItemInfo, this->m_psItemInfo, sizeof(TRADE_ITEM_INFO) * TRADE_ROOM_WIDTH * TRADE_ROOM_HEIGHT);
}
#endif

int KItemSet::GetAbradeRange(IN int nType, IN int nPart)
{
	if (nType < 0 || nType >= enumAbradeNum)
		return 0;
	if (nPart < 0 || nPart >= itempart_num)
		return 0;

	return m_nItemAbradeRate[nType][nPart];
}
