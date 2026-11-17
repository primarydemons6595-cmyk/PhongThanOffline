/*****************************************************************************************
//	界面--聊天窗口
//	Copyright : Kingsoft 2002
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2002-8-27
*****************************************************************************************/
#include "KWin32.h"
#include "KIniFile.h"
#include "PhongThanNativeUiLayout.h"
#include "../Elem/WndMessage.h"
#include "../elem/wnds.h"
#include "UiSkills.h"
#include "../../../../Headers/PhongThanRoleUi.h"
#include "../../../../Headers/PhongThanRealmUi.h"
#include "KTabFile.h"
#include "UiSysMsgCentre.h"
#include "../../../core/src/SkillDef.h"
#include "../../../core/src/coreshell.h"
#include "../../../engine/src/kdebug.h"
#include "../UiBase.h"
#include "crtdbg.h"
#include "../UiSoundSetting.h"

#include "../../../Represent/iRepresent/iRepresentShell.h"

extern iRepresentShell *g_pRepresentShell;

extern iCoreShell *g_pCoreShell;

#define    SCHEME_INI_SHEET            "UiSkillsSheet.ini"
#define    SCHEME_INI_FIGHT            "UiSkillsLive.ini"
#define    SCHEME_INI_FIGHT_SUB_PAGE    "UiSkillsFightSub.ini"
#define    VNG_PAK_UI_SKILLS_FRAME      0x2A4EC2EFUL
#define    VNG_PAK_UI_SKILLS_JIASHI     0x880C7EEEUL
#define    VNG_PAK_UI_SKILLS_DAOSHI     0xA718CBAFUL
#define    VNG_PAK_UI_SKILLS_YIREN      0xCA5AA31CUL
#define        SET_NEW_SKILL_TO_IMMED_SKILL_LEVEL_RANGE    9
#define        AUTO_SET_IMMED_SKILL_MSG_ID                    "24"

static DWORD GetVngProfessionSkillIniId() {
    if (!g_pCoreShell)
        return VNG_PAK_UI_SKILLS_JIASHI;

    KUiPlayerBaseInfo Info;
    memset(&Info, 0, sizeof(Info));
    Info.nProfession = -1;
    g_pCoreShell->GetGameData(GDI_PLAYER_BASE_INFO, (unsigned int)&Info, 0);
    switch (Info.nProfession) {
        case 1:
            return VNG_PAK_UI_SKILLS_DAOSHI;
        case 2:
            return VNG_PAK_UI_SKILLS_YIREN;
        case 0:
        default:
            return VNG_PAK_UI_SKILLS_JIASHI;
    }
}

static int CurrentSkillProfession() {
    KUiPlayerBaseInfo info;memset(&info,0,sizeof(info));info.nProfession=-1;
    if(g_pCoreShell)g_pCoreShell->GetGameData(GDI_PLAYER_BASE_INFO,(unsigned int)&info,0);
    return info.nProfession;
}
static BOOL LoadProfessionSkillIni(KIniFile& ini) {
    if(CurrentSkillProfession()==3)return ini.Load("\\ui\\vusi_skills.ini");
    return ini.LoadPakEntry(GetVngProfessionSkillIniId());
}
#include "UiRealmSkills.inl"
static int s_SkillLayoutProfession=-1;
static int s_SkillLayoutRealm=-1;
KUiFightSkillSubPage::KUiFightSkillSubPage() {
    m_nRemainSkillPoint = 0;
    memset(m_SkillTextLayout, 0, sizeof(m_SkillTextLayout));
//	m_nSubPagIndex = 0;
}

//初始化
void KUiFightSkillSubPage::Initialize(/*int nSubPageIndex*/) {
    for (int i = 0; i < FIGHT_SKILL_COUNT_PER_PAGE; i++) {
        AddChild(&m_FightSkills[i]);
        m_FightSkills[i].Celar();
        m_FightSkills[i].SetContainerId((int) UOC_SKILL_LIST);
    }
//	m_nSubPagIndex = nSubPageIndex;
}

