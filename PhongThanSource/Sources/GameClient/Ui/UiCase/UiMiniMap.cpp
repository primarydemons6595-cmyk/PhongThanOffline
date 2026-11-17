/*****************************************************************************************
//	ΩÁ√Ê--–°µÿÕº
//	Copyright : Kingsoft 2003
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2003-4-21
*****************************************************************************************/
#include "KWin32.h"
#include "KIniFile.h"
#include "../Elem/Wnds.h"
#include "../Elem/WndMessage.h"
#include "UiMiniMap.h"
#include "../UiBase.h"
#include "../UiSoundSetting.h"
#include "UiWorldMap.h"
#include "UiCaveList.h"
#include "UiSysMsgCentre.h"
#include "UiGetString.h"
#include "../../../core/src/coreshell.h"
#include "../../../core/src/gamedatadef.h"
#include "../../../core/src/KCore.h"
#include    "../../Engine/Src/KSG_StringProcess.h"
#include "../../../Represent/iRepresent/iRepresentShell.h"
#include "../../../Represent/iRepresent/KRepresentUnit.h"
#include <time.h>
#include "UiChatItem.h"

extern iRepresentShell *g_pRepresentShell;
extern iCoreShell *g_pCoreShell;
extern IInlinePicEngineSink *g_pIInlinePicSink;

#include "PhongThanMiniMapLeft.inl"
#include "UiPtCrossRoute.inl"
void KUiMiniMap::CancelCrossRoute(){PTCrossRoute::Cancel();}
int KUiMiniMap::NavigateToNpc(int map,int x,int y){return PTCrossRoute::Navigate(map,x,y);}

#define    SCHEME_INI_NOPIC            "UiMiniMapNopic.ini"
#define    SCHEME_INI_SMALL            "UiMiniMapSmall.ini"
#define    SCHEME_INI_BIG                "UiMiniMapBig.ini"
#define    SCHEME_INI_BIGEX            "UiMiniMapBigEX.ini"
#define    defMSG_FORMAT_SCENEPOS        "%d,%d"
#define    defMSG_FORMAT_SCENEPOSF    "%d/%d t◊m"

//µÿÕºÕœ∂Ø∫Ûªπ‘≠µƒ ±º‰(√Î)
#define MAP_RECOVER_TIME        3000
#define    NOT_DRAGING_MAP            -1
#define    WAIT_TO_BE_SET_BACK        -2
#define    IS_DRAGING_MAP            (m_OldPos.x != NOT_DRAGING_MAP)
#define    IS_WAIT_TO_SET_BACK        (m_OldPos.x == WAIT_TO_BE_SET_BACK)

KUiMiniMap *KUiMiniMap::ms_pSelf = NULL;
static MINIMAP_MODE s_eMapMode = MINIMAP_M_NONE;
static MINIMAP_MODE s_eMapOldMode = MINIMAP_M_BRIEF_PIC;
static bool s_ShowTeamMarkers = true;
static unsigned int PtMiniMapElems()
{
    return SCENE_PLACE_MAP_ELEM_PIC | SCENE_PLACE_MAP_ELEM_CHARACTER |
        (s_ShowTeamMarkers ? SCENE_PLACE_MAP_ELEM_PARTNER : 0);
}
static void PtMiniMapControl(KWndWindow* control,KIniFile* ini,const char* section)
{
    // Changing map mode must not retain images/hitboxes from the previous INI.
    if(ini->IsSectionExist(section)) control->Show();else control->Hide();
}

KUiMiniMap::KUiMiniMap() {
    m_OldPos.x = NOT_DRAGING_MAP;
}

//--------------------------------------------------------------------------
//	π¶ƒ‹£∫¥Úø™¥∞ø⁄£¨∑µªÿŒ®“ªµƒ“ª∏ˆ¿‡∂‘œÛ µ¿˝
//--------------------------------------------------------------------------
KUiMiniMap *KUiMiniMap::OpenWindow() {
    if (ms_pSelf == NULL) {
        ms_pSelf = new KUiMiniMap;
        if (ms_pSelf)
            ms_pSelf->Initialize();
    }
    if (ms_pSelf) {
        UiSoundPlay(UI_SI_WND_OPENCLOSE);
        ms_pSelf->Show();
        ms_pSelf->BringToTop();
        if(s_PtMiniMapLeft)s_PtMiniMapLeft->BringToTop();
        if(s_PtMapCoordinate)s_PtMapCoordinate->BringToTop();
    }
    return ms_pSelf;
}

//--------------------------------------------------------------------------
//	π¶ƒ‹£∫»Áπ˚¥∞ø⁄’˝±ªœ‘ æ£¨‘Ú∑µªÿ µ¿˝÷∏’Î
//--------------------------------------------------------------------------
KUiMiniMap *KUiMiniMap::GetIfVisible() {
    if (ms_pSelf && ms_pSelf->IsVisible())
        return ms_pSelf;
    return NULL;
}

