#pragma once

#include "KWin32.h"
#include "KIniFile.h"
#include "KStrBase.h"
#include "GameDataDef.h"
#include <time.h>
#include "../Elem/wnds.h"
#include "../Elem/WndMessage.h"
#include "../UiSoundSetting.h"
#include "UiFaceSelector.h"
#include "UiTaskNote.h"
#include "PhongThanNativeUiLayout.h"
#include "UiTaskDataFile.h"
#include "UiMiniMap.h"
#include "../../../../Headers/PhongThanCommerceNavigation.h"
#include "../../../../Headers/PhongThanCrossRoute.h"
#include "../../../../Headers/PhongThanTaskJournal.h"
#include "../../../../Headers/PhongThanQuestHudText.h"
#include "../../Represent/iRepresent/iRepresentShell.h"
#include "../../../core/src/CoreShell.h"
#include "../../../core/src/gamedatadef.h"
#include "../../../Engine/src/Text.h"
#include "../../../Engine/src/KDebug.h"
#include "../UiBase.h"
#include "../../../Represent/iRepresent/KRepresentUnit.h"
#include "UiSysMsgCentre.h"

extern IInlinePicEngineSink *g_pIInlinePicSink;
extern iCoreShell *g_pCoreShell;
extern iRepresentShell *g_pRepresentShell;
#include "UiCommerceNavigation.inl"


#define    SCHEME_INI_TASKNOTE            "UiTaskNote.ini"
#define    SCHEME_INI_MISSIONNOTE        "UiTaskNote-MissionNote.ini"
#define        DEFAULT_TASK                "\\spr\\ui3\\ÐÂÈÎÎñÏµÍ³\\ÈÎÎñÍ¼±ê\\icon_task_003.spr"


KUiTaskNote *KUiTaskNote::m_pSelf = NULL;