//载入界面方案
void KUiFightSkillSubPage::LoadScheme(KIniFile *pIni, int nSubPageIndex) {
    char Buff[128];
    if (!pIni || nSubPageIndex < 0 || nSubPageIndex >= FIGHT_SKILL_SUB_PAGE_COUNT)
        return;

    sprintf(Buff, "SkillPage_%d", nSubPageIndex);
    KWndPage::Init(pIni, Buff);
    for (int i = 0; i < FIGHT_SKILL_COUNT_PER_PAGE; i++) {
        int nLeft = 0;
        int nTop = 0;
        int nImageLeft = 0;
        int nImageTop = 0;
        int nImageWidth = 33;
        int nImageHeight = 33;
        sprintf(Buff, "Page_%d_Skill_%d", nSubPageIndex, i);
        pIni->GetInteger(Buff, "Left", 0, &nLeft);
        pIni->GetInteger(Buff, "Top", 0, &nTop);
        pIni->GetInteger(Buff, "ImageLeft", 0, &nImageLeft);
        pIni->GetInteger(Buff, "ImageTop", 0, &nImageTop);
        pIni->GetInteger(Buff, "ImageWidth", 33, &nImageWidth);
        pIni->GetInteger(Buff, "ImageHeight", 33, &nImageHeight);
        pIni->GetInteger(Buff, "SkillNameTextLeft", 40,
            &m_SkillTextLayout[i].nNameOffsetX);
        pIni->GetInteger(Buff, "SkillNameTextTop", 8,
            &m_SkillTextLayout[i].nNameOffsetY);
        pIni->GetInteger(Buff,"SkillNameTextWidth",65,&m_SkillTextLayout[i].nNameWidth);
        pIni->GetInteger(Buff, "LevelTextLeft", 38,
            &m_SkillTextLayout[i].nLevelOffsetX);
        pIni->GetInteger(Buff, "LevelTextTop", 24,
            &m_SkillTextLayout[i].nLevelOffsetY);
        pIni->GetInteger(Buff, "LevelTextWidth", 13,
            &m_SkillTextLayout[i].nLevelWidth);
        pIni->GetInteger(Buff, "LevelTextHeight", 13,
            &m_SkillTextLayout[i].nLevelHeight);
        pIni->GetString(Buff, "SkillName", "",
            m_SkillTextLayout[i].szName,
            sizeof(m_SkillTextLayout[i].szName));
        if (m_SkillTextLayout[i].szName[0] == '$')
            memmove(m_SkillTextLayout[i].szName,
                m_SkillTextLayout[i].szName + 1,
                strlen(m_SkillTextLayout[i].szName));
        m_SkillTextLayout[i].nNameOffsetX -= nImageLeft;
        m_SkillTextLayout[i].nNameOffsetY -= nImageTop;
        m_SkillTextLayout[i].nLevelOffsetX -= nImageLeft;
        m_SkillTextLayout[i].nLevelOffsetY -= nImageTop;
        m_FightSkills[i].Init(pIni, Buff);
        m_FightSkills[i].SetPosition(nLeft + nImageLeft, nTop + nImageTop);
        m_FightSkills[i].SetSize(nImageWidth, nImageHeight);
        m_FightSkills[i].EnablePickPut(false);
    }
    pIni->GetInteger("SkillText", "Font", 14, &m_SkillTextParam.nFont);
    m_SkillTextParam.Offset.cx = 0;
    m_SkillTextParam.Offset.cy = 37;
    pIni->GetInteger2("SkillText", "Offset",
        (int*)&m_SkillTextParam.Offset.cx,
        (int*)&m_SkillTextParam.Offset.cy);
    pIni->GetString("SkillText", "Color", "218,255,165", Buff, 16);
    m_SkillTextParam.Color = GetColor(Buff);
}

//更新升级点数
void KUiFightSkillSubPage::UpdateRemainPoint(int nPoint) {
    m_nRemainSkillPoint = nPoint;
}

//更新升级点数
void KUiFightSkillSubPage::UpdateSkill(KUiSkillData *pSkill, int nIndex) {
    _ASSERT(pSkill && nIndex >= 0 && nIndex < FIGHT_SKILL_COUNT_PER_PAGE);
    m_FightSkills[nIndex].HoldObject(pSkill->uGenre, pSkill->uId, 0, 0);
}

//更新数据
void KUiFightSkillSubPage::UpdateData(KUiSkillData *pSkills, int nCount) {
    _ASSERT(pSkills);
    if (nCount < 0)
        nCount = 0;
    if (nCount > FIGHT_SKILL_COUNT_PER_PAGE)
        nCount = FIGHT_SKILL_COUNT_PER_PAGE;
    for (int i = 0; i < FIGHT_SKILL_COUNT_PER_PAGE; i++) {
        m_FightSkills[i].Celar();
        if (i < nCount)
            m_FightSkills[i].HoldObject(pSkills[i].uGenre, pSkills[i].uId, pSkills[i].nLevel, 0);
    }
}

//窗口函数
int KUiFightSkillSubPage::WndProc(unsigned int uMsg, unsigned int uParam, int nParam) {
    switch (uMsg) {
        case WND_N_LEFT_CLICK_ITEM:
            // Learn with books and train in combat; do not spend skill points.
            return 0;
        default:
            return KWndPage::WndProc(uMsg, uParam, nParam);
            break;
    }
    return 0;
}