//--------------------------------------------------------------------------
//	π¶ƒ‹£∫≥ı ºªØ
//--------------------------------------------------------------------------
void KUiMiniMap::Initialize() {
    if(!s_PtMapCoordinate){s_PtMapCoordinate=new PTMapCoordinate;s_PtMapCoordinate->Initialize();}
    if(!s_PtMiniMapLeft){s_PtMiniMapLeft=new PTMiniMapLeft;s_PtMiniMapLeft->Initialize();}
    AddChild(&m_MapHint);
    AddChild(&m_TitleImage);
    AddChild(&m_BorderConner1);
    AddChild(&m_BorderConner2);
    AddChild(&m_BtnUp);
    AddChild(&m_BtnDown);
    AddChild(&m_BtnLeft);
    AddChild(&m_BtnRight);
    AddChild(&m_MinSizeBtn);
    AddChild(&m_BtnQuery);
    AddChild(&m_BtnTeamNote);
    AddChild(&m_HideBuff);
    AddChild(&m_BtnArenaQuery);
    AddChild(&m_Shadow);
    AddChild(&m_SceneName);

    //TamLTM Bang hoi chiem linh
    AddChild(&m_TongMapInfo1);
    AddChild(&m_TongMapInfo2);
    //end code

    AddChild(&m_ScenePos);
    AddChild(&m_SwitchBtn);
    AddChild(&m_WorldMapBtn);
    AddChild(&m_BtnFlag);
    AddChild(&m_CaveMapBtn);
    m_Style &= ~WND_S_VISIBLE;
    char Scheme[128];
    g_UiBase.GetCurSchemePath(Scheme, 256);
    LoadScheme(Scheme);

    Wnd_AddWindow(this, WL_NORMAL);
}

//--------------------------------------------------------------------------
//	π¶ƒ‹£∫πÿ±’¥∞ø⁄
//--------------------------------------------------------------------------
void KUiMiniMap::CloseWindow(bool bDestory) {
    if(bDestory)PtAuctionClose();
    if (ms_pSelf) {
        ms_pSelf->StopScrollMap();
        ms_pSelf->MapMoveBack();
        if (g_pCoreShell)
            g_pCoreShell->SceneMapOperation(GSMOI_IS_SCENE_MAP_SHOWING, SCENE_PLACE_MAP_ELEM_NONE, 0);
        ms_pSelf->Hide();
        if (bDestory) {
            PTCrossRoute::Reset();
            if(s_PtMapCoordinate){s_PtMapCoordinate->Destroy();s_PtMapCoordinate=0;}
            if(s_PtMiniMapLeft){s_PtMiniMapLeft->Destroy();s_PtMiniMapLeft=0;}
            ms_pSelf->Destroy();
            ms_pSelf = NULL;
        }
    }
}

//--------------------------------------------------------------------------
//	π¶ƒ‹£∫‘ÿ»Î¥∞ø⁄µƒΩÁ√Ê∑Ω∞∏
//--------------------------------------------------------------------------
void KUiMiniMap::LoadScheme(const char *pScheme) {
    if (ms_pSelf) {
        char szBuf[128];
        KIniFile Ini;
        if (s_eMapMode == MINIMAP_M_BRIEF_NOT_PIC)
            sprintf(szBuf, "%s\\%s", pScheme, SCHEME_INI_NOPIC);
        else if (s_eMapMode == MINIMAP_M_BRIEF_PIC_BROWSEEX)
            sprintf(szBuf, "%s\\%s", pScheme, SCHEME_INI_BIGEX);
        else {
            //Ban do to o giua man hinh
            //	if (GetAsyncKeyState(VK_TAB))
            //	{
            //	sprintf(szBuf, "%s\\%s", pScheme, SCHEME_INI_BIGEX);
            //	}
            //	else
            //	{
            sprintf(szBuf, "%s\\%s", pScheme,
                    s_eMapMode == MINIMAP_M_BRIEF_PIC ? SCHEME_INI_SMALL : SCHEME_INI_BIG);
            //	}
        }

        if (Ini.Load(szBuf)) {
            ms_pSelf->LoadScheme(&Ini);
        }
    }
}

