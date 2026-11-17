/*
 * File:     KUiHelper.cpp
 * Desc:     详细帮助窗口
 * Author:   flying
 * Creation: 2003/7/16
 */
//////////////////////////////////////////////////////////////////////////
//                     帮助界面文件名字的历史来历
// 为什么详细帮助界面叫做Helper2而由详细帮助界面激活的图片帮助界面叫做
// Helper呢？这是有原因的。什么原因呢？最初的时候传说需要一个类似Diablo
// 按H弹出的关于按钮的图片帮助。这也是最初所理解的帮助系统，而且在那时
// 实现，名字自然叫做Helper了。之后需要一个索引类型的帮助，而且把原先所
// 要求的帮助系统转移到那里去激活，于是在开发时间上在图片帮助之后的索引
// 帮助就叫做Helper2了。口合  口合
//////////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------
#include "KWin32.h"
#include "KIniFile.h"
#include "../Elem/wnds.h"
#include "../Elem/WndMessage.h"
#include "../Elem/WndMessageListBox.h"
#include "../UiBase.h"
#include "UiHelper2.h"
#include "../UiSoundSetting.h"
#include "../../../core/src/gamedatadef.h"
#include "../../../core/src/CoreShell.h"
#include "../ShortcutKey.h"

extern iRepresentShell *g_pRepresentShell;
extern iCoreShell *g_pCoreShell;
//extern IInlinePicEngineSink* g_pIInlinePicSink;
//extern KMouseOver	g_MouseOver;
KUiHelper2 *KUiHelper2::m_pSelf = NULL;

#define        SCHEME_INI_HELP2    "UiHelper2.ini"
#define        DETAIL_HELP_LIST    "DetailHelper.ini"

//--------------------------------------------------------------------------
//	功能：如果窗口正被显示，则返回实例指针
//--------------------------------------------------------------------------
KUiHelper2 *KUiHelper2::GetIfVisible() {
    if (m_pSelf && m_pSelf->IsVisible())
        return m_pSelf;
    return NULL;
}

KUiHelper2::KUiHelper2() {
    m_nIndexCount = 0;m_nCategoryCount=0;
}

KUiHelper2::~KUiHelper2() {
    Clear();
}

//--------------------------------------------------------------------------
//	功能：打开窗口，返回唯一的一个类对象实例
//--------------------------------------------------------------------------
KUiHelper2 *KUiHelper2::OpenWindow(bool bShow) {
    if (m_pSelf == NULL) {
        m_pSelf = new KUiHelper2;
        if (m_pSelf)
            m_pSelf->Initialize();
    }
    if (m_pSelf) {
        int width,height,closeWidth,closeHeight;
        m_pSelf->GetSize(&width,&height);m_pSelf->m_BtnClose.GetSize(&closeWidth,&closeHeight);
        if(width<=0 || height<=0 || closeWidth<=0 || closeHeight<=0){CloseWindow(true);return NULL;}
        if(bShow){
            UiSoundPlay(UI_SI_WND_OPENCLOSE);
            m_pSelf->BringToTop();m_pSelf->Show();
        }else m_pSelf->Hide();
    }
    return m_pSelf;
}

//初始化
void KUiHelper2::Initialize() {
    AddChild(&m_Overlay);
    AddChild(&m_TopicList);AddChild(&m_TopicScroll);m_TopicList.SetScrollbar(&m_TopicScroll);
    AddChild(&m_BtnClose);
    AddChild(&m_IndexList);
    AddChild(&m_IndexScroll);
    AddChild(&m_MessageList);
    m_IndexList.SetScrollbar(&m_IndexScroll);
    AddChild(&m_MessageScroll);
    m_MessageList.SetScrollbar(&m_MessageScroll);

    char Scheme[256];
    g_UiBase.GetCurSchemePath(Scheme, 256);
    LoadScheme(Scheme);

    m_Style &= ~WND_S_VISIBLE;
    Wnd_AddWindow(this);
}

//--------------------------------------------------------------------------
//	功能：关闭窗口，同时可以选则是否删除对象实例
//--------------------------------------------------------------------------
void KUiHelper2::CloseWindow(bool bDestory) {
    if (m_pSelf) {
        m_pSelf->Hide();
        if (bDestory) {
            m_pSelf->Destroy();
            m_pSelf = NULL;
        }
    }
}

void KUiHelper2::LoadScheme(const char *pScheme) {
    if (m_pSelf) {
        char Buff[128];
        KIniFile Ini;
        sprintf(Buff, "%s\\%s", pScheme, SCHEME_INI_HELP2);
        if (Ini.Load(Buff)) {
            m_pSelf->Init(&Ini, "Main");
            m_pSelf->m_BtnClose.Init(&Ini, "BtnClose");
            m_pSelf->m_IndexList.Init(&Ini,"IndexList_Idx");
            m_pSelf->m_TopicList.Init(&Ini,"IndexList_Txt");m_pSelf->m_TopicScroll.Init(&Ini,"IndexScroll_Txt");
            m_pSelf->m_IndexScroll.Init(&Ini,"IndexScroll_Idx");
            m_pSelf->m_MessageScroll.Init(&Ini, "MessageScroll");
            m_pSelf->m_MessageList.Init(&Ini, "MessageList");
            int sw,sh,pw,ph;Wnd_GetScreenSize(sw,sh);m_pSelf->GetSize(&pw,&ph);
            m_pSelf->m_oFixPos.x=(sw-pw)/2;m_pSelf->m_oFixPos.y=(sh-ph)/2;
            m_pSelf->SetPosition(m_pSelf->m_oFixPos.x,m_pSelf->m_oFixPos.y);
            m_pSelf->m_Overlay.Init(&Ini,sw>=1024?"ImgAddition1024":"ImgAddition");
            m_pSelf->m_Overlay.SetPosition(-m_pSelf->m_oFixPos.x,-m_pSelf->m_oFixPos.y);
            m_pSelf->LoadIndexList(pScheme);
        }
    }
}

