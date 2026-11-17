/*
 * File:     UiFaceSelector.cpp
 * Desc:     表情选择窗口
 * Author:   flying
 * Creation: 2003/7/8
 * 今天是偶老婆的生日哦，呵呵
 */
//-----------------------------------------------------------------------------
#include "KWin32.h"
#include "KIniFile.h"
#include "../Elem/Wnds.h"
#include "../Elem/WndMessage.h"
#include "../Elem/MouseHover.h"
#include "UiFaceSelector.h"
#include "../../../../Headers/PhongThanUiGrid.h"
#include "../UiBase.h"
#include "../UiSoundSetting.h"
#include "../../../Represent/iRepresent/iRepresentShell.h"
#include "../../../Represent/iRepresent/KRepresentUnit.h"
#include "../../../Engine/src/Text.h"
#include "../../../Engine/src/KDebug.h"
#include <crtdbg.h>

extern iRepresentShell *g_pRepresentShell;
extern IInlinePicEngineSink *g_pIInlinePicSink;


KUiFaceSelector *KUiFaceSelector::m_pSelf = NULL;
KUiFaceSelector::KFaceItem *KUiFaceSelector::ms_pFaceList = NULL;
int                            KUiFaceSelector::ms_nNumFaces = 0;

#define        SCHEME_INI_FACE            "UiFaceSelector.ini"
#define        SCHEME_INI_FACE_DETAIL  "\\Ui\\FaceDetail.ini"

KUiFaceSelector::KUiFaceSelector() {
    m_nCurrIndex = -1;
    m_pCallerWnd = NULL;
    m_pvCallerParam = NULL;
    m_nCurrPage = 0;
    m_bExpanded = false;
    m_nFacesPageCount = 0;
    m_nMaxFacesPage = 0;
    m_nNormColuCount = 0;
    m_nRowCount = 0;
    m_nBtnWidth = m_nBtnHeight = 1;
    m_nIndentH = m_nIndentV = 0;
}

KUiFaceSelector *KUiFaceSelector::OpenWindow(KWndWindow *pCaller, void *pvParam) {
    if (m_pSelf == NULL) {
        m_pSelf = new KUiFaceSelector;
        if (m_pSelf)
            m_pSelf->Initialize();
    }

    if (m_pSelf) {
        m_pSelf->m_pCallerWnd = pCaller;
        m_pSelf->m_pvCallerParam = pvParam;
        UiSoundPlay(UI_SI_WND_OPENCLOSE);
        m_pSelf->Show();
        m_pSelf->BringToTop();
    }
    return m_pSelf;
}

void KUiFaceSelector::CloseWindow(bool bDestroy) {
    if (m_pSelf) {
        m_pSelf->Hide();
        if (bDestroy) {
            m_pSelf->Destroy();
            m_pSelf = NULL;
        }
    }
}

KUiFaceSelector *KUiFaceSelector::GetIfVisible() {
    if (m_pSelf && m_pSelf->IsVisible())
        return m_pSelf;
    else
        return NULL;
}

int KUiFaceSelector::ConvertFaceText(char *pDest, const char *pSrc, int nCount) {
    if (pDest == NULL || pSrc == NULL || nCount < 0)
        return 0;

    int nConvertCount = 0;
    unsigned char cCharacter;
    int nReadPos = 0;
    while (nReadPos < nCount) {
        cCharacter = pSrc[nReadPos];
        if (cCharacter > 0x80) {
            pDest[nConvertCount++] = cCharacter;
            nReadPos++;
        } else if (cCharacter == 0x0d)    //换行
        {
            if (nReadPos + 1 < nCount && pSrc[nReadPos + 1] == 0x0a)
                nReadPos += 2;
            else
                nReadPos++;
            pDest[nConvertCount++] = 0x0a;
        } else if ((cCharacter >= 0x20 && cCharacter < 0x7F) || cCharacter == 0x0a) {
            if (!ConvertFace(pDest, nConvertCount, pSrc, nCount, nReadPos)) {
                pDest[nConvertCount++] = cCharacter;
                nReadPos++;
            }
        } else
            nReadPos++;
    }
    return nConvertCount;
}

