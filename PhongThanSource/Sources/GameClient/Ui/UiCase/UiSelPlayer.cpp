// -------------------------------------------------------------------------
//	ÎÄ¼þÃû		£º	UiSelPlayer.cpp
//	´´½¨Õß		£º	ÂÀ¹ð»ª
//	´´½¨Ê±¼ä	£º	2002-9-10 14:24:12
//	¹¦ÄÜÃèÊö	£º
// -------------------------------------------------------------------------
#include "KWin32.h"
#include "KIniFile.h"
#include "KTabFile.h"
#include <time.h>
#include "../Elem/WndMessage.h"
#include "../elem/wnds.h"
#include "UiSelPlayer.h"
#include "UiNewPlayer.h"
#include "UiSelServer.h"
#include "UiLoginbg.h"
#include "UiConnectInfo.h"
#include "PhongThanNativeUiLayout.h"
#include "../UiSoundSetting.h"
#include "../UiBase.h"
#include "../UiShell.h"
#include "KDebug.h"
#include "../../Login/Login.h"
#include "../../Core/Src/GameDataDef.h"
#include "../../../Represent/iRepresent/iRepresentShell.h"
#include <crtdbg.h>

extern iRepresentShell *g_pRepresentShell;

#define    SCHEME_INI_SELPLAYER    "UiSelPlayer.ini"
#define    VNG_PAK_UI_SELPLAYER    0x4746029EUL
#define    PROFESSION_INI                "\\Ui\\Profession.ini"
#define    LIFE_TIME_STRING_ID        "25"
#define    LIFE_TIME_DAY_ID        "26"
#define    LIFE_TIME_HOUR_ID        "27"
#define    LIFE_TIME_MINUTE_ID        "28"
#define    LIFE_TIME_SECOND_ID        "29"

const char *PROFESSION_GetTitleString(int nProfession);

KUiSelPlayer *KUiSelPlayer::m_pSelf = NULL;

//--------------------------------------------------------------------------
//	¹¦ÄÜ£ºÎö¹¹º¯Êý
//--------------------------------------------------------------------------
// Rows that fit the SelPlayer list window (PlayListWnd height / row
// spacing of the shipped UiSelPlayer.ini). Pages follow the selection;
// PageUp / PageDown flip pages directly.
#define PT_SELPLAYER_PAGE_SIZE 8

KUiSelPlayer::KUiSelPlayer() {
    m_nListPage = 0;
    m_nRowBaseX = 0;
    m_nRowBaseY = 0;
    m_nRowSpacing = 58;
    m_nSelPlayer = -1;
    m_nNumPlayer = 0;
    m_szLoginBg[0] = 0;
    m_szPlayerImgPrefix[0] = 0;
    m_bJustClicked = false;
    memset(m_szSelPlayerSound, 0, sizeof(m_szSelPlayerSound));
}

//--------------------------------------------------------------------------
//	¹¦ÄÜ£º´ò¿ª´°¿Ú£¬·µ»ØÎ¨Ò»µÄÒ»¸öÀà¶ÔÏóÊµÀý
//--------------------------------------------------------------------------
KUiSelPlayer *KUiSelPlayer::OpenWindow() {
    if (m_pSelf == NULL) {
        m_pSelf = new KUiSelPlayer;
        if (m_pSelf)
            m_pSelf->Initialize();
    }
    if (m_pSelf) {
        UiSoundPlay(UI_SI_POPUP_OUTGAME_WND);
        m_pSelf->UpdateData();
        KUiLoginBackGround::SetConfig(m_pSelf->m_szLoginBg);
        m_pSelf->Show();
    }
    return m_pSelf;
}

//--------------------------------------------------------------------------
//	¹¦ÄÜ£º¹Ø±Õ´°¿Ú£¬Í¬Ê±¿ÉÒÔÑ¡ÔòÊÇ·ñÉ¾³ý¶ÔÏóÊµÀý
//--------------------------------------------------------------------------
void KUiSelPlayer::CloseWindow() {
    if (m_pSelf) {
        m_pSelf->m_bJustClicked = false;
        m_pSelf->Destroy();
        m_pSelf = NULL;
    }
}

void KUiSelPlayer::Hide() {
    m_pSelf->m_bJustClicked = false;
    KWndShowAnimate::Hide();
}

