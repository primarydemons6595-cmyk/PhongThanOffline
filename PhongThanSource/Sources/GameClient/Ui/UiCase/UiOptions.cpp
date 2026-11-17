// -------------------------------------------------------------------------
//	文件名		：	UiOptions.cpp
//	创建者		：	吕桂华
//	创建时间	：	2002-9-11 15:05:15
//	功能描述	：
// -------------------------------------------------------------------------
#include "KWin32.h"
#include "../ChatDock.h"
#include "KIniFile.h"
#include "../Elem/WndMessage.h"
#include "../Elem/wnds.h"
#include "UiOptions.h"
#include "UiInit.h"
#include "PhongThanNativeUiLayout.h"
#include "../../Login/Login.h"
#include "../UiShell.h"
#include "KWin32Wnd.h"
#include "../../../Represent/iRepresent/iRepresentShell.h"
extern iRepresentShell *g_pRepresentShell;
extern bool UiCloseWndsInGame(bool);
#include "../ShortcutKey.h"
#include "../../../core/src/coreshell.h"
#include "../../../core/src/GameDataDef.h"
#include "../UiBase.h"
#include "../Elem/PopupMenu.h"
#include "../UiSoundSetting.h"
#include "../UiChatPhrase.h"
//#include "UiToiUuImage.h" // TamLTM toi uu anh

extern iCoreShell *g_pCoreShell;

#define SCHEME_INI_OPTION        "UiOptions.ini"
#define OPTIONS_SAVE_SECTION    "Options"

#define    DEFAULT_SETTING_SET        "\\Ui\\autoexec.lua"
#define    LEGEND_SETTING_SET        "\\Ui\\legendsetting.lua"

const char *ls_ToggleOptionName[OPTION_INDEX_COUNT] =
        {
                "DynamicLight",
                "Weather",
                "Perspective",
                "Quality",
        };

KUiOptions *KUiOptions::m_pSelf = NULL;

extern int g_bRepresent3;
extern int g_bScreen;

KUiOptions::KUiOptions() {
    m_pReturn = NULL;
    m_Quality = 0; m_Resolution = 1;
    m_nToggleItemCount = 0;
    m_pSkinMenu = NULL;
    m_nBrightness = -1;
    m_nSoundValue = -1;
    m_nMusicValue = -1;
    m_nShortcutSet = -1;
    m_nFirstControlableIndex = 0;
    m_nToggleBtnValidCount = 0;
    m_nToggleItemCount = 0;
    m_nStatusEnableFrame = m_nStatusDisableFrame = m_nStatusInvalidFrame = 0;
    for (int i = 0; i < OPTION_INDEX_COUNT; i++) {
        m_ToggleItemList[i].szName[0] = 0;
    }
    m_ToggleItemList[OPTION_I_DYNALIGHT].bInvalid = !g_bRepresent3;
    m_ToggleItemList[OPTION_I_WEATHER].bInvalid = false;
    m_ToggleItemList[OPTION_I_PERSPECTIVE].bInvalid = !g_bRepresent3;
    m_ToggleItemList[OPTION_I_QUALITY].bInvalid = false;
//	m_ToggleItemList[OPTION_I_OTHER].bInvalid = true;// TamLTM toi uu
}

void KUiOptions::CancelMenu() {
    if (m_pSkinMenu) {
        KPopupMenu::Cancel();
        free(m_pSkinMenu);
        m_pSkinMenu = NULL;
    }
}

//--------------------------------------------------------------------------
//	功能：如果窗口正被显示，则返回实例指针
//--------------------------------------------------------------------------
KUiOptions *KUiOptions::GetIfVisible() {
    if (m_pSelf && m_pSelf->IsVisible())
        return m_pSelf;
    return NULL;
}

//--------------------------------------------------------------------------
//	功能：打开窗口，返回唯一的一个类对象实例
//--------------------------------------------------------------------------
KUiOptions *KUiOptions::OpenWindow(KWndWindow *pReturn) {
    if (m_pSelf == NULL) {
        m_pSelf = new KUiOptions;
        if (m_pSelf)
            m_pSelf->Initialize();
    }
    if (m_pSelf) {
        m_pSelf->m_pReturn = pReturn;
        LoadSetting(true, false);
        UiSoundPlay(UI_SI_POPUP_OUTGAME_WND);
        m_pSelf->Show();
        m_pSelf->BringToTop();
    }

    return m_pSelf;
}

//--------------------------------------------------------------------------
//	功能：关闭窗口
//--------------------------------------------------------------------------
static int s_PendingResolution = -1;

