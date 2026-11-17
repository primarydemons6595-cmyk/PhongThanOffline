#include "PhongThanCompose.h"
#include "PhongThanNativeUiLayout.h"
// Original VNG nine-slot compose layout. Kept separate from purple-item
// forging: that older path consumes different item schemas and must not run.
class KUiVngCompose : public KWndShowAnimate
{
    KWndButton m_Close, m_Combine, m_Stop, m_All, m_Add, m_Sub;
    KWndButton m_AuxUse[3],m_AuxAdd[3],m_AuxSub[3];
    KWndText32 m_RepeatText,m_AuxCount[3];
    KWndImage m_CountBack,m_MoneyBack,m_StaticBack;
    KWndWindow m_AuxPanel[3];
    KWndObjectBox m_AuxEmpty[3];
    KWndImage m_AuxDefault[3];
    KIniFile m_SupportInfo;
    int m_Repeat,m_Remaining,m_AuxSlot[3],m_AuxQuantity[3];
    PHONGTHAN_COMPOSE_PREVIEW m_Quote;
    bool m_Cancel,m_SocketMode;
    void SetBusy(bool busy) {
        if(busy){m_Combine.Hide();m_Stop.Show();}else{m_Stop.Hide();m_Combine.Show();}
        m_All.Enable(!busy);m_Add.Enable(!busy);m_Sub.Enable(!busy);
        for(int a=0;a<3;++a){m_AuxUse[a].Enable(!busy);m_AuxAdd[a].Enable(!busy);m_AuxSub[a].Enable(!busy);}
    }
    void UpdateRepeat() {char text[16];sprintf(text,"%d",m_Repeat);m_RepeatText.SetText(text);}
    void StartOne() {
        ZeroMemory(&m_PendingRequest,sizeof(m_PendingRequest));
        for(int i=0;i<9;++i){KUiDraggedObject item;ZeroMemory(&item,sizeof(item));m_Slots[i].GetObject(item);m_PendingRequest.Items[i]=item.uId;}
        for(int a=0;a<3;++a)if(m_AuxSlot[a]>=0)m_PendingRequest.Quantities[m_AuxSlot[a]]=m_AuxUse[a].IsButtonChecked()?m_AuxQuantity[a]:-1;
        ZeroMemory(&m_Quote,sizeof(m_Quote));
        memcpy(m_Quote.Quantities,m_PendingRequest.Quantities,sizeof(m_Quote.Quantities));
        if(!g_pCoreShell->GetGameData(PHONGTHAN_COMPOSE_CORE_PREVIEW,(unsigned int)&m_Quote,sizeof(m_Quote)) || !m_Quote.Count || m_Quote.Status!=PT_COMPOSE_OK) {
            m_Info.SetText("Nguyen lieu chua dung cong thuc. Chua tru vat pham.");m_Remaining=0;return;
        }
        memcpy(m_PendingRequest.Quantities,m_Quote.Quantities,sizeof(m_PendingRequest.Quantities));
        int socketMode=g_pCoreShell->GetGameData(PHONGTHAN_COMPOSE_CORE_MODE,0,0);m_PendingRequest.SocketPosition=socketMode>=3&&socketMode<=6?socketMode-2:0;
        m_RequestSent=false;m_Waiting=true;m_RequestStarted=GetTickCount();SetBusy(true);BeginAnimation();
    }
    KWndObjectBox m_Slots[9];
    KWndText256 m_Info;
    KWndText80 m_Cost;
    KWndImage m_Effects[3],m_Ghosts[9];
    int m_AnimationStage,m_PhaseLoops,m_WaitLoops,m_ResultCode;
    int m_SlotX[9],m_SlotY[9];
    DWORD m_RequestStarted;
    DWORD m_LastInputs;
    bool m_KeepResult;
    bool m_RequestSent;
    PHONGTHAN_COMPOSE_COMMAND m_PendingRequest;
    void StopAnimation() {
        m_AnimationStage=0;
        for(int i=0;i<3;++i)m_Effects[i].Hide();
        for(int j=0;j<9;++j){m_Ghosts[j].Hide();m_Slots[j].SetPosition(m_SlotX[j],m_SlotY[j]);m_Slots[j].Show();}
    }
    void Phase(int phase) {
        for(int i=0;i<3;++i)m_Effects[i].Hide();
        m_AnimationStage=phase;
        if(phase>0 && phase<=3){m_Effects[phase-1].SetFrame(0);m_Effects[phase-1].UpdateTimer();m_Effects[phase-1].Show();}
    }
    void BeginAnimation() {
        m_ResultCode=-1;m_PhaseLoops=0;
        // Animate the original forge effect; input icons stay in their slots.
        Phase(1);
    }
    bool m_Waiting;
    static KUiVngCompose* s_Window;
public:
    static bool PutInventoryItem(const KUiDraggedObject* item, int container)
    {
        if(!s_Window || !s_Window->IsVisible() || !item || !g_pCoreShell)
            return false;
        if(item->uGenre!=CGOG_ITEM || !item->uId)
            return true;
        if(container!=UOC_ITEM_TAKE_WITH && container!=UOC_ITEM_TAKE_WITH_EX)
            return true;
        if(s_Window->m_Waiting)
        {
            s_Window->m_Info.SetText("§ang xö lý. H·y chê kÕt qu¶ tr­íc khi ®Æt thªm vËt phÈm.");
            return true;
        }

        KUiObjAtRegion current[9];ZeroMemory(current,sizeof(current));
        g_pCoreShell->GetGameData(GDI_BUILD_ITEM,(unsigned int)current,0);
        int freeSlot=-1;
        for(int i=0;i<9;++i)
            if(current[i].Obj.uGenre==CGOG_NOTHING || !current[i].Obj.uId)
            {
                freeSlot=i;
                break;
            }
        if(freeSlot<0)
        {
            s_Window->m_Info.SetText("ChÝn « ®Æt vËt phÈm ®· ®Çy.");
            return true;
        }

        KUiObjAtContRegion from,to;ZeroMemory(&from,sizeof(from));ZeroMemory(&to,sizeof(to));
        from.Obj.uGenre=item->uGenre;from.Obj.uId=item->uId;
        from.Region.h=item->DataX;from.Region.v=item->DataY;
        from.Region.Width=item->DataW;from.Region.Height=item->DataH;
        from.eContainer=(UIOBJECT_CONTAINER)container;
        to.Obj.uGenre=item->uGenre;to.Obj.uId=item->uId;
        to.Region.h=0;to.Region.v=freeSlot;
        to.Region.Width=item->DataW;to.Region.Height=item->DataH;
        to.eContainer=UOC_BUILD_ITEM;
        if(!g_pCoreShell->OperationRequest(GOI_SWITCH_OBJECT,(unsigned int)&from,(int)&to))
            s_Window->m_Info.SetText("Kh«ng thÓ ®­a vËt phÈm vµo « th¨ng. KiÓm tra khãa vËt phÈm.");
        return true;
    }
    static void Open()
    {
        if (!s_Window)
        {
            s_Window=new KUiVngCompose;
            if (!s_Window->Initialize()) { delete s_Window; s_Window=0; return; }
        }
        if(!KUiItem::GetIfVisible()) KUiItem::OpenWindow();
        if(s_Window->m_Waiting){s_Window->BringToTop();return;}
        s_Window->StopAnimation();
        int mode=g_pCoreShell->GetGameData(PHONGTHAN_COMPOSE_CORE_MODE,0,0);s_Window->m_SocketMode=mode>=2&&mode<=6;
        s_Window->m_Waiting=false;
        s_Window->m_LastInputs=0xFFFFFFFF;s_Window->m_KeepResult=false;
        s_Window->m_Repeat=1;s_Window->m_Remaining=0;s_Window->m_Cancel=false;
        if(mode==8||mode==9){s_Window->m_Info.SetText(mode==8?"\x48\x6f\x3f\x6e\x20\x48\x3f\x6e\x3a\x20\x3f\x3f\x74\x20\x74\x72\x61\x6e\x67\x20\x62\x3f\x2c\x20\x48\x3f\x6e\x20\x43\x68\x3f\x20\x76\x3f\x20\x6e\x67\x75\x79\x3f\x6e\x20\x6c\x69\x3f\x75\x2e\x20\x4b\x69\x3f\x6d\x20\x74\x72\x61\x20\x70\x68\x3f\x20\x74\x72\x3f\x3f\x63\x20\x6b\x68\x69\x20\x78\x3f\x63\x20\x6e\x68\x3f\x6e\x3b\x20\x48\x3f\x6e\x20\x43\x68\x3f\x20\x6d\x3f\x69\x20\x73\x3f\x20\x74\x68\x61\x79\x20\x48\x3f\x6e\x20\x43\x68\x3f\x20\x63\x3f\x2e":"\x4e\x3f\x70\x20\x6c\x69\x6e\x68\x20\x6b\x68\x3f\x3a\x20\x3f\x3f\x74\x20\x74\x72\x61\x6e\x67\x20\x62\x3f\x20\x63\x3f\x6e\x67\x20\x54\x68\x69\x3f\x6e\x20\x4c\x69\x6e\x68\x20\x54\x68\x3f\x63\x68\x20\x76\x3f\x61\x20\x64\x3f\x6e\x67\x2e\x20\x4d\x3f\x69\x20\x76\x69\x3f\x6e\x20\x70\x68\x3f\x63\x20\x68\x3f\x69\x20\x32\x30\x30\x20\x3f\x69\x3f\x6d\x3b\x20\x3f\x3f\x79\x20\x6c\x69\x6e\x68\x20\x6b\x68\x3f\x20\x73\x3f\x20\x6b\x68\x3f\x6e\x67\x20\x74\x72\x3f\x20\x3f\x3f\x2e");s_Window->m_All.Hide();s_Window->m_Add.Hide();s_Window->m_Sub.Hide();}else{s_Window->m_All.Show();s_Window->m_Add.Show();s_Window->m_Sub.Show();}
        s_Window->UpdateRepeat();s_Window->SetBusy(false);
        s_Window->BringToTop();
        s_Window->Show();
    }
    static void Close()
    {
        if(s_Window)
        {
            if(s_Window->m_Waiting)return;
            s_Window->StopAnimation();
            if(g_pCoreShell)g_pCoreShell->OperationRequest(GOI_RECOVERY_BOX_COMMAND,pos_builditem,0);
            s_Window->m_Waiting=false;
            s_Window->Hide();
        }
    }
    static void Result(int code)
    {
        if(!s_Window || !s_Window->m_Waiting)return;
        const bool committed=code==PT_COMPOSE_OK || code==PT_COMPOSE_FABAO_FAILED || (code>=PT_COMPOSE_UPGRADE_OK && code<=PT_COMPOSE_FAILED_RESET);
        if(committed && s_Window->m_AnimationStage>0) { s_Window->m_ResultCode=code;return; }
        s_Window->StopAnimation();
        s_Window->m_KeepResult=true;
        const char* messages[]={"Thµnh c«ng. §· nhËn ®å vµo F4.","C«ng thøc nµy ch­a ®­îc më.","H·y ®øng gÇn NPC thùc hiÖn chøc n¨ng nµy.","Nguyªn liÖu kh«ng hîp lÖ.","Can cho trong F4.","VËt phÈm/tµi kho¶n ®ang khãa.","Du lieu hop thanh bi loi.","Nguyªn liÖu ®· thay ®æi.","Kh«ng ®ñ l­îng trong F4.","PhÝ VNG ch­a râ. Kh«ng trõ ®å."};
        const char* upgradeMessages[]={"Th¨ng cÊp thµnh c«ng.","Hoµn nguyªn thµnh c«ng.","ThÊt b¹i. Trang bÞ vÒ +3.","ThÊt b¹i. MÊt c¶ trang bÞ.","ThÊt b¹i. Trang bÞ vÒ +0.","Do cu: can hoan nguyen truoc.","Kh«ng trén quy t¾c th¨ng cÊp.","Tû lÖ nµy ch­a x¸c minh."};
        s_Window->m_Info.SetText(code==PT_COMPOSE_FABAO_FAILED?"\x48\xEE\x70\x20\x74\x68\xB5\x6E\x68\x20\x50\x68\xB8\x70\x20\x42\xB6\x6F\x20\x74\x68\xCA\x74\x20\x62\xB9\x69\x2E\x20\xA7\xB7\x20\x64\xEF\x6E\x67\x20\x6E\x67\x75\x79\xAA\x6E\x20\x6C\x69\xD6\x75\x20\x76\xB5\x20\x70\x68\xDD\x20\x74\x68\x65\x6F\x20\x63\xAB\x6E\x67\x20\x74\x68\xF8\x63\x2E":code==PT_COMPOSE_SOCKET_FAILED?"§ôc lç ch­a thµnh c«ng. §· dïng nguyªn liÖu vµ phÝ; trang bÞ ®­îc gi÷ l¹i.":code>=10 && code<=17?upgradeMessages[code-10]:messages[code>=0 && code<10?code:6]);
        if(code==PT_COMPOSE_OK&&g_pCoreShell){int mode=g_pCoreShell->GetGameData(PHONGTHAN_COMPOSE_CORE_MODE,0,0);if(mode==8||mode==9)s_Window->m_Info.SetText(mode==8?"\x48\x6f\x3f\x6e\x20\x48\x3f\x6e\x20\x74\x68\x3f\x6e\x68\x20\x63\x3f\x6e\x67\x2e\x20\x54\x72\x61\x6e\x67\x20\x62\x3f\x20\x3f\x3f\x20\x3f\x3f\x3f\x63\x20\x74\x72\x3f\x20\x76\x3f\x20\x46\x34\x2e":"\x3f\x3f\x20\x62\x3f\x20\x73\x75\x6e\x67\x20\x6c\x69\x6e\x68\x20\x6b\x68\x3f\x2e\x20\x54\x72\x61\x6e\x67\x20\x62\x3f\x20\x3f\x3f\x20\x3f\x3f\x3f\x63\x20\x74\x72\x3f\x20\x76\x3f\x20\x46\x34\x2e");}
        if(code==PT_COMPOSE_FABAO_FAILED&&s_Window->m_Quote.RecipeId>=1000000)s_Window->m_Info.SetText("\x47\x68\xD0\x70\x20\x63\x68\xAD\x61\x20\x74\x68\xB5\x6E\x68\x20\x63\xAB\x6E\x67\x2E\x20\x4E\x67\x75\x79\xAA\x6E\x20\x6C\x69\xD6\x75\x20\x76\xB5\x20\x70\x68\xDD\x20\xAE\xB7\x20\x64\xEF\x6E\x67\x3B\x20\x6B\x69\xD3\x6D\x20\x74\x72\x61\x20\x6B\xD5\x74\x20\x71\x75\xB6\x20\x74\x72\x61\x6E\x67\x20\x62\xDE\x20\x74\x72\x6F\x6E\x67\x20\x46\x34\x2E");
        s_Window->m_Waiting=false;
        s_Window->SetBusy(false);
        if(s_Window->m_Remaining>0)--s_Window->m_Remaining;
        if(!committed || code==PT_COMPOSE_FAILED_LOST || code==PT_COMPOSE_FABAO_FAILED || s_Window->m_Cancel)s_Window->m_Remaining=0;
        // Re-read synchronized build slots on the next frame before repeating.
        s_Window->m_LastInputs=0xFFFFFFFF;
    }
    bool Initialize()
    {
        char scheme[256],path[300];
        g_UiBase.GetCurSchemePath(scheme,sizeof(scheme));
        sprintf(path,"%s\\UiVngCompose.ini",scheme);
        KIniFile ini;
        if(!ini.Load(path)) return false;
        int screenWidth=800,screenHeight=600,offsetX=0,offsetY=0;
        PhongThanGetNativeUiMetrics(screenWidth,screenHeight,offsetX,offsetY);
        PhongThanAnchorNativePanel(ini,false);
        if(!Init(&ini,"Main")) return false;
        m_Close.Init(&ini,"CloseBtn");AddChild(&m_Close);
        m_MoneyBack.Init(&ini,"MoneyBKImg");AddChild(&m_MoneyBack);
        m_StaticBack.Init(&ini,"StaticBKImg");AddChild(&m_StaticBack);
        m_CountBack.Init(&ini,"EdtCountBKImg");AddChild(&m_CountBack);
        m_Stop.Init(&ini,"BtnStop");AddChild(&m_Stop);
        m_All.Init(&ini,"BtnConfirmAll");AddChild(&m_All);
        m_Add.Init(&ini,"BtnAdd");AddChild(&m_Add);
        m_Sub.Init(&ini,"BtnDesc");AddChild(&m_Sub);
        m_RepeatText.Init(&ini,"TxtConfirmCount");AddChild(&m_RepeatText);
        for(int a=0;a<3;++a){
            char key[32];sprintf(key,"IBItemWnd_%d",a);m_AuxPanel[a].Init(&ini,key);AddChild(&m_AuxPanel[a]);
            m_AuxDefault[a].Init(&ini,"IBItem");m_AuxDefault[a].SetPosition(21,4);m_AuxDefault[a].SetSize(32,32);m_AuxPanel[a].AddChild(&m_AuxDefault[a]);
            m_AuxEmpty[a].Init(&ini,"IBItem");m_AuxEmpty[a].SetObjectGenre(CGOG_ITEM);
            m_AuxEmpty[a].SetContainerId(UOC_BUILD_ITEM);m_AuxEmpty[a].EnablePickPut(true);m_AuxPanel[a].AddChild(&m_AuxEmpty[a]);
            m_AuxUse[a].Init(&ini,"BtnIBItem");m_AuxPanel[a].AddChild(&m_AuxUse[a]);
            char tip[]="Dung vat pham da dat vao; khong tu mua bang Xu.";m_AuxUse[a].SetToolTipInfo(tip,strlen(tip));
            m_AuxAdd[a].Init(&ini,"IBBtnAdd");m_AuxPanel[a].AddChild(&m_AuxAdd[a]);
            m_AuxSub[a].Init(&ini,"IBBtnDesc");m_AuxPanel[a].AddChild(&m_AuxSub[a]);
            m_AuxCount[a].Init(&ini,"IBTxtConfirmCount");m_AuxPanel[a].AddChild(&m_AuxCount[a]);
            m_AuxSlot[a]=-1;m_AuxQuantity[a]=1;m_AuxUse[a].CheckButton(true);m_AuxCount[a].SetText("0");
        }
        m_Combine.Init(&ini,"CombineBtn");AddChild(&m_Combine);
        m_Info.Init(&ini,"TxtInfo");AddChild(&m_Info);
        m_Cost.Init(&ini,"TxtMoneyCost");AddChild(&m_Cost);
        for(int i=0;i<9;++i)
        {
            char key[16];sprintf(key,"Item%d",i+1);
            m_Slots[i].Init(&ini,key);m_Slots[i].SetObjectGenre(CGOG_ITEM);
            m_Slots[i].SetContainerId(UOC_BUILD_ITEM);
            m_Slots[i].EnablePickPut(true);AddChild(&m_Slots[i]);
            ini.GetInteger(key,"Left",0,&m_SlotX[i]);ini.GetInteger(key,"Top",0,&m_SlotY[i]);
            m_Ghosts[i].Init(&ini,key);AddChild(&m_Ghosts[i]);m_Ghosts[i].Hide();
        }
        const char* effects[]={"EffectBegin","EffectWaiting","EffectEnd"};
        for(int e=0;e<3;++e){m_Effects[e].Init(&ini,effects[e]);AddChild(&m_Effects[e]);m_Effects[e].Hide();}
        ini.GetInteger("EffectWaiting","LoopCount",2,&m_WaitLoops);if(m_WaitLoops<1)m_WaitLoops=1;
        m_AnimationStage=0;m_PhaseLoops=0;m_ResultCode=-1;
        // Opening the correct panel is distinct from accepting a transaction.
        // Never invoke the old random-purple crafting algorithm for VNG items.
        m_Cost.SetText("0");
        m_Info.SetText("Cã thÓ mÊt ®å khi thÊt b¹i. PhÝ trõ l­îng F4.");
        sprintf(path,"%s\\UiComposeSupports.ini",scheme);m_SupportInfo.Load(path);
        m_Repeat=1;m_Remaining=0;m_Cancel=false;UpdateRepeat();SetBusy(false);
        ZeroMemory(&m_Quote,sizeof(m_Quote));
        m_Waiting=false;m_RequestSent=false;m_RequestStarted=0;m_LastInputs=0xFFFFFFFF;m_KeepResult=false;
        Wnd_AddWindow(this);
        return m_Width>0 && m_Height>0;
    }
    int WndProc(unsigned int message,unsigned int param,int value)
    {
        if((message==WND_N_BUTTON_CLICK && param==(unsigned int)(KWndWindow*)&m_Close) ||
            (message==WM_KEYDOWN && param==VK_ESCAPE)) { Close();return 1; }
        if(message==WND_N_ITEM_PICKDROP)
        {
            ITEM_PICKDROP_PLACE* pick=(ITEM_PICKDROP_PLACE*)param;
            ITEM_PICKDROP_PLACE* drop=(ITEM_PICKDROP_PLACE*)value;
            KUiObjAtContRegion from,to;ZeroMemory(&from,sizeof(from));ZeroMemory(&to,sizeof(to));
            KUiDraggedObject object;ZeroMemory(&object,sizeof(object));
            int picked=-1,dropped=-1;
            for(int i=0;i<9;++i){if(pick && pick->pWnd==&m_Slots[i])picked=i;if(drop && drop->pWnd==&m_Slots[i])dropped=i;}
            for(int aux=0;aux<3;++aux)if(drop && drop->pWnd==&m_AuxEmpty[aux]) {
                KUiObjAtRegion available[9];ZeroMemory(available,sizeof(available));g_pCoreShell->GetGameData(GDI_BUILD_ITEM,(unsigned int)available,0);
                for(int free=0;free<9;++free)if(!available[free].Obj.uId){dropped=free;break;}
            }
            if((pick && picked<0) || (drop && dropped<0) || m_Waiting)return 1;
            if(pick){m_Slots[picked].GetObject(object);from.Obj.uGenre=object.uGenre;from.Obj.uId=object.uId;from.Region.Width=object.DataW;from.Region.Height=object.DataH;from.Region.v=picked;from.eContainer=UOC_BUILD_ITEM;}
            if(drop){Wnd_GetDragObj(&object);to.Obj.uGenre=object.uGenre;to.Obj.uId=object.uId;to.Region.Width=object.DataW;to.Region.Height=object.DataH;to.Region.v=dropped;to.eContainer=UOC_BUILD_ITEM;}
            g_pCoreShell->OperationRequest(GOI_SWITCH_OBJECT,pick?(unsigned int)&from:0,drop?(int)&to:0);
            return 1;
        }
        if(message==WND_N_BUTTON_CLICK) {
            if(param==(unsigned int)(KWndWindow*)&m_Stop) {
                m_Cancel=true;m_Remaining=0;
                if(!m_RequestSent){StopAnimation();m_Waiting=false;SetBusy(false);m_Info.SetText("Da dung. Chua tru nguyen lieu.");}
                else m_Info.SetText("Dang cho ket qua server; se dung sau lan nay.");
                return 1;
            }
            if(m_Waiting)return 1;
            if(param==(unsigned int)(KWndWindow*)&m_Add || param==(unsigned int)(KWndWindow*)&m_Sub) {
                m_Repeat+=param==(unsigned int)(KWndWindow*)&m_Add?1:-1;
                if(m_Repeat<1)m_Repeat=1;if(m_Repeat>99)m_Repeat=99;UpdateRepeat();return 1;
            }
            for(int a=0;a<3;++a)if(param==(unsigned int)(KWndWindow*)&m_AuxAdd[a] || param==(unsigned int)(KWndWindow*)&m_AuxSub[a] || param==(unsigned int)(KWndWindow*)&m_AuxUse[a]) {
                if(m_AuxSlot[a]>=0){
                    if(param==(unsigned int)(KWndWindow*)&m_AuxAdd[a])++m_AuxQuantity[a];
                    if(param==(unsigned int)(KWndWindow*)&m_AuxSub[a])--m_AuxQuantity[a];
                    if(m_AuxQuantity[a]<1)m_AuxQuantity[a]=1;
                    int maximum=m_Quote.Stacks[m_AuxSlot[a]];if(m_AuxQuantity[a]>maximum)m_AuxQuantity[a]=maximum;
                    if(m_AuxQuantity[a]>3)m_AuxQuantity[a]=3;
                }
                m_LastInputs=0xFFFFFFFF;return 1;
            }
            if(param==(unsigned int)(KWndWindow*)&m_Combine || param==(unsigned int)(KWndWindow*)&m_All) {
                m_Cancel=false;m_Remaining=param==(unsigned int)(KWndWindow*)&m_All?99:m_Repeat;StartOne();return 1;
            }
        }
        return KWndShowAnimate::WndProc(message,param,value);
    }
    void Breathe()
    {
        if(!g_pCoreShell)return;
        if(m_AnimationStage>0) {
            if(m_Effects[m_AnimationStage-1].NextFrame()) {
                if(m_AnimationStage==1)Phase(2);
                else if(m_AnimationStage==2){++m_PhaseLoops;if(m_PhaseLoops>=m_WaitLoops)Phase(3);}
                else {
                    StopAnimation();m_RequestSent=true;m_RequestStarted=GetTickCount();
                    // The authoritative transaction starts only after the visual
                    // finishes. Server validation still rejects changed inputs.
                    if(!g_pCoreShell->OperationRequest(PHONGTHAN_COMPOSE_CORE_OP,(unsigned int)&m_PendingRequest,sizeof(m_PendingRequest)))
                        Result(PT_COMPOSE_INVALID_ITEMS);
                }
            }
        }
        if(m_Waiting && m_RequestSent && GetTickCount()-m_RequestStarted>8000)
        {
            StopAnimation();m_Waiting=false;m_Remaining=0;SetBusy(false);m_KeepResult=true;
            m_Info.SetText("Ch­a nhËn ph¶n håi. KiÓm tra kÕt nèi; ®ãng vµ më l¹i cöa sæ nÕu cÇn.");
        }
        KUiObjAtRegion items[9];ZeroMemory(items,sizeof(items));
        g_pCoreShell->GetGameData(GDI_BUILD_ITEM,(unsigned int)items,0);
        for(int i=0;i<9;++i)m_Slots[i].HoldObject(items[i].Obj.uGenre,items[i].Obj.uId,items[i].Region.Width,items[i].Region.Height);
        // Auxiliary items remain real server-owned build objects, with normal
        // tooltips and drag/drop. Only their display position changes.
        PHONGTHAN_COMPOSE_PREVIEW inventory;ZeroMemory(&inventory,sizeof(inventory));inventory.MetadataOnly=1;
        g_pCoreShell->GetGameData(PHONGTHAN_COMPOSE_CORE_PREVIEW,(unsigned int)&inventory,sizeof(inventory));
        int previous[3];for(int pa=0;pa<3;++pa){previous[pa]=m_AuxSlot[pa];m_AuxSlot[pa]=-1;}
        int level=0,detail=0;
        for(int main=0;main<9;++main)if(items[main].Obj.uId && inventory.Genres[main]==0){level=inventory.Levels[main];detail=inventory.Details[main];break;}
        bool weapon=detail<2;
        int defaults[3]={level>=9?(weapon?370:371):level>=6?(weapon?189:190):(weapon?187:188),level>=9?(weapon?372:373):(weapon?283:284),191};
        for(int ai=0;ai<9;++ai) {
            int d=inventory.Details[ai],a=-1;
            if(!m_SocketMode && inventory.Genres[ai]==8){
                if(d==191)a=2;
                else if(d==283 || d==284 || d==372 || d==373)a=1;
                else if((d>=187 && d<=190) || d==370 || d==371)a=0;
            }
            m_Slots[ai].SetPosition(m_SlotX[ai],m_SlotY[ai]);
            if(a<0 || m_AuxSlot[a]>=0)continue;
            if(previous[a]!=ai){m_AuxQuantity[a]=1;m_AuxUse[a].CheckButton(true);}
            m_AuxSlot[a]=ai;m_Slots[ai].SetPosition(39+59*a,198);
            if(m_AuxQuantity[a]>inventory.Stacks[ai])m_AuxQuantity[a]=inventory.Stacks[ai];
        }
        for(int a=0;a<3;++a){
            if(m_SocketMode||g_pCoreShell->GetGameData(PHONGTHAN_COMPOSE_CORE_MODE,0,0)>=8){m_AuxDefault[a].Hide();m_AuxUse[a].Enable(false);m_AuxAdd[a].Enable(false);m_AuxSub[a].Enable(false);m_AuxCount[a].SetText("0");continue;}
            bool present=m_AuxSlot[a]>=0;int d=present?inventory.Details[m_AuxSlot[a]]:defaults[a];
            char key[32],image[128],tip[256],value[16];sprintf(key,"Support%d",d);
            m_SupportInfo.GetString(key,"Image","",image,sizeof(image));m_SupportInfo.GetString(key,"Tip","",tip,sizeof(tip));
            m_AuxUse[a].SetToolTipInfo(tip,strlen(tip));
            if(present)m_AuxDefault[a].Hide();else {if(strcmp(image,m_AuxDefault[a].GetImage()))m_AuxDefault[a].SetImage(ISI_T_SPR,image,false);m_AuxDefault[a].Show();}
            sprintf(value,"%d",present?m_AuxQuantity[a]:0);m_AuxCount[a].SetText(value);
            m_AuxUse[a].Enable(!m_Waiting && present);m_AuxAdd[a].Enable(!m_Waiting && present);m_AuxSub[a].Enable(!m_Waiting && present);
        }
        DWORD signature=0;for(int s=0;s<9;++s) {
            signature=signature*33+items[s].Obj.uId;
            if(items[s].Obj.uId)signature=signature*33+g_pCoreShell->GetStackNum(items[s].Obj.uId);
        }
        if(!m_Waiting && m_Remaining>0 && !m_Cancel)StartOne();
        if(!m_Waiting && signature!=m_LastInputs) {
            m_LastInputs=signature;PHONGTHAN_COMPOSE_PREVIEW quote;ZeroMemory(&quote,sizeof(quote));
            for(int aq=0;aq<3;++aq)if(m_AuxSlot[aq]>=0)quote.Quantities[m_AuxSlot[aq]]=m_AuxUse[aq].IsButtonChecked()?m_AuxQuantity[aq]:-1;
            if(g_pCoreShell->GetGameData(PHONGTHAN_COMPOSE_CORE_PREVIEW,(unsigned int)&quote,sizeof(quote))) {
                m_Quote=quote;
                if(quote.Status==PT_COMPOSE_OK)m_Cost.Set3IntText(quote.Money);else m_Cost.SetText("--");
                if(quote.Count){m_KeepResult=false;m_Info.SetText(quote.Status==PT_COMPOSE_OK?"§· ®ñ nguyªn liÖu. KiÓm tra phÝ tr­íc khi ghÐp.":"Nguyªn liÖu ch­a ®óng c«ng thøc.");}
                else if(!m_KeepResult)m_Info.SetText("B¹n ch­a ®Æt vµo bÊt cø vËt phÈm nµo!");
            }
        }
    }
};
KUiVngCompose* KUiVngCompose::s_Window=0;
bool PhongThanVngComposePutInventoryItem(const KUiDraggedObject* item, int container)
{
    return KUiVngCompose::PutInventoryItem(item, container);
}
