#include "../../../Headers/PhongThanTeamProtocol.h"
#include "../Elem/WndImagePart.h"
#include "../../../../Headers/PhongThanOverheadDraw.h"
class PTTeamHud : public KWndImage {
    KWndImage rows[8],borders[8];KWndImagePart life[8];KWndText32 names[8],teamName;
    int levels[8], professions[8], observerLevel, memberCount;
public:
    PTTeamHud() : observerLevel(0), memberCount(0) { memset(levels,0,sizeof(levels)); }
    virtual void PaintWindow(){
        KWndImage::PaintWindow();
        if(g_pRepresentShell) for(int i=0;i<memberCount;++i)
            PhongThanDrawTeamBadge(g_pRepresentShell,m_nAbsoluteLeft,
                m_nAbsoluteTop+20+i*30+12,levels[i],observerLevel,professions[i]);
    }
    void Initialize(){
        char path[320];g_UiBase.GetCurSchemePath(path,sizeof(path));strcat(path,"\\UiTeamHud.ini");KIniFile ini;if(!ini.Load(path))return;
        Init(&ini,"Main");AddChild(&teamName);teamName.Init(&ini,"TeamName");
        for(int i=0;i<8;++i){AddChild(&rows[i]);rows[i].Init(&ini,"Member");rows[i].SetPosition(0,20+i*30);
            rows[i].AddChild(&borders[i]);borders[i].Init(&ini,"Border");rows[i].AddChild(&life[i]);life[i].Init(&ini,"Life");
            rows[i].AddChild(&names[i]);names[i].Init(&ini,"Name");rows[i].Hide();}
        Wnd_AddWindow(this,WL_NORMAL);Hide();
    }
    void Update(){
        PT_TEAM_STATE state;memset(&state,0,sizeof(state));int count=g_pCoreShell->TeamOperation(PT_TEAM_UI_GET,(unsigned int)&state,0);
        if(count<=1||(g_pCoreShell->TeamOperation(TEAM_OI_PT_OPTIONS,0,0)&8)){Hide();return;}
        int sw,sh;Wnd_GetScreenSize(sw,sh);SetPosition(max(0,sw-154),170);teamName.SetText(state.Name);
        observerLevel=0;
        for(int actor=0;actor<count&&actor<8;++actor) if(state.Members[actor].Id==state.ActorId) observerLevel=state.Members[actor].Level;
        int row=0;for(int i=0;i<count&&i<8;++i){const PT_TEAM_MEMBER& m=state.Members[i];if(m.Id==state.ActorId)continue;
            names[row].SetText(m.Name);names[row].SetTextColor(m.Id==state.CaptainId?0xffeff74a:0xff00ff00);
            levels[row]=m.Level;professions[row]=m.Profession;
            life[row].SetPart(min(max(0,m.Life),m.MaxLife),m.MaxLife);rows[row].Show();++row;
        }memberCount=row;for(;row<8;++row)rows[row].Hide();Show();
    }
};
static PTTeamHud* s_PtTeamHud=0;
static void PtTeamHudUpdate(){if(!s_PtTeamHud){s_PtTeamHud=new PTTeamHud;s_PtTeamHud->Initialize();}s_PtTeamHud->Update();}
static void PtTeamHudClose(){if(s_PtTeamHud){s_PtTeamHud->Destroy();s_PtTeamHud=0;}}
