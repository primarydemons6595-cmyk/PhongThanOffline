/*
 * File:     UiTaskNote.h
 * Desc:     任务记事窗口
 * Author:   flying
 * Creation: 2003/7/18
 * 即将第二次参加新人培训
 */
//-----------------------------------------------------------------------------
#pragma once

#if !defined _UITASKNOTE
#define _UITASKNOTE

#include "KIniFile.h"
#include "../Elem/WndShowAnimate.h"
#include "../Elem/WndPureTextBtn.h"
#include "../Elem/WndMessageListBox.h"
#include "../Elem/WndList.h"
#include "../Elem/WndShadow.h"
#include "../Elem/WndText.h"
#include "../Elem/WndImage.h"
#include "../Elem/WndScrollBar.h"


#define MAX_TASK_PAGE 3
#define MAX_TASK_ON_PAGE 12
#define MAX_TASK_HEAD 4096
#define MAX_TASK_TRACKER_ROWS 32

enum RECORD_ACTION {
    REA_EXPLAIN = 0,
    REA_PERFORM,
    REA_FINISH,
};

struct KWndTaskButton {
    KWndButton btn;
    KWndImage onimg;
    KWndImage bdimg;
    unsigned id;
};

class KWndQuestText : public KWndText {char buffer[2048];public:KWndQuestText(){SetTextPtr(buffer,sizeof(buffer));buffer[0]=0;}};

class KWndQuestTracker : public KWndWindow {
public:
    KWndMessageListBox* list;KWndScrollBar* scroll;
    KWndPureTextBtn tradeLink;bool tradeVisible;
    KWndText512* titles;KWndQuestText* bodies;int count,total;int titleHeight[MAX_TASK_TRACKER_ROWS],bodyHeight[MAX_TASK_TRACKER_ROWS];
    void Layout();
    KWndQuestTracker():list(NULL),scroll(NULL),tradeVisible(false),titles(NULL),bodies(NULL),count(0),total(0),wheelRemainder(0){}
    virtual void Breathe();
    virtual int WndProc(unsigned int msg,unsigned int param,int value);
    virtual int PtInWindow(int x,int y);
    bool ScrollAt(int x,int y,int delta);
private:
    int wheelRemainder;
};

class KUiTaskNote : public KWndShowAnimate {
public:
    static KUiTaskNote *OpenWindow();

    //关闭窗口，同时可以选则是否删除对象实例
    static void CloseWindow(bool bDestroy);

    static KUiTaskNote *GetIfVisible();
    static bool ScrollTrackerAt(int x,int y,int delta,KWndWindow* active);

    // 由游戏循环唤醒
    static void WakeUp(KMissionRecord *pRecord);

    //载入界面方案
    static void LoadScheme(const char *pScheme);

private:
    KUiTaskNote();

    virtual ~KUiTaskNote();

    int WndProc(unsigned int uMsg, unsigned int uParam, int nParam);

    void Initialize();

    void OnClose();

    void InitData();        //初始化相关存档数据
    void SaveData();        //保存数据
    void UpdateData(int nSel);

    void UpdateList(int nId);

    void OnDelete(int nIndex);

private:
    // here goes data.
    static KUiTaskNote *m_pSelf;

    KWndMessageListBox m_TaskList;
    KWndScrollBar m_TaskScroll;
    KWndButton m_ScrollUp, m_ScrollDown;
    KWndButton m_Follow, m_Unfollow;
    KWndButton m_ClearTracking;
    KWndButton m_BtnClose;
    KWndPureTextBtn m_TradeBuy,m_TradeReturn;int m_TradeInfoHeight;
    void RefreshTradeLinks();
    KWndMessageListBox m_TaskIntro, m_TaskInfo;
    KWndScrollBar m_IntroScroll,m_InfoScroll;
    KWndQuestTracker m_Tracker;
    KWndMessageListBox m_TrackerList;
    KWndScrollBar m_TrackerScroll;
    KWndText512 m_TrackerTitles[MAX_TASK_TRACKER_ROWS];
    KWndQuestText m_TrackerSteps[MAX_TASK_TRACKER_ROWS];
    bool m_Tracked[MAX_TASK_HEAD];
    int m_Selected;
    KIniFile m_Templates;
    void RebuildTracker();
    void GetTaskText(unsigned int id, char *title, char *intro, char *step);

};

class KUiMissionNote : public KWndShadow {
public:
    static KUiMissionNote *OpenWindow(KMissionRecord *pRecord);

    //关闭窗口，同时可以选则是否删除对象实例
    static void CloseWindow(bool bDestroy);

    static KUiMissionNote *GetIfVisible();

    //载入界面方案
    static void LoadScheme(const char *pScheme);

private:
    KUiMissionNote();

    virtual ~KUiMissionNote();

    int WndProc(unsigned int uMsg, unsigned int uParam, int nParam);

    void Initialize();

    int PtInWindow(int x, int y);                //判断一个点是否在窗口范围内,传入的是绝对坐标
private:
    // here goes data.
    static KUiMissionNote *m_pSelf;


    KWndText32 m_Name;
    KWndMessageListBox m_DestText;
    KWndPureTextBtn m_CloseBtn;
};

#endif