static void TaskTemplateText(KIniFile &ini, const char *section, const char *key, char *out, int size) {
    ini.GetString(section, key, "", out, size);
    if (out[0] == '$' && out[1] == ' ')
        memmove(out, out + 2, strlen(out + 2) + 1);
}
static void SetQuestText(KWndText512 &wnd, const char *text) {
    char buffer[1024];
    g_StrCpyLen(buffer, text, sizeof(buffer));
    int len = TEncodeText(buffer, strlen(buffer));
    wnd.SetText(buffer, len);
}
static void SetQuestText(KWndMessageListBox &wnd,const char *text) {
    char buffer[1024];g_StrCpyLen(buffer,text,sizeof(buffer));
    int len=TEncodeText(buffer,strlen(buffer));
    wnd.Clear();wnd.AddOneMessage(buffer,len);wnd.SetFirstShowLine(0);
}
// Only the scrollbar and the trade link consume clicks; quest text is an overlay.
int KWndQuestTracker::PtInWindow(int x,int y){
    return IsVisible() && ((scroll && scroll->PtInWindow(x,y)) ||
        (tradeVisible && tradeLink.PtInWindow(x,y)));
}
bool KWndQuestTracker::ScrollAt(int x,int y,int delta){
    if(!IsVisible() || !scroll || scroll->GetMaxValue()<=0)return false;
    int l,t,w,h;GetAbsolutePos(&l,&t);GetSize(&w,&h);
    if(x<l || x>=l+w || y<t || y>=t+h)return false;
    wheelRemainder+=delta;int steps=wheelRemainder/WHEEL_DELTA;
    wheelRemainder-=steps*WHEEL_DELTA;
    scroll->SetScrollPos(scroll->GetScrollPos()-steps*3);Layout();return true;
}
bool KUiTaskNote::ScrollTrackerAt(int x,int y,int delta,KWndWindow* active){
    if(!m_pSelf || (active && active!=&m_pSelf->m_Tracker))return false;
    return m_pSelf->m_Tracker.ScrollAt(x,y,delta);
}
void KWndQuestTracker::Breathe(){
    if(tradeVisible){PTCommerceRouteInfo trade;if(PTCommerceQuery(trade,0)){char label[82];PTCommerceRouteLabel(trade,label,sizeof(label));tradeLink.SetText(label);int w,h;GetSize(&w,&h);tradeLink.SetPosition(18,h-22);tradeLink.Show();}else tradeLink.Hide();}else tradeLink.Hide();
    if(!scroll||!list)return;int x,y,l,t,w,h;Wnd_GetCursorPos(&x,&y);GetAbsolutePos(&l,&t);GetSize(&w,&h);
    if(x>=l&&x<l+w&&y>=t&&y<t+h&&scroll->GetMaxValue()>0)scroll->Show();else scroll->Hide();
}
int KWndQuestTracker::WndProc(unsigned int msg,unsigned int param,int value){
    if(msg==WND_N_BUTTON_CLICK&&param==(unsigned int)(KWndWindow*)&tradeLink){PTCommerceNavigate(0);return true;}
    if(msg==WND_N_SCORLLBAR_POS_CHANGED&&param==(unsigned int)scroll){Layout();return true;}
    return KWndWindow::WndProc(msg,param,value);
}
void KWndQuestTracker::Layout(){
    if(!scroll||!titles||!bodies)return;int width,height;GetSize(&width,&height);if(tradeVisible)height=max(1,height-24);int top=-scroll->GetScrollPos()*14;
    for(int i=0;i<count;++i){
        KWndText* windows[2]={&titles[i],&bodies[i]};int heights[2]={titleHeight[i],bodyHeight[i]};int lineHeights[2]={17,14};
        for(int part=0;part<2;++part){KWndText* w=windows[part];int h=heights[part],lh=lineHeights[part];
            int skip=top<0?(-top+lh-1)/lh:0,drawTop=top+skip*lh,remaining=max(0,h/lh-skip),visible=min(remaining,max(0,(height-drawTop)/lh));
            if(top>=height||top+h<=0||visible<=0)w->Hide();else{w->SetPosition(part?26:18,max(0,drawTop));w->SetSize(part?252:260,visible*lh);w->SetTopLine(skip);w->Show();}
            top+=h;
        }
        top+=14;
    }
}
KUiTaskNote::KUiTaskNote() {
    memset(m_Tracked, 0, sizeof(m_Tracked));
    m_Selected = -1;
}
KUiTaskNote::~KUiTaskNote() {}
extern void PtQuestAttention(bool pending);
KUiTaskNote *KUiTaskNote::OpenWindow() {
    PtQuestAttention(false);
    if (!m_pSelf) {
        m_pSelf = new KUiTaskNote;
        m_pSelf->Initialize();
        m_pSelf->InitData();
    }
    m_pSelf->BringToTop();
    m_pSelf->Show();
    return m_pSelf;
}
void KUiTaskNote::CloseWindow(bool destroy) {
    if (!m_pSelf) return;
    m_pSelf->Hide();
    if (destroy) {
        m_pSelf->SaveData();
        KTaskDataFile::ClearAll();
        m_pSelf->m_Tracker.Hide();
        m_pSelf->Destroy();
        m_pSelf = NULL;
    }
}
KUiTaskNote *KUiTaskNote::GetIfVisible() {
    return m_pSelf && m_pSelf->IsVisible() ? m_pSelf : NULL;
}
void KUiTaskNote::InitData() {
    KTaskDataFile::LoadData();
    char path[MAX_PATH], key[32];
    g_UiBase.GetUserPrivateDataFolder(path, sizeof(path));
    strcat(path, "TaskTracking.ini");
    KIniFile settings;
    if (settings.Load(path)) {
        for (int id = 0; id < MAX_TASK_HEAD; ++id) {
            int enabled = 0;
            sprintf(key, "Task_%d", id);
            settings.GetInteger("Tracking", key, 0, &enabled);
            m_Tracked[id] = enabled != 0;
        }
    }
    UpdateData(0);
    RebuildTracker();
}
void KUiTaskNote::SaveData() { KTaskDataFile::SaveData(); }
void KUiTaskNote::LoadScheme(const char *scheme) {
    if (!m_pSelf) return;
    char path[MAX_PATH];
    sprintf(path, "%s\\UiTaskNote.ini", scheme);
    KIniFile ini;
    if (!ini.Load(path)) return;
    KUiTaskNote &w = *m_pSelf;
    PhongThanAnchorNativePanel(ini,false);
    w.Init(&ini, "Main");
    w.m_TaskList.Init(&ini, "TaskList");
    w.m_TaskScroll.Init(&ini, "Scroll_Bar");
    w.m_ScrollUp.Init(&ini, "Scroll_BtnUp");
    w.m_ScrollDown.Init(&ini, "Scroll_BtnDown");
    w.m_TaskList.SetScrollbar(&w.m_TaskScroll);
    w.m_TaskIntro.Init(&ini, "TaskIntro");
    w.m_TaskInfo.Init(&ini, "TaskInfo");
    {int width;w.m_TaskInfo.GetSize(&width,&w.m_TradeInfoHeight);}
    w.m_TradeBuy.Init(&ini,"CommerceBuyLink");w.m_TradeReturn.Init(&ini,"CommerceReturnLink");
    w.m_Tracker.tradeLink.Init(&ini,"TrackerCommerceLink");
    w.m_IntroScroll.Init(&ini,"IntroScroll");w.m_InfoScroll.Init(&ini,"InfoScroll");
    w.m_TaskIntro.SetScrollbar(&w.m_IntroScroll);w.m_TaskInfo.SetScrollbar(&w.m_InfoScroll);
    w.m_BtnClose.Init(&ini, "CloseBtn");
    w.m_Follow.Init(&ini, "FollowBtn");
    w.m_Unfollow.Init(&ini, "UnfollowBtn");
    w.m_ClearTracking.Init(&ini, "DeleteBtn");
    w.m_Tracker.Init(&ini, "Tracker");
    w.m_TrackerList.Init(&ini, "TrackerList");
    // Scrollbar positions are slider centres: keep the entire knob inside the tracker.
    int trackerHeight,sliderHeight;
    ini.GetInteger("TrackerScroll","Height",360,&trackerHeight);
    ini.GetInteger("TrackerScroll_Btn","Height",14,&sliderHeight);
    ini.WriteInteger("TrackerScroll","SlideBegin",(sliderHeight+1)/2);
    ini.WriteInteger("TrackerScroll","SlideEnd",max((sliderHeight+1)/2,trackerHeight-sliderHeight/2));
    w.m_TrackerScroll.Init(&ini,"TrackerScroll");
    w.m_Tracker.list=&w.m_TrackerList;w.m_Tracker.scroll=&w.m_TrackerScroll;w.m_Tracker.titles=w.m_TrackerTitles;w.m_Tracker.bodies=w.m_TrackerSteps;
    for (int t=0;t<MAX_TASK_TRACKER_ROWS;++t) {
        w.m_TrackerTitles[t].Init(&ini,"TrackerTitle");
        w.m_TrackerSteps[t].Init(&ini,"TrackerStep");
    }
    w.m_Tracker.Hide();
    sprintf(path, "%s\\taskinfo.ini", scheme);
    w.m_Templates.Load(path);
}
void KUiTaskNote::Initialize() {
    AddChild(&m_TaskList); AddChild(&m_TaskScroll);
    AddChild(&m_ScrollUp); AddChild(&m_ScrollDown);
    AddChild(&m_TaskIntro); AddChild(&m_TaskInfo); AddChild(&m_BtnClose);
    AddChild(&m_IntroScroll);AddChild(&m_InfoScroll);
    AddChild(&m_Follow); AddChild(&m_Unfollow); AddChild(&m_ClearTracking);
    for (int t=0;t<MAX_TASK_TRACKER_ROWS;++t) {
        m_Tracker.AddChild(&m_TrackerTitles[t]);
        m_Tracker.AddChild(&m_TrackerSteps[t]);
    }
    AddChild(&m_TradeBuy);AddChild(&m_TradeReturn);m_TradeBuy.Hide();m_TradeReturn.Hide();
    m_Tracker.AddChild(&m_Tracker.tradeLink);m_Tracker.tradeLink.Hide();
    m_Tracker.AddChild(&m_TrackerList);m_Tracker.AddChild(&m_TrackerScroll);
    char scheme[MAX_PATH];
    g_UiBase.GetCurSchemePath(scheme, sizeof(scheme));
    LoadScheme(scheme);
    Wnd_AddWindow(this);
    Wnd_AddWindow(&m_Tracker, WL_LOWEST);
}
int KUiTaskNote::WndProc(unsigned int msg, unsigned int param, int value) {
    if(msg==WND_N_BUTTON_CLICK&&param==(unsigned int)(KWndWindow*)&m_TradeBuy){PTCommerceNavigate(0);return true;}
    if(msg==WND_N_BUTTON_CLICK&&param==(unsigned int)(KWndWindow*)&m_TradeReturn){PTCommerceNavigate(1);return true;}
    if(msg==WND_N_SCORLLBAR_POS_CHANGED && param==(unsigned int)&m_IntroScroll){m_TaskIntro.SetFirstShowLine(value);return true;}
    if(msg==WND_N_SCORLLBAR_POS_CHANGED && param==(unsigned int)&m_InfoScroll){m_TaskInfo.SetFirstShowLine(value);return true;}
    if (msg == WND_N_SCORLLBAR_POS_CHANGED && param == (unsigned int)&m_TaskScroll) {
        m_TaskList.SetFirstShowLine(value); return true;
    }
    if ((msg == WND_N_LIST_ITEM_SEL || msg == WND_N_LIST_ITEM_ACTIVE) && param == (unsigned int)&m_TaskList) {
        UpdateList(value); return true;
    }
    if (msg == WND_N_BUTTON_CLICK) {
        if (param == (unsigned int)&m_BtnClose) { OnClose(); return true; }
        if (param == (unsigned int)&m_ScrollUp) { m_TaskScroll.ScrollLine(true); return true; }
        if (param == (unsigned int)&m_ScrollDown) { m_TaskScroll.ScrollLine(false); return true; }
        if (param == (unsigned int)&m_ClearTracking) {
            memset(m_Tracked, 0, sizeof(m_Tracked)); UpdateData(0); RebuildTracker(); return true;
        }
        if (param == (unsigned int)&m_Follow || param == (unsigned int)&m_Unfollow) {
            if (m_Selected >= 0 && m_Selected < MAX_TASK_HEAD)
                m_Tracked[m_Selected] = param == (unsigned int)&m_Follow;
            UpdateData(0); RebuildTracker(); return true;
        }
    }
    return KWndShowAnimate::WndProc(msg, param, value);
}
void KUiTaskNote::OnClose() { CloseWindow(false); }
void KUiTaskNote::WakeUp(KMissionRecord *record) {
    if (!record || !record->nHeaderLen || (record->nRecordAct != REA_FINISH && !record->nContentLen)) return;
    const unsigned int incomingId=PhongThanTaskRecordId(atoi(record->szHeader),record->szContent);
    // Accepted journal records start the tracker without opening the F11 panel.
    if(!m_pSelf && (incomingId>0 && incomingId<MAX_TASK_HEAD) && record->nRecordAct!=REA_FINISH){
        m_pSelf=new KUiTaskNote;m_pSelf->Initialize();m_pSelf->InitData();m_pSelf->Hide();
    }
    if (!m_pSelf) KTaskDataFile::LoadData();
    TASK_SYSTEM_RECORD data = {0};
    data.tTime = time(NULL); data.uReserved = record->uValue;
    data.uHeaderID = PhongThanTaskRecordId(atoi(record->szHeader),record->szContent);
    data.nAct = record->nRecordAct;
    if(PhongThanTaskRewardStep(data.uHeaderID,PhongThanTaskRecordStep(data.uHeaderID,data.uReserved,record->szContent)))data.nAct=REA_FINISH;
    data.nContentLen = record->nContentLen; data.pBuffer = record->szContent;
    if (!data.nContentLen) { data.nContentLen=1; data.pBuffer=" "; }
    bool existed=false;
    for(int previous=KTaskDataFile::GetSystemRecordCount()-1;previous>=0;--previous){
        const TASK_SYSTEM_RECORD* old=KTaskDataFile::GetSystemRecord(previous);
        if(old && old->uHeaderID==data.uHeaderID){
            existed=old->nAct!=REA_FINISH;
            if(old->nAct==data.nAct && old->uReserved==data.uReserved && old->nContentLen==data.nContentLen && !memcmp(old->cBuffer,data.pBuffer,data.nContentLen))return;
            break;
        }
    }
    if(m_pSelf && !existed && (incomingId>0 && incomingId<MAX_TASK_HEAD) && data.nAct!=REA_FINISH)m_pSelf->m_Tracked[incomingId]=true;
    bool inserted = KTaskDataFile::InsertSystemRecord(&data);
    if(inserted && !existed && data.nAct!=REA_FINISH && incomingId>0 && incomingId<MAX_TASK_HEAD)PtQuestAttention(!GetIfVisible());
    if (inserted && m_pSelf) KTaskDataFile::SaveData();
    if (!m_pSelf) {
        if (inserted) KTaskDataFile::SaveData();
        KTaskDataFile::ClearAll();
    } else if (inserted) {
        m_pSelf->UpdateData(0); m_pSelf->RebuildTracker();
    }
}
void KUiTaskNote::GetTaskText(unsigned int id, char *title, char *intro, char *step) {
    char section[32], key[32];
    sprintf(section, "Task_%u", id);
    TaskTemplateText(m_Templates, section, "Title", title, 512);
    TaskTemplateText(m_Templates, section, "Intro", intro, 512);
    if(!title[0]&&id>=4001&&id<=4004){static const char* names[]={"V¹n Tiªn TrËn Thæ","V¹n Tiªn TrËn Thñy","V¹n Tiªn TrËn Háa","V¹n Tiªn TrËn Phong"};g_StrCpyLen(title,names[id-4001],512);}
    if (!title[0]) sprintf(title, "NhiÖm vô %u", id);
    int currentStep = 0;
    const TASK_SYSTEM_RECORD *latest = NULL;
    for (int r = KTaskDataFile::GetSystemRecordCount()-1; r >= 0; --r) {
        const TASK_SYSTEM_RECORD *p = KTaskDataFile::GetSystemRecord(r);
        if (p && p->uHeaderID == id) { latest = p; break; }
    }
    if (latest) {
        int templateId, parsedStep;
        if (sscanf(latest->cBuffer, "Task %d - step %d", &templateId, &parsedStep) == 2 && templateId == (int)id)
            currentStep = parsedStep;
        else currentStep = (int)latest->uReserved;
    }
    if(((id>=3000&&id<=3099)||(id>=4001&&id<=4004))&&latest){g_StrCpyLen(step,latest->cBuffer,512);return;}
    if (currentStep < 0 || currentStep > 255) currentStep = 0;
    sprintf(key, "Step_%d", currentStep);
    TaskTemplateText(m_Templates, section, key, step, 512);
    if (step[0] && latest && strchr(step,'%')) {
        char formatted[512];PhongThanFormatTaskStep(formatted,sizeof(formatted),step,latest->cBuffer);
        g_StrCpyLen(step,formatted,512);
    }
    if (!step[0] && latest) {
        const char* text=latest->cBuffer;
        int taskId,stepId;
        if (sscanf(text,"Task %d - step %d",&taskId,&stepId)==2) {
            const char* body=strchr(text,':');
            if(body) { text=body+1; while(*text==' ')++text; }
        }
        g_StrCpyLen(step,text,512);
    }
}
void KUiTaskNote::UpdateData(int unused) {
    bool seen[MAX_TASK_HEAD] = {0};
    int scroll = m_TaskScroll.GetScrollPos();
    m_TaskList.Clear();
    m_TaskList.SetCapability(MAX_TASK_HEAD);
    int selection = -1;
    for (int r = KTaskDataFile::GetSystemRecordCount()-1; r >= 0; --r) {
        const TASK_SYSTEM_RECORD *record = KTaskDataFile::GetSystemRecord(r);
        if (!record || record->uHeaderID >= MAX_TASK_HEAD || seen[record->uHeaderID]) continue;
        unsigned int id = record->uHeaderID;
        seen[id] = true;
        if (record->nAct == REA_FINISH || PhongThanTaskRewardStep(id,PhongThanTaskRecordStep(id,record->uReserved,record->cBuffer))) { m_Tracked[id]=false; continue; }
        char title[512], intro[512], step[512];
        GetTaskText(id, title, intro, step);
        int length = TEncodeText(title, strlen(title));
        length = TRemoveCtrlInEncodedText(title, length);
        if (m_Tracked[id]) {
            memmove(title + 4, title, length);
            title[0] = KTC_COLOR; title[1] = 0; title[2] = (char)255; title[3] = 0;
            length += 4;
        }
        int index = m_TaskList.GetMsgCount();
        m_TaskList.AddOneMessage(title, length, 0, id);
        if ((int)id == m_Selected) selection = index;
    }
    if (selection < 0 && m_TaskList.GetMsgCount()) selection = 0;
    m_TaskList.SetCurSel(selection);
    UpdateList(selection);
    m_TaskScroll.SetScrollPos(scroll);
}
void KUiTaskNote::UpdateList(int index) {
    m_Selected = index >= 0 ? m_TaskList.GetOneMessageData(index) : -1;
    m_TaskIntro.Clear(); m_TaskInfo.Clear();RefreshTradeLinks();
    if (m_Selected < 0 || m_Selected >= MAX_TASK_HEAD) return;
    char title[512], intro[512], step[512];
    GetTaskText(m_Selected, title, intro, step);
    SetQuestText(m_TaskIntro, intro);
    SetQuestText(m_TaskInfo, step);
}
void KUiTaskNote::RebuildTracker() {
    const int oldPos=m_TrackerScroll.GetScrollPos();m_TrackerList.Hide();
    for(int i=0;i<MAX_TASK_TRACKER_ROWS;++i){m_TrackerTitles[i].Hide();m_TrackerSteps[i].Hide();}
    int sw,sh;Wnd_GetScreenSize(sw,sh);const int top=240,height=max(100,min(360,sh-top-95));
    m_Tracker.SetPosition(0,top);m_Tracker.SetSize(280,height);m_TrackerScroll.SetPosition(0,0);m_TrackerScroll.SetSize(14,height);
    bool seen[MAX_TASK_HEAD]={0};int count=0,total=0;
    for(int r=KTaskDataFile::GetSystemRecordCount()-1;r>=0&&count<MAX_TASK_TRACKER_ROWS;--r){
        const TASK_SYSTEM_RECORD* latest=KTaskDataFile::GetSystemRecord(r);if(!latest||latest->uHeaderID>=MAX_TASK_HEAD)continue;
        int id=latest->uHeaderID;if(seen[id])continue;seen[id]=true;if(!m_Tracked[id])continue;
        if(latest->nAct==REA_FINISH||PhongThanTaskRewardStep(id,PhongThanTaskRecordStep(id,latest->uReserved,latest->cBuffer))){m_Tracked[id]=false;continue;}
        char title[512],intro[512],step[512],body[2048];GetTaskText(id,title,intro,step);const char* text=step;
        if((id>=3000&&id<=3099)||(id>=4001&&id<=4004)){const char* newline=strstr(step,"<enter>");if(newline)text=newline+7;}
        PhongThanQuestHudText(body,sizeof(body),text);
        KWndText512& heading=m_TrackerTitles[count];heading.SetFontSize(16);heading.SetTextColor(0xff00ff00);heading.SetSize(260,100);SetQuestText(heading,title);
        KWndQuestText& content=m_TrackerSteps[count];content.SetFontSize(13);content.SetTextColor(0xffe8e8b6);content.SetSize(252,1400);
        int len=TEncodeText(body,strlen(body));content.SetText(body,len);
        m_Tracker.titleHeight[count]=max(1,heading.GetLineCount())*17;m_Tracker.bodyHeight[count]=max(1,content.GetLineCount())*14;
        total+=m_Tracker.titleHeight[count]+m_Tracker.bodyHeight[count]+14;++count;
    }
    PTCommerceRouteInfo trade;m_Tracker.tradeVisible=m_Tracked[3090]&&PTCommerceQuery(trade,0);
    m_Tracker.count=count;m_Tracker.total=total;m_TrackerScroll.SetValueRange(0,max(0,(total-height+(m_Tracker.tradeVisible?24:0)+13)/14));m_TrackerScroll.SetScrollPos(oldPos);m_Tracker.Layout();m_TrackerScroll.Hide();
    if(count)m_Tracker.Show();else m_Tracker.Hide();
    char path[MAX_PATH], key[32];
    g_UiBase.GetUserPrivateDataFolder(path, sizeof(path));
    strcat(path, "TaskTracking.ini");
    KIniFile settings;
    for (int task = 0; task < MAX_TASK_HEAD; ++task) {
        sprintf(key, "Task_%d", task);
        settings.WriteInteger("Tracking", key, m_Tracked[task] ? 1 : 0);
    }
    settings.Save(path);
}

