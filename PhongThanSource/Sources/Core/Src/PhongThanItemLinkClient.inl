#include "../../../Headers/PhongThanItemLink.h"
#include <map>
#include <set>
static std::map<unsigned,PTItemLinkMessage> s_PTItemLinks;
static std::set<int> s_PTItemLinkPreviews;
void PTReceiveItemLink(const void* data,unsigned size) {
    if(!PTValidateItemLinkMessage(data,size))return;
    const PTItemLinkMessage& message=*(const PTItemLinkMessage*)data;
    if(s_PTItemLinks.size()>=2048)s_PTItemLinks.erase(s_PTItemLinks.begin());
    s_PTItemLinks[message.Token]=message;
    CoreDataChanged(PT_GDCNI_ITEM_LINK,(unsigned int)data,size);
}
int PTCreateItemLinkPreview(const char* token) {
    if(!token || strncmp(token,"PTL:",4))return 0;
    char* end=0;unsigned id=strtoul(token+4,&end,10);if(!end || *end || !id)return 0;
    std::map<unsigned,PTItemLinkMessage>::const_iterator found=s_PTItemLinks.find(id);
    if(found==s_PTItemLinks.end())return 0;
    const PTItemLinkMessage& data=found->second;
    int generator[40];memcpy(generator,data.Generator,sizeof(generator));
    const int index=ItemSet.Add(data.Genre,data.Series,data.Level,data.Luck,data.Detail,data.Particular,generator,data.Version,data.Seed,data.Row);
    if(index<=0 || index>=MAX_ITEM)return 0;
    KItem& item=Item[index];
    // No KItemList::Add, owned ID, inventory notification or persistence.
    item.SetID(0);item.SetDurability(data.Durability);item.SetStackNum(data.Stack);item.SetExpireTime(data.Expire);
    item.SetParam(data.Param);item.SetFortune(data.Fortune);
    item.SetBaseAttrib((const KItemNormalAttrib*)data.Base);item.SetRequireAttrib((const KItemNormalAttrib*)data.Require);item.SetMagicAttrib((const KItemNormalAttrib*)data.Magic);
    int level=0,rule=0;if(PhongThanDecodeUpgradeState(data.Upgrade,level,rule)){item.m_CommonAttrib.nUpgradeLvl=level;item.m_CommonAttrib.nUpgradeRule=rule;}
    item.m_CommonAttrib.nPhysicVal=data.PhysicalPower;item.m_CommonAttrib.nMagicVal=data.MagicPower;
    g_StrCpyLen(item.m_CommonAttrib.szItemName,data.Name,sizeof(item.m_CommonAttrib.szItemName));
    s_PTItemLinkPreviews.insert(index);return index;
}
void PTReleaseItemLinkPreview(int index) {
    if(s_PTItemLinkPreviews.erase(index))ItemSet.Remove(index);
}