void KUiMiniMap::LoadScheme(KIniFile *pIni) {
    if (pIni == NULL)
        return;
    Init(pIni, "MiniMap");
    if (s_eMapMode == MINIMAP_M_BRIEF_PIC) {
        int nScreenWidth = 800;
        int nScreenHeight = 600;
        int nWidth = 0;
        int nHeight = 0;
        int nLeft = 0;
        int nTop = 0;
        Wnd_GetScreenSize(nScreenWidth, nScreenHeight);
        GetSize(&nWidth, &nHeight);
        GetPosition(&nLeft, &nTop);
        if (nScreenWidth > 800 && nWidth > 0)
            SetPosition(nScreenWidth - nWidth, nTop);
    }
    // Load the complete original control set.  Missing optional sections
    // remain harmless layout-only controls; this keeps the native PAK/loose
    // UI contract instead of substituting JX artwork.
    m_MapHint.Init(pIni,"TxtTipHint");
    PtMiniMapControl(&m_MapHint,pIni,"TxtTipHint");
    m_TitleImage.Init(pIni, "TitleImage");
    m_BorderConner1.Init(pIni, "BorderConner1");
    m_BorderConner2.Init(pIni, "BorderConner2");
    m_BtnUp.Init(pIni, "BtnUp");
    m_BtnDown.Init(pIni, "BtnDown");
    m_BtnLeft.Init(pIni, "BtnLeft");
    m_BtnRight.Init(pIni, "BtnRight");
    m_MinSizeBtn.Init(pIni, "MinSizeBtn");
    m_BtnQuery.Init(pIni, "BtnQuery");
    m_BtnTeamNote.Init(pIni, "BtnTeamNote");
    m_HideBuff.Init(pIni, "HideBuff");
    m_BtnArenaQuery.Init(pIni, "BtnArenaQuery");
    // Original PT 5ED647..5ED67E hides the four obsolete map arrows.
    m_BtnUp.Hide();
    m_BtnDown.Hide();
    m_BtnLeft.Hide();
    m_BtnRight.Hide();
    m_Shadow.Init(pIni, "NameShadow");
    m_SceneName.Init(pIni, "SceneName");

    //TamLTM Bang hoi chiem linh
    m_TongMapInfo1.Init(pIni, "TongMapInfo1");
    m_TongMapInfo2.Init(pIni, "TongMapInfo2");
    //end code

    m_ScenePos.Init(pIni, "ScenePos");
    m_SwitchBtn.Init(pIni, "SwitchBtn");
    m_WorldMapBtn.Init(pIni, "WorldMapBtn");
    m_BtnFlag.Init(pIni, "BtnFlag");
    m_CaveMapBtn.Init(pIni, "CaveMapBtn");
    PtMiniMapControl(&m_TitleImage,pIni,"TitleImage");
    PtMiniMapControl(&m_BorderConner1,pIni,"BorderConner1");
    PtMiniMapControl(&m_BorderConner2,pIni,"BorderConner2");
    PtMiniMapControl(&m_MinSizeBtn,pIni,"MinSizeBtn");
    PtMiniMapControl(&m_BtnQuery,pIni,"BtnQuery");
    PtMiniMapControl(&m_BtnTeamNote,pIni,"BtnTeamNote");
    PtMiniMapControl(&m_HideBuff,pIni,"HideBuff");
    PtMiniMapControl(&m_BtnArenaQuery,pIni,"BtnArenaQuery");
    m_BtnArenaQuery.Hide(); // Battlefield query is not an ordinary minimap control.
    PtMiniMapControl(&m_CaveMapBtn,pIni,"CaveMapBtn");
    m_BtnTeamNote.CheckButton(s_ShowTeamMarkers);
    int screenWidth,screenHeight,mapWidth,mapHeight,mapLeft,mapTop;
    Wnd_GetScreenSize(screenWidth,screenHeight);
    GetSize(&mapWidth,&mapHeight);GetPosition(&mapLeft,&mapTop);
    if(s_eMapMode==MINIMAP_M_BRIEF_PIC_BROWSE || s_eMapMode==MINIMAP_M_BRIEF_PIC_BROWSEEX)
        SetPosition(max(0,(screenWidth-mapWidth)/2),max(10,(screenHeight-mapHeight)/2));
    else
        SetPosition(screenWidth>mapWidth ? screenWidth-mapWidth : 0,
            mapTop+mapHeight>screenHeight ? max(0,screenHeight-mapHeight) : mapTop);

    pIni->GetInteger("MapRect", "Left", 0, (int *) &m_MapPos.x);
    pIni->GetInteger("MapRect", "Top", 0, (int *) &m_MapPos.y);
    pIni->GetInteger("MapRect", "Width", 0, (int *) &m_MapSize.cx);
    pIni->GetInteger("MapRect", "Height", 0, (int *) &m_MapSize.cy);

    if (IsVisible() && g_pCoreShell) {
        g_pCoreShell->SceneMapOperation(GSMOI_IS_SCENE_MAP_SHOWING,
                                        PtMiniMapElems(),
                                        (m_MapSize.cx | (m_MapSize.cy << 16)));
    }
}