// -------------------------------------------------------------------------
// º¯Êý		: KUiSelPlayer::Initialize
// ¹¦ÄÜ		: ³õÊ¼»¯
// -------------------------------------------------------------------------
void KUiSelPlayer::Initialize() {
    AddChild(&m_btnOk);
    AddChild(&m_btnCancel);
    AddChild(&m_btnDel);
    AddChild(&m_btnNew);
    AddChild(&m_ButterflyWnd);

    char Scheme[128];
    g_UiBase.GetCurSchemePath(Scheme, 128);
    m_pSelf->LoadScheme(Scheme);

    // ×¢²á±¾´°¿Ú
    Wnd_AddWindow(this, WL_TOPMOST);
}

// -------------------------------------------------------------------------
// º¯Êý		: KUiSelPlayer::LoadScheme
// ¹¦ÄÜ		: ÔØÈë½çÃæ·½°¸
// -------------------------------------------------------------------------
void KUiSelPlayer::LoadScheme(const char *pScheme) {
    char Buff[128];
    KIniFile Ini;
    sprintf(Buff, "%s\\%s", pScheme, SCHEME_INI_SELPLAYER);
    if (Ini.LoadPakEntry(VNG_PAK_UI_SELPLAYER) || Ini.Load(Buff)) {
        int nScreenWidth, nScreenHeight, nOffsetX, nOffsetY;
        PhongThanGetNativeUiMetrics(nScreenWidth, nScreenHeight, nOffsetX, nOffsetY);
        PhongThanMakeIniRootNative(Ini, "SelRole", nScreenWidth, nScreenHeight);

        const char *pszCenteredSections[] = {
            "Player", "Shadow", "PlayerInfoBg",
            "Ok", "Cancel", "New", "Del",
            "PlayListWnd", "Butterfly"
        };
        for (int nLayoutSection = 0;
             nLayoutSection < sizeof(pszCenteredSections) / sizeof(pszCenteredSections[0]);
             ++nLayoutSection)
            PhongThanOffsetIniPosition(Ini, pszCenteredSections[nLayoutSection], nOffsetX, nOffsetY);

        // Keep the remaining-time label aligned to the full native canvas.
        Ini.WriteInteger("LifeTime", "Left", 0);
        Ini.WriteInteger("LifeTime", "Width", nScreenWidth);

        Init(&Ini, "SelRole");    // ´°¿Ú±³¾°Êý¾Ý
        Ini.GetString("SelRole", "LoginBg", "", m_szLoginBg, sizeof(m_szLoginBg));
        Ini.GetString("SelRole", "PlayerImgPrefix", "", m_szPlayerImgPrefix, sizeof(m_szPlayerImgPrefix));

        Buff[0] = '\0';    // Çå¿Õ»º´æ
        m_btnOk.Init(&Ini, "Ok");
        m_btnCancel.Init(&Ini, "Cancel");
        m_btnDel.Init(&Ini, "Del");
        m_btnNew.Init(&Ini, "New");
        m_ButterflyWnd.Init(&Ini, "Butterfly");

        m_player[0].Init(&Ini, "Player");
        m_Name[0].Init(&Ini, "Name");
        m_Level[0].Init(&Ini, "Level");
        m_PlayerInfoBg[0].Init(&Ini, "PlayerInfoBg");
        m_Career.Init(&Ini, "Career");
        m_Career.SetSize(65, 15);
        m_Career.SetPosition(155, 26);
        m_Portrait.Init(&Ini, "ImgPortrait");
        m_ExpContainer.Init(&Ini, "Exp");
        m_ExpBar.Init(&Ini, "Exp_Image");
        m_ExpText.Init(&Ini, "Exp_Text");
        m_ExpText.SetPosition(0, -2); m_ExpText.SetSize(124, 15);
        m_LastMap.Init(&Ini, "LastMap");
        m_LastTime.Init(&Ini, "LastTime");
        m_LastTime.SetSize(155, 15);
        m_LastIP.Init(&Ini, "LastIP");
        m_ExpContainer.AddChild(&m_ExpBar);
        m_ExpContainer.AddChild(&m_ExpText);
        m_RoleListBg.Init(&Ini, "PlayListWnd");
        AddChild(&m_RoleListBg);
        AddChild(&m_PlayerInfoBg[0]);
        m_PlayerInfoBg[0].AddChild(&m_Name[0]);
        m_PlayerInfoBg[0].AddChild(&m_Level[0]);
        m_PlayerInfoBg[0].AddChild(&m_Career);
        m_PlayerInfoBg[0].AddChild(&m_Portrait);
        m_PlayerInfoBg[0].AddChild(&m_ExpContainer);
        m_PlayerInfoBg[0].AddChild(&m_LastMap);
        m_PlayerInfoBg[0].AddChild(&m_LastTime);
        m_PlayerInfoBg[0].AddChild(&m_LastIP);
        int spacing = 58;
        Ini.GetInteger("PlayListWnd", "DefaultSpace", 58, &spacing);
        const char *rowSections[] = { "RoleRowName", "RoleRowCareer", "RoleRowLevel" };
        const char *prefixes[] = { "Name", "Metier", "Level" };
        for (int r = 0; r < 3; ++r) {
            const char *suffixes[] = { "Left", "Top", "Width", "Height" };
            for (int k = 0; k < 4; ++k) {
                char key[32]; int value = 0;
                sprintf(key, "%s%s", prefixes[r], suffixes[k]);
                Ini.GetInteger("btnPlayer", key, 0, &value);
                Ini.WriteInteger(rowSections[r], suffixes[k], value);
            }
            Ini.WriteInteger(rowSections[r], "DummyWnd", 1);
            Ini.WriteInteger(rowSections[r], "HAlign", 1);
            Ini.WriteInteger(rowSections[r], "Font", 14);
            Ini.WriteString(rowSections[r], "Color", "214,214,214");
            Ini.WriteString(rowSections[r], "BorderColor", "0,0,0");
        }
        for (int row = 0; row < MAX_PLAYER_PER_ACCOUNT; ++row) {
            m_RoleListButton[row].Init(&Ini, "btnPlayer");
            int x, y;
            m_RoleListButton[row].GetPosition(&x, &y);
            int listX, listY;
            m_RoleListBg.GetPosition(&listX, &listY);
            if (row == 0) {
                m_nRowBaseX = listX + x;
                m_nRowBaseY = listY + y;
                m_nRowSpacing = spacing;
            }
            m_RoleListButton[row].SetPosition(listX + x, listY + y + row * spacing);
            // Buttons notify their immediate parent. The decorative image
            // does not forward selection notifications to this window.
            AddChild(&m_RoleListButton[row]);
            m_RowName[row].Init(&Ini, rowSections[0]);
            m_RowCareer[row].Init(&Ini, rowSections[1]);
            m_RowLevel[row].Init(&Ini, rowSections[2]);
            m_RoleListButton[row].AddChild(&m_RowName[row]);
            m_RoleListButton[row].AddChild(&m_RowCareer[row]);
            m_RoleListButton[row].AddChild(&m_RowLevel[row]);
        }

        for (int i = 1; i < MAX_PLAYER_PER_ACCOUNT; i++) {
            m_player[0].Clone(&m_player[i]);
            m_Name[0].Clone(&m_Name[i]);
            m_Level[0].Clone(&m_Level[i]);
            m_PlayerInfoBg[0].Clone(&m_PlayerInfoBg[i]);
        }

        Ini.GetInteger("SelRole", "Player2Pos_0", 0, &m_ChildWndXOffset[0]);
        Ini.GetInteger("SelRole", "Player2Pos_1", 0, &m_ChildWndXOffset[1]);
        Ini.GetInteger("SelRole", "Player3Pos_0", 0, &m_ChildWndXOffset[2]);
        Ini.GetInteger("SelRole", "Player3Pos_1", 0, &m_ChildWndYOffset);
        Ini.GetInteger("SelRole", "Player3Pos_2", 0, &m_ChildWndXOffset[3]);

        m_player[0].GetPosition(&m_ChildPos[0], NULL);
        m_Name[0].GetPosition(&m_ChildPos[1], NULL);
        m_Level[0].GetPosition(&m_ChildPos[2], NULL);
        m_PlayerInfoBg[0].GetPosition(&m_ChildPos[3], NULL);

        m_LifeTimeText.Init(&Ini, "LifeTime");

        isCheckPlayGame = false; // check nhan vat
    }
}

