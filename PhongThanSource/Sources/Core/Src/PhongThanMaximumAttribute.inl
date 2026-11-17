// Compare the rolled property with its authored range, before upgrade deltas.
// Fixed properties and set activations have no random maximum indicator.
#include "PhongThanPropertyRanges.h"
static bool PhongThanMaximumAttribute(const KItem& item,bool magic,int slot)
{
    if(item.GetGenre()!=item_equip || slot<0 || slot>=(magic?MAX_ITEM_NORMAL_MAGICATTRIB:MAX_ITEM_BASEATTRIB))return false;
    const KBASICPROP_EQUIPMENT* data=ItemGen.CatalogEquipment(item.GetDetailType(),item.GetRow());
    if(!data || data->m_nDetailType!=item.GetDetailType() || data->m_nParticularType!=item.GetParticularType() || data->m_nLevel!=item.m_CommonAttrib.nLevel)return false;
    if(magic)data=PhongThanPropertyRanges(data);
    KItem original=item;
    if(original.GetUpgradeState() && !original.ApplyUpgradeState(0))return false;
    int source=slot;
    if(magic) {
        source=-1;int projected=0;
        for(int i=5;i<MAX_ITEM_BASEATTRIB;++i) {
            int type=data->m_aryPropBasic[i].nType;
            if(type<=magic_normal_begin || type>=magic_normal_end)continue;
            if(projected++==slot){source=i;break;}
        }
        if(source<0)return false;
    }
    const KEQCP_BASIC& range=data->m_aryPropBasic[source];
    const KItemNormalAttrib& actual=magic?original.m_aryMagicAttrib[slot]:original.P0BaseAttributes()[slot];
    return range.nType>0 && actual.nAttribType==range.nType && range.sRange.nMax>range.sRange.nMin &&
        actual.nValue[0]==range.sRange.nMax && !actual.nValue[1] && !actual.nValue[2];
}
