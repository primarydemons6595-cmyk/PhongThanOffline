#include "KIniFile.h"
#include "../Elem/WndList.h"
#include "../Elem/WndEdit.h"
#include "../Elem/WndPureTextBtn.h"
extern iCoreShell* g_pCoreShell;
class KPtNation;
static KPtNation* s_PtNationUi=NULL;
static unsigned int s_PtNationRequest=0;
static void PTNationUiTrace(const char* event,int value){FILE* f=fopen("native_nation_ui.log","a");if(f){fprintf(f,"%s value=%d\n",event,value);fclose(f);}}
static const char* PTNationDutyName(int duty){
    static const char* labels[]={"\247\250ng k\375","L\267nh ch\363a","B\346 Ch\335nh","\247\271i th\307n","D\251n ch\363ng","Kh\270ch m\352i"};
    return duty>=0&&duty<=5?labels[duty]:"";
}
static bool PTNationLoad(KIniFile& ini,const char* name){
    char scheme[256],path[384];g_UiBase.GetCurSchemePath(scheme,sizeof(scheme));
    sprintf(path,"%s\\%s",scheme,name);bool loaded=ini.Load(path)!=0;PTNationUiTrace(name,loaded);return loaded;
}
static void PTNationCommand(int op,unsigned int id,const char* target=NULL,const char* text=NULL,int value=0,int page=0){
    if(!g_pCoreShell)return;PTNationRequest q;memset(&q,0,sizeof(q));
    q.Family=PT_NATION_FAMILY;q.Protocol=1;q.Version=1;q.Operation=(BYTE)op;q.RequestId=++s_PtNationRequest;
    q.NationId=id;q.Value=value;q.Page=page;
    if(target)lstrcpyn(q.Target,target,sizeof(q.Target));if(text)lstrcpyn(q.Text,text,sizeof(q.Text));
    PTNationUiTrace("command",op);PTNationUiTrace("sent",g_pCoreShell->OperationRequest(PT_GOI_NATION,(unsigned int)&q,0));
}
class KPtNationGuidance : public KWndImage {
    KWndText512 text;KWndButton exit,join,ok;
public:
    bool Initialize(){KIniFile ini;if(!PTNationLoad(ini,"UiPtNationNative.ini")||!Init(&ini,"GuidanceMain"))return false;
        text.Init(&ini,"GuidanceText");exit.Init(&ini,"GuidanceExit");join.Init(&ini,"GuidanceJoin");ok.Init(&ini,"GuidanceOk");
        AddChild(&text);AddChild(&exit);AddChild(&join);AddChild(&ok);Wnd_AddWindow(this);Hide();return true;}
    void Open(bool pending,const char* error){
        text.SetText(error&&error[0]?error:(pending?"L\352i xin gia nh\313p l\267nh \256\336a c\361a b\271n ch\255a \256\255\356c th\253ng qua!":"1. Khi \256\271t c\312p 12, b\312m Xin ph\320p \256\323 \256\250ng k\375 nh\313p qu\350c t\365 xa.\n2. Ch\330 khi c\312p \256\351 \256\271t t\355i c\312p 60 th\327 qu\350c gia m\355i c\343 th\323 \256\255\356c th\265nh l\313p.\n3. Sau khi gia nh\313p qu\350c gia th\265nh c\253ng, h\267y s\366 d\364ng ph\335m t\276t F10 \256\323 xem th\253ng tin qu\350c gia hi\326n t\271i."));
        // The original pending notice has only a confirmation button.
        if(pending||(error&&error[0])){join.Hide();ok.Show();}else{join.Show();ok.Hide();}
        int sw,sh;Wnd_GetScreenSize(sw,sh);SetPosition(max(0,(sw-m_Width)/2),max(0,(sh-m_Height)/2));Show();BringToTop();
    }
    virtual int WndProc(unsigned int msg,unsigned int param,int data){
        if(msg==WM_KEYDOWN&&param==VK_ESCAPE){Hide();return 1;}
        if(msg==WND_N_BUTTON_CLICK){if(param==(unsigned int)(KWndWindow*)&exit||param==(unsigned int)(KWndWindow*)&ok){Hide();return 1;}
            if(param==(unsigned int)(KWndWindow*)&join){Hide();PTNationCommand(PT_NATION_LIST,0);return 1;}}
        return KWndImage::WndProc(msg,param,data);
    }
};
class KPtNationInput : public KWndImage {
    KWndText512 description;KWndEdit512 edit;KWndButton confirm,cancel,close;
    int operation;unsigned int nation;bool editing;char target[32];int fixedValue;
public:
    bool Initialize(){
        KIniFile ini;if(!PTNationLoad(ini,"UiPtNationNative.ini")||!Init(&ini,"InputMain"))return false;
        description.Init(&ini,"InputDescription");edit.Init(&ini,"InputEdit");
        confirm.Init(&ini,"InputConfirm");cancel.Init(&ini,"InputCancel");close.Init(&ini,"InputClose");
        AddChild(&description);AddChild(&edit);AddChild(&confirm);AddChild(&cancel);AddChild(&close);
        Wnd_AddWindow(this);Hide();return true;
    }
    void Open(int op,unsigned int id,const char* text,const char* who=NULL){
        operation=op;nation=id;editing=true;fixedValue=0;lstrcpyn(target,who?who:"",sizeof(target));edit.Show();edit.SetText(text?text:"");
        const char* prompt="Nh\313p n\351i dung";
        if(op==PT_NATION_CREATE)prompt="Ki\325n l\313p qu\350c gia: c\312p 60, 500 danh v\344ng v\265 50 v\271n. Nh\313p t\252n qu\350c gia (2-16 k\375 t\371).";
        if(op==PT_NATION_ANNOUNCE)prompt="Nh\313p th\253ng b\270o qu\350c gia";
        if(op==PT_NATION_DEPOSIT)prompt="Nh\313p s\350 l\255\356ng b\271c mu\350n g\343p v\265o ng\251n qu\374";
        if(op==PT_NATION_WITHDRAW)prompt="Nh\313p s\350 l\255\356ng b\271c mu\350n l\312y t\365 ng\251n qu\374";
        if(op==PT_NATION_APPOINT)prompt="Ch\344n ch\370c v\364: 2 = B\346 Ch\335nh, 3 = \247\271i th\307n, 4 = D\251n ch\363ng, 5 = Kh\270ch m\352i.";
        description.SetText(prompt);int sw,sh;Wnd_GetScreenSize(sw,sh);
        SetPosition(max(0,(sw-m_Width)/2),max(0,(sh-m_Height)/2));Show();BringToTop();Wnd_SetExclusive(this);
    }
    void Confirm(int op,unsigned int id,const char* who,const char* text,int value){Open(op,id,"",who);editing=false;fixedValue=value;description.SetText(text);edit.Hide();}
    void Close(){Hide();Wnd_ReleaseExclusive(this);}
    virtual int WndProc(unsigned int msg,unsigned int param,int data){
        if(msg==WM_KEYDOWN&&param==VK_ESCAPE){Close();return 1;}
        if(msg==WND_N_BUTTON_CLICK){
            if(param==(unsigned int)(KWndWindow*)&cancel||param==(unsigned int)(KWndWindow*)&close){Close();return 1;}
            if(param==(unsigned int)(KWndWindow*)&confirm){
                char text[256];text[0]=0;if(editing)edit.GetText(text,sizeof(text),false);int value=fixedValue;
                if(editing&&(operation==PT_NATION_DEPOSIT||operation==PT_NATION_WITHDRAW||operation==PT_NATION_APPOINT)){
                    if(!text[0])return 1;
                    for(const char* p=text;*p;++p){if(*p<'0'||*p>'9'||value>200000000)return 1;value=value*10+*p-'0';}
                    if(value<1||value>2000000000)return 1;
                }
                PTNationCommand(operation,nation,target,text,value);Close();return 1;
            }
        }return KWndImage::WndProc(msg,param,data);
    }
};