void KUiOptions::CloseWindow() {
    if (m_pSelf) {
        KPopupMenu::Cancel();
        m_pSelf->CancelMenu();
        s_PendingResolution = m_pSelf->m_Resolution;
        m_pSelf->StoreSetting();
        if (m_pSelf->m_pReturn) m_pSelf->m_pReturn->Show();
        m_pSelf->Hide();
        m_pSelf->Destroy();
        m_pSelf = NULL;
    }
}

// Recreate the render surface and HUD only between UI traversals. Destroy()
// queues deletion; rebuilding the HUD inside a button callback can reuse
// windows that still belong to the old UI tree.
void UiApplyPendingResolution() {
    UiApplyPendingChatDock();
    if (s_PendingResolution < 0) return;
    UiCloseChatDock();UiApplyPendingChatDock();
    int resolution = s_PendingResolution; s_PendingResolution = -1;
    const int widths[] = {800, 1024, 1920}, heights[] = {600, 768, 1080};
    if (resolution > 2) resolution = 1;
    int width, height; Wnd_GetScreenSize(width, height);
    int wantedWidth = widths[resolution], wantedHeight = heights[resolution];
    if (width == wantedWidth && height == wantedHeight) return;
    if (!g_pRepresentShell) return;
    bool inGame = g_LoginLogic.GetStatus() == LL_S_IN_GAME;
    if (inGame) { UiCloseWndsInGame(true); Wnd_Heartbeat(); }
    if (g_pRepresentShell->Reset(wantedWidth, wantedHeight, g_bScreen != 0)) {
        UiSetScreenSize(wantedWidth, wantedHeight);
        if (g_pCoreShell) g_pCoreShell->SetRepresentAreaSize(wantedWidth, wantedHeight);
        KIniFile ini; ini.Load("\\config.ini");
        ini.WriteInteger("Client", "ScreenWidth", wantedWidth);
        ini.WriteInteger("Client", "ScreenHeight", wantedHeight); ini.Save("\\config.ini");
    } else g_pRepresentShell->Reset(width, height, g_bScreen != 0);
    if (g_pCoreShell) g_pCoreShell->SetRepresentShell(g_pRepresentShell);
    g_UiBase.ReloadNativeLayout();
    if (inGame) UiStartGame();
}

// -------------------------------------------------------------------------
// 功能	: 初始化
// -------------------------------------------------------------------------
void KUiOptions::Initialize() {
    AddChild(&m_CloseBtn);
    //AddChild(&m_SkinBtn);
    AddChild(&m_BrightnessScroll);
    AddChild(&m_BGMValue);
    AddChild(&m_SoundValue);
    AddChild(&m_ShortcutSetView);
    AddChild(&m_QualitySelect); AddChild(&m_ResolutionSelect); AddChild(&m_MaxPlayerCount);
//	AddChild(&m_ToiUuBtn);// TamLTM Toi Uu hinh anh

    for (int i = 0; i < MAX_TOGGLE_BTN_COUNT; i++) {
        AddChild(&m_ToggleBtn[i]);
        AddChild(&m_StatusImage[i]);
    }

    char Scheme[256];
    g_UiBase.GetCurSchemePath(Scheme, 256);
    m_pSelf->LoadScheme(Scheme);

    // 注册本窗口
    // Options must share the login top layer; BringToTop only reorders siblings.
    Wnd_AddWindow(this, WL_TOPMOST);
}

// -------------------------------------------------------------------------
// 功能	: 载入界面方案
// -------------------------------------------------------------------------
void KUiOptions::LoadScheme(const char *pScheme) {
    char Buff[128];
    KIniFile Ini;
    sprintf(Buff, "%s\\%s", pScheme, SCHEME_INI_OPTION);
    if (m_pSelf && Ini.Load(Buff)) {
        m_pSelf->LoadScheme(&Ini);
    }
}

