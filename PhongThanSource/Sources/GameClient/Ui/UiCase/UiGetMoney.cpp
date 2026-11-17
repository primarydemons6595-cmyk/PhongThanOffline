// -------------------------------------------------------------------------
//	文件名		：	UiGetMoney.cpp
//	创建者		：	Wooy(Wu yue)
//	创建时间	：	2003-1-7
//	功能描述	：	取钱界面
// -------------------------------------------------------------------------
#include "KWin32.h"
#include "KIniFile.h"
#include "../Elem/AutoLocateWnd.h"
#include "../Elem/WndMessage.h"
#include "../Elem/Wnds.h"
#include "../UiBase.h"
#include "UiGetMoney.h"
#include "../UiSoundSetting.h"
#include "../../../Engine/src/KDebug.h"
#include "../../../Represent/iRepresent/iRepresentShell.h"
extern iRepresentShell* g_pRepresentShell;

#define    SCHEME_INI    "UiGetMoney.ini"


KUiGetMoney *KUiGetMoney::m_pSelf = NULL;

KUiGetMoney::KUiGetMoney() {
    m_pRequester = NULL;
    m_nMaxMoney = 0;
    m_CheckingInput = false;
}

//--------------------------------------------------------------------------
//	功能：打开窗口，返回唯一的一个类对象实例
//--------------------------------------------------------------------------
KUiGetMoney *KUiGetMoney::OpenWindow(int nMoney, int nMaxMoney,
                                     KWndWindow *pRequester, unsigned int uParam, KWndWindow *pMoneyWnd, const char* title) {
    if (m_pSelf == NULL) {
        m_pSelf = new KUiGetMoney;
        if (m_pSelf)
            m_pSelf->Initialize();
    }
    if (m_pSelf) {
        UiSoundPlay(UI_SI_WND_OPENCLOSE);
        m_pSelf->m_pRequester = pRequester;
        m_pSelf->m_uRequesterParam = uParam;
        if (nMaxMoney > 0)
            m_pSelf->m_nMaxMoney = nMaxMoney;
        else
            m_pSelf->m_nMaxMoney = 0;
        // A missing scheme must never leave a zero-length numeric editor.
        m_pSelf->m_Title.SetText(title?title:"\116\150\313\160\040\163\350\040\164\151\322\156");
        char maximum[128];sprintf(maximum,"\124\350\151\040\256\141\072\040\045\144\040\154\255\356\156\147",m_pSelf->m_nMaxMoney);m_pSelf->m_Max.SetText(maximum);
        m_pSelf->m_Money.SetLimitText(10);
        m_pSelf->m_Money.SetIntText(nMoney < 0 ? 0 : nMoney);
        m_pSelf->Show(pMoneyWnd);
        m_pSelf->BringToTop();
    }
    return m_pSelf;
}

//--------------------------------------------------------------------------
//	功能：关闭销毁窗口
//--------------------------------------------------------------------------
void KUiGetMoney::CloseWindow(bool bDestroy) {
    if (m_pSelf) {
        m_pSelf->m_pRequester = NULL;
        m_pSelf->Hide();
        if (bDestroy) {
            m_pSelf->Destroy();
            m_pSelf = NULL;
        } else
            m_pSelf->Hide();
    }
}

KUiGetMoney *KUiGetMoney::GetIfVisible() {
    if (m_pSelf && m_pSelf->IsVisible())
        return m_pSelf;
    return NULL;
}

//显示窗口
void KUiGetMoney::Show(KWndWindow *pMoneyWnd) {
    int Left, Top;
    if (pMoneyWnd) {
        pMoneyWnd->GetAbsolutePos(&Left, &Top);
        int nWidth;
        pMoneyWnd->GetSize(&nWidth, NULL);
        Left += nWidth / 2;
        ALW_GetWndPosition(Left, Top, m_Width, m_Height, true);
    } else
        ALW_GetWndPosition(Left, Top, m_Width, m_Height);
    SetPosition(Left, Top);
    KWndImage::Show();
    Wnd_SetFocusWnd(&m_Money);
    Wnd_SetExclusive((KWndWindow *) this);
}

//隐藏窗口
void KUiGetMoney::Hide() {
    Wnd_ReleaseExclusive((KWndWindow *) this);
    KWndImage::Hide();
}