// -------------------------------------------------------------------------
// ¹¦ÄÜ	: »æÖÆ´°¿Ú
// -------------------------------------------------------------------------
void KUiSelPlayer::PaintWindow() {
    KWndShowAnimate::PaintWindow();
    for (int i = 0; i < m_nNumPlayer; i++) {
        int nLoopBack = m_player[i].NextFrame();
        if (m_bJustClicked && nLoopBack && i == m_nSelPlayer) {
            char szFileName[128];
            GetRoleImageName(szFileName, m_szPlayerImgPrefix, m_Gender[m_nSelPlayer], m_Profession[m_nSelPlayer], 0);
            m_player[m_nSelPlayer].SetImage(ISI_T_SPR, szFileName);
            m_player[m_nSelPlayer].SetFrame(0);
            m_bJustClicked = false;
        }
    }

    // PT does not display the inherited account-time countdown.
}

// -------------------------------------------------------------------------
// ¹¦ÄÜ	: ¸üÐÂ
// -------------------------------------------------------------------------
void KUiSelPlayer::UpdateData() {
    int nSel;
    m_nNumPlayer = g_LoginLogic.GetRoleCount(nSel);
    if (m_nNumPlayer > 0 && nSel < 0)
        m_nSelPlayer = 0;
    else
        m_nSelPlayer = nSel;

    int i;
    char szFileName[128];
    if (m_nNumPlayer > 0) {
        KIniFile Ini;
        Ini.Load(PROFESSION_INI);

        for (i = 0; i < m_nNumPlayer; i++) {
            KRoleChiefInfo Info;
            g_LoginLogic.GetRoleInfo(i, &Info);
            m_RowName[i].SetText(Info.Name);
            const char *titles[] = { "Gi\xB8p S\xDC", "\xA7\xB9o S\xDC", "D\xDE Nh\xA9n", "V\xF2 S\xDC" };
            m_RowCareer[i].SetText(titles[Info.Profession]);
            char row[64]; sprintf(row, "%s %d", titles[Info.Profession], Info.nLevel);
            m_RowCareer[i].SetText(row);
            m_RowName[i].SetPosition(5, 6); m_RowName[i].SetSize(171, 15);
            m_RowCareer[i].SetPosition(5, 24); m_RowCareer[i].SetSize(171, 15);
            m_RowLevel[i].SetText("");
            // Visibility is decided by ApplyListPage() (paging).
            AddChild(&m_player[i]);
            m_Gender[i] = Info.Gender;
            m_Profession[i] = Info.Profession;

            m_szSelPlayerSound[i][0] = 0;
            Ini.GetString(PROFESSION_GetTitleString(Info.Profession),
                          Info.Gender ? "SelSound_f" : "SelSound_m", "",
                          m_szSelPlayerSound[i], sizeof(m_szSelPlayerSound[i]));
            GetRoleImageName(szFileName, m_szPlayerImgPrefix, Info.Gender, Info.Profession, 2);
            m_player[i].SetImage(ISI_T_SPR, szFileName);
            m_player[i].SetFrame(0);

        }

        if (m_nSelPlayer < 0)
            m_nSelPlayer = 0;

        OnSelectPlayer(m_nSelPlayer);
    }

    for (i = m_nNumPlayer; i < MAX_PLAYER_PER_ACCOUNT; i++) {
        m_RoleListButton[i].Hide();
        m_player[i].LeaveAlone();

    }

    m_btnOk.BringToTop();
    m_btnCancel.BringToTop();
    m_btnDel.BringToTop();
    m_btnNew.BringToTop();

    m_btnNew.Enable(m_nNumPlayer < MAX_PLAYER_PER_ACCOUNT);
    m_btnDel.Enable(m_nNumPlayer > 0 && m_nSelPlayer >= 0);
    m_btnOk.Enable(m_nNumPlayer > 0 && m_nSelPlayer >= 0);

    m_LifeTimeText.SetText("");
    m_LifeTimeText.Hide();

    countTimerLoadGame = 1;
}

