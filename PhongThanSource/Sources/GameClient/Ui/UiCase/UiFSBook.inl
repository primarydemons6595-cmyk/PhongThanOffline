#include "UiFSBook.h"
#include "UiFSBookText.h"
class KPtFSBook : public KWndImage {
    KIniFile scheme;
    KWndImage frame,energyFlag;
    KWndWindow viewport,content;
    KWndButton close,previous,next,first,last;
    KWndButton nodes[64];
    KWndImage lines[256];
    KWndScrollBar scroll;
    KWndText32 title,pageText;
    KWndText256 status;
    int page,profession,nodeCount,lineCount,contentHeight;
    static KPtFSBook* self;
    void Layout(){content.SetPosition(0,-scroll.GetScrollPos());}
    void LoadPage(int selected){
        page=selected<1?1:selected>4?4:selected;
        for(int n=0;n<64;++n)nodes[n].Hide();
        for(n=0;n<256;++n)lines[n].Hide();
        nodeCount=lineCount=0;contentHeight=400;
        const char* names[]={"jiashi","daoshi","yiren"};
        char path[256];sprintf(path,"\\settings\\fsbook\\page_%d\\fsbook_node_%s.txt",page,names[profession]);
        KTabFile table;if(!table.Load(path))return;
        int largestY=0;
        for(int row=3;row<=table.GetHeight();++row){int y=0;table.GetInteger(row,"position_y",0,&y);if(y>largestY)largestY=y;}
        contentHeight=max(400,40+largestY*80+54);content.SetSize(550,contentHeight);
        KTabFile attributes;attributes.Load("\\settings\\fsbook\\fsbook_attributes.txt");
        for(row=3;row<=table.GetHeight()&&nodeCount<64;++row){
            int id=0,type=0,x=0,y=0,cost=0,money=0;char icon[256],name[100],tips[200],tip[256];
            table.GetInteger(row,"id",0,&id);if(id<=0)continue;
            table.GetInteger(row,"type",1,&type);table.GetInteger(row,"position_x",0,&x);table.GetInteger(row,"position_y",0,&y);
            table.GetInteger(row,"illume_consume_energy",0,&cost);table.GetInteger(row,"illume_consume_coin",0,&money);
            table.GetString(row,"name","",name,sizeof(name));table.GetString(row,"tips","",tips,sizeof(tips));
            table.GetString(row,"icon","",icon,sizeof(icon));
            const char* section=type==2?"RareNodeBtn":type==3?"InstallNodeBtn":type==4?"ChannelNodeBtn":"NormalNodeBtn";
            KWndButton& node=nodes[nodeCount++];node.Init(&scheme,section);
            if(icon[0])node.SetImage(ISI_T_SPR,icon,true);
            node.SetPosition(20+x*80,contentHeight-20-y*80-54);node.SetSize(54,54);
            _snprintf(tip,sizeof(tip),PT_FSBOOK_NODE_TIP,name[0]=='#'?name+1:name,cost,money/10000);tip[sizeof(tip)-1]=0;
            node.SetToolTipInfo(tip,strlen(tip));node.Show();
        }
        sprintf(path,"\\settings\\fsbook\\page_%d\\fsbook_line_%s.txt",page,names[profession]);
        KTabFile links;
        if(links.Load(path))for(row=3;row<=links.GetHeight()&&lineCount<256;++row)for(int k=0;k<4&&lineCount<256;++k){
            char key[32];int x1=-1,y1=-1,x2=-1,y2=-1;
            sprintf(key,"fromx_%d",k);links.GetInteger(row,key,-1,&x1);sprintf(key,"fromy_%d",k);links.GetInteger(row,key,-1,&y1);
            sprintf(key,"endx_%d",k);links.GetInteger(row,key,-1,&x2);sprintf(key,"endy_%d",k);links.GetInteger(row,key,-1,&y2);
            if(x1<0||y1<0||x2<0||y2<0||(x1!=x2&&y1!=y2)||(x1==x2&&y1==y2))continue;
            int left=20+min(x1,x2)*80+27,top=contentHeight-20-max(y1,y2)*80-27;
            KWndImage& line=lines[lineCount++];line.Init(&scheme,"LineMain");char image[256];
            scheme.GetString("LineMain",x1==x2?"VerLineDark":"HorLineDark","",image,sizeof(image));
            line.SetImage(ISI_T_SPR,image,true);line.SetPosition(left,top);line.Show();
        }
        char number[32];sprintf(number,"%d / 4",page);pageText.SetText(number);
        scroll.SetValueRange(0,max(0,contentHeight-400));scroll.SetScrollPos(0);Layout();
        previous.Enable(page>1);first.Enable(page>1);next.Enable(page<4);last.Enable(page<4);
    }
    bool Initialize(){
        if(!scheme.Load("\\ui\\ui3\\fsbook.ini"))return false;
        if(!Init(&scheme,"Main"))return false;
        AddChild(&frame);frame.Init(&scheme,"FrameBg");
        AddChild(&energyFlag);energyFlag.Init(&scheme,"FSEnergyFlag");
        AddChild(&viewport);viewport.Init(&scheme,"ViewPortWnd");viewport.AddChild(&content);content.Init(&scheme,"ContentWnd");
        // Lines are behind their original node sprites and clipped by viewport.
        for(int i=0;i<256;++i)content.AddChild(&lines[i]);
        for(i=0;i<64;++i)content.AddChild(&nodes[i]);
        AddChild(&close);close.Init(&scheme,"CloseBtn");
        AddChild(&previous);previous.Init(&scheme,"BtnPrev");AddChild(&next);next.Init(&scheme,"BtnNext");
        AddChild(&first);first.Init(&scheme,"BtnFirst");AddChild(&last);last.Init(&scheme,"BtnLast");
        AddChild(&pageText);pageText.Init(&scheme,"PageText");
        AddChild(&scroll);scroll.Init(&scheme,"Scroll_Bar");scroll.SetPosition(595,120);scroll.SetSize(21,320);
        scheme.WriteInteger("PtTitle","Left",220);scheme.WriteInteger("PtTitle","Top",18);scheme.WriteInteger("PtTitle","Width",220);scheme.WriteInteger("PtTitle","Height",20);scheme.WriteInteger("PtTitle","Font",14);scheme.WriteInteger("PtTitle","HAlign",1);scheme.WriteString("PtTitle","Color","255,225,120");
        AddChild(&title);title.Init(&scheme,"PtTitle");title.SetText(PT_FSBOOK_TITLE);
        scheme.WriteInteger("PtStatus","Left",125);scheme.WriteInteger("PtStatus","Top",43);scheme.WriteInteger("PtStatus","Width",455);scheme.WriteInteger("PtStatus","Height",30);scheme.WriteInteger("PtStatus","Font",14);scheme.WriteInteger("PtStatus","MultiLine",1);scheme.WriteString("PtStatus","Color","218,255,165");
        AddChild(&status);status.Init(&scheme,"PtStatus");status.SetText(PT_FSBOOK_CLOSED);
        int sw,sh;Wnd_GetScreenSize(sw,sh);SetPosition(max(0,(sw-640)/2),max(0,(sh-530)/2));
        Wnd_AddWindow(this);return true;
    }
public:
    KPtFSBook():page(1),profession(0),nodeCount(0),lineCount(0),contentHeight(400){}
    static void Toggle(){
        if(!g_pCoreShell)return;
        if(self&&self->IsVisible()){self->Hide();return;}
        if(!self){self=new KPtFSBook;if(!self->Initialize()){delete self;self=NULL;return;}}
        KUiPlayerBaseInfo info;memset(&info,0,sizeof(info));g_pCoreShell->GetGameData(GDI_PLAYER_BASE_INFO,(unsigned int)&info,0);
        self->profession=info.nProfession>=0&&info.nProfession<3?info.nProfession:0;
        self->LoadPage(1);self->Show();self->BringToTop();
    }
    static void Close(bool destroy){if(self){self->Hide();if(destroy){self->Destroy();self=NULL;}}}
    virtual int WndProc(unsigned int msg,unsigned int param,int data){
        if(msg==WM_KEYDOWN&&param==VK_ESCAPE){Hide();return 1;}
        if(msg==WM_MOUSEWHEEL){scroll.SetScrollPos(scroll.GetScrollPos()-(short)HIWORD(param)/WHEEL_DELTA*40);Layout();return 1;}
        if(msg==WND_N_SCORLLBAR_POS_CHANGED){Layout();return 1;}
        if(msg==WND_N_BUTTON_CLICK){
            if(param==(unsigned int)(KWndWindow*)&close){Hide();return 1;}
            if(param==(unsigned int)(KWndWindow*)&previous){LoadPage(page-1);return 1;}
            if(param==(unsigned int)(KWndWindow*)&next){LoadPage(page+1);return 1;}
            if(param==(unsigned int)(KWndWindow*)&first){LoadPage(1);return 1;}
            if(param==(unsigned int)(KWndWindow*)&last){LoadPage(4);return 1;}
            // No unlock/spend request until the server implements the original
            // prerequisites, persistent nodes, seals and currency transaction.
            for(int i=0;i<nodeCount;++i)if(param==(unsigned int)(KWndWindow*)&nodes[i])return 1;
        }
        return KWndImage::WndProc(msg,param,data);
    }
};
KPtFSBook* KPtFSBook::self=NULL;
void PtFSBookToggle(){KPtFSBook::Toggle();}
void PtFSBookClose(bool destroy){KPtFSBook::Close(destroy);}