//--------------------------------------------------------------------------
//	π¶ƒ‹£∫¥∞ø⁄∫Ø ˝
//--------------------------------------------------------------------------
int KUiMiniMap::WndProc(unsigned int uMsg, unsigned int uParam, int nParam) {
    if((uMsg==WND_N_BUTTON_CLICK||uMsg==WND_N_BUTTON_HOLD)&&g_pCoreShell){
        int dx=0,dy=0;
        if(uParam==(unsigned int)&m_BtnUp)dy=-12;
        else if(uParam==(unsigned int)&m_BtnDown)dy=12;
        else if(uParam==(unsigned int)&m_BtnLeft)dx=-12;
        else if(uParam==(unsigned int)&m_BtnRight)dx=12;
        if(dx||dy){m_nOffsetX=dx;m_nOffsetY=dy;MapScroll(0);m_OldPos.x=WAIT_TO_BE_SET_BACK;m_uLastScrollTime=IR_GetCurrentTime();return 1;}
    }

    int nRet = 0;
    switch (uMsg) {
        case WND_N_BUTTON_CLICK:


            if (uParam == (unsigned int)(KWndWindow*)&m_BtnTeamNote) {
                s_ShowTeamMarkers = !s_ShowTeamMarkers;
                m_BtnTeamNote.CheckButton(s_ShowTeamMarkers);
                if(g_pCoreShell) g_pCoreShell->SceneMapOperation(GSMOI_IS_SCENE_MAP_SHOWING,
                    PtMiniMapElems(),m_MapSize.cx | (m_MapSize.cy<<16));
            }
            else if (uParam == (unsigned int)(KWndWindow*)&m_MinSizeBtn)
                MapSetMode(s_eMapMode == MINIMAP_M_BRIEF_PIC ? MINIMAP_M_BRIEF_NOT_PIC : MINIMAP_M_BRIEF_PIC);
            else if (uParam == (unsigned int) (KWndWindow *) &m_SwitchBtn)
                MapSetMode(s_eMapMode == MINIMAP_M_BRIEF_PIC ? MINIMAP_M_BRIEF_PIC_BROWSE : MINIMAP_M_BRIEF_PIC);
            else if (uParam == (unsigned int) (KWndWindow *) &m_WorldMapBtn)
                MapSetMode(MINIMAP_M_WORLD_MAP);
            else if (uParam == (unsigned int) (KWndWindow *) &m_CaveMapBtn)
                MapSetMode(MINIMAP_M_CAVELIST_MAP);
            else if (uParam == (unsigned int) (KWndWindow *) &m_BtnFlag) // Button la co
            {
                BOOL bFlag = g_pCoreShell->GetFlagMode();
                g_pCoreShell->SetFlagMode(!bFlag);
                g_pCoreShell->SetPaintMode(!bFlag);
                //	g_DebugLog("SetPaintMode");
            } else if(uParam==(unsigned int)(KWndWindow*)&m_BtnQuery){PTCrossRoute::Open(m_MpsID);}
            else if (uParam == (unsigned int) (KWndWindow *) &m_ScenePos ||
                       false) // Coordinate input is distinct from cross-region search
            {
                //TamLTM Nhap toa do cho mini map
                KUiGetString::OpenWindow(GSA_NORMAL, "Xin nhÀp t‰a ÆÈ", "",
                                         (KWndWindow *) this, 0, 0, 5, 9);
                //	g_DebugLog("%d + %d Xin nhÀp t‰a ÆÈ ");
            }
            break;
        case WM_LBUTTONDOWN: // Cam co tren mini map
            int nCursorX, nCursorY;
            Wnd_GetCursorPos(&nCursorX, &nCursorY);
            if(nCursorX<m_nAbsoluteLeft+m_MapPos.x || nCursorY<m_nAbsoluteTop+m_MapPos.y ||
                nCursorX>=m_nAbsoluteLeft+m_MapPos.x+m_MapSize.cx || nCursorY>=m_nAbsoluteTop+m_MapPos.y+m_MapSize.cy)
                return 0;

            if (g_pCoreShell->SceneMapOperation(GSMOI_IS_SCENE_DIRECT_MAP, nCursorX, nCursorY)) {
                g_pCoreShell->SceneMapOperation(GSMOI_IS_SCENE_DO_DIRECT_MAP, nCursorX, nCursorY);
            } else {
                g_pCoreShell->SetPaintMode(TRUE);
                g_pCoreShell->SetFlagMode(FALSE);
                g_pCoreShell->DirectFindPos(nCursorX, nCursorY, TRUE, TRUE);
            }

            //Cam co` thi set lai
            g_pCoreShell->SetAutoRun(FALSE);
            g_pCoreShell->SetFlagAutoRun(TRUE, ms_pSelf->m_MpsX, ms_pSelf->m_MpsY);
            //	g_DebugLog("%d + %d 5 ", ms_pSelf->m_MpsX, ms_pSelf->m_MpsY);

            break;
        case WM_LBUTTONUP:
        case WM_LBUTTONDBLCLK:
        case WM_MOUSEHOVER: //Di chuyen va cam co den dia diem map mini
            Wnd_GetCursorPos(&nCursorX, &nCursorY);
            if (g_pCoreShell->SceneMapOperation(GSMOI_IS_SCENE_DIRECT_MAP, nCursorX, nCursorY))
                Wnd_SwitchCursor(MOUSE_CURSOR_PICK);
            else {
                Wnd_SwitchCursor(MOUSE_CURSOR_NORMAL);
                Wnd_TransmitInputToGameSpace(uMsg, uParam, nParam);

                //	g_DebugLog("%d + %d WM_MOUSEHOVER ", nCursorX, nCursorY); //luc Chay luc ko chay
            }
            //	g_DebugLog("debug 14"); // De im chuot se chay ham nay
            break;
        case WND_N_CHILD_MOVE:
            if (uParam == (unsigned int) (KWndWindow *) &m_SceneName ||
                uParam == (unsigned int) (KWndWindow *) &m_ScenePos ||
                uParam == (unsigned int) (KWndWindow *) &m_Shadow) {
                KWndWindow *pWnd = (KWndWindow *) uParam;
                int nMoveOffsetX = (short) (nParam & 0xffff);
                int nMoveOffsetY = (short) ((nParam >> 16) & 0xffff);

                int x, y;
                pWnd->GetPosition(&x, &y);
                x -= nMoveOffsetX;
                y -= nMoveOffsetY;
                pWnd->SetPosition(x, y);    //∞¥≈•∏˙◊≈¥∞ø⁄±‰ªØ£¨À˘“‘“™œ»±‰ªÿ‘≠—˘
                GetPosition(&x, &y);
                x += nMoveOffsetX;
                y += nMoveOffsetY;
                SetPosition(x, y);
            }
            //	g_DebugLog("debug 1");
            break;
        case WM_RBUTTONDOWN:
            Wnd_SetCapture(this);
            Wnd_GetCursorPos((int *) &m_OldPos.x, (int *) &m_OldPos.y);
            //	g_DebugLog("2");
            break;
        case WM_MOUSEMOVE:
            Wnd_GetCursorPos(&nCursorX, &nCursorY);
            if (IS_DRAGING_MAP && (uParam & MK_RBUTTON)) {
                int nCursorX, nCursorY;
                Wnd_GetCursorPos(&nCursorX, &nCursorY);
                m_nOffsetX = m_OldPos.x - nCursorX;
                m_nOffsetY = m_OldPos.y - nCursorY;
                m_OldPos.x = nCursorX;
                m_OldPos.y = nCursorY;
                MapScroll(uParam & MK_CONTROL);
            } else if (g_pCoreShell->SceneMapOperation(GSMOI_IS_SCENE_DIRECT_MAP, nCursorX, nCursorY))
                Wnd_SwitchCursor(MOUSE_CURSOR_PICK);
            else {
                Wnd_SwitchCursor(MOUSE_CURSOR_NORMAL);
                Wnd_TransmitInputToGameSpace(uMsg, uParam, nParam);
            }
            g_pCoreShell->CheckHoverMouseMiniMap(TRUE);
            //	g_DebugLog("%d - %d WM_MOUSEMOVE 2", nCursorX, nCursorY);
            break;
        case WM_RBUTTONUP:
            if (IS_DRAGING_MAP && ((uParam & MK_CONTROL) == 0) && g_pCoreShell)
                g_pCoreShell->SceneMapOperation(GSMOI_SCENE_FOLLOW_WITH_MAP, 0, 0);
            StopScrollMap();
            //	g_DebugLog("debug 17");
            break;
        case WND_M_OTHER_WORK_RESULT: // enter nhap toa do
            if (nParam > 0) {
                int x = 0, y = 0, i = 0;
                const char *pcszTemp = (const char *) nParam;
                char Separator = 0;
                while (pcszTemp[i]) {
                    if (pcszTemp[i] < '0' || pcszTemp[i] > '9') {
                        Separator = pcszTemp[i];
                        break;
                    }
                    i++;
                }
                x = KSG_StringGetInt(&pcszTemp, 255);
                KSG_StringSkipSymbol(&pcszTemp, Separator);
                y = KSG_StringGetInt(&pcszTemp, 255);
                if (g_pCoreShell->GetFlagMode())
                    g_pCoreShell->SetFlagMode(FALSE);

                g_pCoreShell->SetPaintMode(TRUE);
                g_pCoreShell->DirectFindPos(x, y, FALSE, TRUE);

                //TamLTM check auto run
                g_pCoreShell->SetAutoRun(TRUE);
                g_pCoreShell->SetFlagAutoRun(FALSE, 0, 0);

                //	g_DebugLog("WND_M_OTHER_WORK_RESULT %d + %d ", x * 8 * 32, y * 16 * 32);
            }
            break;
        default:
            nRet = KWndWindow::WndProc(uMsg, uParam, nParam);
//		g_DebugLog("debug else");
            g_pCoreShell->CheckHoverMouseMiniMap(FALSE);
            break;
    }
    return nRet;
}