void
KUiSelPlayer::GetRoleImageName(char *pszName, const char *pszPrefix, unsigned char bGender, unsigned char bAttribute,
                               int nIndex) {
    if (pszName && bAttribute < 3 && nIndex >= 0 && nIndex < 3) {
        sprintf(pszName, "\\spr\\pt_role\\role_%d_%d_%d.spr", bAttribute, bGender ? 1 : 0, nIndex);
        return;
    }
    _ASSERT(pszName && bAttribute >= series_metal && bAttribute < series_num);
    static const char *pszAttribute[series_num] = {"½ð", "Ä¾", "Ë®", "»ð", "ÍÁ"};
    static const char *pszGender[2] = {"ÄÐ", "Å®"};
    sprintf(pszName, "%s_%s_%s_%d.spr", pszPrefix, pszAttribute[bAttribute],
            (bGender ? pszGender[1] : pszGender[0]), nIndex);
}

// -------------------------------------------------------------------------
// º¯Êý	: ÏûÏ¢´¦Àí
// -------------------------------------------------------------------------
int KUiSelPlayer::WndProc(unsigned int uMsg, unsigned int uParam, int nParam) {
    int nRet = 0;
    switch (uMsg) {
        case WND_N_BUTTON_CLICK:
            OnClickButton((KWndButton *) (KWndWindow *) uParam, false);
            break;

        case WND_N_BUTTON_DB_CLICK:
            OnClickButton((KWndButton *) (KWndWindow *) uParam, true);
            break;
        case WM_KEYDOWN:
            nRet = OnKeyDown(uParam);
            break;
        default:
            nRet = KWndShowAnimate::WndProc(uMsg, uParam, nParam);
            break;
    }
    return nRet;
}

