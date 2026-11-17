#include "../../../../Headers/PhongThanRealmUi.h"
#include "../../../../Headers/PhongThanRealmUiText.h"
void KUiStatus::InitRealmPage(KIniFile* ini)
{
    ini->GetString("JEvilPageSettings","JusticImg","",m_RealmStatusImages[0],256);
    ini->GetString("JEvilPageSettings","EvilImg","",m_RealmStatusImages[1],256);
    AddChild(&m_RealmPage);
    PtPageControl(*ini,"PtRealmPage",34,174,222,232);
    ini->WriteInteger("PtRealmPage","DummyWnd",1);
    m_RealmPage.Init(ini,"PtRealmPage");
    for(int i=0;i<19;++i){
        char section[40];sprintf(section,"PtRealmLine%d",i);
        PtPageControl(*ini,section,0,i*12,222,12);
        ini->WriteInteger(section,"Font",12);ini->WriteString(section,"BorderColor","0,0,0");
        m_RealmPage.AddChild(&m_RealmLines[i]);m_RealmLines[i].Init(ini,section);
    }
    m_RealmPage.Hide();
}
void KUiStatus::UpdateRealmPage()
{
    PTRoleValues role;memset(&role,0,sizeof(role));
    if(!g_pCoreShell||!g_pCoreShell->GetGameData(PT_GDI_ROLE_VALUES,(unsigned int)&role,sizeof(role))||
        !PhongThanRealmUnlocked(role.RealmStage,role.RealmSide,role.RealmLevel))return;
    KUiPlayerAttribute a;KUiPlayerRuntimeInfo vitals;PTStatusDetail detail;
    memset(&a,0,sizeof(a));memset(&vitals,0,sizeof(vitals));memset(&detail,0,sizeof(detail));
    g_pCoreShell->GetGameData(GDI_PLAYER_RT_ATTRIBUTE,(unsigned int)&a,0);
    g_pCoreShell->GetGameData(GDI_PLAYER_RT_INFO,(unsigned int)&vitals,0);
    g_pCoreShell->GetGameData(PT_GDI_STATUS_DETAIL,(unsigned int)&detail,sizeof(detail));
    char line[19][256];
    sprintf(line[0],PT_REALM_UI_Level,role.RealmSide==1?PT_REALM_UI_Tien:PT_REALM_UI_Ma,role.RealmLevel);
    sprintf(line[1],PT_REALM_UI_Experience,role.RealmExperience,role.RealmExperienceNext);
    sprintf(line[2],PT_REALM_UI_TienRep,role.TienReputation);sprintf(line[3],PT_REALM_UI_MaRep,role.MaReputation);
    // These combat values always describe the actually active equipment bank.
    // Reading F3 never equips items, changes stats or simulates a second actor.
    strcpy(line[4],role.EquipmentRealm?PT_REALM_UI_RealmBank:PT_REALM_UI_HumanBank);
    sprintf(line[5],PT_REALM_UI_Strength,a.nStrength,a.nDexterity);sprintf(line[6],PT_REALM_UI_Vitality,a.nVitality,a.nEnergy);
    sprintf(line[7],PT_REALM_UI_Life,vitals.nLifeFull,vitals.nManaFull);
    sprintf(line[8],PT_REALM_UI_LeftDamage,a.nKillMIN,a.nKillMAX);sprintf(line[9],PT_REALM_UI_RightDamage,a.nRightKillMin,a.nRightKillMax);
    sprintf(line[10],PT_REALM_UI_Attack,a.nLeftAttack,a.nRightAttack);sprintf(line[11],PT_REALM_UI_Defence,detail.Defence,detail.Evasion);
    sprintf(line[12],PT_REALM_UI_Speed,a.nAttackSpeed,a.nCastSpeed);sprintf(line[13],PT_REALM_UI_Physical,a.nPhyDef,detail.CriticalPercent);
    sprintf(line[14],PT_REALM_UI_Cold,a.nCoolDef,a.nLightDef);sprintf(line[15],PT_REALM_UI_Earth,a.nEarthDef,a.nFireDef);
    sprintf(line[16],PT_REALM_UI_Stamina,vitals.nStaminaFull,a.nMoveSpeed);
    sprintf(line[17],PT_REALM_UI_Element1,detail.ElementMin[0],detail.ElementMax[0],detail.ElementMin[1],detail.ElementMax[1]);
    sprintf(line[18],PT_REALM_UI_Element2,detail.ElementMin[2],detail.ElementMax[2],detail.ElementMin[3],detail.ElementMax[3]);
    for(int i=0;i<19;++i)m_RealmLines[i].SetText(line[i]);
}
