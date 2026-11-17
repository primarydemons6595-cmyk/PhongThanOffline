/*****************************************************************************************
//	界面--屏幕顶控制操作条
//	Copyright : Kingsoft 2003
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2003-4-22
------------------------------------------------------------------------------------------
*****************************************************************************************/
#pragma once

#include "../Elem/WndToolBar.h"
#include "../Elem/WndButton.h"
#include "UiPortraitMask.h"
#include "UiPtCompanionHud.h"

class KPtAvatarCaption : public KWndText80 {public:void SetCaptionAlign(int align){m_Style=(m_Style&~WNDTEXT_ES_HALIGN_FILTER)|(align==2?WNDTEXT_ES_HALIGN_RIGHT:align==1?WNDTEXT_ES_HALIGN_CENTRE:0);}};

class KUiHeaderControlBar : public KWndToolBar {
public:
    //----界面面板统一的接口函数----
    static KUiHeaderControlBar *OpenWindow();    //打开窗口，返回唯一的一个类对象实例
    static void CloseWindow();        //关闭窗口
    static void LoadScheme(const char *pScheme);//载入界面方案
    static void DefaultScheme(const char *pScheme);//重新初始化界面
    static KUiHeaderControlBar *GetSelf() { return m_pSelf; }

private:
    ~KUiHeaderControlBar() {}

    void Initialize();                            //初始化
    void Breathe();
    void UpdateAvatarInfo();

private:
    static KUiHeaderControlBar *m_pSelf;
    KPtCompanionHud* m_CompanionHud;
    char m_RealmSkins[3][256];int m_RealmSkin;
    KUiPortraitMask m_Portrait;
    KWndImage m_AntiAddiction;
    KWndText80 m_LevelText;
    KWndText80 m_RankWorldText;
    KPtAvatarCaption m_AvatarInfo;
    char m_AvatarInfoText[4][80];
    unsigned int m_AvatarInfoPoll;int m_AvatarInfoMode,m_AvatarInfoSeconds,m_AvatarInfoFont,m_AvatarTextX,m_AvatarTextY,m_AvatarTextAlign;
};

// Official Phong Than VNG top strip.  The layout is loaded by native PAK id
// so an obsolete loose SwordOnline skin cannot replace it.
class KUiPhongThanTopBar : public KWndToolBar {
public:
    static KUiPhongThanTopBar *OpenWindow();
    static void CloseWindow();
    static KUiPhongThanTopBar *GetSelf() { return m_pSelf; }

private:
    ~KUiPhongThanTopBar() {}
    void Initialize();

private:
    static KUiPhongThanTopBar *m_pSelf;
};
