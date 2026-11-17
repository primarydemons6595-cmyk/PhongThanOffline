// -------------------------------------------------------------------------
//	文件名		：	UiESCDlg.cpp
//	创建者		：	彭建波
//	创建时间	：	2002-9-16 10:32:27
//	功能描述	：
//
// -------------------------------------------------------------------------
#include "KWin32.h"
#include "KIniFile.h"
#include "../Elem/WndMessage.h"
#include "../Elem/Wnds.h"
#include "UiESCDlg.h"
#include "UiSelPlayer.h"
#include "UiInit.h"
#include "UiOptions.h"
#include "UiItem.h"
#include "UiSkills.h"
#include "UiStatus.h"
#include "UiConnectInfo.h"
#include "../UiShell.h"
#include "../../../core/src/coreshell.h"
#include "../UiBase.h"
#include "../UiSoundSetting.h"
#include "UiHelper2.h"
#include "UiTaskNote.h"
#include "PhongThanNativeUiLayout.h"
#include "../Elem/PopupMenu.h"
#include "Text.h"
#include "../../../Represent/iRepresent/iRepresentShell.h"
#include "../../../Represent/iRepresent/KRepresentUnit.h"
extern iRepresentShell *g_pRepresentShell;
#include "PhongThanExitDialog.inl"

extern iCoreShell *g_pCoreShell;

#define    SCHEME_INI_INIT    "UiESCDlg.ini"


KUiESCDlg *KUiESCDlg::m_pSelf = NULL;

//--------------------------------------------------------------------------
//	功能：打开窗口，返回唯一的一个类对象实例
//--------------------------------------------------------------------------
KUiESCDlg *KUiESCDlg::OpenWindow() {
    if (m_pSelf == NULL) {
        m_pSelf = new KUiESCDlg;
        if (m_pSelf)
            m_pSelf->Initialize();
    }
    if (m_pSelf) {
        UiSoundPlay(UI_SI_WND_OPENCLOSE);
        m_pSelf->Show();
        m_pSelf->BringToTop();
    }
    return m_pSelf;
}

void KUiESCDlg::OpenExitWindow() { KPhongThanExitDialog::Open(); }

KUiESCDlg::~KUiESCDlg() {
    Hide();
}

//--------------------------------------------------------------------------
//	功能：关闭销毁窗口
//--------------------------------------------------------------------------
void KUiESCDlg::CloseWindow(bool bDestroy) {
    if (m_pSelf) {
        if (bDestroy) {
            m_pSelf->Destroy();
            m_pSelf = NULL;
        } else
            m_pSelf->Hide();
    }
}

KUiESCDlg *KUiESCDlg::GetIfVisible() {
    if (m_pSelf && m_pSelf->IsVisible())
        return m_pSelf;
    return NULL;
}

//显示窗口
void KUiESCDlg::Show() {
    KWndShowAnimate::Show();
    Wnd_SetExclusive((KWndWindow *) this);
}

//隐藏窗口
void KUiESCDlg::Hide() {
    Wnd_ReleaseExclusive((KWndWindow *) this);
    KWndShowAnimate::Hide();
}

//--------------------------------------------------------------------------
//	功能：初始化
//--------------------------------------------------------------------------
int KUiESCDlg::Initialize() {
    AddChild(&m_ESCBg);
    AddChild(&m_ContinueGameBtn);
    AddChild(&m_ExitGameBtn);
    AddChild(&m_SelectPlayerBtn);
    AddChild(&m_OptionsBtn);
    //AddChild(&m_ExitBtn);

    //AddChild(&m_TaskBtn);
    Wnd_AddWindow(this, WL_TOPMOST);

    char Scheme[256];
    g_UiBase.GetCurSchemePath(Scheme, 256);
    LoadScheme(Scheme);

    return true;
}

//--------------------------------------------------------------------------
//	功能：载入窗口的界面方案
//--------------------------------------------------------------------------
void KUiESCDlg::LoadScheme(const char *pScheme) {
    char Buff[128];
    KIniFile Ini;
    sprintf(Buff, "%s\\%s", pScheme, SCHEME_INI_INIT);
    if (Ini.Load(Buff)) {
        int width,height;Wnd_GetScreenSize(width,height);
        Ini.WriteInteger("Main","Left",(width-296)/2);
        Ini.WriteInteger("Main","Top",(height-156)/2);
        Ini.WriteInteger("Main","Width",296);Ini.WriteInteger("Main","Height",156);
        // Explicitly centre actual 240px buttons inside the compact frame.
        const char* rows[]={"ContiumeGame","Options","GameToSelPlayer","ExitGame"};
        for(int row=0;row<4;++row){Ini.WriteInteger(rows[row],"Left",28);Ini.WriteInteger(rows[row],"Top",20+row*27);}
        Init(&Ini, "Main");
        m_ContinueGameBtn.Init(&Ini, "ContiumeGame");
        m_OptionsBtn.Init(&Ini, "Options");
        m_SelectPlayerBtn.Init(&Ini, "GameToSelPlayer");
        //m_ExitBtn.Init(&Ini, "CloseGame");
        m_ExitGameBtn.Init(&Ini, "ExitGame");

        //m_TaskBtn.Init(&Ini, "GameTask");
        m_ESCBg.Hide();
    }
}