//绘制窗口
void KUiFightSkillSubPage::PaintWindow() {
    KWndPage::PaintWindow();
    for (int i = 0; i < FIGHT_SKILL_COUNT_PER_PAGE; i++) {
        KUiDraggedObject Obj;
        m_FightSkills[i].GetObject(Obj);
        if (Obj.uGenre != CGOG_NOTHING) {
            int nLeft, nTop, nWidth, nHeight;
            m_FightSkills[i].GetAbsolutePos(&nLeft, &nTop);
            m_FightSkills[i].GetSize(&nWidth, &nHeight);
            g_pCoreShell->DrawGameObj(CGOG_SKILL_FIGHT, Obj.uId, nLeft, nTop, nWidth, nHeight, 0);

            const char* pszName = m_SkillTextLayout[i].szName;
            if (pszName[0]) {
                g_pRepresentShell->OutputText(m_SkillTextParam.nFont,
                    (char*)pszName, min((int)strlen(pszName),m_SkillTextLayout[i].nNameWidth*2/max(m_SkillTextParam.nFont,1)),
                    nLeft + m_SkillTextLayout[i].nNameOffsetX,
                    nTop + m_SkillTextLayout[i].nNameOffsetY,
                    m_SkillTextParam.Color, 0);
            }

            unsigned int dwColor = 0;
            int nLevel = 0;
            if (g_pCoreShell->GetSkillData(Obj.uId, &nLevel) == TRUE)
                dwColor = 0xff3130ff;
            else
                dwColor = m_SkillTextParam.Color;
            if (nLevel >= 0) {
                char szLevel[8];
                int nLen;
                itoa(nLevel, szLevel, 10);
                nLen = strlen(szLevel);
                nLeft += m_SkillTextLayout[i].nLevelOffsetX;
                nTop += m_SkillTextLayout[i].nLevelOffsetY;
                nLeft += (m_SkillTextLayout[i].nLevelWidth -
                    nLen * m_SkillTextParam.nFont / 2) / 2;

                g_pRepresentShell->OutputText(m_SkillTextParam.nFont, szLevel, nLen, nLeft, nTop,
                                              dwColor, 0);
            }
        }
    }
}

//初始化
void KUiFightSkillSub::Initialize() {
    memset(m_nPageSkillCount, 0, sizeof(m_nPageSkillCount));
    for (int i = 0; i < FIGHT_SKILL_SUB_PAGE_COUNT; i++) {
        m_SubPages[i].Initialize();
        AddPage(&m_SubPages[i], &m_SubPageBtn[i]);
    }
}

//载入界面方案
void KUiFightSkillSub::LoadScheme(const char *pScheme) {
    char Buff[128];
    KIniFile Ini;
    if (LoadProfessionSkillIni(Ini)) {
        PtPrepareRealmSkillIni(Ini);
        KWndPageSet::Init(&Ini, "Main");
        m_oFixPos.x = 0;
        m_oFixPos.y = 0;
        SetPosition(0, 0);
        SetSize(206, 280);
        int nPageCount = 0;
        Ini.GetInteger("Main", "PageCount", 0, &nPageCount);
        if (nPageCount > FIGHT_SKILL_SUB_PAGE_COUNT)
            nPageCount = FIGHT_SKILL_SUB_PAGE_COUNT;
        for (int i = 0; i < FIGHT_SKILL_SUB_PAGE_COUNT; i++) {
            if (i < nPageCount) {
                m_SubPages[i].LoadScheme(&Ini, i);
                sprintf(Buff, "PageBtn_%d", i);
                m_SubPageBtn[i].Init(&Ini, Buff);
                sprintf(Buff, "SkillPage_%d", i);
                Ini.GetInteger(Buff, "SkillCount", 0, &m_nPageSkillCount[i]);
                if (m_nPageSkillCount[i] > FIGHT_SKILL_COUNT_PER_PAGE)
                    m_nPageSkillCount[i] = FIGHT_SKILL_COUNT_PER_PAGE;
                m_SubPageBtn[i].Show();
            } else {
                m_nPageSkillCount[i] = 0;
                m_SubPageBtn[i].Hide();
                m_SubPages[i].Hide();
            }
        }
        if(GetActivePageIndex()>=nPageCount)ActivePage(0);
        Show();
    }
}

//更新升级点数
void KUiFightSkillSub::UpdateRemainPoint(int nPoint) {
    for (int i = 0; i < FIGHT_SKILL_SUB_PAGE_COUNT; i++)
        m_SubPages[i].UpdateRemainPoint(nPoint);
}

