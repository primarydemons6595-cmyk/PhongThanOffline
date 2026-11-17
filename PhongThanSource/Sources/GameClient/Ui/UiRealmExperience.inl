IMPLEMENT_COMCLASS(Player_NewExp)
IMPLEMENT_COMCLASS(Player_JEExp)
void Player_JEExp::UpdateData()
{
    PTRoleValues role;memset(&role,0,sizeof(role));
    if(!g_pCoreShell||!g_pCoreShell->GetGameData(PT_GDI_ROLE_VALUES,(unsigned int)&role,sizeof(role))||
        !PhongThanRealmUnlocked(role.RealmStage,role.RealmSide,role.RealmLevel)){Hide();return;}
    Show();
    int percent=role.RealmExperienceNext>0?(int)((__int64)role.RealmExperience*100/role.RealmExperienceNext):100;
    if(percent<0)percent=0;if(percent>100)percent=100;
    Set2IntValue(percent,100);
    char text[64];sprintf(text,"%s %d - %d%%",role.RealmSide==1?"Ti\xaa\x6e":"Ma",role.RealmLevel,percent);
    if(Player_Exp::m_bText)SetText(text,strlen(text));else SetText(0,0);
}
void Player_JEExp::OnButtonClick(){Player_Exp::m_bText=!Player_Exp::m_bText;UpdateData();}
int Player_JEExp::GetToolTipInfo(char* text,int size)
{
    PTRoleValues role;memset(&role,0,sizeof(role));
    if(!text||size<1||!g_pCoreShell||!g_pCoreShell->GetGameData(PT_GDI_ROLE_VALUES,(unsigned int)&role,sizeof(role))||
        !PhongThanRealmUnlocked(role.RealmStage,role.RealmSide,role.RealmLevel))return 0;
    char buffer[160];sprintf(buffer,"%s %d: %d/%d",role.RealmSide==1?"Ti\xaa\x6e":"Ma",role.RealmLevel,role.RealmExperience,role.RealmExperienceNext);
    int length=strlen(buffer);if(length>=size)return 0;memcpy(text,buffer,length);return length;
}