//载入界面方案
void KUiOptions::LoadScheme(KIniFile *pIni) {
//	assert(pIni);

    int width, height, dx, dy;
    PhongThanGetNativeUiMetrics(width, height, dx, dy);
    PhongThanCenterIniWindow(*pIni, "Main", dx, dy);
    Init(pIni, "Main");    // 窗口背景数据
    m_CloseBtn.Init(pIni, "CloseBtn");
    m_ShortcutKeyBtn.Init(pIni, "ShortcutKeyBtn");
//	m_SkinBtn.Init(pIni, "SkinBtn");

    m_BrightnessScroll.Init(pIni, "Brightness");
    m_BGMValue.Init(pIni, "Music");
    m_SoundValue.Init(pIni, "Sound");
    // The original resolution dropdown occupies this row. The JX shortcut
    // selector was erroneously rendered over it.
    m_ShortcutSetView.LeaveAlone();
    const char *sections[] = { "CmbAniQuality", "Resolution" };
    KWndLabeledButton *buttons[] = { &m_QualitySelect, &m_ResolutionSelect };
    for (int b = 0; b < 2; ++b) {
        char section[64], image[128]; sprintf(section, "%s_Btn", sections[b]);
        pIni->GetString(section, "Image", "", image, sizeof(image));
        pIni->WriteString(sections[b], "Image", image);
        pIni->WriteString(sections[b], "Color", "218,255,165");
        pIni->WriteInteger(sections[b], "Font", 14);
        pIni->WriteInteger(sections[b], "LabelXOffset", 3);
        pIni->WriteInteger(sections[b], "Up", 0); pIni->WriteInteger(sections[b], "Down", 1);
        buttons[b]->Init(pIni, sections[b]);
    }
    m_MaxPlayerCount.Init(pIni, "MaxPlayerCount");
    KIniFile *graphics = g_UiBase.GetCommSettingFile();
    int maxPlayers = 255;
    if (graphics) {
        graphics->GetInteger("Options", "AnimationQuality", 0, &m_Quality);
        graphics->GetInteger("Options", "MaxPlayerCount", 255, &maxPlayers);
        g_UiBase.CloseCommSettingFile(false);
    }
    if (m_Quality < 0 || m_Quality > 3) m_Quality = 0;
    char qualityName[32], key[32]; sprintf(key, "AniLevelName_%d", m_Quality);
    pIni->GetString("CmbAniQuality", key, "", qualityName, sizeof(qualityName));
    m_QualitySelect.SetLabel(qualityName[0] == '$' ? qualityName + 1 : qualityName);
    m_Resolution = width >= 1920 ? 2 : (width > 800 ? 1 : 0);
    m_ResolutionSelect.SetLabel(m_Resolution == 2 ? "1920*1080" : (m_Resolution ? "1024*768" : "800*600"));
    m_MaxPlayerCount.SetScrollPos(maxPlayers);
    m_Scroll.Init(pIni, "Scroll");

    //TamLTM them toi uu hinh anh
//	m_ToiUuBtn.Init(pIni, "ToiUu");
//	m_ToiUuBtn.SetText("T鑙 璾");
    //end code

    m_StatusImage[0].Init(pIni, "ToggleStatus");
    pIni->GetInteger("ToggleStatus", "NotCheckFrame", 0, &m_nStatusDisableFrame);
    pIni->GetInteger("ToggleStatus", "CheckFrame", 0, &m_nStatusEnableFrame);
    pIni->GetInteger("ToggleStatus", "DisableFrame", 0, &m_nStatusInvalidFrame);

    char Buff[32];
    pIni->GetString("ToggleBtn", "Color", "", Buff, sizeof(Buff));
    m_uEnableTextColor = GetColor(Buff);
    pIni->GetString("ToggleBtn", "SelectColor", "173,174,173", Buff, sizeof(Buff));
    m_uInvalidTextColor = GetColor(Buff);
    pIni->GetString("ToggleBtn", "DisableColor", "173,174,173", Buff, sizeof(Buff));
    m_uDisableTextColor = GetColor(Buff);

    int nIntervalH, nIntervalV, nCountPerLine;
    m_ToggleBtn[0].Init(pIni, "ToggleBtn");
    pIni->GetInteger2("ToggleBtn", "Interval", &nIntervalH, &nIntervalV);
    pIni->GetInteger("ToggleBtn", "CountPerLine", 1, &nCountPerLine);
    int i, nBtnX, nBtnY, nImgX, nImgY;
    m_ToggleBtn[0].GetPosition(&nBtnX, &nBtnY);
    m_StatusImage[0].GetPosition(&nImgX, &nImgY);
    if (nCountPerLine < 1)
        nCountPerLine = 1;
    for (i = 1; i < MAX_TOGGLE_BTN_COUNT; i++) {
        int dx = (i % nCountPerLine) * nIntervalH;
        int dy = (i / nCountPerLine) * nIntervalV;
        m_ToggleBtn[0].Clone(&m_ToggleBtn[i]);
        m_ToggleBtn[i].SetPosition(nBtnX + dx, nBtnY + dy);
        m_StatusImage[0].Clone(&m_StatusImage[i]);
        m_StatusImage[i].SetPosition(nImgX + dx, nImgY + dy);
    }

    for (i = 0; i < OPTION_INDEX_COUNT; i++) {
        char szKey[16];
        pIni->GetString("ToggleOptionsName", itoa(i, szKey, 10),
                        "", m_ToggleItemList[i].szName, sizeof(m_ToggleItemList[i].szName));
    }

    m_nFirstControlableIndex = 0;
    m_nToggleBtnValidCount = 3;
    UpdateAllToggleBtn();
    UpdateAllStatusImg();
}

