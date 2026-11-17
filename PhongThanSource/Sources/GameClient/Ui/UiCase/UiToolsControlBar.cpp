/*****************************************************************************************
*****************************************************************************************/
#include "KWin32.h"
#include "KIniFile.h"
#include "../elem/wnds.h"
#include "../Elem/WndMessage.h"
#include "../UiBase.h"
#include "UiToolsControlBar.h"
#include "UiChatCentre.h"
#include "UiTaskNote.h"

#include "../ShortcutKey.h"
#include "../../../core/src/coreshell.h"
#include "../../../Engine/src/KDebug.h"
#include "GameDataDef.h"

extern iCoreShell *g_pCoreShell;
static bool s_PtPkConfirmPending = false;

#define    SCHEME_INI            "gong_ju_kong_zhi_tiao.ini"
#define    PHONGTHAN_VNG_TOOLBAR_INI "\\Ui\\ui3\\gong_ju_kong_zhi_tiao.ini"

char szArray_NormalPK[][64] =
        {
                "LuyÖn c«ng",
                "ChiÕn ®Êu",
                "§å s¸t",
//	"Bang chiÕn"
        };

#include "PhongThanInteractionBar.inl"
KUiToolsControlBar *KUiToolsControlBar::m_pSelf = NULL;

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
KUiToolsControlBar *KUiToolsControlBar::OpenWindow() {
    PtInteractionOpen();
    if (m_pSelf == NULL) {
        m_pSelf = new KUiToolsControlBar;
        if (m_pSelf)
            m_pSelf->Initialize();
    }
    if (m_pSelf)
        m_pSelf->Show();
    return m_pSelf;
}

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
void KUiToolsControlBar::CloseWindow() {
    PtInteractionClose();
    if (m_pSelf) {
        m_pSelf->Destroy();
        m_pSelf = NULL;
    }
}


void KUiToolsControlBar::Initialize() {
    char Scheme[256];
    g_UiBase.GetCurSchemePath(Scheme, 256);
    LoadScheme(Scheme);

    m_Style &= ~WND_S_VISIBLE;
    Wnd_AddWindow(this, WL_LOWEST);
}


void KUiToolsControlBar::LoadScheme(const char *pScheme) {
    char Buff[128];
    KIniFile Ini;
    if (m_pSelf) {
        // Bind the runtime window to the original Phong Than Ui3 toolbar entry.
        // KPakFile mode 0 is loose-first, so the byte-identical Ui3 recovery
        // remains the authoritative development override.
        BOOL bLoaded = Ini.Load(PHONGTHAN_VNG_TOOLBAR_INI);
        if (!bLoaded && pScheme && pScheme[0]) {
            sprintf(Buff, "%s\\%s", pScheme, SCHEME_INI);
            bLoaded = Ini.Load(Buff);
        }

        if (bLoaded) {
            // Do not synthesize controls here.  The recovered Phong Than
            // Ui4 definition is authoritative for the toolbar button set.
            // Original PT Status declares sex-specific images, not Image.
            char szStatusImage[MAX_PATH];
            const char* szStatusKey = (g_pCoreShell &&
                !g_pCoreShell->GetGameData(GDI_PLAYER_IS_MALE, 0, 0)) ? "FemaleImage" : "MaleImage";
            if (Ini.GetString("Status", szStatusKey, "", szStatusImage, sizeof(szStatusImage)) && szStatusImage[0])
                Ini.WriteString("Status", "Image", szStatusImage);
            m_pSelf->Init(&Ini, "Main");

            // The original toolbar is independently right-anchored.
            int nScreenWidth = 800;
            int nScreenHeight = 600;
            int nWidth = 0;
            int nHeight = 0;
            Wnd_GetScreenSize(nScreenWidth, nScreenHeight);
            m_pSelf->GetSize(&nWidth, &nHeight);
            if (nWidth > 0 && nHeight > 0)
                m_pSelf->SetPosition(nScreenWidth - nWidth,
                                     nScreenHeight - nHeight - 2);
        }
    }
}

int KUiToolsControlBar::WndProc(unsigned int uMsg, unsigned int uParam, int nParam) {
    int nRet = 0;
    switch (uMsg) {
        case WND_N_BUTTON_CLICK:
            break;
        case WND_M_OTHER_WORK_RESULT:
            if (s_PtPkConfirmPending && uParam == 0x7D1) {
                s_PtPkConfirmPending = false;
                if (nParam == WND_OPER_RESULT_0 && g_pCoreShell)
                    g_pCoreShell->OperationRequest(GOI_PK_SETTING,0,8);
            }
            break;
        case WND_M_MENUITEM_SELECTED:
            if (uParam == (unsigned int) (KWndWindow *) this) {
                if ((short) (LOWORD(nParam) >= 0))
                    SwitchPK((short) (LOWORD(nParam)));
            }
            break;
        default:
            nRet = KWndImage::WndProc(uMsg, uParam, nParam);
    }
    return nRet;
}

void KUiToolsControlBar::DefaultScheme(const char *pScheme) {
    char Buff[128];
    KIniFile Ini;
    if (m_pSelf) {
        BOOL bLoaded = Ini.Load(PHONGTHAN_VNG_TOOLBAR_INI);
        if (!bLoaded && pScheme && pScheme[0]) {
            sprintf(Buff, "%s\\%s", pScheme, SCHEME_INI);
            bLoaded = Ini.Load(Buff);
        }
        if (bLoaded) {
            int nValue1, nValue2;
            Ini.GetInteger("Main", "Left", 0, &nValue1);
            Ini.GetInteger("Main", "Top", 0, &nValue2);
            int nScreenWidth, nScreenHeight, nWidth, nHeight;
            Wnd_GetScreenSize(nScreenWidth, nScreenHeight);
            m_pSelf->GetSize(&nWidth, &nHeight);
            m_pSelf->SetPosition(nScreenWidth - nWidth, nScreenHeight - nHeight - 2);
        }
    }
}

void KUiToolsControlBar::Breathe() {
    if(s_PtInteractionBar)s_PtInteractionBar->Anchor();
    UpdateData();
}

#include "UiPtPkMenu.inl"