//--------------------------------------------------------------------------
//	功能：转换表情符
//--------------------------------------------------------------------------
int KUiFaceSelector::ConvertFace(char *pDest, int &nConvertCount, const char *pSrc, int nCount, int &nReadPos) {
    int nRemainCount = nCount - nReadPos;
    _ASSERT(pDest != NULL && pSrc != NULL && nRemainCount > 0);

    for (int i = 0; i < ms_nNumFaces; i++) {
        if (ms_pFaceList[i].wFaceID>=MAX_SYSTEM_INLINE_PICTURES) continue;
        int nLen = strlen(ms_pFaceList[i].szFaceText);
        if (nLen <= nRemainCount && memcmp(ms_pFaceList[i].szFaceText, pSrc + nReadPos, nLen) == 0) {
            nConvertCount += sprintf(pDest + nConvertCount, "<pic=%d>", ms_pFaceList[i].wFaceID);
            nReadPos += nLen;
            return true;
        }
    }
    return false;
}

//初始化
void KUiFaceSelector::Initialize() {
    AddChild(&m_PrevBtn);
    AddChild(&m_NextBtn);
    char szScheme[128];
    g_UiBase.GetCurSchemePath(szScheme, sizeof(szScheme));
    LoadScheme(szScheme);
    Wnd_AddWindow(this, WL_TOPMOST);
}

void KUiFaceSelector::LoadFaceList() {
    if (ms_pFaceList != NULL) {
        free(ms_pFaceList);
        ms_pFaceList = NULL;
    }
    ms_nNumFaces = 0;

    KIniFile Ini;
    if (Ini.Load(SCHEME_INI_FACE_DETAIL)) {
        int nCount, i, ID;
        char szSection[8];
        char szSpr[MAX_PATH];

        Ini.GetInteger("List", "Count", 0, &nCount);
        if (nCount > 0) {
            ms_pFaceList = (KFaceItem *) malloc(sizeof(KFaceItem) * nCount);
            if (ms_pFaceList) {
                ms_nNumFaces = nCount;
                for (i = 0; i < nCount; i++) {
                    sprintf(szSection, "Face%d", i + 1);
                    Ini.GetString(szSection, "Text", "", ms_pFaceList[i].szFaceText,
                                  sizeof(ms_pFaceList[i].szFaceText));
                    Ini.GetString(szSection, "Tip", "", ms_pFaceList[i].szFaceTip,
                                  sizeof(ms_pFaceList[i].szFaceTip));
                    Ini.GetInteger(szSection, "ID", 0, &ID);
                    ms_pFaceList[i].wFaceID = (WORD) ID;
                    // FaceDetail's authentic PT entries are SPR-addressed and
                    // omit explicit IDs.  Assign the deterministic ID before
                    // registration; registering every SPR as ID 0 corrupts
                    // the inline-picture registry and renders palette noise.
                    if (ms_pFaceList[i].wFaceID == 0)
                        ms_pFaceList[i].wFaceID = (WORD)(i + 1);
                    Ini.GetString(szSection, "Spr", "", szSpr, sizeof(szSpr));
                    // Keep the existing one-based ID; custom IDs >=512 are stripped by TEncodeText.
                    if (szSpr[0] && g_pIInlinePicSink) {
                        int width=0,height=0;
                        if (FAILED(g_pIInlinePicSink->GetPicSize(ms_pFaceList[i].wFaceID,width,height)) ||
                            width<=0 || height<=0) ms_pFaceList[i].wFaceID=0xffff;
                    }
                    if (!ms_pFaceList[i].szFaceText[0])
                        sprintf(ms_pFaceList[i].szFaceText, "<pic=%d>", ms_pFaceList[i].wFaceID);
                }
            }
        }
    }
}

