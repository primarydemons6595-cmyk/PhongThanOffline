#include "PhongThanPropertyRanges.h"
// Increase one additional scalar random property within its equipment-family range.
// Upgrade deltas are removed then restored; activation/socket properties stay intact.
BOOL KItem::ImprovePhongThanProperty(int choice)
{
 if(GetGenre()!=item_equip)return FALSE;
 const KBASICPROP_EQUIPMENT* data=ItemGen.CatalogEquipment(GetDetailType(),GetRow());
 if(!data||data->m_nDetailType!=GetDetailType()||data->m_nParticularType!=GetParticularType()||data->m_nLevel!=m_CommonAttrib.nLevel)return FALSE;
 data=PhongThanPropertyRanges(data);
 KItem result=*this;int state=GetUpgradeState();if(state&&!result.ApplyUpgradeState(0))return FALSE;
 KItemNormalAttrib* attributes[MAX_ITEM_BASEATTRIB];int maximum[MAX_ITEM_BASEATTRIB],minimum[MAX_ITEM_BASEATTRIB],count=0,magicSlot=0;
 for(int source=5;source<MAX_ITEM_BASEATTRIB;++source){
  const KEQCP_BASIC& range=data->m_aryPropBasic[source];KItemNormalAttrib* actual=0;
  if(range.nType>magic_normal_begin&&range.nType<magic_normal_end){if(magicSlot<MAX_ITEM_NORMAL_MAGICATTRIB)actual=&result.m_aryMagicAttrib[magicSlot];++magicSlot;}
  if(!actual||range.nType<=0||actual->nAttribType!=range.nType||actual->nValue[1]||actual->nValue[2]||range.sRange.nMax<=range.sRange.nMin||actual->nValue[0]<range.sRange.nMin||actual->nValue[0]>=range.sRange.nMax)continue;
  attributes[count]=actual;maximum[count]=range.sRange.nMax;minimum[count]=range.sRange.nMin;++count;
 }
 if(!count)return FALSE;if(choice<0)return TRUE;
 int at=choice%count,step=max(1,(maximum[at]-minimum[at]+9)/10);
 attributes[at]->nValue[0]=min(maximum[at],attributes[at]->nValue[0]+step);
 if(state&&!result.ApplyUpgradeState(state))return FALSE;*this=result;return TRUE;
}