// -------------------------------------------------------------------------
// 功能	: 窗口函数
// -------------------------------------------------------------------------
int KUiOptions::WndProc(unsigned int uMsg, unsigned int uParam, int nParam) {
    int nRet = 0;

    switch (uMsg) {
        case WND_N_SCORLLBAR_POS_CHANGED:
            OnScrollBarPosChanged((KWndWindow *) uParam, nParam);
            break;
        case WND_N_BUTTON_CLICK:
            if (uParam == (unsigned int) (KWndWindow *) &m_CloseBtn)
                CloseWindow();
                //else if (uParam == (unsigned int)(KWndWindow*)&m_SkinBtn)
                //	PopupSkinMenu();
            else if ((KWndWindow*)uParam == &m_QualitySelect) PopupGraphicsMenu(false);
            else if ((KWndWindow*)uParam == &m_ResolutionSelect) PopupGraphicsMenu(true);
            else if (uParam == (unsigned int) (KWndWindow *) &m_ShortcutSetView) {
                int nX, nY, nWidth, nHeight;
                m_ShortcutSetView.GetPosition(&nX, &nY);
                m_ShortcutSetView.GetSize(&nWidth, &nHeight);
                nX += nWidth + m_nAbsoluteLeft;
                nY += nHeight + m_nAbsoluteTop;
                PopupSeleteSetMenu(nX, nY);
            }
                //TamLTM Them toi uu hinh anh
                //	else if (uParam == (unsigned int)(KWndWindow*)&m_ToiUuBtn)
                //	{
                //	KUiToiUuImage::OpenWindow(); // Them mo toi uu hinh anh game
                //	}
                //End code
            else {
                for (int i = 0; i < m_nToggleBtnValidCount; i++) {
                    if (uParam == (unsigned int) (KWndWindow *) &m_ToggleBtn[i]) {
                        ToggleOption(m_nFirstControlableIndex + i);
                    }
                }
            }
            break;
        case WM_KEYDOWN:
            if (uParam == VK_ESCAPE)
                CloseWindow();
            nRet = true;
            break;
        case WND_M_MENUITEM_SELECTED:
            if (uParam == (unsigned int) (KWndWindow *) this) {
                if (HIWORD(nParam) == 10 || HIWORD(nParam) == 11) {
                    int choice = (short)LOWORD(nParam);
                    bool resolution = HIWORD(nParam) == 11;
                    if (choice >= 0 && choice < (resolution ? 3 : 4)) {
                        if (resolution) {
                            m_Resolution = choice;
                            m_ResolutionSelect.SetLabel(choice == 2 ? "1920*1080" : (choice ? "1024*768" : "800*600"));
                        } else {
                            m_Quality = choice;
                            if (g_pCoreShell) g_pCoreShell->OperationRequest(GOI_OPTION_SETTING, OPTION_ANIMATION_QUALITY, choice);
                            KIniFile ini; char path[256], key[32], name[32];
                            g_UiBase.GetCurSchemePath(path, sizeof(path)); strcat(path, "\\UiOptions.ini");
                            ini.Load(path); sprintf(key, "AniLevelName_%d", choice);
                            ini.GetString("CmbAniQuality", key, "", name, sizeof(name));
                            m_QualitySelect.SetLabel(name[0] == '$' ? name + 1 : name);
                        }
                    }
                    KPopupMenu::Cancel();
                } else if (HIWORD(nParam) == 1) {
                    if (short(LOWORD(nParam)) >= 0) {
                        m_nShortcutSet = LOWORD(nParam);
                        UpdateSettingSet(LOWORD(nParam), false);
                    }
                    KPopupMenu::Cancel();
                } else {
                    CancelMenu();
                }
                //if (m_pSkinMenu && short(LOWORD(nParam)) >= 0 && LOWORD(nParam) < m_pSkinMenu->nNumItem)
                //{
                //	if (g_UiBase.LoadScheme(m_pSkinMenu->Items[LOWORD(nParam)].szData))
                //		m_SkinBtn.SetLabel(m_pSkinMenu->Items[LOWORD(nParam)].szData);
                //}
            }
            break;
        default:
            nRet = KWndWindow::WndProc(uMsg, uParam, nParam);
            break;
    }
    return nRet;
}