//Õ£÷πÕœ∂ØµÿÕº
void KUiMiniMap::StopScrollMap() {
    if (IS_DRAGING_MAP) {
        m_uLastScrollTime = IR_GetCurrentTime();
        Wnd_ReleaseCapture();
        m_OldPos.x = WAIT_TO_BE_SET_BACK;
    }
}

//µÿÕºªπ‘≠‘≠ º∆´“∆
void KUiMiniMap::MapMoveBack() {
    m_OldPos.x = NOT_DRAGING_MAP;
    m_nOffsetX = 0;
    m_nOffsetY = 0;
    if (g_pCoreShell) {
        g_pCoreShell->SceneMapOperation(GSMOI_SCENE_FOLLOW_WITH_MAP, 0, 0);
        g_pCoreShell->SceneMapOperation(GSMOI_SCENE_MAP_FOCUS_OFFSET, 0, 0);
    }

}

void KUiMiniMap::PaintWindow() {
    KWndImage::PaintWindow();
    if (g_pCoreShell) {
        int nX = m_MapPos.x + m_nAbsoluteLeft;
        int nY = m_MapPos.y + m_nAbsoluteTop;
        g_pCoreShell->SceneMapOperation(GSMOI_PAINT_SCENE_MAP, nX, nY);

        if (g_pRepresentShell) {
            KRURect rect;
            rect.Color.Color_dw = 0xff000000;
            rect.oEndPos.nX = rect.oPosition.nX = nX - 1;
            rect.oEndPos.nY = rect.oPosition.nY = nY - 1;
            rect.oEndPos.nZ = rect.oPosition.nZ = 0;
            rect.oEndPos.nX += m_MapSize.cx;
            rect.oEndPos.nY += m_MapSize.cy;
            g_pRepresentShell->DrawPrimitives(1, &rect, RU_T_RECT, true);
        }
    }
}