//--------------------------------------------------------------------------
//	功能：初始化
//--------------------------------------------------------------------------
int KUiGetMoney::Initialize() {
    AddChild(&m_Title);AddChild(&m_Label);AddChild(&m_Max);
    AddChild(&m_Money);
    AddChild(&m_OkBtn);
    AddChild(&m_CancelBtn);
    Wnd_AddWindow(this);

    char Scheme[256];
    g_UiBase.GetCurSchemePath(Scheme, 256);
    LoadScheme(Scheme);

    return true;
}

//--------------------------------------------------------------------------
//	功能：载入窗口的界面方案 ////////////////
//--------------------------------------------------------------------------
void KUiGetMoney::LoadScheme(const char *pScheme) {
    if (m_pSelf) {
        char Buff[128];
        KIniFile Ini;
        sprintf(Buff, "%s\\%s", pScheme, SCHEME_INI);
        if (Ini.Load(Buff)) {
            m_pSelf->Init(&Ini, "Main");
            m_pSelf->m_Title.Init(&Ini,"Title");m_pSelf->m_Label.Init(&Ini,"Label");m_pSelf->m_Max.Init(&Ini,"Maximum");
            m_pSelf->m_Money.Init(&Ini, "MoneyInput");
            m_pSelf->m_OkBtn.Init(&Ini, "OkBtn");
            m_pSelf->m_CancelBtn.Init(&Ini, "CancelBtn");
            m_pSelf->m_OkBtn.SetLabel("\130\270\143\040\156\150\313\156");m_pSelf->m_CancelBtn.SetLabel("\110\361\171\040\142\341");
        }
    }
}

//--------------------------------------------------------------------------
//	功能：窗口消息函数
//--------------------------------------------------------------------------
int KUiGetMoney::WndProc(unsigned int uMsg, unsigned int uParam, int nParam) {
    int nRet = 0;
    switch (uMsg) {
        case WND_N_BUTTON_CLICK:
            if (uParam == (unsigned int) (KWndWindow *) &m_OkBtn)
                OnOk();
            else if (uParam == (unsigned int) (KWndWindow *) &m_CancelBtn)
                OnCancel();
            break;
        case WM_KEYDOWN:
            if (uParam == VK_RETURN) {
                OnOk();
                nRet = 1;
            } else if (uParam == VK_ESCAPE) {
                OnCancel();
                nRet = 1;
            }
            break;
        case WND_N_EDIT_SPECIAL_KEY_DOWN:
            if (nParam == VK_RETURN &&
                uParam == (unsigned int) (KWndWindow *) &m_Money) {
                OnOk();
                nRet = 1;
            }
            break;
        case WND_N_EDIT_CHANGE:
            OnCheckInput();
            break;
        default:
            nRet = KWndImage::WndProc(uMsg, uParam, nParam);
    }
    return nRet;
}

//--------------------------------------------------------------------------
//	功能：响应点击确认按钮
//--------------------------------------------------------------------------
void KUiGetMoney::OnOk() {
    OnCheckInput();
    DWORD nMoney = m_Money.GetIntNumber();
    if (m_pRequester)
        m_pRequester->WndProc(WND_M_OTHER_WORK_RESULT, m_uRequesterParam, WND_OPER_RESULT(nMoney));
    CloseWindow(true);
}

void KUiGetMoney::OnCheckInput() {
    // SetIntText synchronously emits WND_N_EDIT_CHANGE. Guard normalization.
    if (m_CheckingInput) return;
    m_CheckingInput = true;
    DWORD nMoney = m_Money.GetIntNumber();

    if (nMoney > m_nMaxMoney)
        nMoney = m_nMaxMoney;
    char szBuff1[16], szBuff2[16];
    itoa(nMoney, szBuff1, 10);
    m_Money.GetText(szBuff2, sizeof(szBuff2), true);
    if (strcmp(szBuff1, szBuff2))
        m_Money.SetIntText(nMoney);
    m_CheckingInput = false;
}

void KUiGetMoney::OnCancel() {
    if (m_pRequester)
        m_pRequester->WndProc(WND_M_OTHER_WORK_RESULT, m_uRequesterParam, WND_OPER_RESULT(-1));
    CloseWindow(true);
}

void KUiGetMoney::PaintWindow(){int x,y;GetAbsolutePos(&x,&y);if(g_pRepresentShell){KRUShadow bg;memset(&bg,0,sizeof(bg));bg.Color.Color_b.a=16;bg.oPosition.nX=x+6;bg.oPosition.nY=y+20;bg.oEndPos.nX=x+294;bg.oEndPos.nY=y+136;g_pRepresentShell->DrawPrimitives(1,&bg,RU_T_SHADOW,true);}KWndImage::PaintWindow();}