void KUiOptions::UpdateSettingSet(int eSet, bool bOnlyUpdateUi) {
//这部分是根据CheckBox来做的
/*	if (eSet < SETTING_DEFAULT || eSet > SETTING_LEGEND)
		return;
	m_eSettingSet = eSet;
	m_IDefault.CheckButton(m_eSettingSet == SETTING_DEFAULT);
//	m_IMu.CheckButton(m_eSettingSet== SETTING_MU);
	m_ILegend.CheckButton(m_eSettingSet == SETTING_LEGEND);
	if (bOnlyUpdateUi == false)
	{
		KShortcutKeyCentre::ClearScript();
		const char* pszFileName;
//		if (m_eSettingSet== SETTING_MU)
//			pszFileName = MU_SETTING_SET;
		if (m_eSettingSet== SETTING_LEGEND)
			pszFileName = LEGEND_SETTING_SET;
		else
			pszFileName = DEFAULT_SETTING_SET;
		KShortcutKeyCentre::LoadScript((char*)pszFileName);
	}*/

    //这部分是按照按钮和弹出式菜单来做的，更新按钮和载入新配置
    KIniFile *pSetting = g_UiBase.GetCommConfigFile();
    if (pSetting) {
        int nCount;
        char szKey[16], szBuff[128];

        pSetting->GetInteger("ShortcutSet", "Count", 0, &nCount);
        if (eSet < 0 || eSet >= nCount)
            eSet = 0;
        m_nShortcutSet = eSet;

        sprintf(szKey, "%d_Name", eSet);
        pSetting->GetString("ShortcutSet", szKey, "", szBuff, sizeof(szBuff));
        m_ShortcutSetView.SetLabel(szBuff);

        if (bOnlyUpdateUi == false) {
            sprintf(szKey, "%d_File", eSet);
            pSetting->GetString("ShortcutSet", szKey, "", szBuff, sizeof(szBuff));
            if (szBuff[0]) {
                KShortcutKeyCentre::ClearScript();
                KShortcutKeyCentre::LoadScript(szBuff);
            }
        }
        g_UiBase.CloseCommConfigFile();
    }
}

// -------------------------------------------------------------------------
// 功能	: 弹出选择配置方案的菜单
// -------------------------------------------------------------------------
void KUiOptions::PopupSeleteSetMenu(int nX, int nY) {
    KIniFile *pSetting = g_UiBase.GetCommConfigFile();
    if (pSetting) {
        char szKey[16];
        int nCount, i;
        pSetting->GetInteger("ShortcutSet", "Count", 0, &nCount);
        if (nCount > 0) {
            KPopupMenuData *pMenuData = (KPopupMenuData *) malloc(MENU_DATA_SIZE(nCount));
            if (pMenuData) {
                KPopupMenu::InitMenuData(pMenuData, nCount);
                for (i = 0; i < nCount; i++) {
                    sprintf(szKey, "%d_Name", i);
                    pSetting->GetString("ShortcutSet", szKey, "", pMenuData->Items[i].szData,
                                        sizeof(pMenuData->Items[i].szData));
                    pMenuData->Items[i].uDataLen = strlen(pMenuData->Items[i].szData);
                }
                pMenuData->nX = nX;
                pMenuData->nY = nY;
                KPopupMenu::Popup(pMenuData, (KWndWindow *) this, 1);
            }
        }
        g_UiBase.CloseCommConfigFile();
    }
}


/*void KUiOptions::PopupSkinMenu()
{
	if (m_pSkinMenu)
		return;

	int nCount = g_UiBase.SchemeCount();
	if (nCount <= 0)
		return;
	m_pSkinMenu = (KPopupMenuData*)malloc(MENU_DATA_SIZE(nCount));
	if (m_pSkinMenu == NULL)
		return;

	KPopupMenu::InitMenuData(m_pSkinMenu, nCount);
	for (int i = 0; i < nCount; i++)
	{
		g_UiBase.GetScheme(i, m_pSkinMenu->Items[i].szData, NULL);
		m_pSkinMenu->Items[i].uDataLen = strlen(m_pSkinMenu->Items[i].szData);
	}
	int x, y;
	m_SkinBtn.GetPosition(&x, &y);
	m_pSkinMenu->nX = x + m_nAbsoluteLeft;
	m_pSkinMenu->nY = y + m_nAbsoluteTop;
	KPopupMenu::Popup(m_pSkinMenu, (KWndWindow*)this, 0);
}*/

