/*****************************************************************************************
//	界面--屏幕顶控制操作条
//	Copyright : Kingsoft 2003
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2003-4-22
*****************************************************************************************/
#include "KWin32.h"
#include "KIniFile.h"
#include "../elem/wnds.h"
#include "../Elem/WndMessage.h"
#include "../UiBase.h"
#include "UiHeaderControlBar.h"
#include "UiSkills.h"
#include "../../../../Headers/PhongThanRoleUi.h"
#include "../../../../Headers/PhongThanRealmUi.h"
#include "UiChatCentre.h"

#include "../ShortcutKey.h"
#include "../../../core/src/coreshell.h"

extern iCoreShell *g_pCoreShell;
extern iRepresentShell *g_pRepresentShell;

#include "../../../Represent/iRepresent/iRepresentShell.h"
#include <stdio.h>
#include "UiPortraitMask.inl"
#include "../../../../Headers/PhongThanAvatarLayout.h"
#include "../../../../Headers/PhongThanAvatarInfo.h"
#include "../../../../Headers/PhongThanConnectionInfo.h"
#include "../../Login/Login.h"
#include "../../NetConnect/NetConnectAgent.h"

#include "KTabFile.h"
#include "UiPtCompanionHud.inl"

#define    SCHEME_INI            "UiHeaderControlBar.ini"

KUiHeaderControlBar *KUiHeaderControlBar::m_pSelf = NULL;

// Native VNG id of the top strip containing ranking, hide-players,
// interaction, cash-shop and announcement controls.
#define PHONGTHAN_VNG_TOPBAR_INI_ID 1989332095UL

KUiPhongThanTopBar *KUiPhongThanTopBar::m_pSelf = NULL;

KUiPhongThanTopBar *KUiPhongThanTopBar::OpenWindow() {
    if (m_pSelf == NULL) {
        m_pSelf = new KUiPhongThanTopBar;
        if (m_pSelf)
            m_pSelf->Initialize();
    }
    if (m_pSelf)
        m_pSelf->Show();
    return m_pSelf;
}

void KUiPhongThanTopBar::CloseWindow() {
    if (m_pSelf) {
        m_pSelf->Destroy();
        m_pSelf = NULL;
    }
}

void KUiPhongThanTopBar::Initialize() {
    KIniFile Ini;
    if (Ini.LoadPakEntry(PHONGTHAN_VNG_TOPBAR_INI_ID))
        Init(&Ini, "Main");

    m_Style &= ~WND_S_VISIBLE;
    Wnd_AddWindow(this, WL_TOPMOST);
}

//--------------------------------------------------------------------------
//	功能：打开窗口，返回唯一的一个类对象实例
//--------------------------------------------------------------------------
KUiHeaderControlBar *KUiHeaderControlBar::OpenWindow() {
    if (m_pSelf == NULL) {
        m_pSelf = new KUiHeaderControlBar;
        if (m_pSelf)
            m_pSelf->Initialize();
    }
    if (m_pSelf)
        m_pSelf->Show();
    m_pSelf->UpdateData();
    return m_pSelf;
}

//--------------------------------------------------------------------------
//	功能：关闭窗口
//--------------------------------------------------------------------------
extern void PhongThanClosePlayerTarget();
void KUiHeaderControlBar::CloseWindow() {
    PhongThanClosePlayerTarget();
    if (m_pSelf) {
        if(m_pSelf->m_CompanionHud)m_pSelf->m_CompanionHud->Destroy();
        m_pSelf->Destroy();
        m_pSelf = NULL;
    }
}

//初始化
void KUiHeaderControlBar::Initialize() {
    memset(m_RealmSkins,0,sizeof(m_RealmSkins));m_RealmSkin=0;
    AddChild(&m_Portrait);
    AddChild(&m_AntiAddiction);
    AddChild(&m_LevelText);
    AddChild(&m_RankWorldText);
    AddChild(&m_AvatarInfo);memset(m_AvatarInfoText,0,sizeof(m_AvatarInfoText));m_AvatarInfoPoll=0;m_AvatarInfoMode=0;m_AvatarInfoSeconds=8;m_AvatarInfoFont=12;m_AvatarTextX=m_AvatarTextY=0;m_AvatarTextAlign=1;
    char Scheme[256];
    g_UiBase.GetCurSchemePath(Scheme, 256);
    LoadScheme(Scheme);
    m_CompanionHud=new KPtCompanionHud;if(m_CompanionHud)m_CompanionHud->Initialize(Scheme);

    m_Style &= ~WND_S_VISIBLE;
    Wnd_AddWindow(this, WL_TOPMOST);
}

