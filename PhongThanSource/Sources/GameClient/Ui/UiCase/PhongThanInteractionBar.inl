// Native PT interaction toolbar. Player actions are armed, then applied to
// the next selected player through the existing validated interaction path.
#include "UiGame.h"
extern void PhongThanArmPeopleAction(int action);
extern void PhongThanHideClientToTray();
extern void PtAutoSettings();
class PTInteractionBar : public KWndImage {
    KWndButton buttons[11];
    bool expanded;
public:
    PTInteractionBar():expanded(true){}
    void Initialize(){
        char scheme[256],file[320];g_UiBase.GetCurSchemePath(scheme,sizeof(scheme));
        sprintf(file,"%s\\PtInteractionBar.ini",scheme);KIniFile ini;
        if(!ini.Load(file))return;
        Init(&ini,"Main");
        const char* names[]={"Chat","Exchange","AddFriend","ViewEquip","Follow","BlackList","LookStall","EmoteBalloon","HideWindow","OpenInteractivePanel","AutoKill"};
        for(int i=0;i<11;++i){AddChild(&buttons[i]);buttons[i].Init(&ini,names[i]);}
        Wnd_AddWindow(this,WL_NORMAL);Show();Anchor();
    }
    void Anchor(){int width,height;Wnd_GetScreenSize(width,height);SetPosition(width-(expanded?35:8),height-600+230);}
    virtual int WndProc(unsigned int msg,unsigned int param,int data){
        if(msg==WND_N_BUTTON_CLICK){
            for(int i=0;i<11;++i)if(param==(unsigned int)&buttons[i]){
                if(i==9){expanded=!expanded;for(int n=0;n<11;++n){if(n==9)continue;if(expanded)buttons[n].Show();else buttons[n].Hide();}SetSize(expanded?35:8,245);m_nClipWidth=expanded?0:8;m_nClipHeight=expanded?0:224;Anchor();return 0;}
                if(i==8){PhongThanHideClientToTray();return 0;}
                if(i==10){PtAutoSettings();return 0;}
                const int actions[]={ACTION_CHAT,ACTION_TRADE,ACTION_MAKEFRIEND,ACTION_VIEWITEM,ACTION_FOLLOW,ACTION_BLACKLIST,14,ACTION_CONTEXT_FACE};
                PhongThanArmPeopleAction(actions[i]);return 0;
            }
        }
        return KWndImage::WndProc(msg,param,data);
    }
};
static PTInteractionBar* s_PtInteractionBar=0;
static void PtInteractionOpen(){if(!s_PtInteractionBar){s_PtInteractionBar=new PTInteractionBar;s_PtInteractionBar->Initialize();}s_PtInteractionBar->Show();}
static void PtInteractionClose(){PhongThanArmPeopleAction(-1);if(s_PtInteractionBar){s_PtInteractionBar->Destroy();s_PtInteractionBar=0;}}
