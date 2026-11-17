#include "../../../Headers/PhongThanPkContract.h"
Player_PK::Player_PK() : m_nPtCamp(-1) { m_szPtBaseImage[0]=0; }
int Player_PK::Init(KIniFile* ini, const char* section)
{
    int result = KWndButton::Init(ini,section);
    if (!result) return result;
    ini->GetString(section,"Image","",m_szPtBaseImage,sizeof(m_szPtBaseImage));
    for (int i=0;i<6;++i) {
        char key[32]; sprintf(key,"ImgFile_%d",i);
        ini->GetString(section,key,"",m_szPtImages[i],sizeof(m_szPtImages[i]));
    }
    m_nPtCamp=-1;
    return result;
}
void Player_PK::UpdateData()
{
    if (!g_pCoreShell) return;
    int camp=g_pCoreShell->GetGameData(GDI_PK_SETTING,0,0);
    if (camp==m_nPtCamp) return;
    int row=PtPkMenuRow(camp);
    const char* image=m_szPtBaseImage; // ImgFile_N are menu swatches, not toolbar faces.
    if (image[0]) { SetImage(ISI_T_SPR,image); SetFrame(0); }
    m_nPtCamp=camp;
}