//--------------------------------------------------------------------------
//	功能：窗口消息函数
//--------------------------------------------------------------------------
int KUiESCDlg::WndProc(unsigned int uMsg, unsigned int uParam, int nParam) {
    int nRet = 0;
    switch (uMsg) {
        case WND_N_BUTTON_CLICK:
            OnClickButton((KWndWindow *) uParam);
            break;
        case WM_KEYDOWN:
            nRet = OnKeyDown(uParam);
            break;
        default:
            nRet = KWndShowAnimate::WndProc(uMsg, uParam, nParam);
    }
    return nRet;
}

int KUiESCDlg::OnKeyDown(unsigned int uKey) {
    int nRet = 1;
    KWndButton *pActive = NULL;
    KWndButton *pToActive = NULL;
    if (uKey == VK_RETURN) {
        if (pActive = GetActiveBtn())
            OnClickButton(pActive);
    } else if (uKey == VK_ESCAPE)
        CloseWindow(false);
    else if (uKey == VK_UP || uKey == VK_DOWN) {
        KWndButton *buttons[] = { &m_ContinueGameBtn, &m_OptionsBtn,
                                  &m_SelectPlayerBtn, &m_ExitGameBtn };
        pActive = GetActiveBtn();
        int index = 0;
        for (int i = 0; i < 4; ++i)
            if (buttons[i] == pActive) index = i;
        pToActive = buttons[(index + (uKey == VK_DOWN ? 1 : 3)) % 4];
    } else
        nRet = 0;
    if (pToActive) {
        POINT Pos;
        SIZE Size;
        pToActive->GetAbsolutePos((int *) &Pos.x, (int *) &Pos.y);
        pToActive->GetSize((int *) &Size.cx, (int *) &Size.cy);
        Pos.x += Size.cx / 2;
        Pos.y += Size.cy / 2;
        Wnd_SetCursorPos(Pos.x, Pos.y);
    }
    return nRet;
}

//--------------------------------------------------------------------------
//	功能：响应点击按钮
//--------------------------------------------------------------------------
void KUiESCDlg::OnClickButton(KWndWindow *pWnd) {
    if (pWnd == (KWndWindow *) &m_ContinueGameBtn)
        Hide();
    else if (pWnd == (KWndWindow *) &m_OptionsBtn) {
        Hide();
        KUiOptions::OpenWindow();
    } else if (pWnd == (KWndWindow *) &m_ExitGameBtn) {
        Hide();
        OpenExitWindow();
    } else if (pWnd == (KWndWindow *) &m_SelectPlayerBtn) {
        Hide();
        g_pCoreShell->OperationRequest(GOI_EXIT_GAME, 0, 0);
        // UiEndGame destroys this dialog; do not access members afterwards.
        UiEndGame();
        if (g_LoginLogic.ReturnToCharacterSelection())
            KUiConnectInfo::OpenWindow(CI_MI_GETTING_ROLE_DATA, LL_S_ROLE_LIST_READY);
        else
            KUiConnectInfo::OpenWindow(CI_MI_CONNECT_FAILED, CI_NS_INIT_WND);
    }
}

KWndButton *KUiESCDlg::GetActiveBtn() {
    KWndButton *pBtn = NULL;
    if (m_ContinueGameBtn.IsButtonActive())
        pBtn = &m_ContinueGameBtn;
    else if (m_OptionsBtn.IsButtonActive())
        pBtn = &m_OptionsBtn;
        //else if (m_ExitBtn.IsButtonActive())
        //	pBtn = &m_ExitBtn;
    else if (m_SelectPlayerBtn.IsButtonActive())
        pBtn = &m_SelectPlayerBtn;
        //else if (m_TaskBtn.IsButtonActive())
        //	pBtn = &m_TaskBtn;
    else if (m_ExitGameBtn.IsButtonActive())
        pBtn = &m_ExitGameBtn;

    return pBtn;
}

void KUiESCDlg::PaintWindow(){
    KWndWindow::PaintWindow();
    if(!g_pRepresentShell||!m_Image.szImage[0])return;
    KRUImage image;memset(&image,0,sizeof(image));memcpy(&image,&m_Image,sizeof(KRUImage));
    image.oPosition.nX=m_nAbsoluteLeft;image.oPosition.nY=m_nAbsoluteTop;
    image.oEndPos.nX=m_nAbsoluteLeft+296;image.oEndPos.nY=m_nAbsoluteTop+156;
    g_pRepresentShell->DrawPrimitives(1,&image,RU_T_IMAGE_STRETCH,true);
}

void KUiESCDlg::OfflineResult(int result,unsigned int sequence){KPhongThanExitDialog::Result(result,sequence);}