//更新技能
void KUiFightSkillSub::UpdateSkill(KUiSkillData *pSkill, int nIndex) {
    // The notification index belongs to the core skill list, not a UI slot.
    // Rebuild the fixed profession pages after a level/skill change.
    if (pSkill)
        UpdateData();
}

//更新数据
void KUiFightSkillSub::UpdateData() {
    KUiSkillData Skills[FIGHT_SKILL_COUNT];
    memset(Skills, 0, sizeof(Skills));
    g_pCoreShell->GetGameData(GDI_FIGHT_SKILLS, (unsigned int) Skills, 0);
    int nOffset = 0;
    for (int i = 0; i < FIGHT_SKILL_SUB_PAGE_COUNT; i++) {
        m_SubPages[i].UpdateData(&Skills[nOffset], m_nPageSkillCount[i]);
        nOffset += m_nPageSkillCount[i];
    }
}

//--------------------------------------------------------------------------
//	功能：初始化
//--------------------------------------------------------------------------
void KUiFightSkill::Initialize() {
    m_InternalPad.Initialize();
    AddChild(&m_InternalPad);
    AddChild(&m_RemainSkillPoint);
}

//--------------------------------------------------------------------------
//	功能：载入窗口的界面方案
//--------------------------------------------------------------------------
void KUiFightSkill::LoadScheme(const char *pScheme) {
    KIniFile Ini;
    if (LoadProfessionSkillIni(Ini)) {
        PtPrepareRealmSkillIni(Ini);
        KWndPage::Init(&Ini, "Main");
        m_InternalPad.LoadScheme(pScheme);
        m_RemainSkillPoint.Hide();
    }
}

//--------------------------------------------------------------------------
//	功能：更新数据
//--------------------------------------------------------------------------
void KUiFightSkill::UpdateData() {
    int nRemainSkillPoint = g_pCoreShell->GetGameData(GDI_FIGHT_SKILL_POINT, 0, 0);
    m_InternalPad.UpdateRemainPoint(nRemainSkillPoint);
    m_RemainSkillPoint.SetIntText(nRemainSkillPoint);
    m_InternalPad.UpdateData();
}

//--------------------------------------------------------------------------
//	功能：更新技能
//--------------------------------------------------------------------------
void KUiFightSkill::UpdateSkill(KUiSkillData *pSkill, int nIndex) {
    m_InternalPad.UpdateSkill(pSkill, nIndex);
}

//--------------------------------------------------------------------------
//	功能：更新升级点数
//--------------------------------------------------------------------------
void KUiFightSkill::UpdateRemainPoint(int nPoint) {
    m_RemainSkillPoint.SetIntText(nPoint);
    m_InternalPad.UpdateRemainPoint(nPoint);

}


KUiSkills *KUiSkills::m_pSelf = NULL;

//--------------------------------------------------------------------------
//	功能：打开窗口，返回唯一的一个类对象实例
//--------------------------------------------------------------------------
KUiSkills *KUiSkills::OpenWindow() {
    if (m_pSelf == NULL) {
        m_pSelf = new KUiSkills;
        if (m_pSelf)
            m_pSelf->Initialize();
    }
    if (m_pSelf) {
        if(s_SkillLayoutProfession!=CurrentSkillProfession()||s_SkillLayoutRealm!=PtRealmSkillState()) {
            char scheme[256];g_UiBase.GetCurSchemePath(scheme,sizeof(scheme));
            m_pSelf->LoadScheme(scheme);
        }
        UiSoundPlay(UI_SI_WND_OPENCLOSE);
        m_pSelf->m_FightSkillPad.UpdateData();

        m_pSelf->BringToTop();
        m_pSelf->Show();
    }
    return m_pSelf;
}

//--------------------------------------------------------------------------
//	功能：如果窗口正被显示，则返回实例指针
//--------------------------------------------------------------------------
void KUiSkills::RefreshRealmState(){
    if(m_pSelf&&m_pSelf->IsVisible()&&(s_SkillLayoutRealm!=PtRealmSkillState()||s_SkillLayoutProfession!=CurrentSkillProfession())){
        char scheme[256];g_UiBase.GetCurSchemePath(scheme,sizeof(scheme));LoadScheme(scheme);
        m_pSelf->m_FightSkillPad.UpdateData();
    }
}
KUiSkills *KUiSkills::GetIfVisible() {
    if (m_pSelf && m_pSelf->IsVisible())
        return m_pSelf;
    return NULL;
}