// -------------------------------------------------------------------------
// 功能	: 响应滚动条被拖动
// -------------------------------------------------------------------------
void KUiOptions::OnScrollBarPosChanged(KWndWindow *pWnd, int nPos) {
    if (pWnd == (KWndWindow *) &m_BGMValue)
        SetMusicValue(nPos);
    else if (pWnd == (KWndWindow *) &m_SoundValue)
        SetSoundValue(nPos);
    else if (pWnd == (KWndWindow *) &m_BrightnessScroll)
        SetBrightness(nPos);
    else if (pWnd == &m_MaxPlayerCount) {
        if (g_pCoreShell) g_pCoreShell->OperationRequest(GOI_OPTION_SETTING, OPTION_MAX_VISIBLE_PLAYERS, nPos);
    }
    else if (pWnd == (KWndWindow *) &m_Scroll) {
        if (m_nToggleItemCount < m_nToggleItemCount &&
            nPos != m_nFirstControlableIndex &&
            nPos < m_nToggleItemCount && nPos >= 0) {
            m_nFirstControlableIndex = nPos;
            m_nToggleBtnValidCount = m_nToggleItemCount - m_nFirstControlableIndex;
            if (m_nToggleBtnValidCount > MAX_TOGGLE_BTN_COUNT)
                m_nToggleBtnValidCount = MAX_TOGGLE_BTN_COUNT;
            UpdateAllToggleBtn();
            UpdateAllStatusImg();
        }
    }
}

void KUiOptions::UpdateAllToggleBtn() {
    int i = 0;
    for (; i < m_nToggleBtnValidCount; i++) {
        m_ToggleBtn[i].Show();
        m_StatusImage[i].Show();
        int nIndex = m_nFirstControlableIndex + i;
        m_ToggleBtn[i].SetLabel(m_ToggleItemList[nIndex].szName);
        if (m_ToggleItemList[nIndex].bInvalid == false) {
            m_ToggleBtn[i].Enable(true);
            m_ToggleBtn[i].SetLabelColor(m_ToggleItemList[nIndex].bEnable ?
                                         m_uEnableTextColor : m_uInvalidTextColor);
        } else {
            m_ToggleBtn[i].Enable(false);
            m_ToggleBtn[i].SetLabelColor(m_uDisableTextColor);
        }
    }
    for (; i < MAX_TOGGLE_BTN_COUNT; i++) {
        m_ToggleBtn[i].Hide();
        m_StatusImage[i].Hide();
    }
    m_StatusImage[OPTION_I_QUALITY].SetFrame(m_nStatusEnableFrame);
    m_ToggleBtn[OPTION_I_QUALITY].SetLabelColor(m_uEnableTextColor);
}

void KUiOptions::UpdateAllStatusImg() {
    int nFrame;
    for (int i = 0; i < m_nToggleBtnValidCount; i++) {
        if (m_ToggleItemList[m_nFirstControlableIndex + i].bInvalid == false) {
            nFrame = m_ToggleItemList[m_nFirstControlableIndex + i].bEnable ?
                     m_nStatusEnableFrame : m_nStatusDisableFrame;
        } else {
            nFrame = m_nStatusInvalidFrame;
        }
        m_StatusImage[i].SetFrame(nFrame);
    }
}