// Dedicated original country admission panel (WndTongList).
class KPtNationJoin : public KWndImage {
    KWndImage caption,header;KWndButton close,cancel,join,first,next,previous;
    KWndText32 page;KWndText256 message;KWndList columns[3];
    char cells[3][PT_NATION_ROWS][80];PTNationResponse snapshot;int selected;bool selecting;
public:
    bool Initialize(){
        KIniFile ini;if(!PTNationLoad(ini,"UiPtNationJoin.ini")||!Init(&ini,"WndTongList"))return false;
        caption.Init(&ini,"WndTongList_ImgCaption");header.Init(&ini,"WndTongList_ImgListHeader");AddChild(&caption);AddChild(&header);
        KWndButton* buttons[]={&close,&cancel,&join,&first,&next,&previous};
        const char* names[]={"WndTongList_BtnClose","WndTongList_BtnCancel","WndTongList_BtnJoin","WndTongList_BtnFirstPage","WndTongList_BtnNext","WndTongList_BtnPrev"};
        for(int i=0;i<6;++i){buttons[i]->Init(&ini,names[i]);AddChild(buttons[i]);}
        page.Init(&ini,"WndTongList_TxtPageCode");AddChild(&page);message.Init(&ini,"JoinMessage");AddChild(&message);
        for(i=0;i<3;++i){char key[32];sprintf(key,"JoinColumn%d",i);columns[i].Init(&ini,key);AddChild(&columns[i]);}
        memset(&snapshot,0,sizeof(snapshot));selected=-1;selecting=false;Wnd_AddWindow(this);Hide();return true;
    }
    void Arrive(const PTNationResponse& data){
        snapshot=data;selected=-1;selecting=true;memset(cells,0,sizeof(cells));
        for(int row=0;row<data.Count;++row){lstrcpyn(cells[0][row],data.Rows[row].Name,80);lstrcpyn(cells[1][row],data.Rows[row].Leader,80);sprintf(cells[2][row],"%u",data.Rows[row].Level);}
        for(int c=0;c<3;++c){columns[c].SetContent((BYTE*)cells[c],data.Count,80,0);columns[c].SetCurSel(-1);}selecting=false;
        char text[32];sprintf(text,"%d",data.Page+1);page.SetText(text);message.SetText(data.Message);
        previous.Enable(data.Page>0);first.Enable(data.Page>0);next.Enable((data.Page+1)*PT_NATION_ROWS<data.Total);join.Enable(false);
        int sw,sh;Wnd_GetScreenSize(sw,sh);SetPosition(max(0,(sw-m_Width)/2),max(0,(sh-m_Height)/2));Show();BringToTop();
    }
    virtual int WndProc(unsigned int msg,unsigned int param,int data){
        if(msg==WM_KEYDOWN&&param==VK_ESCAPE){Hide();return 1;}
        if(msg==WND_N_LIST_ITEM_SEL&&!selecting){for(int c=0;c<3;++c)if(param==(unsigned int)(KWndWindow*)&columns[c]){
            selected=data;if(selected<0||selected>=snapshot.Count)selected=-1;
            selecting=true;for(int j=0;j<3;++j)if(j!=c)columns[j].SetCurSel(selected);selecting=false;
            join.Enable(selected>=0&&!snapshot.SelfNationId);return 1;}}
        if(msg==WND_N_BUTTON_CLICK){
            if(param==(unsigned int)(KWndWindow*)&close||param==(unsigned int)(KWndWindow*)&cancel){Hide();return 1;}
            if(param==(unsigned int)(KWndWindow*)&first){PTNationCommand(PT_NATION_LIST,0);return 1;}
            if(param==(unsigned int)(KWndWindow*)&next){PTNationCommand(PT_NATION_LIST,0,NULL,NULL,0,snapshot.Page+1);return 1;}
            if(param==(unsigned int)(KWndWindow*)&previous){if(snapshot.Page>0)PTNationCommand(PT_NATION_LIST,0,NULL,NULL,0,snapshot.Page-1);return 1;}
            if(param==(unsigned int)(KWndWindow*)&join&&selected>=0&&selected<snapshot.Count&&!snapshot.SelfNationId){PTNationCommand(PT_NATION_APPLY,snapshot.Rows[selected].Id);join.Enable(false);return 1;}
        }return KWndImage::WndProc(msg,param,data);
    }
};