//--------------------------------------------------------------------------
//	功能：关闭窗口，同时可以选则是否删除对象实例
//--------------------------------------------------------------------------
void KUiSkills::CloseWindow(bool bDestroy) {
    if (m_pSelf) {
        if (bDestroy == false)
            m_pSelf->Hide();
        else {
            m_pSelf->Destroy();
            m_pSelf = NULL;
        }
    }
}

//--------------------------------------------------------------------------
//	功能：初始化
//--------------------------------------------------------------------------
void KUiSkills::Initialize() {
    m_FightSkillPad.Initialize();
    AddPage(&m_FightSkillPad, &m_FightSkillPadBtn);

    AddChild(&m_Close);

    char Scheme[256];
    g_UiBase.GetCurSchemePath(Scheme, 256);
    LoadScheme(Scheme);

    Wnd_AddWindow(this);
}

//--------------------------------------------------------------------------
//	功能：载入窗口的界面方案
//--------------------------------------------------------------------------
void KUiSkills::LoadScheme(const char *pScheme) {
    s_SkillLayoutProfession=CurrentSkillProfession();s_SkillLayoutRealm=PtRealmSkillState();
    char Buff[128];
    KIniFile Ini;
    sprintf(Buff, "%s\\%s", pScheme, SCHEME_INI_SHEET);
    if (m_pSelf && (Ini.LoadPakEntry(VNG_PAK_UI_SKILLS_FRAME) || Ini.Load(Buff))) {
    PhongThanAnchorNativePanel(Ini, false);
        m_pSelf->Init(&Ini, "Main");
        int screenWidth,screenHeight,panelWidth,panelHeight,left,top;
        Wnd_GetScreenSize(screenWidth,screenHeight);
        m_pSelf->GetSize(&panelWidth,&panelHeight);m_pSelf->GetPosition(&left,&top);
        m_pSelf->m_oFixPos.x=0;
        m_pSelf->SetPosition(m_pSelf->m_oFixPos.x,top);
        m_pSelf->m_FightSkillPadBtn.Init(&Ini, "FightBtn");
        m_pSelf->m_Close.Init(&Ini, "CloseBtn");

        m_pSelf->m_FightSkillPad.LoadScheme(pScheme);
    }
}

//--------------------------------------------------------------------------
//	功能：更新技能
//--------------------------------------------------------------------------
void KUiSkills::UpdateSkill(KUiSkillData *pSkill, int nIndex) {
    if (pSkill) {
        if (m_pSelf) {
            if (pSkill->uGenre == CGOG_SKILL_FIGHT)
                m_pSelf->m_FightSkillPad.UpdateSkill(pSkill, nIndex);
        }
        if (g_pCoreShell) {
            KUiPlayerAttribute Info;
            memset(&Info, 0, sizeof(KUiPlayerAttribute));
            g_pCoreShell->GetGameData(GDI_PLAYER_RT_ATTRIBUTE, (unsigned int) &Info, 0);
            if (Info.nLevel <= SET_NEW_SKILL_TO_IMMED_SKILL_LEVEL_RANGE) {
                g_pCoreShell->OperationRequest(GOI_SET_IMMDIA_SKILL, (unsigned int) pSkill, 1);
                KSystemMessage Msg;
                Msg.byConfirmType = SMCT_NONE;
                Msg.byParamSize = 0;
                Msg.byPriority = 0;
                Msg.eType = SMT_NORMAL;
                Msg.uReservedForUi = 0;

                KIniFile *pIni = g_UiBase.GetCommConfigFile();
                if (pIni) {
                    if (pIni->GetString("InfoString", AUTO_SET_IMMED_SKILL_MSG_ID,
                                        "", Msg.szMessage, sizeof(Msg.szMessage))) {
                        KUiSysMsgCentre::AMessageArrival(&Msg, NULL);
                    }
                    g_UiBase.CloseCommConfigFile();
                }
            }
        }
    }
}

//--------------------------------------------------------------------------
//	功能：更新战斗技能升级点数
//--------------------------------------------------------------------------
void KUiSkills::UpdateFightRemainPoint(int nPoint) {
    m_FightSkillPad.UpdateRemainPoint(nPoint);
}

//窗口函数
int KUiSkills::WndProc(unsigned int uMsg, unsigned int uParam, int nParam) {
    if (uMsg == WND_N_BUTTON_CLICK && (KWndWindow *) uParam == (KWndWindow *) &m_Close) {
        Hide();    // 关闭装备框
        return 0;
    }
    return KWndPageSet::WndProc(uMsg, uParam, nParam);
}

#include "UiLifeSkills.inl"
#include "UiFSBook.inl"