void KUiMiniMap::Breathe() {
    int hoverX,hoverY;Wnd_GetCursorPos(&hoverX,&hoverY);
    bool overMap=hoverX>=m_nAbsoluteLeft+m_MapPos.x&&hoverX<m_nAbsoluteLeft+m_MapPos.x+m_MapSize.cx&&hoverY>=m_nAbsoluteTop+m_MapPos.y&&hoverY<m_nAbsoluteTop+m_MapPos.y+m_MapSize.cy;
    KWndButton* arrows[]={&m_BtnUp,&m_BtnDown,&m_BtnLeft,&m_BtnRight};
    for(int hit=0;hit<4;++hit){int x,y,w,h;arrows[hit]->GetAbsolutePos(&x,&y);arrows[hit]->GetSize(&w,&h);if(hoverX>=x&&hoverX<x+w&&hoverY>=y&&hoverY<y+h)overMap=true;}
    bool hover=IsVisible()&&s_eMapMode!=MINIMAP_M_BRIEF_NOT_PIC&&overMap;
    for(int arrow=0;arrow<4;++arrow){if(hover)arrows[arrow]->Show();else arrows[arrow]->Hide();}

    PTCrossRoute::Update(m_MpsID);
    if(s_PtMapCoordinate){
        bool expanded=s_eMapMode==MINIMAP_M_BRIEF_PIC_BROWSE || s_eMapMode==MINIMAP_M_BRIEF_PIC_BROWSEEX;
        if(IsVisible()&&(s_eMapMode==MINIMAP_M_BRIEF_PIC || expanded)){
            int x,y;GetPosition(&x,&y);
            s_PtMapCoordinate->SetPosition(x+(expanded?400:4),y+(expanded?410:141));
            s_PtMapCoordinate->Show();
        }else s_PtMapCoordinate->Hide();
    }
    if(s_PtMiniMapLeft){
        if(IsVisible()&&s_eMapMode==MINIMAP_M_BRIEF_PIC){int x,y;GetPosition(&x,&y);s_PtMiniMapLeft->SetPosition(x-20,y+7);s_PtMiniMapLeft->Show();}
        else s_PtMiniMapLeft->Hide();
    }

    if (IS_WAIT_TO_SET_BACK && g_pCoreShell &&
        IR_IsTimePassed(MAP_RECOVER_TIME, m_uLastScrollTime)) {
        MapMoveBack();
    }
    int nCursorX, nCursorY;
    if (g_pCoreShell->GetFlagMode()) {
        Wnd_GetCursorPos(&nCursorX, &nCursorY);
        g_pCoreShell->DirectFindPos(nCursorX, nCursorY, TRUE, FALSE);
    }
}

//TamLTM change map
int changeNameMap = 1;
int saveIdMap = 0;
bool isCheckClosePostItem = false;
int countClosePostItem = 0;

