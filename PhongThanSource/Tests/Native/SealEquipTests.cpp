#include <windows.h>
#include <assert.h>
#include <stdio.h>
enum ITEM_PART
{
	itempart_head = 0,
	itempart_body,
	itempart_belt,
	itempart_weapon,
	itempart_foot,
	itempart_cuff,
	itempart_amulet,
	itempart_ring1,
	itempart_ring2,
	itempart_pendant,
	itempart_horse,
	itempart_signet,
	itempart_shipin,
	itempart_num,
};
enum EQUIPDETAILTYPE
{
	equip_meleeweapon = 0,
	equip_rangeweapon,
	equip_armor,
	equip_ring,
	equip_amulet,
	equip_boots,
	equip_belt,
	equip_helm,
	equip_cuff,
	equip_pendant,
	equip_horse,
	equip_signet,
	equip_shipin,
	equip_ptseal, // original PT detail 13, F4 Signet slot
	equip_detailnum,
};
enum{item_equip=0};
#define _ASSERT assert
struct KItem{int GetGenre(){return 0;}int detail;int GetDetailType(){return detail;}}Item[4];
class KItemList{public:int GetEquipPlace(int);BOOL Fit(int,int);BOOL Fit(KItem*,int);};
int KItemList::GetEquipPlace(int nType)
{
	int nRet = -1;
	switch(nType)
	{
	case equip_meleeweapon:
	case equip_rangeweapon:
		nRet = itempart_weapon;
		break;
	case equip_armor:
		nRet = itempart_body;
		break;
	case equip_helm:
		nRet = itempart_head;
		break;
	case equip_boots:
		nRet = itempart_foot;
		break;
	case equip_ring:
		nRet = itempart_ring1;
		break;
	case equip_amulet:
		nRet = itempart_amulet;
		break;
	case equip_belt:
		nRet = itempart_belt;
		break;
	case equip_cuff:
		nRet = itempart_cuff;
		break;
	case equip_pendant:
		nRet = itempart_pendant;
		break;
	case equip_horse:
		nRet = itempart_horse;
		break;
	case equip_signet:
		nRet = itempart_shipin;
		break;
	case equip_shipin:
		nRet = itempart_cuff;
		break;
    case equip_ptseal:nRet=itempart_signet;break;
	default:
		break;
	}
	return nRet;
}
BOOL KItemList::Fit(int nIdx, int nPlace)
{
	BOOL	bRet = FALSE;
	_ASSERT(Item[nIdx].GetGenre() == item_equip);
	switch(Item[nIdx].GetDetailType())
	{
	case equip_meleeweapon:
	case equip_rangeweapon:
		if (nPlace == itempart_weapon)
			bRet = TRUE;
		break;
	case equip_armor:
		if (nPlace == itempart_body)
			bRet = TRUE;
		break;
	case equip_belt:
		if (nPlace == itempart_belt)
			bRet = TRUE;
		break;
	case equip_boots:
		if (nPlace == itempart_foot)
			bRet = TRUE;
		break;
	case equip_cuff:
		if (nPlace == itempart_cuff)
			bRet = TRUE;
		break;
	case equip_ring:
		if (nPlace == itempart_ring1 || nPlace == itempart_ring2)
			bRet = TRUE;
		break;
	case equip_amulet:
		if (nPlace == itempart_amulet || nPlace == itempart_ring1)
			bRet = TRUE;
		break;
	case equip_pendant:
		if (nPlace == itempart_pendant)
			bRet = TRUE;
		break;
	case equip_helm:
		if (nPlace == itempart_head)
			bRet = TRUE;
		break;
	case equip_horse:
		if (nPlace == itempart_horse)
			bRet = TRUE;
		break;
	case equip_signet:
		if (nPlace == itempart_shipin)
			bRet = TRUE;
		break;
	case equip_shipin:
		if (nPlace == itempart_cuff)
			bRet = TRUE;
		break;
    case equip_ptseal:if(nPlace==itempart_signet)bRet=TRUE;break;
	}
	return bRet;
}
BOOL KItemList::Fit(KItem* pItem, int nPlace)
{
	BOOL	bRet = FALSE;
	_ASSERT(pItem->GetGenre() == item_equip);
	switch(pItem->GetDetailType())
	{
	case equip_meleeweapon:
	case equip_rangeweapon:
		if (nPlace == itempart_weapon)
			bRet = TRUE;
		break;
	case equip_armor:
		if (nPlace == itempart_body)
			bRet = TRUE;
		break;
	case equip_belt:
		if (nPlace == itempart_belt)
			bRet = TRUE;
		break;
	case equip_boots:
		if (nPlace == itempart_foot)
			bRet = TRUE;
		break;
	case equip_cuff:
		if (nPlace == itempart_cuff)
			bRet = TRUE;
		break;
	case equip_ring:
		if (nPlace == itempart_ring1 || nPlace == itempart_ring2)
			bRet = TRUE;
		break;
	case equip_amulet:
		if (nPlace == itempart_amulet || nPlace == itempart_ring1)
			bRet = TRUE;
		break;
	case equip_pendant:
		if (nPlace == itempart_pendant)
			bRet = TRUE;
		break;
	case equip_helm:
		if (nPlace == itempart_head)
			bRet = TRUE;
		break;
	case equip_horse:
		if (nPlace == itempart_horse)
			bRet = TRUE;
		break;
	case equip_signet:
		if (nPlace == itempart_shipin)
			bRet = TRUE;
		break;
	case equip_shipin:
		if (nPlace == itempart_cuff)
			bRet = TRUE;
		break;
    case equip_ptseal:if(nPlace==itempart_signet)bRet=TRUE;break;
	}
	return bRet;
}
int main(){KItemList list;Item[1].detail=equip_ptseal;assert(list.GetEquipPlace(equip_ptseal)==itempart_signet);for(int i=0;i<itempart_num;++i){assert((bool)list.Fit(1,i)==(i==itempart_signet));assert((bool)list.Fit(&Item[1],i)==(i==itempart_signet));}assert(list.GetEquipPlace(equip_signet)==itempart_shipin);assert(list.GetEquipPlace(equip_shipin)==itempart_cuff);puts("PASS production seal destination and both fit overloads, preserved instrument/jade slots");return 0;}