// -------------------------------------------------------------------------
// 功能	: 更新数据
// -------------------------------------------------------------------------
int KUiOptions::LoadSetting(bool bReload, bool bUpdateOption) {
    int nBrightness = 50;
    int nSoundValue = 100;
    int nMusicValue = 100;
    int nSettingSet = 0;
    int i;

    int bOptionsEnable[OPTION_INDEX_COUNT] =
            {
                    true, true, true, true
            };

    if (bReload == false && m_pSelf) {
        KIniFile *pSetting = g_UiBase.GetCommSettingFile();
        if (pSetting) {
            if (m_pSelf->m_nBrightness == -1)
                pSetting->GetInteger(OPTIONS_SAVE_SECTION, "Brightness", 50, &m_pSelf->m_nBrightness);
            if (m_pSelf->m_nSoundValue == -1)
                pSetting->GetInteger(OPTIONS_SAVE_SECTION, "SoundValue", 100, &m_pSelf->m_nSoundValue);
            if (m_pSelf->m_nMusicValue == -1)
                pSetting->GetInteger(OPTIONS_SAVE_SECTION, "MusicValue", 100, &m_pSelf->m_nMusicValue);
            if (m_pSelf->m_nShortcutSet == -1)
                pSetting->GetInteger(OPTIONS_SAVE_SECTION, "ShortcutSet", 0, &m_pSelf->m_nShortcutSet);
            g_UiBase.CloseCommSettingFile(false);
        }

        nBrightness = m_pSelf->m_nBrightness;
        nSoundValue = m_pSelf->m_nSoundValue;
        nMusicValue = m_pSelf->m_nMusicValue;
        nSettingSet = m_pSelf->m_nShortcutSet;

        for (i = 0; i < OPTION_INDEX_COUNT; i++)
            bOptionsEnable[i] = m_pSelf->m_ToggleItemList[i].bEnable;
    } else {
        KIniFile *pSetting = g_UiBase.GetCommSettingFile();
        if (pSetting) {
            pSetting->GetInteger(OPTIONS_SAVE_SECTION, "Brightness", 50, &nBrightness);
            pSetting->GetInteger(OPTIONS_SAVE_SECTION, "MusicValue", 100, &nMusicValue);
            pSetting->GetInteger(OPTIONS_SAVE_SECTION, "SoundValue", 100, &nSoundValue);
            pSetting->GetInteger(OPTIONS_SAVE_SECTION, "ShortcutSet", 0, &nSettingSet);
            for (i = 0; i < OPTION_INDEX_COUNT; i++)
                pSetting->GetInteger(OPTIONS_SAVE_SECTION, ls_ToggleOptionName[i], true, &bOptionsEnable[i]);
            g_UiBase.CloseCommSettingFile(false);
        }
    }
    if (g_bRepresent3 == false) {
        bOptionsEnable[OPTION_I_DYNALIGHT] = false;
        bOptionsEnable[OPTION_I_PERSPECTIVE] = false;
    }
    //TamLTM Fix option
    bOptionsEnable[OPTION_I_WEATHER] = false;

    if (bUpdateOption && g_pCoreShell) {
        KIniFile *visual = g_UiBase.GetCommSettingFile();
        int quality = 0, maxPlayers = 255;
        if (visual) {
            visual->GetInteger("Options", "AnimationQuality", 0, &quality);
            visual->GetInteger("Options", "MaxPlayerCount", 255, &maxPlayers);
            g_UiBase.CloseCommSettingFile(false);
        }
        g_pCoreShell->OperationRequest(GOI_OPTION_SETTING, OPTION_ANIMATION_QUALITY, quality);
        g_pCoreShell->OperationRequest(GOI_OPTION_SETTING, OPTION_MAX_VISIBLE_PLAYERS, maxPlayers);

        if (g_bRepresent3) {
            g_pCoreShell->OperationRequest(GOI_OPTION_SETTING, OPTION_PERSPECTIVE,
                                           bOptionsEnable[OPTION_I_PERSPECTIVE]);
            g_pCoreShell->OperationRequest(GOI_OPTION_SETTING, OPTION_DYNALIGHT, bOptionsEnable[OPTION_I_DYNALIGHT]);
            g_pCoreShell->OperationRequest(GOI_OPTION_SETTING, OPTION_BRIGHTNESS, nBrightness);
        }

        g_pCoreShell->OperationRequest(GOI_OPTION_SETTING, OPTION_MUSIC_VALUE, nMusicValue);
        g_pCoreShell->OperationRequest(GOI_OPTION_SETTING, OPTION_SOUND_VALUE, nSoundValue);
        g_pCoreShell->OperationRequest(GOI_OPTION_SETTING, OPTION_WEATHER, bOptionsEnable[OPTION_I_WEATHER]);
        //	g_pCoreShell->OperationRequest(GOI_OPTION_SETTING, OPTION_OTHER, bOptionsEnable[OPTION_I_OTHER]);//TamLTM them option toi uu hinh anh
    }

    if (m_pSelf) {
        if (bReload) {
            for (i = 0; i < OPTION_INDEX_COUNT; i++)
                m_pSelf->m_ToggleItemList[i].bEnable = bOptionsEnable[i];
            m_pSelf->m_nShortcutSet = nSettingSet;
            m_pSelf->m_nMusicValue = nMusicValue;
            m_pSelf->m_nSoundValue = nSoundValue;
            m_pSelf->m_nBrightness = nBrightness;
            m_pSelf->m_BrightnessScroll.SetScrollPos(nBrightness);
            m_pSelf->m_BGMValue.SetScrollPos(nMusicValue);
            m_pSelf->m_SoundValue.SetScrollPos(nSoundValue);
            m_pSelf->UpdateSettingSet(nSettingSet, true);
            m_pSelf->UpdateAllToggleBtn();
            m_pSelf->UpdateAllStatusImg();
        }

        m_pSelf->m_BrightnessScroll.Enable(g_bRepresent3 && g_bScreen);
    }
    return nMusicValue;
}