//窗口函数
int KUiHelper2::WndProc(unsigned int uMsg, unsigned int uParam, int nParam) {
    int nRet = 0;
    switch (uMsg) {
        // A button is clicked.
        case WND_N_BUTTON_CLICK:
            if (uParam == (unsigned int) (KWndWindow *) &m_BtnClose)
                CloseWindow(false);
            break;
            // scrollbar's position changed.
        case WND_N_SCORLLBAR_POS_CHANGED:
            if (uParam == (unsigned int) (KWndWindow *) &m_IndexScroll)
                m_IndexList.SetTopItemIndex(nParam);
            else if(uParam==(unsigned int)&m_TopicScroll)m_TopicList.SetTopItemIndex(nParam);
            else if (uParam == (unsigned int) (KWndWindow *) &m_MessageScroll)
                m_MessageList.SetFirstShowLine(nParam);
            break;
            // list's click
        case WND_N_LIST_ITEM_SEL:
            if(uParam==(unsigned int)&m_IndexList)LoadCategory(nParam);
            else if(uParam==(unsigned int)&m_TopicList)UpdateData(nParam);
            break;
        default:
            nRet = KWndShowAnimate::WndProc(uMsg, uParam, nParam);
    }
    return nRet;
}


void KUiHelper2::Clear() {
    m_IndexList.SetContent(0, 0, 0, 0);
    m_nIndexCount = 0;m_nCategoryCount=0;
}

void KUiHelper2::LoadIndexList(const char* scheme){
    KIniFile ini;char file[320];sprintf(file,"%s\\DetailHelper.ini",scheme);
    if(!ini.Load(file))return;
    ini.GetInteger("List_Idx","Count",0,&m_nCategoryCount);
    if(m_nCategoryCount<0)m_nCategoryCount=0;if(m_nCategoryCount>MAX_INDEX_NUM)m_nCategoryCount=MAX_INDEX_NUM;
    for(int i=0;i<m_nCategoryCount;++i){
        char key[40];IndexData& d=m_CategoryData[i];
        sprintf(key,"%d_Name",i);ini.GetString("List_Idx",key,"",d.szName,sizeof(d.szName));
        if(d.szName[0]=='$'||d.szName[0]=='#')memmove(d.szName,d.szName+1,strlen(d.szName));
        sprintf(key,"%d_Ini",i);ini.GetString("List_Idx",key,"",d.szIni,sizeof(d.szIni));
        sprintf(key,"%d_IniSection",i);ini.GetString("List_Idx",key,"",d.szIniSection,sizeof(d.szIniSection));
    }
    m_IndexList.SetContent((unsigned char*)m_CategoryData,m_nCategoryCount,sizeof(IndexData),0);
    if(m_nCategoryCount){m_IndexList.SetCurSel(0);LoadCategory(0);}
}
void KUiHelper2::LoadCategory(int index){
    if(index<0||index>=m_nCategoryCount)return;
    KIniFile ini;char scheme[256],file[320];g_UiBase.GetCurSchemePath(scheme,sizeof(scheme));
    sprintf(file,"%s\\%s",scheme,m_CategoryData[index].szIni);if(!ini.Load(file))return;
    const char* section=m_CategoryData[index].szIniSection;
    ini.GetInteger(section,"Count",0,&m_nIndexCount);
    if(m_nIndexCount<0)m_nIndexCount=0;if(m_nIndexCount>MAX_INDEX_NUM)m_nIndexCount=MAX_INDEX_NUM;
    for(int i=0;i<m_nIndexCount;++i){
        char key[40];IndexData& d=m_IndexData[i];
        sprintf(key,"%d_Name",i);ini.GetString(section,key,"",d.szName,sizeof(d.szName));
        if(d.szName[0]=='$'||d.szName[0]=='#')memmove(d.szName,d.szName+1,strlen(d.szName));
        sprintf(key,"%d_Ini",i);ini.GetString(section,key,"",d.szIni,sizeof(d.szIni));
        sprintf(key,"%d_IniSection",i);ini.GetString(section,key,"",d.szIniSection,sizeof(d.szIniSection));
    }
    m_TopicList.SetContent((unsigned char*)m_IndexData,m_nIndexCount,sizeof(IndexData),0);
    m_MessageList.Clear();
    if(m_nIndexCount){m_TopicList.SetCurSel(0);UpdateData(0);}
}

void KUiHelper2::UpdateData(int nIndex) {
    if(nIndex<0||nIndex>=m_nIndexCount)return;
    if (strlen(m_IndexData[nIndex].szIni) == 0) {
        m_MessageList.Clear();
        return;
    }
    KIniFile Ini;
    char Scheme[256];
    char szIniFile[128];
    g_UiBase.GetCurSchemePath(Scheme, 256);
    sprintf(szIniFile, "%s\\%s", Scheme, m_IndexData[nIndex].szIni);

    if (Ini.Load(szIniFile)) {
        m_MessageList.SetFirstShowLine(0);
        m_MessageScroll.SetScrollPos(0);
        MsgListBox_LoadContent(&m_MessageList, &Ini,
                               m_IndexData[nIndex].szIniSection);
    }
}