//载入界面方案
void KUiHeaderControlBar::LoadScheme(const char *pScheme) {
    char Buff[128];
    KIniFile Ini;
    if (m_pSelf) {
        sprintf(Buff, "%s\\"SCHEME_INI, pScheme);
        if (Ini.Load(Buff)) {
            m_pSelf->Init(&Ini, "Main");
            Ini.GetString("Main","Image","",m_pSelf->m_RealmSkins[0],256);
            Ini.GetString("Main","Image2",m_pSelf->m_RealmSkins[0],m_pSelf->m_RealmSkins[1],256);
            Ini.GetString("Main","Image3",m_pSelf->m_RealmSkins[0],m_pSelf->m_RealmSkins[2],256);
            m_pSelf->m_RealmSkin=0;
            m_pSelf->m_Portrait.Init(&Ini, "ImgPortrait");
            m_pSelf->m_AntiAddiction.Init(&Ini, "ImgAntiAddiction");
            m_pSelf->m_LevelText.Init(&Ini, "Txt_Level");
            m_pSelf->m_RankWorldText.Init(&Ini, "Txt_WorldSort");
            m_pSelf->m_AvatarInfo.Init(&Ini,"AvatarInfo");m_pSelf->m_AvatarInfo.SetSize(143,14);m_pSelf->m_AvatarInfo.SetCaptionAlign(1);m_pSelf->m_AvatarInfo.SetBorderTextColor(0xff101010);
        }
    }
}

//重新初始化界面
void KUiHeaderControlBar::DefaultScheme(const char *pScheme) {
    char Buff[128];
    KIniFile Ini;
    if (m_pSelf) {
        sprintf(Buff, "%s\\"SCHEME_INI, pScheme);
        if (Ini.Load(Buff)) {
            int nValue1, nValue2;
            Ini.GetInteger("Main", "Left", 0, &nValue1);
            Ini.GetInteger("Main", "Top", 0, &nValue2);
            m_pSelf->SetPosition(nValue1, nValue2);
        }
    }
}

void KUiHeaderControlBar::Breathe() {
    if(g_pCoreShell){
        PTRoleValues role;memset(&role,0,sizeof(role));
        if(g_pCoreShell->GetGameData(PT_GDI_ROLE_VALUES,(unsigned int)&role,sizeof(role))){
            const int skin=PhongThanRealmUnlocked(role.RealmStage,role.RealmSide,role.RealmLevel)?(role.EquipmentRealm?2:1):0;
            if(skin!=m_RealmSkin){if(m_RealmSkins[skin][0])SetImage(ISI_T_SPR,m_RealmSkins[skin],true);m_RealmSkin=skin;}
            for(int bar=0;bar<m_nButtonNum;++bar){KButtonInfo& item=m_pButtons[bar];const char* section=item.szTitle;int x=0,y=0;item.m_pButton->GetPosition(&x,&y);
                if(!strcmp(section,"Life")||!strcmp(section,"Mana")||!strcmp(section,"Exp")||!strcmp(section,"JEExp")||!strcmp(section,"NewExp")){
                    if(skin==0){x=!strcmp(section,"Life")?68:!strcmp(section,"Exp")?69:70;y=!strcmp(section,"Life")?22:!strcmp(section,"Mana")?32:!strcmp(section,"JEExp")?52:42;}
                    PTAvatarBarLayout layout=PhongThanAvatarBarLayout(skin,section,x,y);item.m_pButton->SetPosition(layout.X,layout.Y);
                }
            }
        }
        KUiSkills::RefreshRealmState();
    }
    // Original realm frame apertures are shifted 11/9 px right and 4 px down.
    m_Portrait.SetPosition(m_RealmSkin==1?11:m_RealmSkin==2?9:0,m_RealmSkin?7:3);
    UpdateData();
    UpdateAvatarInfo();
    if(m_CompanionHud)m_CompanionHud->Update();
    if (g_pCoreShell) {
        int nVisual = g_pCoreShell->GetGameData(GDI_IS_CHECK_IMAGE, 0, 0);
        int nFaceVariant = nVisual > 0 ? (nVisual - 1) % 3 : 0;
        int nPortrait = nFaceVariant * 2 +
                        (g_pCoreShell->GetGameData(GDI_PLAYER_IS_MALE, 0, 0) ? 1 : 2);
        char szPortrait[128];
        sprintf(szPortrait, "\\Spr\\Ui4\\\xD0\xA4\xCF\xF1\\%03d.spr", nPortrait);
        // Preserve the 72x72 portrait slot and mask authored by UiHeaderControlBar.ini.
        if (strcmp(m_Portrait.GetImage(), szPortrait) != 0)
            m_Portrait.SetImage(ISI_T_SPR, szPortrait, false);
    }
}