void KUiFaceSelector::Clear() {
    if (m_pSelf)
        m_pSelf->CloseWindow(true);
    if (ms_pFaceList) {
        free(ms_pFaceList);
        ms_pFaceList = NULL;
    }
    ms_nNumFaces = 0;
}

void KUiFaceSelector::OnSelFace() {
    if (m_pCallerWnd && m_nCurrIndex >= 0 && m_nCurrIndex < ms_nNumFaces) {
        m_pCallerWnd->WndProc(WND_M_OTHER_WORK_RESULT,
                              (unsigned int) m_pvCallerParam, (int) ms_pFaceList[m_nCurrIndex].szFaceText);
    }
    CloseWindow(false);
}

int KUiFaceSelector::WndProc(unsigned int uMsg, unsigned int uParam, int nParam) {
    int nResult = false;
    switch (uMsg) {
        case WM_LBUTTONDOWN:
        case WM_RBUTTONDOWN:
            m_nCurrIndex = GetFaceAtPos(LOWORD(nParam), HIWORD(nParam));
            OnSelFace();
            nResult = true;
            break;
        case WM_MOUSEMOVE: {
            int x = (int) (short) (LOWORD(nParam));
            int y = (int) (short) (HIWORD(nParam));
            int nIndex = GetFaceAtPos(x, y);
            if (nIndex != m_nCurrIndex) {
                m_nCurrIndex = nIndex;
                UpdateFaceTip(x, y);
            }
        }
            nResult = true;
            break;
        case WND_N_BUTTON_OVER:
            if (m_nCurrIndex >= 0) {
                m_nCurrIndex = -1;
                UpdateFaceTip(0, 0);
            }
            break;
        case WND_N_BUTTON_CLICK:
            if (uParam == (unsigned int)&m_NextBtn || uParam == (unsigned int)&m_PrevBtn) {
                m_bExpanded=uParam == (unsigned int)&m_NextBtn;
                SetSize(m_nBtnWidth*m_nNormColuCount*(m_bExpanded?2:1),m_nRowCount*m_nBtnHeight);
                if(m_bExpanded){m_NextBtn.Hide();m_PrevBtn.Show();}
                else {m_NextBtn.Show();m_PrevBtn.Hide();}
                m_nCurrIndex=-1; g_MouseOver.CancelMouseHoverInfo();
            }
            nResult = true;
            break;
        default:
            nResult = KWndWindow::WndProc(uMsg, uParam, nParam);
            break;
    }
    return nResult;
}

void KUiFaceSelector::LoadScheme(const char *pScheme) {
    char Buff[128];
    KIniFile Ini;
    sprintf(Buff, "%s\\%s", pScheme, SCHEME_INI_FACE);

    if (Ini.Load(Buff)) {
        Init(&Ini, "Main");
        m_NextBtn.Init(&Ini, "BtnMore");
        m_PrevBtn.Init(&Ini, "BtnLess");
        m_PrevBtn.Hide();

        Ini.GetInteger("Main", "ColumnCount", 0, &m_nNormColuCount);
        Ini.GetInteger("Main", "Row", 0, &m_nRowCount);

        Ini.GetInteger("Main", "BtnWidth", 1, &m_nBtnWidth);
        if (m_nBtnWidth < 1)
            m_nBtnWidth = 1;
        Ini.GetInteger("Main", "BtnHeight", 1, &m_nBtnHeight);
        if (m_nBtnHeight < 1)
            m_nBtnHeight = 1;
        Ini.GetInteger("Main", "IndentH", 0, &m_nIndentH);
        Ini.GetInteger("Main", "IndentV", 0, &m_nIndentV);

        Ini.GetString("Main", "BgColor", "0,0,0", Buff, 128);
        m_nBgColor = (GetColor(Buff) & 0x00ffffff);
        Ini.GetString("Main", "BgOver", "0,0,0", Buff, 128);
        m_nBgOver = (GetColor(Buff) & 0x00ffffff);
        Ini.GetString("Main", "BorderColor", "0,0,0", Buff, 128);
        m_nBorderColor = GetColor(Buff);// & 0x00ffffff);	// alpha
        Ini.GetInteger("Main", "BgAlpha", 0, (int *) &m_nBgAlpha);
        m_nBgColor |= (m_nBgAlpha << 24);    // 处理alpha

        SetSize(m_nBtnWidth * m_nNormColuCount, m_Height);
        m_nFacesPageCount = m_nRowCount * m_nNormColuCount - 2;
        if (ms_nNumFaces % m_nFacesPageCount == 0)
            m_nMaxFacesPage = ms_nNumFaces / m_nFacesPageCount - 1;
        else
            m_nMaxFacesPage = ms_nNumFaces / m_nFacesPageCount;
    }
    return;
}

