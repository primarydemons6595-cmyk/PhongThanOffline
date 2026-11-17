#ifndef PT_LEGACY_DROP_NATIVE_CATALOG_H
#define PT_LEGACY_DROP_NATIVE_CATALOG_H
#include "LegacyDropExecutionPolicy.h"
#include "LegacyDropItemAdapter.h"
class LegacyDropNativeCatalog:public LegacyDrop::Execution::Catalog {
    const LegacyDropItemAdapter& adapter;
    const KItemGenerator& generator;
public:
    LegacyDropNativeCatalog(const LegacyDropItemAdapter& a,const KItemGenerator& g):adapter(a),generator(g){}
    bool EquipmentRequirement(int detail,int particular,int level,int& required) const {
        required=0;const KBASICPROP_EQUIPMENT* found=0;
        for(int row=0;row<generator.CatalogEquipmentCount(detail);++row){
            const KBASICPROP_EQUIPMENT* e=generator.CatalogEquipment(detail,row);
            if(e&&e->m_nParticularType==particular&&e->m_nLevel==level){
                if(found)return false;found=e;
            }
        }
        if(!found)return false;
        for(int r=0;r<6;++r)if(found->m_aryPropReq[r].nType==magic_requirelevel){
            if(found->m_aryPropReq[r].nPara<0)return false;
            if(found->m_aryPropReq[r].nPara>required)required=found->m_aryPropReq[r].nPara;
        }
        return true;
    }
    bool Has(int genre,int detail,int particular,int level) const {
        PhongThanQuestItemTuple t;if(level<0||!adapter.Map(genre,detail,particular,t))return false;
        const KLibOfBPT& c=generator.Catalog();
        switch(t.genre){
        case item_equip:{
            for(int row=0;row<generator.CatalogEquipmentCount(t.detail);++row){
                const KBASICPROP_EQUIPMENT* e=generator.CatalogEquipment(t.detail,row);
                if(e&&e->m_nParticularType==t.particular&&e->m_nLevel==level)return true;
            }return false;}
        case item_event:{const KBASICPROP_EVENTITEM* e=c.GetEvent(t.detail);return level==0&&e&&e->m_nItemGenre==t.genre&&e->m_nDetailType==t.detail;}
        case item_medicine:return c.FindMedicine(t.detail,level)!=0;
        case item_materials:return level==0&&c.GetMaterial(t.detail)!=0;
        case item_task:return level==0&&c.GetQuestRecord(t.detail)!=0;
        case item_townportal:return level==0&&c.GetTownPortalRecord(t.detail)!=0;
        case item_magicscript:return c.GetMagicScript(t.detail)!=0;
        case item_skillbook:return c.GetSkillBook(t.detail,t.particular)!=0;
        case item_ibitem:return level==0&&c.GetIBItem(t.detail,t.particular)!=0;
        }return false;
    }
};
#endif
