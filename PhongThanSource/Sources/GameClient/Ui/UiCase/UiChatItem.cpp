#include "KWin32.h"
#include "KIniFile.h"
#include "KTabFile.h"
#include "KStrBase.h"
#include "../Elem/Wnds.h"
#include "../Elem/WndMessage.h"
#include "../Elem/MouseHover.h"
#include "UiChatItem.h"
#include "../UiBase.h"
#include "../UiSoundSetting.h"
#include "../../../Represent/iRepresent/iRepresentShell.h"
#include "../../../Represent/iRepresent/KRepresentUnit.h"
#include "../../../Engine/src/Text.h"
#include "../../../Engine/src/KDebug.h"
#include "../../../Engine/src/KRandom.h"
#include "../../../core/src/coreshell.h"
#include <crtdbg.h>

extern iRepresentShell *g_pRepresentShell;
extern iCoreShell *g_pCoreShell;
#define        SCHEME_INI_CHATITEM            "UiChatItemInfo.ini"

KUiChatItem *KUiChatItem::m_pSelf = NULL;

KUiChatItem *KUiChatItem::OpenWindow(int uId,int x,int y) {
    if (m_pSelf == NULL) {
        m_pSelf = new KUiChatItem;
        if (m_pSelf)
            m_pSelf->Initialize();
    }

    if (m_pSelf) {
        if(x<0||y<0)Wnd_GetCursorPos(&x,&y);
        m_pSelf->m_ClickX=x;m_pSelf->m_ClickY=y;
        g_MouseOver.CancelMouseHoverInfo();
        m_pSelf->Updatedata(uId);
        m_pSelf->Show();
        m_pSelf->BringToTop();
    }
    return m_pSelf;
}

void KUiChatItem::CloseWindow(bool bDestory) {
    if (m_pSelf) {
        if (bDestory) {
            m_pSelf->Destroy();
            m_pSelf = NULL;
        } else
            m_pSelf->Hide();
    }
}

KUiChatItem::KUiChatItem() {
    m_ClickX=m_ClickY=0;
    m_nBgColor = 0x10181818;
    m_nRectColor = 0xFF888888;
    m_nBgItemAlpha = 150;
    m_nBgAlpha = 0;
    memset(&m_sBgItemColor, 0, sizeof(m_sBgItemColor));
    pItemLen = 0;
    pItemDesc[0] = 0;
    nNumLine = 0;
    nMaxLineLen = 0;
    memset(&m_Image, 0, sizeof(KRUImage));
    m_nImgWidth = 0;
    m_nImgHeight = 0;
    memset(&m_IconImage, 0, sizeof(KRUImage));
    ZeroMemory(m_szIconImage, sizeof(m_szIconImage));
    m_nIconWidth = 0;
    m_nIconHeight = 0;
    m_bLock = false;
    memset(&m_LockImage, 0, sizeof(KRUImage));
}


KUiChatItem *KUiChatItem::GetIfVisible() {
    if (m_pSelf && m_pSelf->IsVisible())
        return m_pSelf;
    else
        return NULL;
}


void KUiChatItem::Initialize() {
    char szScheme[128];
    g_UiBase.GetCurSchemePath(szScheme, sizeof(szScheme));
    LoadScheme(szScheme);
    m_Preview.LoadScheme(szScheme);
    Wnd_AddWindow(this, WL_TOPMOST);
}


int KUiChatItem::WndProc(unsigned int uMsg, unsigned int uParam, int nParam) {
    int nResult = false;
    switch (uMsg) {
        case WM_LBUTTONDOWN:
            Hide();
            nResult = true;
        case WM_RBUTTONDOWN:
        case WM_KEYDOWN:
            break;
        default:
            nResult = KWndWindow::WndProc(uMsg, uParam, nParam);
            break;
    }
    return nResult;
}

void KUiChatItem::LoadScheme(const char *pScheme) {
    char Buff[128];
    KIniFile Ini;

    sprintf(Buff, "%s\\%s", pScheme, SCHEME_INI_CHATITEM);

    if (Ini.Load(Buff)) {
        Init(&Ini, "Main");
        Ini.GetString("Main", "BgColor", "0,0,0", Buff, 128);
        m_nBgColor = (GetColor(Buff) & 0x00ffffff);
        Ini.GetInteger("Main", "BgAlpha", 0, (int *) &m_nBgAlpha);
        m_nBgColor |= (m_nBgAlpha << 24);

        Ini.GetInteger("Border", "ImgWidth", 0, (int *) &m_nImgWidth);
        Ini.GetInteger("Border", "ImgHeight", 0, (int *) &m_nImgHeight);
        if (m_nImgWidth < 0)
            m_nImgWidth = 0;
        if (m_nImgHeight < 0)
            m_nImgHeight = 0;
        m_Image.nType = ISI_T_SPR;
        m_Image.bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;
        m_Image.Color.Color_b.a = 255;
        m_Image.uImage = 0;
        m_Image.nISPosition = IMAGE_IS_POSITION_INIT;
        Ini.GetString("Border", "Image", "", m_Image.szImage, sizeof(m_Image.szImage));
        m_Image.nFrame = 0;

        Ini.GetString("ItemImage", "RectItemColor", "0,0,0", Buff, 128);
        m_nRectColor = (GetColor(Buff) & 0x00ffffff);
        Ini.GetString("ItemImage", "BgItemColor", "0,0,0", m_sBgItemColor, sizeof(m_sBgItemColor));
        Ini.GetInteger("ItemImage", "BgItemAlpha", 0, (int *) &m_nBgItemAlpha);
    }
    return;
}

#define PREFIX_SIZE 25

void KUiChatItem::PaintWindow() {
    // F4 and posted equipment share text, sprites, socket effects and background.
    // All data was copied before the temporary posted item was released.
    m_Preview.PaintMouseHoverInfo();
}

void KUiChatItem::Show() {
    int x,y,width,height;m_Preview.GetPreviewBounds(x,y,width,height);
    SetPosition(x,y);SetSize(width,height);
    KWndWindow::Show();
}

void KUiChatItem::Updatedata(int uId) {
    m_Preview.CancelMouseHoverInfo();
    if(uId&&g_pCoreShell)
        SetMouseHoverObjectDescInto(m_Preview,this,0,CGOG_ITEM,uId,UOC_ITEM_TAKE_WITH,m_ClickX,m_ClickY);
    m_Preview.AnchorPreview(m_ClickX,m_ClickY);
}

void KUiChatItem::Hide() {
    m_Preview.CancelMouseHoverInfo();
    pItemLen = 0;
    pItemDesc[0] = 0;
    nNumLine = 0;
    nMaxLineLen = 0;
    memset(&m_IconImage, 0, sizeof(KRUImage));
    ZeroMemory(m_szIconImage, sizeof(m_szIconImage));
    memset(&m_LockImage, 0, sizeof(KRUImage));
    m_nIconWidth = 0;
    m_nIconHeight = 0;
    m_bLock = false;
    KWndWindow::Hide();
}

