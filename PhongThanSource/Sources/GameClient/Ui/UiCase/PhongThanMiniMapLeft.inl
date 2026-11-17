#include "UiStrengthRank.h"
#include "UiChatCentre.h"
#include "UiPtAuction.inl"
#include "../Elem/PopupMenu.h"
#include "../../../Engine/src/Text.h"
extern void PtAutoSettings();
class PTMiniMapLeft : public KWndImage {
    KWndButton buttons[6],worldMap;
public:
    void Initialize(){
        char scheme[256],file[320];g_UiBase.GetCurSchemePath(scheme,sizeof(scheme));
        sprintf(file,"%s\\PtMiniMapLeft.ini",scheme);KIniFile ini;
        if(!ini.Load(file))return;
        Init(&ini,"Main");
        AddChild(&worldMap);worldMap.Init(&ini,"WorldMapBtn");
        const char* names[]={"Topten","Communication","HidePet","HideSetting","Active_Btn","Auction_Btn"};
        for(int i=0;i<6;++i){AddChild(&buttons[i]);buttons[i].Init(&ini,names[i]);buttons[i].SetSize(20,20);}
        buttons[2].Enable(false);
        Wnd_AddWindow(this,WL_NORMAL);Hide();
    }
    void OpenShieldMenu(){
        char scheme[256],file[320];g_UiBase.GetCurSchemePath(scheme,sizeof(scheme));sprintf(file,"%s\\PtMiniMapLeft.ini",scheme);
        KIniFile ini;if(!ini.Load(file))return;
        KPopupMenuData* menu=(KPopupMenuData*)malloc(MENU_DATA_SIZE(4));if(!menu)return;
        KPopupMenu::InitMenuData(menu,4);menu->nNumItem=4;menu->usMenuFlag|=PM_F_COLOR_SWATCH|PM_F_AUTO_DEL_WHEN_HIDE;
        menu->nItemTitleIndent=26;menu->uTextBorderColor=0;menu->uSelTextBorderColor=0;
        for(int i=0;i<4;++i){char key[32],label[64];sprintf(key,"MenuText_%d",i);ini.GetString("ShieldOption",key,"",label,sizeof(label));
            const char* text=label[0]=='$'?label+1:label;int len=strlen(text);if(!len||len>56){free(menu);return;}
            menu->Items[i].szData[0]=KTC_COLOR;menu->Items[i].szData[1]=i==3?0:(char)255;menu->Items[i].szData[2]=i==3?(char)255:0;menu->Items[i].szData[3]=0;
            memcpy(menu->Items[i].szData+4,text,len);menu->Items[i].szData[4+len]=KTC_COLOR_RESTORE;menu->Items[i].szData[5+len]=0;menu->Items[i].uDataLen=len+5;
        }
        int x,y;GetAbsolutePos(&x,&y);menu->nX=(short)(x-220);menu->nY=(short)(y+90);KPopupMenu::Popup(menu,this,0);
    }
    virtual int WndProc(unsigned int msg,unsigned int param,int data){
        if(msg==WND_N_BUTTON_CLICK){
            if(param==(unsigned int)&worldMap)MapSetMode(MINIMAP_M_WORLD_MAP);
            else if(param==(unsigned int)&buttons[0])KUiStrengthRank::OpenDefaultWindow();
            else if(param==(unsigned int)&buttons[1])KUiChatCentre::OpenWindow(true);
            else if(param==(unsigned int)&buttons[3])OpenShieldMenu();
            else if(param==(unsigned int)&buttons[4])PtAutoSettings();
            else if(param==(unsigned int)&buttons[5])PtAuctionOpen();
            return 0;
        }
        if(msg==WND_M_MENUITEM_SELECTED&&param==(unsigned int)this){
            int row=(short)LOWORD(data);if(row>=0&&row<4&&g_pCoreShell)g_pCoreShell->OperationRequest(GOI_TOI_UU_IMAGE_COMMAND,3,row);return 1;
        }
        return KWndImage::WndProc(msg,param,data);
    }
};
static PTMiniMapLeft* s_PtMiniMapLeft=0;
class PTMapCoordinate : public KWndImage {
    KWndEdit32 xEdit,yEdit;
    KWndButton view,move,current,copy;
    int currentX,currentY;
public:
    PTMapCoordinate():currentX(0),currentY(0){}
    void Initialize(){
        char scheme[256],file[320];g_UiBase.GetCurSchemePath(scheme,sizeof(scheme));
        sprintf(file,"%s\\PtMapCoordinate.ini",scheme);KIniFile ini;if(!ini.Load(file))return;
        Init(&ini,"Main");AddChild(&xEdit);xEdit.Init(&ini,"EditX");AddChild(&yEdit);yEdit.Init(&ini,"EditY");
        AddChild(&view);view.Init(&ini,"BtnConfirm");AddChild(&move);move.Init(&ini,"BtnMove");
        AddChild(&current);current.Init(&ini,"BtnCopy");AddChild(&copy);copy.Init(&ini,"BtnLink");
        Wnd_AddWindow(this,WL_NORMAL);Hide();
    }
    void UpdatePosition(int x,int y){currentX=x;currentY=y;}
    virtual int WndProc(unsigned int msg,unsigned int param,int data){
        if(msg==WND_N_BUTTON_CLICK){
            if(param==(unsigned int)&current){xEdit.SetIntText(currentX);yEdit.SetIntText(currentY);return 0;}
            int x=xEdit.GetIntNumber(),y=yEdit.GetIntNumber();
            if(x<=0||y<=0||x>999||y>999)return 0;
            if(param==(unsigned int)&view||param==(unsigned int)&move){
                g_pCoreShell->SetFlagMode(FALSE);g_pCoreShell->SetPaintMode(TRUE);
                g_pCoreShell->DirectFindPos(x,y,FALSE,TRUE);
                if(param==(unsigned int)&move){g_pCoreShell->SetFlagAutoRun(FALSE,0,0);g_pCoreShell->SetAutoRun(TRUE);}
            }else if(param==(unsigned int)&copy){
                char value[32];sprintf(value,"%d/%d",x,y);
                HGLOBAL h=GlobalAlloc(GMEM_MOVEABLE,strlen(value)+1);
                if(h){char* p=(char*)GlobalLock(h);if(p){strcpy(p,value);GlobalUnlock(h);
                    if(OpenClipboard(NULL)){EmptyClipboard();if(!SetClipboardData(CF_TEXT,h))GlobalFree(h);CloseClipboard();}else GlobalFree(h);
                }else GlobalFree(h);}
            }
            return 0;
        }
        return KWndImage::WndProc(msg,param,data);
    }
};
static PTMapCoordinate* s_PtMapCoordinate=0;