// Native page containers must relay child notifications to their owner.
class KPtNationPage : public KWndImage {
public:
    virtual int WndProc(unsigned int msg,unsigned int param,int data){
        if((msg==WND_N_BUTTON_CLICK||msg==WND_N_LIST_ITEM_SEL||msg==WND_N_LIST_ITEM_R_CLICK||msg==WND_N_SCORLLBAR_POS_CHANGED)&&GetParent())
            return GetParent()->WndProc(msg,param,data);
        return KWndImage::WndProc(msg,param,data);
    }
};
class KPtNation : public KWndImage {
    KPtNationPage members,functions,infoPanel,actionsPanel;
    KWndText256 name,duty,leader,count,money,page,message;
    KWndText512 board,guide;
    KWndButton close,tabs[4],countryTab,actionsTab;
    KWndPureTextBtn actions[12],previous,next,refresh,registration,listCountries;
    KWndList columns[4];char cells[4][PT_NATION_ROWS][80];
    PTNationResponse snapshot;KPtNationInput input;KPtNationGuidance guidance;KPtNationJoin joinPanel;
    int selected,topTab,filter,confirmOp,confirmValue;bool listMode,selecting;
    char confirmTarget[32];unsigned int confirmNation;
    bool Right(int right){return !listMode&&snapshot.Duty>0&&(snapshot.Rights&(1u<<(right-1)));}
    void Query(int pg=0){PTNationCommand(listMode?PT_NATION_LIST:PT_NATION_GET,0,NULL,NULL,filter,pg);}
    void Confirm(int op,const char* text,const char* who=NULL,int value=0){
        unsigned int id=listMode&&selected>=0?snapshot.Rows[selected].Id:snapshot.SelfNationId;
        input.Confirm(op,id,who,text,value);
    }
    void UpdateButtons(){
        bool row=selected>=0&&selected<snapshot.Count;int other=row?snapshot.Rows[selected].Duty:0;
        actions[0].SetText(snapshot.Duty?"R\352i qu\350c gia":"H\361y \256\250ng k\375");
        actions[0].Enable(snapshot.SelfNationId&&snapshot.Duty!=1&&!listMode);
        actions[1].Enable(Right(5)&&row&&!other);actions[2].Enable(Right(5)&&row&&!other);
        actions[3].Enable(Right(6)&&row&&other>snapshot.Duty);
        actions[4].Enable(Right(4)&&row&&other>snapshot.Duty);
        actions[5].Enable(snapshot.Duty==1&&row&&other>1&&!listMode);
        actions[6].Enable(Right(3));actions[7].Enable(Right(7));actions[8].Enable(Right(8));
        actions[9].Enable(Right(1));actions[10].Enable(!snapshot.SelfNationId);
        actions[11].Enable(listMode&&row&&!snapshot.SelfNationId);
        registration.Enable(Right(5));
    }
    void Display(){
        name.SetText(listMode?"Danh s\270ch qu\350c gia":snapshot.Name);duty.SetText(PTNationDutyName(snapshot.Duty));
        leader.SetText(snapshot.Leader);char text[80];sprintf(text,"%d",snapshot.MemberCount);count.SetText(text);
        sprintf(text,"%d l\255\356ng",snapshot.Money);money.SetText(text);
        sprintf(text,"%d / %d",snapshot.Page+1,max(1,(snapshot.Total+PT_NATION_ROWS-1)/PT_NATION_ROWS));page.SetText(text);
        message.SetText(snapshot.Message);board.SetText(snapshot.Notice);selected=-1;selecting=true;
        memset(cells,0,sizeof(cells));
        for(int i=0;i<snapshot.Count;++i){const PTNationRow& row=snapshot.Rows[i];
            lstrcpyn(cells[0][i],row.Name,80);lstrcpyn(cells[1][i],listMode?row.Leader:PTNationDutyName(row.Duty),80);
            sprintf(cells[2][i],"%d",row.Level);lstrcpyn(cells[3][i],listMode?"":(row.Online?"Tr\371c tuy\325n":"Ngo\271i tuy\325n"),80);
        }
        bool showGuide=!snapshot.SelfNationId&&!listMode;
        guide.SetText("1. Khi \256\271t c\312p 12, b\312m Xin ph\320p \256\323 \256\250ng k\375 nh\313p qu\350c t\365 xa.\n2. Ch\330 khi c\312p \256\351 \256\271t t\355i c\312p 60 th\327 qu\350c gia m\355i c\343 th\323 \256\255\356c th\265nh l\313p.\n3. Sau khi gia nh\313p qu\350c gia th\265nh c\253ng, h\267y s\366 d\364ng ph\335m t\276t F10 \256\323 xem th\253ng tin qu\350c gia hi\326n t\271i.");
        if(showGuide)guide.Show();else guide.Hide();
        if(!showGuide&&!listMode&&topTab==0)board.Show();else board.Hide();
        for(int c=0;c<4;++c){
            columns[c].SetContent((BYTE*)cells[c],snapshot.Count,80,0);columns[c].SetCurSel(-1);
            if(!showGuide&&(listMode||topTab!=0))columns[c].Show();else columns[c].Hide();
        }
        selecting=false;previous.Enable(snapshot.Page>0);next.Enable((snapshot.Page+1)*PT_NATION_ROWS<snapshot.Total);
        UpdateButtons();
    }
    void Action(int index){
        const PTNationRow* row=selected>=0&&selected<snapshot.Count?&snapshot.Rows[selected]:NULL;
        if(index==0)Confirm(snapshot.Duty?PT_NATION_LEAVE:PT_NATION_CANCEL,"B\271n mu\350n r\352i qu\350c gia ho\306c h\361y \256\254n \256\250ng k\375?");
        if(index==1&&row)Confirm(PT_NATION_ACCEPT,"\247\345ng \375 cho ng\255\352i n\265y gia nh\313p qu\350c gia?",row->Name);
        if(index==2&&row)Confirm(PT_NATION_REJECT,"T\365 ch\350i \256\254n gia nh\313p n\265y?",row->Name);
        if(index==3&&row)Confirm(PT_NATION_KICK,"Khai tr\365 ng\255\352i n\265y kh\341i qu\350c gia?",row->Name);
        if(index==4&&row)input.Open(PT_NATION_APPOINT,snapshot.SelfNationId,"",row->Name);
        if(index==5&&row)Confirm(PT_NATION_TRANSFER,"Nh\255\352ng quy\322n L\267nh ch\363a cho ng\255\352i n\265y?",row->Name);
        if(index==6)input.Open(PT_NATION_ANNOUNCE,snapshot.SelfNationId,snapshot.Notice);
        if(index==7)input.Open(PT_NATION_DEPOSIT,snapshot.SelfNationId,"");
        if(index==8)input.Open(PT_NATION_WITHDRAW,snapshot.SelfNationId,"");
        if(index==9)Confirm(PT_NATION_DISSOLVE,"Gi\266i t\270n qu\350c gia? T\312t c\266 th\265nh vi\252n s\317 r\352i qu\350c gia. Ng\251n qu\374 c\337n l\271i kh\253ng th\323 nh\313n l\271i.");
        if(index==10)input.Open(PT_NATION_CREATE,0,"");
        if(index==11&&row)Confirm(PT_NATION_APPLY,"G\366i \256\254n xin gia nh\313p qu\350c gia \256\267 ch\344n?");
    }
public:
    bool Initialize(){
        memset(&snapshot,0,sizeof(snapshot));selected=-1;topTab=0;filter=0;listMode=false;selecting=false;
        KIniFile ini;if(!PTNationLoad(ini,"UiPtNationMain.ini")||!Init(&ini,"Main"))return false;
        close.Init(&ini,"BtnClose");name.Init(&ini,"TxtTongName");duty.Init(&ini,"TxtSelfDuty");
        AddChild(&close);AddChild(&name);AddChild(&duty);
        if(!PTNationLoad(ini,"UiPtNationMembers.ini"))return false;
        members.Init(&ini,"WndMembers");AddChild(&members);
        const char* sections[]={"PageBoard_Btn","PageLeader_Btn","PageSages_Btn","PageMembers_Btn"};
        for(int i=0;i<4;++i){tabs[i].Init(&ini,sections[i]);tabs[i].SetPosition(i*50,0);members.AddChild(&tabs[i]);}
        if(!PTNationLoad(ini,"UiPtNationFunctions.ini"))return false;
        functions.Init(&ini,"WndFunctions");AddChild(&functions);
        infoPanel.Init(&ini,"PageTongInfo");functions.AddChild(&infoPanel);
        leader.Init(&ini,"PageTongInfo_TxtLeader");count.Init(&ini,"PageTongInfo_TxtMembersNum");money.Init(&ini,"PageTongInfo_TxtTongMoney");
        infoPanel.AddChild(&leader);infoPanel.AddChild(&count);infoPanel.AddChild(&money);
        countryTab.Init(&ini,"PageTongInfo_Btn");actionsTab.Init(&ini,"PageTransaction_Btn");
        countryTab.SetPosition(0,0);actionsTab.SetPosition(102,0);
        functions.AddChild(&countryTab);functions.AddChild(&actionsTab);
        actionsPanel.Init(&ini,"PageTransaction");functions.AddChild(&actionsPanel);
        if(!PTNationLoad(ini,"UiPtNationNative.ini"))return false;
        board.Init(&ini,"Board");guide.Init(&ini,"Guide");members.AddChild(&board);members.AddChild(&guide);
        for(int col=0;col<4;++col){char key[32];sprintf(key,"Column%d",col);columns[col].Init(&ini,key);members.AddChild(&columns[col]);}
        const char* captions[]={"R\352i qu\350c gia","Ch\312p thu\313n","T\365 ch\350i","Khai tr\365","B\346 nhi\326m","Nh\255\352ng quy\322n","Th\253ng b\270o","G\343p ng\251n qu\374","R\363t ng\251n qu\374","Gi\266i t\270n","Ki\325n l\313p","Xin gia nh\313p"};
        for(int a=0;a<12;++a){char key[32];sprintf(key,"Action%d",a);actions[a].Init(&ini,key);actions[a].SetText(captions[a]);actionsPanel.AddChild(&actions[a]);}
        page.Init(&ini,"Page");message.Init(&ini,"Message");AddChild(&page);AddChild(&message);
        previous.Init(&ini,"Previous");next.Init(&ini,"Next");refresh.Init(&ini,"Refresh");registration.Init(&ini,"Registration");listCountries.Init(&ini,"Countries");
        AddChild(&previous);AddChild(&next);AddChild(&refresh);AddChild(&registration);AddChild(&listCountries);
        if(!input.Initialize()||!guidance.Initialize()||!joinPanel.Initialize())return false;actionsPanel.Hide();Wnd_AddWindow(this);Hide();return true;
    }
    void Open(){
        int sw,sh;Wnd_GetScreenSize(sw,sh);SetPosition(max(0,(sw-m_Width)/2),max(0,(sh-m_Height)/2));
        if(IsVisible()||guidance.IsVisible()||joinPanel.IsVisible()){Close();return;}
        // Always ask for current membership; listMode belongs only to a list view.
        listMode=false;topTab=0;filter=0;infoPanel.Show();actionsPanel.Hide();Query();
    }
    void Close(){input.Close();guidance.Hide();joinPanel.Hide();Hide();}
    void Arrive(const PTNationResponse& data){
        snapshot=data;listMode=data.Operation==PT_NATION_LIST;
        if(listMode){if(data.SelfNationId&&data.Duty>0){listMode=false;topTab=0;filter=0;joinPanel.Hide();Query();return;}Hide();guidance.Hide();joinPanel.Arrive(data);return;}
        joinPanel.Hide();
        if(data.Operation==PT_NATION_CREATE&&data.Result>0&&!data.SelfNationId){Hide();guidance.Hide();input.Open(PT_NATION_CREATE,0,"");return;}
        if(!listMode&&!data.Duty){Hide();guidance.Open(data.SelfNationId!=0,data.Result?NULL:data.Message);return;}
        guidance.Hide();Show();BringToTop();Display();
    }
    virtual int WndProc(unsigned int msg,unsigned int param,int data){
        if(msg==WM_KEYDOWN&&param==VK_ESCAPE){Close();return 1;}
        if(msg==WND_N_LIST_ITEM_R_CLICK&&!listMode&&param==(unsigned int)(KWndWindow*)&columns[0]){
            if(data>=0&&data<snapshot.Count){selected=data;selecting=true;for(int col=0;col<4;++col)columns[col].SetCurSel(selected);selecting=false;UpdateButtons();int x,y;Wnd_GetCursorPos(&x,&y);PhongThanOpenRemotePeopleMenu(snapshot.Rows[data].Name,x,y);}return 1;
        }
        if(msg==WND_N_LIST_ITEM_SEL&&!selecting){for(int c=0;c<4;++c)if(param==(unsigned int)(KWndWindow*)&columns[c]){
            selected=data;if(selected<0||selected>=snapshot.Count)selected=-1;
            selecting=true;for(int j=0;j<4;++j)if(j!=c)columns[j].SetCurSel(selected);selecting=false;UpdateButtons();return 1;
        }}
        if(msg==WND_N_BUTTON_CLICK){
            if(param==(unsigned int)(KWndWindow*)&close){Close();return 1;}
            if(param==(unsigned int)(KWndWindow*)&refresh){Query(snapshot.Page);return 1;}
            if(param==(unsigned int)(KWndWindow*)&previous){if(snapshot.Page>0)Query(snapshot.Page-1);return 1;}
            if(param==(unsigned int)(KWndWindow*)&next){if((snapshot.Page+1)*PT_NATION_ROWS<snapshot.Total)Query(snapshot.Page+1);return 1;}
            if(param==(unsigned int)(KWndWindow*)&registration){listMode=false;topTab=4;filter=-1;Query();return 1;}
            if(param==(unsigned int)(KWndWindow*)&listCountries){listMode=true;filter=0;Query();return 1;}
            if(param==(unsigned int)(KWndWindow*)&countryTab){infoPanel.Show();actionsPanel.Hide();return 1;}
            if(param==(unsigned int)(KWndWindow*)&actionsTab){infoPanel.Hide();actionsPanel.Show();return 1;}
            for(int t=0;t<4;++t)if(param==(unsigned int)(KWndWindow*)&tabs[t]){listMode=false;topTab=t;filter=t==1?1:(t==2?2:(t==3?4:0));Query();return 1;}
            for(int a=0;a<12;++a)if(param==(unsigned int)(KWndWindow*)&actions[a]){if(!actions[a].IsDisable())Action(a);return 1;}
        }return KWndImage::WndProc(msg,param,data);
    }
};
void PhongThanNationOpenGuide(){
    PTNationUiTrace("open_guide",1);
    if(!g_pCoreShell)return;
    if(!s_PtNationUi){s_PtNationUi=new KPtNation;if(!s_PtNationUi->Initialize()){delete s_PtNationUi;s_PtNationUi=NULL;return;}}
    s_PtNationUi->Open();
}
void PhongThanNationClose(){if(s_PtNationUi)s_PtNationUi->Close();}
void PhongThanNationArrive(const PTNationResponse* response,unsigned int size){
    PTNationUiTrace("response_valid",PTNationResponseValid(response,size));if(!PTNationResponseValid(response,size))return;
    if(!s_PtNationUi){s_PtNationUi=new KPtNation;if(!s_PtNationUi->Initialize()){delete s_PtNationUi;s_PtNationUi=NULL;return;}}
    s_PtNationUi->Arrive(*response);
}

