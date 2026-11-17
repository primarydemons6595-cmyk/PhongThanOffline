#include "../../../../Headers/PhongThanPlayerTarget.h"
#include "UiPortraitMask.h"
class KPtPlayerTarget : public KWndImage {
    KUiPortraitMask m_Head;
    KWndText80 m_Name;
    KWndText32 m_Level,m_Health;
    KWndImagePart m_Life;
    KIniFile m_Original;
    PT_PLAYER_TARGET m_Target;
public:
    bool Initialize() {
        char scheme[256],path[300],image[256];g_UiBase.GetCurSchemePath(scheme,sizeof(scheme));
        sprintf(path,"%s\\UiPlayerTarget.ini",scheme);if(!m_Original.Load(path))return false;
        if(!Init(&m_Original,"Main"))return false;
        m_Original.GetString("Main","MaskPath","",image,sizeof(image));
        if(image[0])m_Original.WriteString("ImgHead","MaskPath",image);
        m_Head.Init(&m_Original,"ImgHead");AddChild(&m_Head);
        m_Name.Init(&m_Original,"txtName");AddChild(&m_Name);
        m_Level.Init(&m_Original,"txtLevel");AddChild(&m_Level);
        int x=45,y=22;m_Original.GetInteger("Main","LifeLeft",45,&x);m_Original.GetInteger("Main","LifeTop",22,&y);
        m_Original.GetString("Main","LifeImage","",image,sizeof(image));
        m_Original.WriteString("Life","Image",image);m_Original.WriteInteger("Life","Left",x);m_Original.WriteInteger("Life","Top",y);
        m_Life.Init(&m_Original,"Life");AddChild(&m_Life);
        m_Original.WriteInteger("Health","Left",x);m_Original.WriteInteger("Health","Top",y);
        m_Original.WriteInteger("Health","Width",228);m_Original.WriteInteger("Health","Height",16);
        m_Original.WriteInteger("Health","Font",14);m_Original.WriteInteger("Health","HAlign",1);
        m_Original.WriteInteger("Health","DummyWnd",1);m_Original.WriteString("Health","Color","248,252,0");
        m_Health.Init(&m_Original,"Health");AddChild(&m_Health);
        ZeroMemory(&m_Target,sizeof(m_Target));Wnd_AddWindow(this,WL_NORMAL);Hide();return true;
    }
    void Update() {
        ZeroMemory(&m_Target,sizeof(m_Target));
        if(!g_pCoreShell || !g_pCoreShell->GetGameData(PT_PLAYER_TARGET_GET,(unsigned int)&m_Target,sizeof(m_Target)) || !m_Target.Id){Hide();return;}
        m_Name.SetText(m_Target.Name);m_Level.SetIntText(m_Target.Level);
        m_Life.SetPart(m_Target.Life,m_Target.MaxLife>0?m_Target.MaxLife:1);
        char value[64];sprintf(value,"%d/%d",m_Target.Life,m_Target.MaxLife);m_Health.SetText(value);
        int face=(m_Target.Head>0?(m_Target.Head-1)%3:0)*2+(m_Target.Sex==0?1:0);
        char key[32],path[256];sprintf(key,"HeadImg_%d",face);m_Original.GetString("Main",key,"",path,sizeof(path));
        if(path[0] && strcmp(path,m_Head.GetImage()))m_Head.SetImage(ISI_T_SPR,path,false);
        Show();
    }
    virtual int WndProc(unsigned int msg,unsigned int param,int value) {
        if(msg==WM_RBUTTONDOWN && m_Target.Id && g_pCoreShell) {
            KUiPlayerItem current;ZeroMemory(&current,sizeof(current));int kind=-1;
            if(g_pCoreShell->FindSpecialNPC(m_Target.Name,&current,kind) && kind==kind_player && current.uId==m_Target.Id)
                PopUpContextPeopleMenu(current,(short)LOWORD(value),(short)HIWORD(value));
            return 1;
        }
        return KWndImage::WndProc(msg,param,value);
    }
};
static KPtPlayerTarget* s_PtPlayerTarget=0;
static void PhongThanUpdatePlayerTarget() {
    if(!s_PtPlayerTarget){s_PtPlayerTarget=new KPtPlayerTarget;if(!s_PtPlayerTarget->Initialize()){delete s_PtPlayerTarget;s_PtPlayerTarget=0;return;}}
    s_PtPlayerTarget->Update();
}

void PhongThanClosePlayerTarget(){
    if(s_PtPlayerTarget){s_PtPlayerTarget->Destroy();s_PtPlayerTarget=0;}
    if(g_pCoreShell)g_pCoreShell->OperationRequest(PT_PLAYER_TARGET_SELECT,0,0);
}