/////////////////////////////////////
/////////////////////////////////////
KUiMissionNote *KUiMissionNote::m_pSelf = NULL;

// constructor.
KUiMissionNote::KUiMissionNote() {
}

// destructor
KUiMissionNote::~KUiMissionNote() {
}

// opennnnnnnnnnnnnnnnnnnnnnnnnnn
KUiMissionNote *KUiMissionNote::OpenWindow(KMissionRecord *pRecord) {
    if (m_pSelf == NULL) {
        m_pSelf = new KUiMissionNote;
        if (m_pSelf) {
            m_pSelf->Initialize();
        }
    }
    if (m_pSelf) {
        m_pSelf->m_DestText.Clear();
        if (pRecord->nHeaderLen && pRecord->nContentLen) {
            m_pSelf->m_Name.SetText(pRecord->szHeader);
            int nLen = TEncodeText(pRecord->szContent, pRecord->nContentLen);
            m_pSelf->m_DestText.AddOneMessage(pRecord->szContent, nLen, 0);
        }
        UiSoundPlay(UI_SI_WND_OPENCLOSE);
        m_pSelf->BringToTop();
        m_pSelf->Show();
    }
    return m_pSelf;
}

// ....
void KUiMissionNote::CloseWindow(bool bDestroy) {
    if (m_pSelf) {
        m_pSelf->Hide();
        if (bDestroy) {
            m_pSelf->Destroy();
            m_pSelf = NULL;
        }
    }
}