//ªÓ∂Ø∫Ø ˝
void KUiMiniMap::UpdateSceneTimeInfo(KUiSceneTimeInfo *pInfo) {
    if (ms_pSelf && pInfo) {
        static KIniFile mapNames;static bool namesLoaded=false;
        if(!namesLoaded)namesLoaded=mapNames.Load("\\settings\\MapList.ini")!=0;
        char nameKey[32],sceneName[80];sprintf(nameKey,"%d_name",pInfo->nSceneId);
        mapNames.GetString("List",nameKey,pInfo->szSceneName,sceneName,sizeof(sceneName));
        const char* displayName=sceneName[0]=='$'?sceneName+1:sceneName;
        ms_pSelf->m_SceneName.SetText(displayName);
        lstrcpyn(ms_pSelf->m_szMapName,displayName,sizeof(ms_pSelf->m_szMapName));

        //TamLTM Thue suat thanh thi
        char szinfo[256];

        if (pInfo->szTongName[0] && s_eMapMode != MINIMAP_M_BRIEF_PIC) {
            sprintf(szinfo, "Thu’ su t:%d%%, Chÿ sË vÀt gi∏:%d%%", pInfo->szTongT, pInfo->szTongVG);
            ms_pSelf->m_TongMapInfo1.SetText(szinfo);

            sprintf(szinfo, "Th∏i thÛ:%s, Bang hÈi chi’m l‹nh:%s", pInfo->szTongNameBC, pInfo->szTongName);
            ms_pSelf->m_TongMapInfo2.SetText(szinfo);
        } else {
            ms_pSelf->m_TongMapInfo1.SetText("");
            ms_pSelf->m_TongMapInfo2.SetText("");

            /*	if (s_eMapMode == MINIMAP_M_BRIEF_PIC)
                {

                }
                else
                {
                    sprintf(szinfo,"Thu’ su t:0%%,Chÿ sË vÀt gi∏:0%%");
                    ms_pSelf->m_TongMapInfo1.SetText(szinfo);

                    sprintf(szinfo,"Th∏i thÛ: Ch≠a c„, Bang hÈi chi’m l‹nh: Ch≠a c„");
                    ms_pSelf->m_TongMapInfo2.SetText(szinfo);
                } // */
        }
        //end code

        ms_pSelf->m_MpsID = pInfo->nSceneId;
        PTCrossRoute::Update(pInfo->nSceneId);
        ms_pSelf->m_MpsX = pInfo->nScenePos0 / 32 / 8;
        ms_pSelf->m_MpsY = pInfo->nScenePos1 / 32 / 16;
        if(s_PtMapCoordinate)s_PtMapCoordinate->UpdatePosition(ms_pSelf->m_MpsX,ms_pSelf->m_MpsY);

        /*	//TamLTM Check post item
            changeNameMap++;
            if (changeNameMap == 2)
            {
                saveIdMap = ms_pSelf->m_MpsID;
            }

            if (saveIdMap != pInfo->nSceneId)
            {
                KUiChatItem::CloseBoxPostItem();
                changeNameMap = 1;
            }

            if (saveIdMap == pInfo->nSceneId)
            {
                if ((GetKeyState(VK_LBUTTON) & 0x8000) != 0 && isCheckClosePostItem)
                {
                    countClosePostItem = 0;
                    KUiChatItem::CloseBoxPostItem();
                }
                //Truong hop bam vao train cap ma ko bam tat post item di tu dong se close
                else if (isCheckClosePostItem)
                {
                    countClosePostItem++;

                    if (countClosePostItem == 150)
                    {
                        countClosePostItem = 0;
                        KUiChatItem::CloseBoxPostItem();
                    }
                }
            } // */

        //end code

        char Buff[16];
        if (g_pCoreShell->GetFlagMode() && g_pCoreShell->GetPaintMode()) {
            KUiSceneTimeInfoOften Spot;
            g_pCoreShell->SceneMapOperation(GSMOI_SCENE_TIME_INFO_OFTEN, (unsigned int) &Spot, 0);
            if (Spot.nScenePos0 && Spot.nScenePos1)
                sprintf(Buff, defMSG_FORMAT_SCENEPOSF, Spot.nScenePos0, Spot.nScenePos1);
            else
                sprintf(Buff, defMSG_FORMAT_SCENEPOSF, ms_pSelf->m_MpsX, ms_pSelf->m_MpsY);

            //	g_DebugLog("%d + %d ", ms_pSelf->m_MpsX, ms_pSelf->m_MpsY);
        } else {
            sprintf(Buff, defMSG_FORMAT_SCENEPOS,
                    ms_pSelf->m_MpsX, ms_pSelf->m_MpsY);
        }

        ms_pSelf->m_ScenePos.SetText(Buff);

        //	g_DebugLog("%d + %d ", ms_pSelf->m_MpsX, ms_pSelf->m_MpsY);
    }
}

//œ‘ æ¥∞ø⁄
void KUiMiniMap::Show() {
    KWndImage::Show();
    if (g_pCoreShell) {
        g_pCoreShell->SceneMapOperation(GSMOI_IS_SCENE_MAP_SHOWING,
                                        PtMiniMapElems(),
                                        (ms_pSelf->m_MapSize.cx | (ms_pSelf->m_MapSize.cy << 16)));
    }
}

//“˛≤ÿ¥∞ø⁄
void KUiMiniMap::Hide() {
    if(s_PtMapCoordinate)s_PtMapCoordinate->Hide();
    if(s_PtMiniMapLeft)s_PtMiniMapLeft->Hide();
    KWndImage::Hide();
    if (g_pCoreShell)
        g_pCoreShell->SceneMapOperation(GSMOI_IS_SCENE_MAP_SHOWING, SCENE_PLACE_MAP_ELEM_NONE, 0);
}

//TamLTM Check
bool KUiMiniMap::CloseWindowOfPostItem(bool isCheck) {
    isCheckClosePostItem = isCheck;
    return isCheck;
}
//end code

//µÿÕºæÌ∂Ø
void KUiMiniMap::MapScroll(int nbScrollScene) {
    KSceneMapInfo MapInfo;
    if (g_pCoreShell && g_pCoreShell->SceneMapOperation(GSMOI_SCENE_MAP_INFO, (unsigned int) &MapInfo, 0)) {
        int nX = MapInfo.nOrigFocusH + MapInfo.nFocusOffsetH + MapInfo.nScallH * m_nOffsetX;
        int nY = MapInfo.nOrigFocusV + MapInfo.nFocusOffsetV + MapInfo.nScallV * m_nOffsetY;
        if (nX < MapInfo.nFocusMinH)
            nX = MapInfo.nFocusMinH;
        else if (nX > MapInfo.nFocusMaxH)
            nX = MapInfo.nFocusMaxH;
        if (nY < MapInfo.nFocusMinV)
            nY = MapInfo.nFocusMinV;
        else if (nY > MapInfo.nFocusMaxV)
            nY = MapInfo.nFocusMaxV;

        nX -= MapInfo.nOrigFocusH;
        nY -= MapInfo.nOrigFocusV;

        g_pCoreShell->SceneMapOperation(GSMOI_SCENE_FOLLOW_WITH_MAP, 0, nbScrollScene);
        g_pCoreShell->SceneMapOperation(GSMOI_SCENE_MAP_FOCUS_OFFSET, (unsigned int) nX, nY);
    }
}