int KUiSelPlayer::OnKeyDown(unsigned int uKey) {
    int nRet = 1;
    KWndWindow *pActive = NULL;
    KWndButton *pToActive = NULL;
    switch (uKey) {
        case VK_RETURN:
            pActive = GetActiveBtn();
            if (pActive == NULL && m_btnOk.IsDisable() == FALSE)
                pActive = &m_btnOk;
            OnClickButton((KWndButton *) pActive, false);
            break;
        case VK_ESCAPE:
            OnCancel();
            nRet = 1;
            break;
        case VK_UP:
            pActive = GetActiveBtn();
            if (pActive == &m_btnCancel)
                pToActive = &m_btnDel;
            else if (pActive == &m_btnDel)
                pToActive = &m_btnNew;
            else if (pActive == &m_btnOk)
                pToActive = &m_btnCancel;
            else
                pToActive = &m_btnOk;
            nRet = 1;
            break;
        case VK_DOWN:
            pActive = GetActiveBtn();
            if (pActive == &m_btnOk)
                pToActive = &m_btnNew;
            else if (pActive == &m_btnNew)
                pToActive = &m_btnDel;
            else if (pActive == &m_btnCancel)
                pToActive = &m_btnOk;
            else
                pToActive = &m_btnCancel;
            nRet = 1;
            break;
        case VK_PRIOR: // PageUp
            if (m_nSelPlayer > 0)
                OnSelectPlayer(max(0, m_nSelPlayer - PT_SELPLAYER_PAGE_SIZE));
            m_btnDel.Enable(m_nSelPlayer >= 0);
            nRet = 1;
            break;
        case VK_NEXT: // PageDown
            if (m_nSelPlayer >= 0 && m_nSelPlayer < m_nNumPlayer - 1)
                OnSelectPlayer(min(m_nNumPlayer - 1, m_nSelPlayer + PT_SELPLAYER_PAGE_SIZE));
            m_btnDel.Enable(m_nSelPlayer >= 0);
            nRet = 1;
            break;
        case VK_LEFT:
            if (m_nSelPlayer > 0)
                OnSelectPlayer(m_nSelPlayer - 1);
            else if (m_nSelPlayer < 0 && m_nNumPlayer > 0)
                OnSelectPlayer(0);
            m_btnDel.Enable(m_nSelPlayer >= 0);
            nRet = 1;
            break;
        case VK_RIGHT:
        case VK_SPACE:
            if (m_nSelPlayer < m_nNumPlayer - 1)
                OnSelectPlayer(m_nSelPlayer + 1);
            else if (m_nNumPlayer < 0 && m_nNumPlayer > 0)
                OnSelectPlayer(0);
            m_btnDel.Enable(m_nSelPlayer >= 0);
            nRet = 1;
            break;
        default:
            nRet = 0;
    }
    if (pToActive)
        pToActive->SetCursorAbove();
    return nRet;
}

