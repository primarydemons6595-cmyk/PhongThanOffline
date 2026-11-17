#include "../../../../Headers/PhongThanRoleUi.h"
void KUiStatus::InitPtDetails(KIniFile* ini,int x,int y)
{
    const char* sections[4]={"StatusPage_LightingHurt","StatusPage_EarthHurt","StatusPage_IceHurt","StatusPage_FireHurt"};
    for(int i=0;i<4;++i) {
        m_PtElement[i].Init(ini,sections[i]);
        OffsetStatusPageControl(&m_PtElement[i],x,y);
    }
    m_PtCritical.Init(ini,"StatusPage_HuiXin");
    OffsetStatusPageControl(&m_PtCritical,x,y);
    m_PtEvasion.Init(ini,"StatusPage_Evade");
    OffsetStatusPageControl(&m_PtEvasion,x,y);
    // Native detail frame is a sibling of F3, aligned at the root top edge.
    const int left=270, top=0;
    const char* tabs[3]={"b_AttackProperty","b_DefenceProperty","b_BaseProperty"};
    const char* bgs[3]={"img_background1","img_background2","img_background3"};
    for(i=0;i<3;++i){
        m_PtTabs[i].Init(ini,tabs[i]); m_PtTabs[i].SetPosition(left+30+i*55,top+36);
        m_PtTabs[i].Enable(true); m_PtTabs[i].CheckButton(i==0); m_PtTabs[i].Hide();
        m_PtTabBg[i].Init(ini,bgs[i]); m_PtTabBg[i].SetPosition(left+35,top+74); m_PtTabBg[i].Hide();
    }
    m_OpenExtAttrPage.Init(ini,"BtnOpenExtAttrPage");
    OffsetStatusPageControl(&m_OpenExtAttrPage,x,y);
    m_PtDetailMode=false;
    m_PtDetailBg.Init(ini,"MainProperty"); m_PtDetailBg.SetPosition(left,top); m_PtDetailBg.Hide();
    m_PtDetailTitle.Init(ini,"PtDetailTitle"); m_PtDetailTitle.Hide();
    m_PtDetailClose.Init(ini,"CloseBtn"); m_PtDetailClose.SetPosition(left+264,top+4); m_PtDetailClose.Hide();
    const char* detailSections[3][14]={
        {"m_ImproveBasicDamage","m_HitValue","m_ImproveHurtAction","m_ImproveFreezeTime","m_IgnoreDefense","m_IgnorePhysicalResistance","m_DeadlyStrikeEnhance","m_DeadlyStrikeDamagePercent","m_MagicDeadlyEnhance","m_MagicDeadlyDamagePercent","m_IgnoreLightResist","m_IgnoreColdResist","m_IgnoreEarthResist","m_IgnoreFireResist"},
        {"m_DropBasicDamage","m_DropHitValue","m_DropHurtAction","m_DropFreezeTime","m_PhysicalAbsorbDamage","m_ManaAbsorbDamage","m_DecDeadlyStrikeEnhance","m_DecDeadlyStrikeDamageold","m_DecMagicDeadlyEnhance","m_DecMagicDeadlyDamagePercent","m_AbsorbLightResist","m_AbsorbColdResist","m_AbsorbEarthResist","m_AbsorbFireResist"},
        {"m_SpeedUp","m_LifeRecover","m_ManaRecover","m_DropHurtToPlayer","m_DropHurtToNPC","m_DamageToMana",0,0,0,0,0,0,0,0}
    };
    for(int page=0;page<3;++page)for(int row=0;row<14;++row){
        const char* section=detailSections[page][row];
        m_PtDetailX[page][row]=190;m_PtDetailY[page][row]=88+row*13;m_PtDetailWidth[page][row]=77;
        if(section){ini->GetInteger(section,"Left",190,&m_PtDetailX[page][row]);ini->GetInteger(section,"Top",88+row*13,&m_PtDetailY[page][row]);ini->GetInteger(section,"Width",77,&m_PtDetailWidth[page][row]);}
    }
    for(i=0;i<14;++i){m_PtDetailLine[i].Init(ini,detailSections[0][i]);m_PtDetailLine[i].SetText("0");m_PtDetailLine[i].Hide();}

}
void KUiStatus::UpdatePtDetails()
{
    if(m_StatusPageMode!=0)return;
    PTStatusDetail detail;memset(&detail,0,sizeof(detail));
    if(!g_pCoreShell || !g_pCoreShell->GetGameData(PT_GDI_STATUS_DETAIL,(unsigned int)&detail,sizeof(detail))) return;
    PTRoleValues role; memset(&role,0,sizeof(role));
    if(g_pCoreShell->GetGameData(PT_GDI_ROLE_VALUES,(unsigned int)&role,sizeof(role))) {
        m_PtExploit.SetIntText(role.Exploit);m_PtExploit.BringToTop();
        m_Repute.SetIntText(role.Credit);m_Repute.BringToTop();
    }
    m_StatusDesc.SetText(detail.Profession);
    m_PtEvasion.SetIntText(detail.Evasion);
    m_PtEvasion.BringToTop();
    m_Defence.SetIntText(detail.Defence);
    m_Defence.BringToTop();
    for(int i=0;i<4;++i) {
        m_PtElement[i].Set2IntText(detail.ElementMin[i],detail.ElementMax[i],'-');
        m_PtElement[i].BringToTop();
    }
    m_PtCritical.Set6IntText(detail.CriticalPercent,'%');
    m_PtCritical.BringToTop();
    m_OpenExtAttrPage.BringToTop();
    if(!m_PtDetailMode)return; // Closed extended panel needs no extra attribute query/layout.
    PTStatusOriginal original;memset(&original,0,sizeof(original));
    if(!g_pCoreShell->GetGameData(PT_GDI_STATUS_ORIGINAL,(unsigned int)&original,sizeof(original))) return;
    // Values, sums and units match original game.dat:0x607040 exactly.
    // Layout comes from the native per-control INI sections.
    // Page sprites contain labels already. Overlay only values in their slots.
    char line[14][80];
    int count=m_PtTab==2?6:14;
    for(int row=0;row<count;++row){
        int value=PhongThanStatusOriginalRow(original,m_PtTab,row);
        if(!PhongThanStatusOriginalAvailable(m_PtTab,row))strcpy(line[row],"--");
        else sprintf(line[row],PhongThanStatusOriginalIsPercent(m_PtTab,row)?"%d%%":"%d",value);
    }
    if(m_PtDetailMode){
        m_PtDetailBg.Show();m_PtDetailBg.BringToTop();
        for(int tab=0;tab<3;++tab)m_PtTabBg[tab].Hide();
        m_PtTabBg[m_PtTab].Show();m_PtTabBg[m_PtTab].BringToTop();
        for(tab=0;tab<3;++tab){m_PtTabs[tab].Show();m_PtTabs[tab].BringToTop();}
        m_PtDetailClose.Show();m_PtDetailClose.BringToTop();
    }
    for(int j=0;j<14;++j){
        m_PtDetailLine[j].Hide();
        if(m_PtDetailMode && j<count){
            m_PtDetailLine[j].SetPosition(270+m_PtDetailX[m_PtTab][j],m_PtDetailY[m_PtTab][j]);
            m_PtDetailLine[j].SetSize(m_PtDetailWidth[m_PtTab][j],14);
            m_PtDetailLine[j].SetText(line[j]);m_PtDetailLine[j].Show();m_PtDetailLine[j].BringToTop();
        }
    }
}