void MapToggleStatus() {
/**********************************************************************************
	if (s_eMapMode == MINIMAP_M_NONE || s_eMapMode == MINIMAP_M_WORLD_MAP || s_eMapMode == MINIMAP_M_CAVELIST_MAP)
		MapSetMode(s_eMapOldMode);
	else if (s_eMapMode == MINIMAP_M_BRIEF_PIC)
		MapSetMode(MINIMAP_M_BRIEF_PIC_BROWSE);
	else
		MapSetMode(s_eMapOldMode);
/**********************************************************************************/
    if (s_eMapMode == MINIMAP_M_NONE || s_eMapMode == MINIMAP_M_WORLD_MAP || s_eMapMode == MINIMAP_M_CAVELIST_MAP) {
        MapSetMode(MINIMAP_M_BRIEF_PIC);
    } else if (s_eMapMode == MINIMAP_M_BRIEF_PIC) {
        MapSetMode(MINIMAP_M_BRIEF_PIC_BROWSE);
    } else if (s_eMapMode == MINIMAP_M_BRIEF_NOT_PIC) {
    } else
        MapSetMode(MINIMAP_M_NONE);
}

MINIMAP_MODE MapGetMode() {
    return s_eMapMode;
}

//…Ë÷√œ‘ æƒ⁄»›ƒ£ Ω
void MapSetMode(MINIMAP_MODE eMode) {
    //if (eMode == s_eMapMode && (eMode < MINIMAP_M_NONE || eMode > MINIMAP_M_CAVELIST_MAP))
    if (s_eMapMode == MINIMAP_M_BRIEF_NOT_PIC && eMode != MINIMAP_M_BRIEF_PIC && eMode < MINIMAP_M_WORLD_MAP)
        return;
    if ((s_eMapMode == MINIMAP_M_WORLD_MAP || s_eMapMode == MINIMAP_M_CAVELIST_MAP) &&
        s_eMapOldMode == MINIMAP_M_BRIEF_NOT_PIC)
        eMode = MINIMAP_M_BRIEF_NOT_PIC;
    if (eMode == MINIMAP_M_BRIEF_PIC_BROWSEEX && s_eMapMode == MINIMAP_M_BRIEF_PIC_BROWSEEX)
        eMode = MINIMAP_M_BRIEF_PIC;
    if (eMode == s_eMapMode &&
        (eMode < MINIMAP_M_NONE || eMode > MINIMAP_M_WORLD_MAP || eMode > MINIMAP_M_CAVELIST_MAP))
        return;
    if (s_eMapMode != MINIMAP_M_WORLD_MAP)
        s_eMapOldMode = s_eMapMode;
    switch (s_eMapMode = eMode) {
        case MINIMAP_M_NONE:            //√ª”–œ‘ æµÿÕº
            KUiMiniMap::CloseWindow();
            KUiWorldmap::CloseWindow();
            KUiCaveList::CloseWindow();
            break;
        case MINIMAP_M_BRIEF_NOT_PIC:
        case MINIMAP_M_BRIEF_PIC:        //Àı¬‘Õº
        case MINIMAP_M_BRIEF_PIC_BROWSE://Àı¬‘Õº‰Ø¿¿/∑≈¥Ûƒ£ Ω
        case MINIMAP_M_BRIEF_PIC_BROWSEEX:
            KUiWorldmap::CloseWindow();
            KUiCaveList::CloseWindow();
            if (KUiMiniMap::GetIfVisible() == NULL)
                KUiMiniMap::OpenWindow();
            else {
                char Scheme[128];
                g_UiBase.GetCurSchemePath(Scheme, 256);
                KUiMiniMap::LoadScheme(Scheme);
            }
            break;
        case MINIMAP_M_WORLD_MAP:        // ¿ΩÁµÿÕº
            KUiMiniMap::CloseWindow();
            KUiWorldmap::OpenWindow();
            break;
        case MINIMAP_M_CAVELIST_MAP:
            KUiMiniMap::CloseWindow();
            KUiCaveList::OpenWindow();
            break;
    }
    // A mode change reloads the map's overlapping frame. Keep the coordinate
    // controls above that frame so a click in X/Y cannot place a world flag.
    if(s_eMapMode==MINIMAP_M_BRIEF_PIC || s_eMapMode==MINIMAP_M_BRIEF_PIC_BROWSE || s_eMapMode==MINIMAP_M_BRIEF_PIC_BROWSEEX){
        if(s_eMapMode==MINIMAP_M_BRIEF_PIC && s_PtMiniMapLeft)s_PtMiniMapLeft->BringToTop();
        if(s_PtMapCoordinate)s_PtMapCoordinate->BringToTop();
    }

}
