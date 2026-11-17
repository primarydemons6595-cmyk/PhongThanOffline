// Dedicated Event Handbook test controls. Server checks book ownership again
// at every action. No client-supplied task/skill identity is accepted.
#ifndef PT_HANDBOOK_REALM_TEST
#include "KPakFile.h"
#endif
static bool PtHandbookRealmScriptReady(int skill)
{
    for(int row=2;row<=g_OrdinSkillsSetting.GetHeight();++row){
        int id;g_OrdinSkillsSetting.GetInteger(row,"SkillId",0,&id);if(id!=skill)continue;
        char script[MAX_PATH];g_OrdinSkillsSetting.GetString(row,"LvlSetScript","",script,sizeof(script));
        if(!script[0])return true;
        KPakFile file;return file.Open(script)&&file.Size()>0;
    }
    return false;
}
static int PtHandbookRealm(int player,int operation,int value)
{
    if(!PtHandbookAllowed(player))return 0;
    KPlayer& owner=Player[player];const int profession=owner.m_cProfession.GetProfession();
    if(profession<0||profession>=4)return 0;
    int side=owner.m_cTask.GetSaveVal(4868),level=owner.m_cTask.GetSaveVal(4869);
    bool unlocked=PhongThanRealmUnlocked(owner.m_cTask.GetSaveVal(4871),side,level);
    if(operation==20){
        if(value!=1&&value!=2)return 0;
        // A side change never silently invalidates the currently worn set.
        // Native switching owns validation/rollback and preserves inactive gear.
        if(side!=value&&owner.m_ItemList.EquipmentRealm()&& !owner.m_ItemList.SwitchEquipmentRealm(0))return 0;
        if(!unlocked){level=1;owner.m_cTask.SetSaveVal(4869,level,TRUE);owner.m_cTask.SetSaveVal(PT_REALM_EXP_TASK,0,TRUE);}
        owner.m_cTask.SetSaveVal(4868,value,TRUE);
        owner.m_cTask.SetSaveVal(4872,value,TRUE);
        owner.m_cTask.SetSaveVal(4871,13,TRUE);
    }else if(operation==21){
        if(!unlocked||value<1||value>PT_REALM_MAX_LEVEL)return 0;
        const PT_REALM_GROWTH* growth=PhongThanRealmGrowth(profession,value);if(!growth)return 0;
        // Lowering level while Realm gear is worn must pass native unequip
        // transaction before a requirement can become invalid.
        if(value<level&&owner.m_ItemList.EquipmentRealm()&&!owner.m_ItemList.SwitchEquipmentRealm(0))return 0;
        owner.m_cTask.SetSaveVal(4869,value,TRUE);
        owner.m_cTask.SetSaveVal(PT_REALM_EXP_TASK,growth->experience,TRUE);
    }else if(operation==22||operation==23){
        if(!unlocked||value<0||value>1000000000)return 0;
        PtRealmDaily::Migrate(player);
        owner.m_cTask.SetSaveVal(operation==22?PtIslandTask::TienCredit:PtIslandTask::MaCredit,value,TRUE);
    }else if(operation==24){
        if(!unlocked)return 0;
        PtHandbookSkill rows[PT_REALM_SKILL_COUNT];
        for(int i=0;i<PT_REALM_SKILL_COUNT;++i){
            rows[i].profession=profession;rows[i].id=ptRealmSkills[profession][i];
            if(!PtHandbookRealmScriptReady(rows[i].id))return -9;
            rows[i].maximum=g_SkillManager.GetSkillMaxLevel(rows[i].id);
        }
        PtHandbookSkillActor actor(player);
        if(!PhongThanHandbookMaxSkills(rows,PT_REALM_SKILL_COUNT,actor))return 0;
    }else return 0;
    owner.UpdataCurData();owner.m_uMustSave=SAVE_REQUEST;
    owner.SendCurNormalSyncData();owner.SendSyncData_Skill();
    return 1;
}
