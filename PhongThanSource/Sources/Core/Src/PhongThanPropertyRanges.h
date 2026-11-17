#ifndef PT_PROPERTY_RANGES_H
#define PT_PROPERTY_RANGES_H
// Fixed starter/quest copies retain their identity and restrictions, but their
// improvable affixes use the authored ranges of the same ordinary equipment.
// Never borrow a different weapon, tier, element/family, image or affix layout.
static const KBASICPROP_EQUIPMENT* PhongThanPropertyRanges(const KBASICPROP_EQUIPMENT* data){
 if(!data)return 0;const char* suffix=strstr(data->m_szName,"(N.v)");if(!suffix)return data;
 char name[256];int length=suffix-data->m_szName;if(length<=0||length>=sizeof(name))return data;
 memcpy(name,data->m_szName,length);while(length>0&&name[length-1]==' ')--length;name[length]=0;
 for(int row=0;row<ItemGen.CatalogEquipmentCount(data->m_nDetailType);++row){
  const KBASICPROP_EQUIPMENT* candidate=ItemGen.CatalogEquipment(data->m_nDetailType,row);
  if(!candidate||candidate==data||strcmp(candidate->m_szName,name)||candidate->m_nLevel!=data->m_nLevel||candidate->m_nSeries!=data->m_nSeries||candidate->m_nSetId!=data->m_nSetId||strcmp(candidate->m_szImageName,data->m_szImageName))continue;
  bool valid=true,wider=false;
  for(int slot=0;slot<MAX_ITEM_BASEATTRIB;++slot){const KEQCP_BASIC& a=data->m_aryPropBasic[slot];const KEQCP_BASIC& b=candidate->m_aryPropBasic[slot];
   if(a.nType!=b.nType){valid=false;break;}
   if(slot>=5&&a.nType>0){if(b.sRange.nMin>a.sRange.nMin||b.sRange.nMax<a.sRange.nMax){valid=false;break;}if(b.sRange.nMax>a.sRange.nMax)wider=true;}
  }
  if(valid&&wider)return candidate;
 }
 return data;
}
#endif