KUiMissionNote *KUiMissionNote::GetIfVisible() {
    if (m_pSelf && m_pSelf->IsVisible())
        return m_pSelf;
    return NULL;
}


void KUiMissionNote::LoadScheme(const char *pScheme) {
    if (m_pSelf) {
        char Buff[128];
        KIniFile Ini;
        sprintf(Buff, "%s\\%s", pScheme, SCHEME_INI_MISSIONNOTE);
        if (Ini.Load(Buff)) {
            m_pSelf->Init(&Ini, "Main");
            m_pSelf->m_Name.Init(&Ini, "Name");
            m_pSelf->m_DestText.Init(&Ini, "DestText");
            m_pSelf->m_CloseBtn.Init(&Ini, "CloseBtn");
        }
    }
}

// Initialize
void KUiMissionNote::Initialize() {
    AddChild(&m_Name);
    AddChild(&m_DestText);
    AddChild(&m_CloseBtn);
    char Scheme[256];
    g_UiBase.GetCurSchemePath(Scheme, sizeof(Scheme));
    LoadScheme(Scheme);

    Wnd_AddWindow(this);
}

// Deal with the window messages.
int KUiMissionNote::WndProc(unsigned int uMsg, unsigned int uParam, int nParam) {
    int nResult = false;
    if (uMsg == WND_N_BUTTON_CLICK) {
        if (uParam == (unsigned int) (KWndWindow *) &m_CloseBtn) {
            CloseWindow(true);
            nResult = true;
        }
    } else {
        nResult = KWndShadow::WndProc(uMsg, uParam, nParam);
    }
    return nResult;
}

int KUiMissionNote::PtInWindow(int x, int y) {
    int nRet = 0;
    if (KWndWindow::PtInWindow(x, y))
        nRet = m_CloseBtn.PtInWindow(x, y);

    return nRet;
}