void KUiOptions::StoreSetting() {
    KIniFile *pSetting = NULL;
    pSetting = g_UiBase.GetCommSettingFile();

    if (pSetting) {
        for (int i = 0; i < OPTION_INDEX_COUNT; i++) {
            pSetting->WriteInteger(OPTIONS_SAVE_SECTION, ls_ToggleOptionName[i],
                                   m_ToggleItemList[i].bEnable);
        }
        pSetting->WriteInteger(OPTIONS_SAVE_SECTION, ls_ToggleOptionName[OPTION_I_QUALITY],
                               true);
        pSetting->WriteInteger(OPTIONS_SAVE_SECTION, "AnimationQuality", m_Quality);
        pSetting->WriteInteger(OPTIONS_SAVE_SECTION, "MaxPlayerCount", m_MaxPlayerCount.GetScrollPos());
        pSetting->WriteInteger(OPTIONS_SAVE_SECTION, "Brightness", m_nBrightness);
        pSetting->WriteInteger(OPTIONS_SAVE_SECTION, "MusicValue", m_nMusicValue);
        pSetting->WriteInteger(OPTIONS_SAVE_SECTION, "SoundValue", m_nSoundValue);
        pSetting->WriteInteger(OPTIONS_SAVE_SECTION, "ShortcutSet", m_nShortcutSet);
    }
    g_UiBase.CloseCommSettingFile(true);
}

//切换开关型选项
void KUiOptions::ToggleOption(int nIndex) {
    if (nIndex < OPTION_I_START || nIndex >= OPTION_INDEX_COUNT)
        return;
    if (m_ToggleItemList[nIndex].bInvalid)
        return;
    bool bEnable = m_ToggleItemList[nIndex].bEnable = !m_ToggleItemList[nIndex].bEnable;

    switch (nIndex) {
        case OPTION_I_DYNALIGHT:    //动态光影
            if (g_pCoreShell)
                g_pCoreShell->OperationRequest(GOI_OPTION_SETTING, OPTION_DYNALIGHT, bEnable);
            break;
        case OPTION_I_WEATHER:        //天气开关
            if (g_pCoreShell)
                g_pCoreShell->OperationRequest(GOI_OPTION_SETTING, OPTION_WEATHER, bEnable);
            break;
        case OPTION_I_PERSPECTIVE:    //透视模式
            if (g_pCoreShell)
                g_pCoreShell->OperationRequest(GOI_OPTION_SETTING, OPTION_PERSPECTIVE, bEnable);
            break;
            // TamLTM thme case toi uu hinh anh
            /*	case OPTION_I_OTHER:	//透视模式
                    if (g_pCoreShell)
                        g_pCoreShell->OperationRequest(GOI_OPTION_SETTING, OPTION_OTHER, bEnable);
                    break;*/
    }
    //end code

    UpdateAllStatusImg();
    UpdateAllToggleBtn();
}

void KUiOptions::SetMusicValue(int n) {
    if (m_nMusicValue != n) {
        m_nMusicValue = n;
        g_pCoreShell->OperationRequest(GOI_OPTION_SETTING, OPTION_MUSIC_VALUE, m_nMusicValue);
    }
}

void KUiOptions::SetSoundValue(int n) {
    if (m_nSoundValue != n) {
        m_nSoundValue = n;
        g_pCoreShell->OperationRequest(GOI_OPTION_SETTING, OPTION_SOUND_VALUE, m_nSoundValue);
    }
}

void KUiOptions::SetBrightness(int n) {
    if (g_bRepresent3) {
        if (m_nBrightness != n) {
            m_nBrightness = n;
            g_pCoreShell->OperationRequest(GOI_OPTION_SETTING, OPTION_BRIGHTNESS, m_nBrightness);
        }
    }
}

void KUiOptions::PopupGraphicsMenu(bool resolution) {
    int count = resolution ? 3 : 4;
    KPopupMenuData *menu = (KPopupMenuData*)malloc(MENU_DATA_SIZE(count));
    if (!menu) return;
    KPopupMenu::InitMenuData(menu, count);
    menu->uTextColor = 0xffdaffa5; menu->uSelTextColor = 0xffffffff;
    menu->uTextBorderColor = menu->uSelTextBorderColor = 0xff000000;
    menu->uSelItemBgColor = 0x00605c40;
    KIniFile ini; char path[256]; g_UiBase.GetCurSchemePath(path, sizeof(path));
    strcat(path, "\\UiOptions.ini"); ini.Load(path);
    for (int i = 0; i < count; ++i) {
        char key[32], name[64];
        sprintf(key, resolution ? "Resolution_%d" : "AniLevelName_%d", i);
        ini.GetString(resolution ? "Resolution" : "CmbAniQuality", key, "", name, sizeof(name));
        strcpy(menu->Items[i].szData, name[0] == '$' ? name + 1 : name);
        menu->Items[i].uDataLen = strlen(menu->Items[i].szData);
    }
    int x, y, width, height;
    KWndLabeledButton *button = resolution ? &m_ResolutionSelect : &m_QualitySelect;
    button->GetAbsolutePos(&x, &y); button->GetSize(&width, &height);
    menu->nX = x; menu->nY = y + height; KPopupMenu::Popup(menu, this, resolution ? 11 : 10);
}
