// VNG game.exe 0x7087e0: each basic property's rolled value / authored
// maximum multiplies its power weight. *a,b* supplies separate weights for
// ordinary properties and maximum physical damage (type 29), not RNG bounds.
// Additional properties use their authored maximum and additional weight.
#ifndef _SERVER
// Cache only successful calculations. All mathematical item inputs form the
// key; no item index, serial or pointer can reuse another item's result.
static std::map<std::string,int>& PtEquipmentPowerMemo(){static std::map<std::string,int> memo;return memo;}
static bool PtEquipmentPowerKey(const KItem& item,std::string& key){
 int param=item.GetUnderlyingParam();PT_SOCKET_STATE state;
 // A definition arriving later must never leave a cached socket-less value.
 if((PtSocketParam(item.GetParam())&&!PtSocketRead(item.GetParam(),state))||
    (PtSocketParam(param)&&!PtSocketRead(param,state)))return false;
 int identity[]={item.GetGenre(),item.GetDetailType(),item.m_CommonAttrib.nParticularType,
  item.m_CommonAttrib.nLevel,item.GetSeries(),item.GetRow(),item.GetParam(),param,
  item.m_CommonAttrib.nUpgradeLvl,item.m_CommonAttrib.nUpgradeRule,
  (int)item.m_CommonAttrib.nUpgradeAddedBaseMask,(int)item.m_CommonAttrib.nUpgradeAddedRequireMask,
  item.m_CommonAttrib.nPhysicVal,item.m_CommonAttrib.nMagicVal};
 key.assign((const char*)identity,sizeof(identity));
 key.append((const char*)item.P0BaseAttributes(),MAX_ITEM_BASEATTRIB*sizeof(KItemNormalAttrib));
 key.append((const char*)item.TooltipRequirements(),6*sizeof(KItemNormalAttrib));
 key.append((const char*)item.m_aryMagicAttrib,sizeof(item.m_aryMagicAttrib));return true;
}
#endif
BOOL KItem::GetEquipmentPower(int& total) const
{
    total=0;if(GetGenre()!=item_equip)return FALSE;
#ifndef _SERVER
    std::string memoKey;bool memoReady=PtEquipmentPowerKey(*this,memoKey);
    if(memoReady){std::map<std::string,int>::iterator found=PtEquipmentPowerMemo().find(memoKey);
        if(found!=PtEquipmentPowerMemo().end()){total=found->second;return TRUE;}}
#endif

    const KBASICPROP_EQUIPMENT* data=ItemGen.CatalogEquipment(GetDetailType(),GetRow());
    if(!data)return FALSE;
    KItem original=*this;
    if(original.GetUpgradeState() && !original.ApplyUpgradeState(0))return FALSE;
    __int64 score=0;
    int forgePower=0;bool forged=false;
    if(!PhongThanForgePower(original,data->m_nMagicPower,forgePower,forged))return FALSE;
    int projected=0;
    for(int slot=0;slot<MAX_ITEM_BASEATTRIB;++slot) {
        const KEQCP_BASIC& source=data->m_aryPropBasic[slot];
        if(source.nType<=magic_normal_begin || source.nType>=magic_normal_end)continue;
        const bool modifier=slot>=5;
        if(modifier&&forged)continue;
        const KItemNormalAttrib* actual=0;
        if(modifier) {
            if(projected>=MAX_ITEM_NORMAL_MAGICATTRIB)continue;
            actual=&original.m_aryMagicAttrib[projected++];
        } else actual=&original.m_aryBaseAttrib[slot];
        if(actual->nAttribType!=source.nType || source.sRange.nMax<=0)continue;
        int weight=data->m_nMagicPower;
        if(!modifier) {
            if(source.nType==31)continue; // VNG excludes durability from power.
            if(data->m_nBasePowerKind==PT_POWER_PAIR)
                weight=source.nType==29?data->m_nBasePowerSecond:data->m_nBasePowerFirst;
            else if(data->m_nBasePowerKind==PT_POWER_SCALAR)weight=data->m_nBasePower;
            else if(data->m_nBasePowerKind==PT_POWER_EMPTY)weight=0;
            else return FALSE;
        }
        if(weight>0 && actual->nValue[0]>0)
            score+=(__int64)actual->nValue[0]*weight/source.sRange.nMax;
        if(score>INT_MAX)return FALSE;
    }
    int upgrade=0;
    if(!GetUpgradePower(upgrade))return FALSE;
    score+=forgePower;score+=upgrade;score+=PtSocketPower(m_CommonAttrib.nParam);if(score>INT_MAX)return FALSE;
    total=(int)score;
#ifndef _SERVER
    if(memoReady){if(PtEquipmentPowerMemo().size()>=512)PtEquipmentPowerMemo().clear();PtEquipmentPowerMemo()[memoKey]=total;}
#endif
    return TRUE;
}