void KUiSelPlayer::ApplyListPage() {
    int nPage = (m_nSelPlayer > 0) ? (m_nSelPlayer / PT_SELPLAYER_PAGE_SIZE) : 0;
    m_nListPage = nPage;
    int nFirst = nPage * PT_SELPLAYER_PAGE_SIZE;
    int nLast = nFirst + PT_SELPLAYER_PAGE_SIZE;
    if (nLast > m_nNumPlayer)
        nLast = m_nNumPlayer;
    for (int i = 0; i < m_nNumPlayer; i++) {
        if (i >= nFirst && i < nLast) {
            m_RoleListButton[i].SetPosition(m_nRowBaseX,
                m_nRowBaseY + (i - nFirst) * m_nRowSpacing);
            m_RoleListButton[i].Show();
        } else {
            m_RoleListButton[i].Hide();
        }
    }
}

void KUiSelPlayer::OnSelectPlayer(int nSel, bool bPlaySound) {
    if (nSel < 0 || nSel >= m_nNumPlayer) return;
    char szFileName[128];
    m_nSelPlayer = nSel;
    for (int i = 0; i < m_nNumPlayer; ++i) {
        if (i == nSel) m_player[i].Show(); else m_player[i].Hide();
        m_RoleListButton[i].CheckButton(i == nSel);
        unsigned int color = i == nSel ? 0xff00ff00 : 0xffd6d6d6;
        m_RowName[i].SetTextColor(color);
        m_RowCareer[i].SetTextColor(color);
        m_RowLevel[i].SetTextColor(color);
    }
    KRoleChiefInfo info;
    if (g_LoginLogic.GetRoleInfo(nSel, &info)) {
        m_Name[0].SetText(info.Name);
        m_Level[0].SetIntText(info.nLevel);
        const char *titles[] = { "Gi\xB8p S\xDC", "\xA7\xB9o S\xDC", "D\xDE Nh\xA9n", "V\xF2 S\xDC" };
        m_Career.SetText(titles[info.Profession]);
        int variant = info.Portrait > 0 ? (info.Portrait - 1) % 3 : 0;
        char portrait[128];
        sprintf(portrait, "\\Spr\\Ui4\\\xD0\xA4\xCF\xF1\\%03d.spr",
                variant * 2 + (info.Gender ? 2 : 1));
        m_Portrait.SetImage(ISI_T_SPR, portrait);
        KTabFile exp;
        int nextExp = 0;
        if (exp.Load("\\Settings\\npc\\player\\level_exp.txt"))
            exp.GetInteger(info.nLevel + 1, 2 + info.Transcendence, 0, &nextExp);
        int current = info.FightExperience < 0 ? 0 : info.FightExperience;
        double rate = nextExp > 0 ? (double)current * 100 / nextExp : 0;
        int percent = (int)rate;
        if (percent > 100) percent = 100;
        char text[128];
        sprintf(text, "%.3f%%", rate);
        m_ExpText.SetText(text);
        m_ExpBar.SetPart(percent, 100);
        KIniFile maps;
        char key[32];
        sprintf(key, "%d_name", info.MapId);
        sprintf(text, "%d", info.MapId);
        if (maps.Load("\\Settings\\MapList.ini"))
            maps.GetString("List", key, text, text, sizeof(text));
        m_LastMap.SetText(text[0] == '$' ? text + 1 : text);
        time_t saved = (time_t)info.LastLoginUtc;
        struct tm *local = localtime(&saved);
        if (saved && local)
            strftime(text, sizeof(text), "%Y-%m-%d %H:%M", local);
        else strcpy(text, "--");
        m_LastTime.SetText(text);
        // IP and time come from the server-owned login metadata.
        unsigned long ip = info.LastLoginIP;
        if (ip) sprintf(text, "%lu.%lu.%lu.%lu", ip & 255, (ip >> 8) & 255, (ip >> 16) & 255, (ip >> 24) & 255);
        else strcpy(text, "--");
        m_LastIP.SetText(text);

    }
    GetRoleImageName(szFileName, m_szPlayerImgPrefix, m_Gender[m_nSelPlayer], m_Profession[m_nSelPlayer], 1);
    m_player[m_nSelPlayer].SetImage(ISI_T_SPR, szFileName);
    m_player[m_nSelPlayer].SetFrame(0);
    m_bJustClicked = true;
    if (bPlaySound)
        UiSoundPlay(m_szSelPlayerSound[m_nSelPlayer]);
    ApplyListPage();
}

