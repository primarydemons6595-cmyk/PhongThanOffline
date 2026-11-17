#ifndef PHONGTHAN_MAIL_ITEM_H
#define PHONGTHAN_MAIL_ITEM_H
#include "PhongThanPlayerServices.h"
#include "PhongThanUpgradeState.h"
#include "PhongThanForgeState.h"
// Snapshots contain the actual rolled and upgraded attributes. Applying the
// upgrade formula again would either change those rolls or reject non-equipment.
template<class T> bool PtPsRestoreItem(T& item,const PT_PS_ITEM& in){
 int level=0,rule=0;
 if(in.Stack<=0||!PhongThanDecodeUpgradeState(in.Upgrade,level,rule)||
    (in.Genre!=0&&(level||rule))||in.AddedBaseMask&~0xfffffU||in.AddedRequireMask&~63U)return false;
 item.SetStackNum(in.Stack);item.SetDurability(in.Durability);item.SetExpireTime(in.Expire);
 item.SetParam(in.Param);if(in.Genre==0&&PhongThanForgeRecipe(in.Param)&&item.GetParam()!=in.Param)return false;item.SetFortune(in.Fortune);
 item.SetBaseAttrib((const KItemNormalAttrib*)in.Base);
 item.SetRequireAttrib((const KItemNormalAttrib*)in.Require);
 item.SetMagicAttrib((const KItemNormalAttrib*)in.Attributes);
 item.m_CommonAttrib.nPhysicVal=in.Physical;item.m_CommonAttrib.nMagicVal=in.Magic;
 item.m_CommonAttrib.nUpgradeLvl=level;item.m_CommonAttrib.nUpgradeRule=rule;
 item.m_CommonAttrib.nUpgradeAddedBaseMask=in.AddedBaseMask;
 item.m_CommonAttrib.nUpgradeAddedRequireMask=in.AddedRequireMask;return true;
}
#endif