void KUiFaceSelector::UpdateFaceTip(int x, int y) {
    if (m_nCurrIndex >= 0 && ms_pFaceList[m_nCurrIndex].szFaceTip[0]) {
        char szToolTip[64] = "";
        if (ms_pFaceList[m_nCurrIndex].szFaceText[0])
            sprintf(szToolTip, "%s(%s)", ms_pFaceList[m_nCurrIndex].szFaceTip,
                    ms_pFaceList[m_nCurrIndex].szFaceText);
        else
            sprintf(szToolTip, "%s", ms_pFaceList[m_nCurrIndex].szFaceTip);
        g_MouseOver.SetMouseHoverInfo((void *) (KWndWindow *) this, m_nCurrIndex,
                                      x, y, false, true);
        g_MouseOver.SetMouseHoverTitle(szToolTip, strlen(szToolTip), 0xffffffff);
    } else {
        g_MouseOver.CancelMouseHoverInfo();
    }
}


void KUiFaceSelector::PaintWindow() {
    KWndWindow::PaintWindow();
    if (!g_pRepresentShell || !ms_pFaceList) return;
    int perPanel=m_nRowCount*m_nNormColuCount;
    int available=min(ms_nNumFaces,perPanel*(m_bExpanded?2:1)-1);
    for(int i=0;i<available;++i) {
        int panel=i/perPanel,cell=i%perPanel;
        int x=m_nAbsoluteLeft+(panel*m_nNormColuCount+cell%m_nNormColuCount)*m_nBtnWidth;
        int y=m_nAbsoluteTop+cell/m_nNormColuCount*m_nBtnHeight;
        KRUShadow bg;memset(&bg,0,sizeof(bg));
        bg.Color.Color_dw=(i==m_nCurrIndex?m_nBgOver:m_nBgColor)&0xffffff;
        bg.Color.Color_b.a=(BYTE)(32-min(m_nBgAlpha,255U)*32/255);
        bg.oPosition.nX=x;bg.oPosition.nY=y;bg.oEndPos.nX=x+m_nBtnWidth;bg.oEndPos.nY=y+m_nBtnHeight;
        g_pRepresentShell->DrawPrimitives(1,&bg,RU_T_SHADOW,true);
        char encoded[3];encoded[0]=KTC_INLINE_PIC;memcpy(encoded+1,&ms_pFaceList[i].wFaceID,2);
        KOutputTextParam param;memset(&param,0,sizeof(param));
        param.nX=x+m_nIndentH;param.nY=y+m_nIndentV;param.nNumLine=2;
        param.nVertAlign=1;param.nZ=TEXT_IN_SINGLE_PLANE_COORD;
        g_pRepresentShell->OutputRichText(12,&param,encoded,3);
    }
}

void KUiFaceSelector::Show() {
    Wnd_SetCapture(this);
    KWndWindow::Show();
}

void KUiFaceSelector::Hide() {
    Wnd_ReleaseCapture();
    KWndWindow::Hide();
}

int KUiFaceSelector::GetFaceAtPos(int x,int y) {
    if(!KWndWindow::PtInWindow(x,y))return -1;
    return PtFaceGridIndex(x-m_nAbsoluteLeft,y-m_nAbsoluteTop,ms_nNumFaces,
        m_nNormColuCount,m_nRowCount,m_nBtnWidth,m_nBtnHeight,m_bExpanded);
}
