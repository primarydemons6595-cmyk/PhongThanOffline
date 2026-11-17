#ifndef PHONGTHAN_EQUIPMENT_SKILLS_H
#define PHONGTHAN_EQUIPMENT_SKILLS_H
#include "PhongThanSoulState.h"
// Rebuild only the transient skill overlay; never reapply actor attributes.
template<class Item,class Catalog,class SkillList>
int PhongThanRefreshEquipmentSkills(const int* equipped,int slots,const Item* items,int itemLimit,
    const Catalog& catalog,SkillList& skills,int inactiveSlot,int actorLevel=0,int profession=-1)
{
 int ids[32],levels[32],count=0;
 for(int i=0;i<slots&&count<32;++i){int index=equipped[i];if(i==inactiveSlot||index<=0||index>=itemLimit||items[index].GetGenre()!=0)continue;
  const KBASICPROP_EQUIPMENT* row=catalog.CatalogEquipment(items[index].GetDetailType(),items[index].GetRow());
  if(row&&row->m_nActivateSkill>0){ids[count]=row->m_nActivateSkill;levels[count++]=row->m_nLevel;}
  PT_SOUL_STATE soul;
  if(count<32&&PtSoulFromParam(items[index].GetParam(),soul)){
   const PTSoulFormula* f=PtSoulFormula(soul.Formula);
   if(f&&soul.Aura>=f->cost&&actorLevel>=f->requiredLevel&&profession==f->profession){ids[count]=f->skill;levels[count++]=f->level;}
  }
 }
 return skills.SetEquipmentSkills(ids,levels,count);
}
#endif
