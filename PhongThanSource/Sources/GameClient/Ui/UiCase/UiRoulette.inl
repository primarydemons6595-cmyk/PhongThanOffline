#include "../../../Core/Src/PhongThanScriptAction.h"
#include "PhongThanRoulette.h"
#include "PhongThanRouletteMotion.h"
#include "PhongThanNativeUiLayout.h"

// Original wheel artwork and twelve marker positions; rewards stay on server.
class KUiPtRoulette : public KWndShowAnimate {
    KWndButton m_Close,m_Stop;
    KWndImage m_Point;
    KWndText256 m_Status;
    DWORD m_Token,m_Started,m_StopAt;
    int m_Outcome,m_X[12],m_Y[12];
    PTRouletteMotion m_Motion;
    bool m_FinishAttempted;
    static KUiPtRoulette* s_Window;
public:
    static void Open(const KPhongThanScriptAction* action) {
        if(!action || action->NumberArgument<0 || action->NumberArgument>11 || !action->DialogToken)return;
        if(!s_Window) {
            s_Window=new KUiPtRoulette;
            if(!s_Window->Initialize()){delete s_Window;s_Window=0;return;}
        }
        s_Window->m_Token=action->DialogToken;
        s_Window->m_Outcome=action->NumberArgument;
        s_Window->m_Started=GetTickCount();s_Window->m_Motion.Start(s_Window->m_Started);
        s_Window->m_FinishAttempted=false;s_Window->m_StopAt=0;s_Window->m_Stop.Enable(true);
        s_Window->m_Status.SetText("\247\141\156\147\040\170\157\141\171\056\040\102\312\155\040\104\365\156\147\040\256\323\040\144\365\156\147\040\166\337\156\147\040\161\165\141\171\056");
        s_Window->BringToTop();s_Window->Show();
    }
    bool Initialize() {
        char scheme[256],path[300],key[32],image[256];
        g_UiBase.GetCurSchemePath(scheme,sizeof(scheme));sprintf(path,"%s\\UiRoulette.ini",scheme);
        KIniFile ini;if(!ini.Load(path))return false;
        int w=800,h=600,x=0,y=0;PhongThanGetNativeUiMetrics(w,h,x,y);
        PhongThanCenterIniWindow(ini,"Main",x,y);
        if(!Init(&ini,"Main") || m_Width<=0 || m_Height<=0)return false;
        m_Close.Init(&ini,"CloseBtn");AddChild(&m_Close);
        ini.WriteInteger("BtnStart&Stop&Close","Up",2);ini.WriteInteger("BtnStart&Stop&Close","Down",3);ini.WriteInteger("BtnStart&Stop&Close","Over",0);m_Stop.Init(&ini,"BtnStart&Stop&Close");AddChild(&m_Stop);
        m_Status.Init(&ini,"Status");m_Status.SetPosition(35,275);m_Status.SetSize(310,32);AddChild(&m_Status);
        ini.GetString("Main","PointImage","",image,sizeof(image));
        m_Point.SetImage(ISI_T_SPR,image,true);AddChild(&m_Point);
        for(int i=0;i<12;++i){sprintf(key,"Pos_%d",i);ini.GetInteger2("Position",key,&m_X[i],&m_Y[i]);}
        m_Token=0;m_Started=0;m_StopAt=0;Wnd_AddWindow(this,WL_TOPMOST);Hide();return true;
    }
    void Finish() {
        if(!m_Token){Hide();return;}
        if(g_pCoreShell && g_pCoreShell->OperationRequest(PT_ROULETTE_FINISH,m_Token,0)) {
            m_Token=0;Hide();
        } else { m_Status.SetText("Ch­a nhËn ®­îc ph¶n håi. BÊm ®ãng ®Ó thö l¹i."); }
    }
    int WndProc(unsigned int message,unsigned int param,int value) {
        if(message==WND_N_BUTTON_CLICK && param==(unsigned int)(KWndWindow*)&m_Stop){if(m_Token&&!m_StopAt){m_StopAt=GetTickCount();m_Motion.Stop(m_Outcome,m_StopAt);if(!m_StopAt)m_StopAt=1;m_Stop.Enable(false);m_Status.SetText("\247\141\156\147\040\144\365\156\147\040\166\337\156\147\040\161\165\141\171\056\056\056");}return 1;}
        if((message==WM_KEYDOWN && param==VK_ESCAPE) ||
            (message==WND_N_BUTTON_CLICK && param==(unsigned int)(KWndWindow*)&m_Close)) {
            m_Token=0;Hide();return 1; // A disconnect must never trap the UI.
        }
        return KWndShowAnimate::WndProc(message,param,value);
    }
    void Breathe() {
        if(!m_Token || !IsVisible())return;
        DWORD now=GetTickCount();m_Motion.Tick(now,m_StopAt!=0);
        m_Point.SetPosition(m_X[m_Motion.Sector],m_Y[m_Motion.Sector]);m_Point.NextFrame();
        if(m_StopAt&&m_Motion.Remaining==0){
            m_Status.SetText("\247\267\040\144\365\156\147\056\040\247\141\156\147\040\156\150\313\156\040\164\150\255\353\156\147\056\056\056");
            if(now-m_Motion.FinishedAt>=600&&!m_FinishAttempted){m_FinishAttempted=true;Finish();}
        }
    }
};
KUiPtRoulette* KUiPtRoulette::s_Window=0;
