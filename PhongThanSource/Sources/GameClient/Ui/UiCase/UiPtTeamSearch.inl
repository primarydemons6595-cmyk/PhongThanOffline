// Separate reference team-search panel. The G panel remains team management.
#include "UiPtTeamSearch.h"
class KPtTeamSearch : public KWndImage {
    KWndList leaders;
    KWndText80 headers[2];
    KWndScrollBar scroll;
    KWndButton close,refresh,join;
    KUiTeamItem teams[256];
    int count;
    DWORD lastRequest;
    static KPtTeamSearch* self;
    void Refresh() {
        count=0;leaders.SetContent(NULL,0,0,0);join.Enable(false);
        if(g_pCoreShell)g_pCoreShell->TeamOperation(TEAM_OI_COLLECT_NEARBY_LIST,0,0);
    }
    bool Initialize() {
        char scheme[256],path[384];g_UiBase.GetCurSchemePath(scheme,sizeof(scheme));
        sprintf(path,"%s\\UiPtTeamSearch.ini",scheme);KIniFile ini;
        if(!ini.Load(path))return false;
        if(!Init(&ini,"Main"))return false;
        leaders.Init(&ini,"NearbyList");scroll.Init(&ini,"NearbyScroll_Bar");
        close.Init(&ini,"Cancel");refresh.Init(&ini,"Refresh");join.Init(&ini,"JionTeam");
        AddChild(&leaders);AddChild(&scroll);AddChild(&close);AddChild(&refresh);AddChild(&join);
        const char* headerKeys[]={"ColumnText_0","ColumnText_2"};
        for(int h=0;h<2;++h){char section[32],label[128];sprintf(section,"PtSearchHeader%d",h);
            ini.GetString("NearbyList",headerKeys[h],"",label,sizeof(label));
            ini.WriteInteger(section,"Left",h?229:25);ini.WriteInteger(section,"Top",50);
            ini.WriteInteger(section,"Width",120);ini.WriteInteger(section,"Height",18);
            ini.WriteInteger(section,"Font",14);ini.WriteInteger(section,"HAlign",1);
            ini.WriteInteger(section,"DummyWnd",1);ini.WriteString(section,"Color","255,255,255");
            AddChild(&headers[h]);headers[h].Init(&ini,section);headers[h].SetText(label[0]=='$'?label+1:label);
        }
        leaders.SetPosition(25,75);leaders.SetSize(320,113);scroll.SetPosition(349,77);
        leaders.SetScrollbar(&scroll);
        int sw,sh;Wnd_GetScreenSize(sw,sh);SetPosition(max(0,(sw-m_Width)/2),max(0,(sh-m_Height)/2));
        count=0;lastRequest=0;Wnd_AddWindow(this);return true;
    }
    void Join() {
        int selected=leaders.GetCurSel();if(!g_pCoreShell||selected<0||selected>=count)return;
        KUiPlayerTeam info;memset(&info,0,sizeof(info));
        g_pCoreShell->TeamOperation(TEAM_OI_GD_INFO,(unsigned int)&info,0);
        if(info.cNumMember)return;
        DWORD now=GetTickCount();if(lastRequest && now-lastRequest<1000)return;
        lastRequest=now;
        g_pCoreShell->TeamOperation(TEAM_OI_APPLY,(unsigned int)&teams[selected],0);
    }
public:
    static void Open() {
        if(!g_pCoreShell)return;
        if(!self){self=new KPtTeamSearch;if(!self->Initialize()){delete self;self=NULL;return;}}
        self->Show();self->BringToTop();self->Refresh();
    }
    static void Close(){if(self)self->Hide();}
    static void Append(KUiTeamItem* input,int n) {
        if(!self||!self->IsVisible()||!input||n<=0||n>256)return;
        for(int i=0;i<n;++i){
            bool duplicate=false;
            for(int j=0;j<self->count;++j)if(self->teams[j].Leader.uId==input[i].Leader.uId){duplicate=true;break;}
            if(!duplicate&&self->count<256){self->teams[self->count]=input[i];self->teams[self->count].Leader.Name[sizeof(input[i].Leader.Name)-1]=0;++self->count;}
        }
        self->leaders.SetContent((BYTE*)self->teams,self->count,sizeof(KUiTeamItem),
            (int)((char*)&self->teams[0].Leader.Name-(char*)&self->teams[0]));
        self->join.Enable(self->count>0);
    }
    virtual int WndProc(unsigned int msg,unsigned int param,int data) {
        if(msg==WM_KEYDOWN && param==VK_ESCAPE){Hide();return 1;}
        if(msg==WND_N_BUTTON_CLICK){
            if(param==(unsigned int)(KWndWindow*)&close){Hide();return 1;}
            if(param==(unsigned int)(KWndWindow*)&refresh){Refresh();return 1;}
            if(param==(unsigned int)(KWndWindow*)&join){Join();return 1;}
        }
        if(msg==WND_N_LIST_ITEM_D_CLICK && param==(unsigned int)(KWndWindow*)&leaders){Join();return 1;}
        return KWndImage::WndProc(msg,param,data);
    }
};
KPtTeamSearch* KPtTeamSearch::self=NULL;
void PtTeamSearchOpen(){KPtTeamSearch::Open();}
void PtTeamSearchClose(){KPtTeamSearch::Close();}
void PtTeamSearchAppend(KUiTeamItem* teams,int count){KPtTeamSearch::Append(teams,count);}