// Independent bottom caption. It never changes the stat bars or portrait.
void KUiHeaderControlBar::UpdateAvatarInfo(){
 unsigned int now=GetTickCount();
 if(!m_AvatarInfoPoll||now-m_AvatarInfoPoll>=2000){
  m_AvatarInfoPoll=now;KIniFile info;
  if(info.Load("\\settings\\phongthan\\AvatarInfo.ini")){
   info.GetInteger("Main","Mode",1,&m_AvatarInfoMode);info.GetInteger("Main","RotateSeconds",8,&m_AvatarInfoSeconds);info.GetInteger("Main","Font",12,&m_AvatarInfoFont);
   if(m_AvatarInfoFont<12||m_AvatarInfoFont>14)m_AvatarInfoFont=12;
   info.GetInteger("Main","OffsetX",0,&m_AvatarTextX);info.GetInteger("Main","OffsetY",0,&m_AvatarTextY);info.GetInteger("Main","Align",1,&m_AvatarTextAlign);
   if(m_AvatarTextX < -40)m_AvatarTextX=-40;if(m_AvatarTextX>40)m_AvatarTextX=40;if(m_AvatarTextY < -12)m_AvatarTextY=-12;if(m_AvatarTextY>12)m_AvatarTextY=12;
   m_AvatarInfo.SetCaptionAlign(m_AvatarTextAlign);
   const char* keys[]={"Website","CustomText"};
   for(int i=0;i<2;++i)info.GetString("Main",keys[i],"",m_AvatarInfoText[i+2],80);
   int red=230,green=212,blue=150;info.GetInteger("Main","Red",230,&red);info.GetInteger("Main","Green",212,&green);info.GetInteger("Main","Blue",150,&blue);
   if(red<0||red>255)red=230;if(green<0||green>255)green=212;if(blue<0||blue>255)blue=150;
   m_AvatarInfo.SetTextColor(0xff000000|(red<<16)|(green<<8)|blue);m_AvatarInfo.SetFontSize(m_AvatarInfoFont);
  }else m_AvatarInfoMode=0;
 }
 char region[32]={0},server[32]={0};g_LoginLogic.GetRegionServer(region,server);lstrcpyn(m_AvatarInfoText[0],server,80);
 PT_CONNECTION_INFO connection;memset(&connection,0,sizeof(connection));bool connected=g_NetConnectAgent.IsConnecting(1)!=0;
 if(g_pCoreShell)g_pCoreShell->GetGameData(PT_CONNECTION_INFO_QUERY,(unsigned int)&connection,sizeof(connection));
 const char* status[]={"\x4D\xB9\x6E\x67\x3A\x20\x4D\xCA\x74\x20\x6B\xD5\x74\x20\x6E\xE8\x69","\x4D\xB9\x6E\x67\x3A\x20\x43\x68\xEA\x20\x6D\xC9\x75\x20\x6D\xED\x69","\x54\xE8\x74","\x56\xF5\x61","\x4B\xD0\x6D"};int quality=PhongThanConnectionQuality(connected,connection);
 if(quality>=2)sprintf(m_AvatarInfoText[1],"\x4D\xB9\x6E\x67\x3A\x20\x25\x73\x20\x28\x25\x6C\x75\x20\x6D\x73\x29",status[quality],connection.RoundTripMs);else lstrcpyn(m_AvatarInfoText[1],status[quality],80);
 for(int t=0;t<4;++t)for(char* c=m_AvatarInfoText[t];*c;++c)if((unsigned char)*c<32||*c=='<'||*c=='>')*c=' ';
 int selected=PhongThanAvatarInfoSelect(m_AvatarInfoMode,now,m_AvatarInfoSeconds,m_AvatarInfoText);
 if(selected<0){m_AvatarInfo.Hide();return;}
 char display[80];PhongThanAvatarInfoClip(m_AvatarInfoText[selected],display,sizeof(display),m_AvatarInfoFont);
 m_AvatarInfo.SetPosition((m_RealmSkin?80:70)+m_AvatarTextX,PhongThanAvatarInfoY(m_RealmSkin)+m_AvatarTextY);m_AvatarInfo.SetText(display);
 char networkTip[256];char* tip=m_AvatarInfoText[selected];
 if(selected==1&&connection.HasSample){sprintf(networkTip,"\x50\x68\xB6\x6E\x20\x68\xE5\x69\x20\x6C\xC7\x6E\x20\xAE\x6F\x20\x74\x72\xAD\xED\x63\x3A\x20\x25\x6C\x75\x20\x6D\x73\x3B\x20\x63\xB8\x63\x68\x20\xAE\xA9\x79\x20\x25\x6C\x75\x20\x67\x69\xA9\x79\x2E\x20\x4D\xC9\x75\x20\x63\xF2\x20\xAE\x61\x6E\x67\x20\x63\x68\xEA\x20\x63\xCB\x70\x20\x6E\x68\xCB\x74\x2C\x20\x6B\x68\xAB\x6E\x67\x20\x64\xEF\x6E\x67\x20\xAE\xD3\x20\x6B\xD5\x74\x20\x6C\x75\xCB\x6E\x20\x6D\xB9\x6E\x67\x20\x6B\xD0\x6D\x2E",connection.RoundTripMs,connection.SampleAgeMs/1000);tip=networkTip;}m_AvatarInfo.SetToolTipInfo(tip,strlen(tip));
 if(!m_AvatarInfo.IsVisible())m_AvatarInfo.Show();
}
