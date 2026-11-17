// These fields are used only by ephemeral KViewItem objects. They must never
// be applied through inventory/owned-item lookup by the remote serial number.
#include "../../../Headers/PhongThanUpgradeState.h"
typedef char PtViewBaseCount[(MAX_ITEM_BASEATTRIB==20)?1:-1];
typedef char PtViewMagicCount[(MAX_ITEM_MAGICATTRIB==20)?1:-1];
// GameServer's aggregate write buffer is 16 KiB (32 bytes reserved).
typedef char PtViewPacketFits[(sizeof(VIEW_EQUIP_SYNC)<16384-32)?1:-1];
static bool PhongThanViewedOwnerValid(const VIEW_EQUIP_SYNC& v){
    return v.m_dwNpcID&&v.m_Owner.Name[0]&&memchr(v.m_Owner.Name,0,sizeof(v.m_Owner.Name))&&
        memchr(v.m_Owner.szTongName,0,sizeof(v.m_Owner.szTongName))&&memchr(v.m_Owner.szMateName,0,sizeof(v.m_Owner.szMateName))&&
        memchr(v.m_Owner.Title,0,sizeof(v.m_Owner.Title))&&memchr(v.m_Owner.StatusDesc,0,sizeof(v.m_Owner.StatusDesc))&&
        v.m_OwnerLevel>0&&v.m_OwnerLevel<=255&&(v.m_OwnerMale==0||v.m_OwnerMale==1);
}

static bool PhongThanViewedItemValid(const PHONGTHAN_VIEW_ITEM_INFO& info) {
    int level,rule;
    if(info.m_WireVersion!=PHONGTHAN_VIEW_ITEM_VERSION || info.m_btGenre!=item_equip ||
       info.m_btDetail<0 || info.m_btDetail>=equip_detailnum || info.m_nTemplateRow<0 ||
       !memchr(info.m_Name,0,sizeof(info.m_Name)) ||
       !PhongThanDecodeUpgradeState(info.m_nUpgradeState,level,rule))return false;
    for(int group=0;group<3;++group) {
        const PHONGTHAN_ATTRIBUTE_WIRE* attrs=group==0?info.m_Base:group==1?info.m_Require:info.m_Magic;
        int count=group==1?6:20;
        for(int i=0;i<count;++i)if(attrs[i].Type<0 || attrs[i].Type>=magic_normal_end)return false;
    }
    if(info.m_HasSockets>1 || (PtSocketParam(info.m_nParam)?1:0)!=info.m_HasSockets)return false;
    return !info.m_HasSockets || PtSocketValid(info.m_Sockets);
}
static bool PhongThanWriteViewedItem(KItem& item,PHONGTHAN_VIEW_ITEM_INFO& info) {
    info.m_WireVersion=PHONGTHAN_VIEW_ITEM_VERSION;
    info.m_nParam=item.GetParam();info.m_nDurability=item.GetDurability();info.m_nFortune=item.GetFortune();
    info.m_nPhysicalValue=item.m_CommonAttrib.nPhysicVal;info.m_nMagicValue=item.m_CommonAttrib.nMagicVal;
    info.m_nSetId=item.GetSetID();info.m_nGroup=item.GetGroup();
    info.m_nNeed1=item.GetNeedToActive1();info.m_nNeed2=item.GetNeedToActive2();info.m_ExpireTime=item.GetExpireTime();
    strncpy(info.m_Name,item.GetName(),sizeof(info.m_Name)-1);info.m_Name[sizeof(info.m_Name)-1]=0;
    for(int group=0;group<3;++group) {
        const KItemNormalAttrib* attrs=group==0?item.P0BaseAttributes():group==1?item.TooltipRequirements():item.m_aryMagicAttrib;
        PHONGTHAN_ATTRIBUTE_WIRE* dest=group==0?info.m_Base:group==1?info.m_Require:info.m_Magic;
        int count=group==1?6:20;
        for(int i=0;i<count;++i) {
            dest[i].Type=attrs[i].nAttribType;
            for(int k=0;k<3;++k)dest[i].Value[k]=attrs[i].nValue[k];
        }
    }
    info.m_HasSockets=PtSocketParam(info.m_nParam)?1:0;
    if(info.m_HasSockets && !PtSocketRead(info.m_nParam,info.m_Sockets))return false;
    return PhongThanViewedItemValid(info);
}
static bool PhongThanReadViewedItem(KItem& item,const PHONGTHAN_VIEW_ITEM_INFO& info) {
    if(!PhongThanViewedItemValid(info))return false;
    if(info.m_HasSockets && !PtSocketRegister(info.m_nParam,info.m_Sockets))return false;
    item.SetParam(info.m_nParam);
    if(item.GetParam()!=info.m_nParam || !item.ApplyUpgradeState(info.m_nUpgradeState))return false;
    // State/upgrade helpers can generate local values. Replace them with the
    // final server values afterwards, so upgrade bonuses are not added twice.
    KItemNormalAttrib attrs[20];
    for(int group=0;group<3;++group) {
        ZeroMemory(attrs,sizeof(attrs));
        const PHONGTHAN_ATTRIBUTE_WIRE* source=group==0?info.m_Base:group==1?info.m_Require:info.m_Magic;
        int count=group==1?6:20;
        for(int i=0;i<count;++i) {
            attrs[i].nAttribType=source[i].Type;
            for(int k=0;k<3;++k)attrs[i].nValue[k]=source[i].Value[k];
        }
        if(group==0)item.SetBaseAttrib(attrs);else if(group==1)item.SetRequireAttrib(attrs);else item.SetMagicAttrib(attrs);
    }
    strncpy(item.m_CommonAttrib.szItemName,info.m_Name,sizeof(item.m_CommonAttrib.szItemName)-1);
    item.m_CommonAttrib.szItemName[sizeof(item.m_CommonAttrib.szItemName)-1]=0;
    item.m_CommonAttrib.nSetID=info.m_nSetId;item.m_CommonAttrib.nGroup=info.m_nGroup;
    item.m_CommonAttrib.nNeedToActive1=info.m_nNeed1;item.m_CommonAttrib.nNeedToActive2=info.m_nNeed2;
    item.m_CommonAttrib.nPhysicVal=info.m_nPhysicalValue;item.m_CommonAttrib.nMagicVal=info.m_nMagicValue;
    item.SetFortune(info.m_nFortune);item.SetExpireTime(info.m_ExpireTime);item.SetDurability(info.m_nDurability);
    return true;
}
