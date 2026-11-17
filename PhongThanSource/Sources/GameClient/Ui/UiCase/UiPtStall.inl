#include "../../../../Headers/PhongThanStallMessages.h"
#include "../../../../Headers/PhongThanStall.h"
#include "../Elem/WndEdit.h"
class KPtStallToggle : public KWndButton {
public:
    virtual void PaintWindow(){
        if(!g_pRepresentShell)return;
        KRUImageStretch bg;memset(&bg,0,sizeof(bg));bg.nType=ISI_T_SPR;bg.Color.Color_b.a=255;
        bg.bRenderStyle=IMAGE_RENDER_STYLE_ALPHA;bg.nISPosition=IMAGE_IS_POSITION_INIT;
        strcpy(bg.szImage,"\\spr\\phongthan\\dialog\\answer.spr");
        bg.oPosition.nX=m_nAbsoluteLeft;bg.oPosition.nY=m_nAbsoluteTop;
        bg.oEndPos.nX=m_nAbsoluteLeft+94;bg.oEndPos.nY=m_nAbsoluteTop+24;
        g_pRepresentShell->DrawPrimitives(1,&bg,RU_T_IMAGE_STRETCH,true);
        const char* caption=g_pCoreShell&&g_pCoreShell->GetTradeState()?ptStallStop:ptStallStart;
        g_pRepresentShell->OutputText(12,caption,-1,m_nAbsoluteLeft+(94-6*strlen(caption))/2,m_nAbsoluteTop+6,0xffffd94e,0);
    }
};
class KPtStall : public KWndImage {
    KWndButton close,clear;KPtStallToggle start;KWndEdit32 advertisement;KWndObjectMatrix items;
    KWndText32 level;KWndText80 experience;
    DWORD lastRefresh,lastRequest;static KPtStall* self;
    bool Initialize() {
        char scheme[256],path[384];g_UiBase.GetCurSchemePath(scheme,sizeof(scheme));
        sprintf(path,"%s\\\xB0\xDA\xCC\xAF\xBF\xD8\xD6\xC6\xC3\xE6\xB0\xE5.ini",scheme);KIniFile ini;if(!ini.Load(path))return false;
        PhongThanAnchorNativePanel(ini,false);Init(&ini,"Main");
        close.Init(&ini,"BtnClose");start.Init(&ini,"BtnStartStall");start.SetPosition(24,328);start.SetSize(94,24);clear.Init(&ini,"BtnClearStall");
        advertisement.Init(&ini,"Advertisement");items.Init(&ini,"StallBox");items.SetContainerId(UOC_ITEM_TAKE_WITH);items.EnablePickPut(false);
        level.Init(&ini,"TxtStallLevel");experience.Init(&ini,"TxtStallLevel");experience.SetPosition(25,58);experience.SetSize(190,14);experience.SetFontSize(12);AddChild(&level);AddChild(&experience);
        AddChild(&close);AddChild(&start);AddChild(&clear);AddChild(&advertisement);AddChild(&items);
        KUiPlayerBaseInfo role;memset(&role,0,sizeof(role));g_pCoreShell->GetGameData(GDI_PLAYER_BASE_INFO,(unsigned int)&role,0);
        advertisement.SetText(role.Name);lastRefresh=lastRequest=0;Wnd_AddWindow(this);return true;
    }
    void SetPrice(KUiDraggedObject* item) {
        if(!item || !g_pCoreShell || g_pCoreShell->GetTradeState() || item->uGenre!=CGOG_ITEM || item->uId<1 || item->uId>=MAX_ITEM)return;
        KUiObjAtContRegion object;memset(&object,0,sizeof(object));object.Obj.uGenre=item->uGenre;object.Obj.uId=item->uId;
        object.eContainer=UOC_ITEM_TAKE_WITH;object.Region.h=item->DataX;object.Region.v=item->DataY;
        object.Region.Width=item->DataW;object.Region.Height=item->DataH;
        KUiItemBuySelInfo price;memset(&price,0,sizeof(price));
        if(g_pCoreShell->GetGameData(GDI_TRADE_ITEM_PRICE,(unsigned int)&object,(int)&price))
            KUiSetPrice::OpenWindow(g_pCoreShell->GetTradePrice(item->uId),&object,&price);
    }
    void Close() {
        Hide();if(g_UiBase.GetStatus()==UIS_S_TRADE_SETPRICE)g_UiBase.SetStatus(UIS_S_IDLE);
        KUiItem::OnNpcTradeMode(false);
    }
public:
    static void Open() {
        if(!g_pCoreShell)return;
        if(!self){self=new KPtStall;if(!self->Initialize()){delete self;self=0;return;}}
        if(!g_pCoreShell->GetTradeState())g_UiBase.SetStatus(UIS_S_TRADE_SETPRICE);
        KUiItem::OnNpcTradeMode(true);self->lastRefresh=0;self->Breathe();self->Show();self->BringToTop();
    }
    virtual void Breathe() {
        if(!g_pCoreShell)return;
        const bool active=g_pCoreShell->GetTradeState()!=0;
        if(IsVisible() && !active && g_UiBase.GetStatus()==UIS_S_IDLE)g_UiBase.SetStatus(UIS_S_TRADE_SETPRICE);
        PTStallStatus progress;memset(&progress,0,sizeof(progress));
        if(g_pCoreShell->GetGameData(PT_GDI_STALL_STATUS,(unsigned int)&progress,sizeof(progress))){
            level.SetIntText(progress.Level);char text[64];if(progress.NextSales)sprintf(text,ptStallSold,progress.Sales,progress.NextSales);else sprintf(text,ptStallMax,progress.Sales);experience.SetText(text);
        }
        start.CheckButton(active);clear.Enable(!active);advertisement.Enable(!active);
        DWORD now=GetTickCount();if(lastRefresh && now-lastRefresh<250)return;lastRefresh=now;
        KUiObjAtRegion objects[35];memset(objects,0,sizeof(objects));
        int count=g_pCoreShell->GetGameData(PT_GDI_BAG_FIRST,(unsigned int)objects,35);
        if(count<0 || count>35)return;
        for(int oldIndex=items.GetObjectNum()-1;oldIndex>=0;--oldIndex){
            KUiDraggedObject previous=*items.GetObjectByIndex(oldIndex);bool keep=false;
            for(int row=0;row<count;++row)if(objects[row].Obj.uId==previous.uId &&
                objects[row].Region.h==previous.DataX && objects[row].Region.v==previous.DataY &&
                g_pCoreShell->GetTradePrice(previous.uId)>0){keep=true;break;}
            if(!keep)items.RemoveObject(&previous);
        }
        for(int i=0;i<count;++i)if(g_pCoreShell->GetTradePrice(objects[i].Obj.uId)>0) {
            KUiDraggedObject object;memset(&object,0,sizeof(object));object.uGenre=objects[i].Obj.uGenre;object.uId=objects[i].Obj.uId;
            object.DataX=objects[i].Region.h;object.DataY=objects[i].Region.v;object.DataW=objects[i].Region.Width;object.DataH=objects[i].Region.Height;
            KUiDraggedObject existing;
            if(!items.GetObject(existing,object.DataX,object.DataY) || existing.uId!=object.uId)
                items.AddObject(&object,1);
        }
    }
    virtual int WndProc(unsigned int message,unsigned int param,int data) {
        if(message==WM_KEYDOWN && param==VK_ESCAPE){Close();return 1;}
        if(message==WND_N_RIGHT_CLICK_ITEM){
            KUiDraggedObject* item=(KUiDraggedObject*)param;
            if(item && g_pCoreShell && !g_pCoreShell->GetTradeState() && item->uGenre==CGOG_ITEM && item->uId>0 && item->uId<MAX_ITEM){
                KUiObjAtContRegion object;memset(&object,0,sizeof(object));object.Obj.uGenre=item->uGenre;object.Obj.uId=item->uId;
                object.eContainer=UOC_ITEM_TAKE_WITH;object.Region.h=item->DataX;object.Region.v=item->DataY;
                object.Region.Width=item->DataW;object.Region.Height=item->DataH;
                g_pCoreShell->OperationRequest(GOI_PLAYER_SETPRICE,(unsigned int)&object,0);
            }return 1;
        }
        if(message==WND_N_LEFT_CLICK_ITEM){SetPrice((KUiDraggedObject*)param);return 1;}
        if(message==WND_N_BUTTON_CLICK) {
            if(param==(unsigned int)(KWndWindow*)&close){Close();return 1;}
            if(param==(unsigned int)(KWndWindow*)&start) {
                DWORD now=GetTickCount();if(lastRequest && now-lastRequest<1000)return 1;
                char name[32];memset(name,0,sizeof(name));advertisement.GetText(name,sizeof(name),true);
                if(g_pCoreShell->GetTradeState())name[0]=0;
                if(!name[0] && !g_pCoreShell->GetTradeState())return 1;lastRequest=now;
                g_pCoreShell->OperationRequest(GOI_PLAYER_DOTRADE,(unsigned int)name,0);return 1;
            }
            if(param==(unsigned int)(KWndWindow*)&clear && !g_pCoreShell->GetTradeState()) {
                KUiObjAtRegion objects[35];memset(objects,0,sizeof(objects));
                int count=g_pCoreShell->GetGameData(PT_GDI_BAG_FIRST,(unsigned int)objects,35);if(count<0 || count>35)return 1;
                for(int i=0;i<count;++i) {
                    KUiObjAtContRegion object;memset(&object,0,sizeof(object));
                    object.Obj=objects[i].Obj;object.Region=objects[i].Region;object.eContainer=UOC_ITEM_TAKE_WITH;
                    if(g_pCoreShell->GetTradePrice(object.Obj.uId)>0)g_pCoreShell->OperationRequest(GOI_PLAYER_SETPRICE,(unsigned int)&object,0);
                }
                return 1;
            }
        }
        return KWndImage::WndProc(message,param,data);
    }
};
KPtStall* KPtStall::self=0;