// -------------------------------------------------------------------------
// º¯Êý		: KUiSelPlayer::OnClickButton
// ¹¦ÄÜ		: ÏìÓ¦µã»÷°´Å¥
// ²ÎÊý		: KWndWindow* pWnd ---> ±»µã»÷µÄ¿Ø¼þÖ¸Õë
// -------------------------------------------------------------------------
void KUiSelPlayer::OnClickButton(KWndButton *pWnd, bool bDoubleClick) {
    // ---> ÅÐ¶ÏµãÖÐµÄÊÇÄÇ¸ö¿Ø¼þ
    for (int i = 0; i < m_nNumPlayer; i++) {
        if (pWnd == (KWndWindow *) &m_player[i] || pWnd == &m_RoleListButton[i])    // µãÖÐÁËÄ³¸öÍæ¼ÒÐÅÏ¢¿ò
        {
            OnSelectPlayer(i, !bDoubleClick);
            // Èç¹ûÊÇË«»÷£¬¾Í·ÂÕæÑ¡ÔñÓÃ»§
            if (bDoubleClick) {
                OnEnterGame();
            }
            return;
        }
    }

    if (pWnd == (KWndWindow *) &m_btnNew)
        OnNew();
    else if (pWnd == (KWndWindow *) &m_btnCancel)
        OnCancel();
    else if (pWnd == (KWndWindow *) &m_btnOk) {
        OnEnterGame();
    } else if (pWnd == (KWndWindow *) &m_btnDel)
        OnDel();
}

//TamLTM select nhan vat game and xac nhan
void KUiSelPlayer::OnEnterGame() {
    //TamLTM select nhan vat game and xac nhan

    //g_DebugLog("m_nSelPlayer, xac nhan vao tro choi");
    //end code

    if (m_nSelPlayer >= 0) {
        if (g_LoginLogic.SelectRole(m_nSelPlayer)) {
            KUiConnectInfo::OpenWindow(CI_MI_ENTERING_GAME, LL_S_IN_GAME);
            Hide();
        }
    }
}

// -------------------------------------------------------------------------
// º¯Êý		: KUiSelPlayer::OnNew
// ¹¦ÄÜ		: ÐÂ½¨°´Å¥µã»÷ÊÂ¼þ´¦Àí
// -------------------------------------------------------------------------
void KUiSelPlayer::OnNew() {
    int nSel;
    if (g_LoginLogic.GetRoleCount(nSel) < MAX_PLAYER_PER_ACCOUNT) {
        // VNG chooses birthplace from the profession inside the new-role UI.
        if (KUiNewPlayer::OpenWindow(0))
            Hide();
    }
}

void KUiSelPlayer::OnDel() {
    if (m_nSelPlayer >= 0) {
        KUiConnectInfo::OpenWindow(CI_MI_TO_DEL_ROLE, CI_NS_SEL_ROLE_WND, m_nSelPlayer);

        //Khong the xoa nhan vat nay
        /* Ban offline: da mo khoa xoa nhan vat (xem dong tren) */
        Hide();

        //	g_DebugLog("OpenWindow(CI_MI_TO_DEL_ROLE, CI_NS_SEL_ROLE_WND, m_nSelPlayer) HiÖn t¹i kh«ng thÓ xãa nh©n vËt nµy.");
    }
}

// -------------------------------------------------------------------------
// º¯Êý		: KUiSelPlayer::OnCancel
// ¹¦ÄÜ		: È¡Ïû°´Å¥µã»÷ÊÂ¼þ´¦Àí
// -------------------------------------------------------------------------
void KUiSelPlayer::OnCancel() {
    Hide();
    g_LoginLogic.ReturnToIdle();
    KUiSelServer::OpenWindow();
}

KWndButton *KUiSelPlayer::GetActiveBtn() {
    KWndButton *pBtn = NULL;
    if (m_btnOk.IsButtonActive())
        pBtn = &m_btnOk;
    else if (m_btnCancel.IsButtonActive())
        pBtn = &m_btnCancel;
    else if (m_btnDel.IsButtonActive())
        pBtn = &m_btnDel;
    else if (m_btnNew.IsButtonActive())
        pBtn = &m_btnNew;

    return pBtn;
}

//TamLTM Check timer in game
void KUiSelPlayer::Breathe() {
//	if (countTimerLoadGame)
//		countTimerLoadGame++;

//	if (countTimerLoadGame == 20)
//	{
    isCheckPlayGame = true;
//		g_DebugLog("Log Breathe");
//	}
//	g_DebugLog("Log Breathe");
}
//end code
