#include "../../../Headers/PhongThanTeamProtocol.h"
#include "../Elem/PopupMenu.h"
#include "../Elem/WndEdit.h"

class PTTeamSettings : public KWndImage {
    KWndImage editBG;
    KWndEdit32 edit;
    KWndButton recruit,submit,autoApply,closed,autoJoin,refuse,showMember,cancel,loot;
    KWndText256 labels[10],expText,lootText;
    char lootNames[2][64];
public:
    void Initialize(){
        char path[320];g_UiBase.GetCurSchemePath(path,sizeof(path));strcat(path,"\\UiTeamSettings.ini");
        KIniFile ini;if(!ini.Load(path))return;Init(&ini,"Main");
        AddChild(&editBG);editBG.Init(&ini,"EditBG");AddChild(&edit);edit.Init(&ini,"EditInfo");
        KWndButton* buttons[]={&recruit,&submit,&autoApply,&closed,&autoJoin,&refuse,&showMember,&cancel,&loot};
        const char* sections[]={"SetRecruitInfo","SubmitInfo","AutoAcceptApply","CloseTeam","AutoJoinTeam","ChkRefuseInvite","ChkShowMemberInfo","Cancel","ComboxItemShare_Btn"};
        for(int i=0;i<9;++i){AddChild(buttons[i]);buttons[i]->Init(&ini,sections[i]);}
        int x,y;ini.GetInteger("ComboxItemShare","Left",155,&x);ini.GetInteger("ComboxItemShare","Top",265,&y);
        loot.SetPosition(x+8,y-3);
        const char* texts[]={"SetRecruitInfoT_Text","TeamLeaderText","TeamMemberText","AutoAcceptApplyInfo_Text","CloseTeamInfomation_Text","AutoJoinTeamInfomation_Text","ChkRefuseInviteInfo_Text","ChkShowMemberInfoT_Text","TeamShareText"};
        const char* parents[]={"SetRecruitInfoT",0,0,"AutoAcceptApplyInfo","CloseTeamInfomation","AutoJoinTeamInfomation","ChkRefuseInviteInfo","ChkShowMemberInfoT",0};
        for(i=0;i<9;++i){
            AddChild(&labels[i]);labels[i].Init(&ini,texts[i]);
            if(parents[i]){ini.GetInteger(parents[i],"Left",0,&x);ini.GetInteger(parents[i],"Top",0,&y);labels[i].SetPosition(x,y);}
        }
        const int widths[]={180,230,230,105,95,105,95,220,240};
        for(i=0;i<9;++i)labels[i].SetSize(widths[i],18);
        AddChild(&expText);expText.Init(&ini,"ComboxExpShare_Edit");expText.SetPosition(39,266);expText.SetSize(112,16);
        AddChild(&lootText);lootText.Init(&ini,"ComboxItemShare_Edit");lootText.SetPosition(167,265);lootText.SetSize(100,16);
        char text[128];ini.GetString("ComboxExpShare","SelText_0","",text,sizeof(text));expText.SetText(text[0]=='$'?text+1:text);
        for(i=0;i<2;++i){char key[32];sprintf(key,"SelText_%d",i);ini.GetString("ComboxItemShare",key,"",text,sizeof(text));lstrcpyn(lootNames[i],text[0]=='$'?text+1:text,64);}
        // These native controls require additional server/UI contracts.
        recruit.Enable(true);edit.Enable(true);submit.Enable(true);showMember.Enable(true);
        Wnd_AddWindow(this,WL_NORMAL);Hide();
    }
    void Refresh(){
        unsigned int options=g_pCoreShell->TeamOperation(TEAM_OI_PT_OPTIONS,0,0);
        showMember.CheckButton(!(options&8));
        autoJoin.CheckButton(options&1);refuse.CheckButton(options&2);autoApply.CheckButton(options&4);
        KUiPlayerTeam team;memset(&team,0,sizeof(team));g_pCoreShell->TeamOperation(TEAM_OI_GD_INFO,(unsigned int)&team,0);
        closed.CheckButton(team.cNumMember>0 && !team.bOpened);
        edit.Enable(team.bTeamLeader);submit.Enable(team.bTeamLeader);recruit.Enable(team.bTeamLeader);
        PT_TEAM_STATE state;memset(&state,0,sizeof(state));if(g_pCoreShell->TeamOperation(PT_TEAM_UI_GET,(unsigned int)&state,0))recruit.CheckButton(state.ShowName!=0);
        closed.Enable(team.bTeamLeader);autoApply.Enable(team.bTeamLeader);loot.Enable(team.bTeamLeader);
        lootText.SetText(lootNames[team.nModePick==2?0:1]);
    }
    void Open(){
        Refresh();PT_TEAM_STATE state;memset(&state,0,sizeof(state));g_pCoreShell->TeamOperation(PT_TEAM_UI_GET,(unsigned int)&state,0);edit.SetText(state.Name);int sw,sh,w,h;Wnd_GetScreenSize(sw,sh);GetSize(&w,&h);
        SetPosition(max(0,(sw-w)/2),max(0,(sh-h)/2));Show();BringToTop();
    }
    virtual int WndProc(unsigned int msg,unsigned int param,int data){
        if(msg==WND_N_BUTTON_CLICK){
            if(param==(unsigned int)&cancel){Hide();return 0;}
            unsigned int options=g_pCoreShell->TeamOperation(TEAM_OI_PT_OPTIONS,0,0);
            if(param==(unsigned int)&recruit){g_pCoreShell->TeamOperation(PT_TEAM_UI_VISIBLE,recruit.IsButtonChecked(),0);return 0;}
            if(param==(unsigned int)&submit){char name[32];edit.GetText(name,sizeof(name),false);if(PhongThanValidTeamName(name))g_pCoreShell->TeamOperation(PT_TEAM_UI_NAME,(unsigned int)name,0);return 0;}
            if(param==(unsigned int)&showMember){if(showMember.IsButtonChecked())options&=~8;else options|=8;}
            else if(param==(unsigned int)&autoJoin){if(autoJoin.IsButtonChecked())options=(options|1)&~2;else options&=~1;}
            else if(param==(unsigned int)&refuse){if(refuse.IsButtonChecked())options=(options|2)&~1;else options&=~2;}
            else if(param==(unsigned int)&autoApply){
                if(autoApply.IsButtonChecked()){options|=4;g_pCoreShell->TeamOperation(TEAM_OI_CLOSE,0,FALSE);}else options&=~4;
            }else if(param==(unsigned int)&closed){
                bool close=closed.IsButtonChecked()!=0;if(close)options&=~4;
                g_pCoreShell->TeamOperation(TEAM_OI_CLOSE,0,close);
            }else if(param==(unsigned int)&loot){
                KPopupMenuData* menu=(KPopupMenuData*)malloc(MENU_DATA_SIZE(2));if(!menu)return 0;
                KPopupMenu::InitMenuData(menu,2);menu->usMenuFlag|=PM_F_AUTO_DEL_WHEN_HIDE;
                menu->byFontSize=14;menu->nItemWidth=140;
                for(int i=0;i<2;++i){strcpy(menu->Items[i].szData,lootNames[i]);menu->Items[i].uDataLen=strlen(lootNames[i]);menu->Items[i].uID=i;}
                int x,y;loot.GetAbsolutePos(&x,&y);menu->nX=x;menu->nY=y+18;KPopupMenu::Popup(menu,this,1);return 0;
            }else return 0;
            g_pCoreShell->TeamOperation(TEAM_OI_PT_OPTIONS,options,1);Refresh();return 0;
        }
        if(msg==WND_M_MENUITEM_SELECTED){
            if(param==(unsigned int)this && HIWORD(data)==1 && LOWORD(data)<2)
                g_pCoreShell->TeamOperation(TEAM_OI_MODEPICK,0,LOWORD(data)==0?2:1);
            KPopupMenu::Cancel();Refresh();return 0;
        }
        if(msg==WM_KEYDOWN && param==VK_ESCAPE){Hide();return 0;}
        return KWndImage::WndProc(msg,param,data);
    }
    virtual void Breathe(){Refresh();}
};
static PTTeamSettings* s_PtTeamSettings=0;
static void PtTeamSettingsOpen(){
    if(!s_PtTeamSettings){s_PtTeamSettings=new PTTeamSettings;s_PtTeamSettings->Initialize();}
    s_PtTeamSettings->Open();
}
static void PtTeamSettingsClose(){
    if(s_PtTeamSettings){s_PtTeamSettings->Destroy();s_PtTeamSettings=0;}
}
