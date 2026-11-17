/*****************************************************************************************
//	½çÃæ--´¢ÎïÏä½çÃæ
//	Copyright : Kingsoft 2003
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2003-4-21
*****************************************************************************************/
#include "KWin32.h"
#include "KIniFile.h"
#include "../elem/wnds.h"
#include "UiStoreBox.h"
#include "UiGetMoney.h"
#include "UiGetString.h"
#include "UiItem.h"
#include "../../../core/src/coreshell.h"
#include "../../../Represent/iRepresent/iRepresentShell.h"
#include "../../../core/src/GameDataDef.h"
#include "../UiSoundSetting.h"
#include "../UiBase.h"
#include "UiPlayerShop.h"
#include "UiPlayerLock.h"
#include "PhongThanNativeUiLayout.h"
#include "../../../../Headers/PhongThanF4Protocol.h"
#include <crtdbg.h>
#include <time.h>
#include "../../../../Headers/PhongThanBank.h"
// Keep this local query ID in sync with CoreClient; it is not a wire opcode.

extern iRepresentShell *g_pRepresentShell;
extern iCoreShell *g_pCoreShell;

#define SCHEME_INI    "UiStoreBox.ini"

KUiStoreBox *KUiStoreBox::m_pSelf = NULL;

enum WAIT_OTHER_WND_OPER_PARAM {
    UISTOREBOX_WAIT_GETMONEY,
    UISTOREBOX_WAIT_SAVEMONEY,
};

//--------------------------------------------------------------------------
//	¹¦ÄÜ£ºÈç¹û´°¿ÚÕý±»ÏÔÊ¾£¬Ôò·µ»ØÊµÀýÖ¸Õë
//--------------------------------------------------------------------------
KUiStoreBox *KUiStoreBox::GetIfVisible() {
    if (m_pSelf && m_pSelf->IsVisible())
        return m_pSelf;
    return NULL;
}

//--------------------------------------------------------------------------
//	¹¦ÄÜ£º´ò¿ª´°¿Ú£¬·µ»ØÎ¨Ò»µÄÒ»¸öÀà¶ÔÏóÊµÀý
//--------------------------------------------------------------------------
KUiStoreBox *KUiStoreBox::OpenWindow(int nToPage) {
    if (m_pSelf == NULL) {
        m_pSelf = new KUiStoreBox;
        if (m_pSelf)
            m_pSelf->Initialize();
    }
    if (m_pSelf) {
        if (KUiItem::GetIfVisible() == FALSE)
            KUiItem::OpenWindow();
        UiSoundPlay(UI_SI_WND_OPENCLOSE);
        m_pSelf->UpdateData(nToPage);
        m_pSelf->BringToTop();
        if(m_pSelf->m_Width<=0 || m_pSelf->m_Height<=0) { CloseWindow(); return NULL; }
        m_pSelf->Show();
        Wnd_GameSpaceHandleInput(false);

    }
    return m_pSelf;
}

//--------------------------------------------------------------------------
//	¹¦ÄÜ£º¹Ø±Õ´°¿Ú
//--------------------------------------------------------------------------
void KUiStoreBox::CloseWindow() {
    if (m_pSelf) {
        if(g_pCoreShell){PTBankSecurityRequest q;memset(&q,0,sizeof(q));q.Action=3;g_pCoreShell->OperationRequest(PT_GOI_BANK_SECURITY,(unsigned int)&q,sizeof(q));}
        Wnd_GameSpaceHandleInput(true);
        m_pSelf->Destroy();
        m_pSelf = NULL;
    }
}

// -------------------------------------------------------------------------
// ¹¦ÄÜ	: ³õÊ¼»¯
// -------------------------------------------------------------------------
void KUiStoreBox::Initialize() {
    m_nCurPage = -1;m_nRowOffset=0;

    AddChild(&m_Money);AddChild(&m_RowUp);AddChild(&m_RowDown);
    AddChild(&m_GetMoneyBtn);
    AddChild(&m_SaveMoneyBtn);AddChild(&m_SortBtn);AddChild(&m_LockBtn);
    AddChild(&m_RentImage);AddChild(&m_RentInfo);AddChild(&m_TimeLeft);AddChild(&m_RentBtn);
    AddChild(&m_CloseBtn);
    AddChild(&m_ItemBox);
    m_ItemBox.SetContainerId((int) UOC_STORE_BOX);
    for(int page=0;page<5;++page)AddChild(&m_PageBtn[page]);
    char Scheme[256];
    g_UiBase.GetCurSchemePath(Scheme, 256);
    LoadScheme(Scheme);

    m_nMoney = 0;
    Wnd_AddWindow(this);
}

//--------------------------------------------------------------------------
//	¹¦ÄÜ£º¹¹Ôìº¯Êý
//--------------------------------------------------------------------------
void KUiStoreBox::UpdateData(int nCurPage) {
    m_nExpandBoxNum = g_pCoreShell->GetGameData(GDI_EXBOX_NUM, NULL, NULL);
    if(nCurPage < -1 || nCurPage > REPOSITORY_FIVE)nCurPage=-1;
    m_nCurPage = nCurPage;
    for(int page=0;page<5;++page)m_PageBtn[page].CheckButton(page==m_nCurPage+1);

    m_ItemBox.Clear();
    m_nMoney = 0;m_Money.Set3IntText(0);
    m_nRowOffset=0;m_ItemBox.SetRowViewport(0,8,10);m_RowUp.Enable(false);m_RowDown.Enable(true);

    KUiObjAtRegion *pObjs = NULL;
    m_ItemBox.SetContainerId((int) UOC_STORE_BOX);
    unsigned uIndex = GDI_ITEM_IN_STORE_BOX;
    switch (m_nCurPage) {
        case REPOSITORY_ONE:
            m_CurPage.SetText("R­¬ng më réng 1");
            m_ItemBox.SetContainerId((int) UOC_STORE_BOX1);
            uIndex = GDI_ITEM_IN_STORE_BOX1;
            break;
        case REPOSITORY_TWO:
            m_CurPage.SetText("R­¬ng më réng 2");
            m_ItemBox.SetContainerId((int) UOC_STORE_BOX2);
            uIndex = GDI_ITEM_IN_STORE_BOX2;
            break;
        case REPOSITORY_THREE:
            m_CurPage.SetText("R­¬ng më réng 3");
            m_ItemBox.SetContainerId((int) UOC_STORE_BOX3);
            uIndex = GDI_ITEM_IN_STORE_BOX3;
            break;
        case REPOSITORY_FOUR:
            m_CurPage.SetText("R­¬ng më réng 4");
            uIndex = GDI_ITEM_IN_STORE_BOX4;
            m_ItemBox.SetContainerId((int) UOC_STORE_BOX4);
            break;
        case REPOSITORY_FIVE:
            m_CurPage.SetText("R­¬ng më réng 5");
            uIndex = GDI_ITEM_IN_STORE_BOX5;
            m_ItemBox.SetContainerId((int) UOC_STORE_BOX5);
            break;
        default:
            m_CurPage.SetText("R­¬ng chøa ®å");
            m_ItemBox.SetContainerId((int) UOC_STORE_BOX);
            uIndex = GDI_ITEM_IN_STORE_BOX;
            break;
    }

    int nCount = g_pCoreShell->GetGameData(uIndex, 0, 0);
    if (nCount == 0)
        return;

    if (pObjs = (KUiObjAtRegion *) malloc(sizeof(KUiObjAtRegion) * nCount)) {
        g_pCoreShell->GetGameData(uIndex, (unsigned int) pObjs, nCount);//µ¥Ïß³ÌÖ´ÐÐ£¬nCountÖµ²»±ä
        for (int i = 0; i < nCount; i++)
            UpdateItem(m_nCurPage, &pObjs[i], true);
        free(pObjs);
        pObjs = NULL;
    }
}

// -------------------------------------------------------------------------
// ¹¦ÄÜ	: ÎïÆ·±ä»¯¸üÐÂ
// -------------------------------------------------------------------------
void KUiStoreBox::UpdateItem(int nToPage, KUiObjAtRegion *pItem, int bAdd) {
    if (!pItem) {UpdateData(m_nCurPage);return;}
    if (pItem->Obj.uGenre != CGOG_MONEY && m_nCurPage != nToPage)
        return;

    if (pItem) {
        UiSoundPlay(UI_SI_PICKPUT_ITEM);
        if (pItem->Obj.uGenre != CGOG_MONEY) {
            KUiDraggedObject Obj;
            memset(&Obj,0,sizeof(Obj));
            Obj.uGenre = pItem->Obj.uGenre;
            Obj.uId = pItem->Obj.uId;
            Obj.DataX = pItem->Region.h;
            Obj.DataY = pItem->Region.v;
            Obj.DataW = pItem->Region.Width;
            Obj.DataH = pItem->Region.Height;
            if (bAdd)
                m_ItemBox.AddObject(&Obj, 1);
            else
                m_ItemBox.RemoveObject(&Obj);
        } else {
            m_nMoney = pItem->Obj.uId;
            m_Money.Set3IntText(m_nMoney);
        }
    } else
        UpdateData(m_nCurPage);
}

// -------------------------------------------------------------------------
// ¹¦ÄÜ	: ÔØÈë½çÃæ·½°¸
// -------------------------------------------------------------------------
void KUiStoreBox::LoadScheme(const char *pScheme) {
    char Buff[128];
    KIniFile Ini;
    sprintf(Buff, "%s\\%s", pScheme, SCHEME_INI);
    if (m_pSelf && Ini.Load(Buff)) {
        PhongThanAnchorNativePanel(Ini,false);
        m_pSelf->Init(&Ini, "Main");
        m_pSelf->m_SaveMoneyBtn.Init(&Ini,"SaveMoneyBtn");m_pSelf->m_SortBtn.Init(&Ini,"ClearStoreBtn");
        Ini.WriteInteger("BtnRent","Width",52);Ini.WriteInteger("BtnRent","Height",19);Ini.WriteInteger("BtnRent","Up",0);Ini.WriteInteger("BtnRent","Down",1);Ini.WriteInteger("BtnRent","Over",0);
        m_pSelf->m_RentBtn.Init(&Ini,"BtnRent");
        m_pSelf->m_RentImage.Init(&Ini,"ImgRent");m_pSelf->m_RentInfo.Init(&Ini,"TxtInfo");m_pSelf->m_TimeLeft.Init(&Ini,"TxtTimeLeft");
        char timeLimit[64];Ini.GetString("Main","TimeLimitText","",timeLimit,sizeof(timeLimit));m_pSelf->m_TimeLeft.SetText(timeLimit);
        m_pSelf->m_Money.Init(&Ini, "Money");
        m_pSelf->m_GetMoneyBtn.Init(&Ini, "GetMoneyBtn");
        m_pSelf->m_CloseBtn.Init(&Ini, "CloseBtn");
        m_pSelf->m_ItemBox.Init(&Ini, "ItemBox");
        m_pSelf->m_ItemBox.EnableTracePutPos(true);
        m_pSelf->m_ItemBox.SetRowViewport(0,8,10);
        const char* rowButtons[]={"BankRowUp","BankRowDown"};
        for(int b=0;b<2;++b){Ini.WriteInteger(rowButtons[b],"Left",238);Ini.WriteInteger(rowButtons[b],"Top",b?344:74);Ini.WriteInteger(rowButtons[b],"Width",16);Ini.WriteInteger(rowButtons[b],"Height",22);Ini.WriteInteger(rowButtons[b],"Font",14);Ini.WriteString(rowButtons[b],"Color","255,217,78");Ini.WriteString(rowButtons[b],"OverColor","255,255,255");Ini.WriteString(rowButtons[b],"DisableColor","96,96,96");}
        m_pSelf->m_RowUp.Init(&Ini,rowButtons[0]);m_pSelf->m_RowUp.SetLabel("^");m_pSelf->m_RowUp.Enable(false);
        m_pSelf->m_RowDown.Init(&Ini,rowButtons[1]);m_pSelf->m_RowDown.SetLabel("v");
        // Restore the original treasury password control.
        m_pSelf->m_LockBtn.Init(&Ini,"BtnCreatePassword");
        for(int page=0;page<5;++page){
            char section[32];sprintf(section,"BtnPage_%d",page);
            m_pSelf->m_PageBtn[page].Init(&Ini,section);
        }
        m_pSelf->m_RentImage.Enable(false);m_pSelf->m_RentInfo.Enable(false);m_pSelf->m_TimeLeft.Enable(false);m_pSelf->m_RentBtn.BringToTop();m_pSelf->m_LockBtn.BringToTop();

    }
}

// -------------------------------------------------------------------------
// ¹¦ÄÜ	: ´°¿Úº¯Êý
// -------------------------------------------------------------------------
int KUiStoreBox::WndProc(unsigned int uMsg, unsigned int uParam, int nParam) {
    switch (uMsg) {
        case WM_KEYDOWN:
            if(uParam==VK_ESCAPE) { CloseWindow(); return 1; }
            break;
        case WND_N_ITEM_PICKDROP:
            OnItemPickDrop((ITEM_PICKDROP_PLACE *) uParam, (ITEM_PICKDROP_PLACE *) nParam);
            break;
        case WND_N_RIGHT_CLICK_ITEM:
            OnClickItem((KUiDraggedObject *) uParam);
            break;
        case WND_N_BUTTON_CLICK: {
            if(uParam==(unsigned int)(KWndWindow*)&m_RowUp || uParam==(unsigned int)(KWndWindow*)&m_RowDown) {
                m_nRowOffset+=uParam==(unsigned int)(KWndWindow*)&m_RowUp?-1:1;
                if(m_nRowOffset<0)m_nRowOffset=0;if(m_nRowOffset>2)m_nRowOffset=2;
                m_ItemBox.SetRowViewport(m_nRowOffset,8,10);m_RowUp.Enable(m_nRowOffset>0);m_RowDown.Enable(m_nRowOffset<2);return 1;
            }
            if(uParam==(unsigned int)(KWndWindow*)&m_RentBtn){
                if(m_nCurPage>=0&&m_nCurPage<4)g_pCoreShell->OperationRequest(PT_GOI_F4,PT_F4_BANK_MENU,m_nCurPage+5);return 1;
            }
            if(uParam==(unsigned int)(KWndWindow*)&m_LockBtn){
                PTBankSecurityRequest q;memset(&q,0,sizeof(q));if(g_pCoreShell)g_pCoreShell->OperationRequest(PT_GOI_BANK_SECURITY,(unsigned int)&q,sizeof(q));return 1;
            }
            for(int page=0;page<5;++page)
                if(uParam==(unsigned int)(KWndWindow*)&m_PageBtn[page]){UpdateData(page-1);return 1;}
            if(uParam==(unsigned int)(KWndWindow*)&m_SaveMoneyBtn) {
                int amount=g_pCoreShell->GetGameData(GDI_PLAYER_HOLD_MONEY,0,0);
                KUiGetMoney::OpenWindow(0,amount,this,UISTOREBOX_WAIT_SAVEMONEY,&m_Money,"\107\366\151\040\164\151\322\156\040\166\265\157\040\153\150\157");return 1;
            }
            if(uParam==(unsigned int)(KWndWindow*)&m_SortBtn) {
                if(PageAccessible())g_pCoreShell->OperationRequest(PT_GOI_F4,PT_F4_SORT,m_nCurPage+5);
                return 1;
            }
            if (uParam == (unsigned int) (KWndWindow *) &m_CloseBtn)
                CloseWindow();
            else if (uParam == (unsigned int) (KWndWindow *) &m_GetMoneyBtn)
                KUiGetMoney::OpenWindow(0, m_nMoney, this, UISTOREBOX_WAIT_GETMONEY, &m_Money,"\122\363\164\040\164\151\322\156\040\164\162\157\156\147\040\153\150\157");
            else if (uParam == (unsigned int) (KWndWindow *) &m_PreBtn) {
                m_nCurPage--;
                if (m_nCurPage < -1) {
                    m_nCurPage = -1;
                    return 0;
                }
                this->UpdateData(m_nCurPage);
            } else if (uParam == (unsigned int) (KWndWindow *) &m_NextBtn) {
                m_nCurPage++;
                if (m_nCurPage > REPOSITORY_FIVE) {
                    m_nCurPage = REPOSITORY_FIVE;
                    return 0;
                }
                this->UpdateData(m_nCurPage);
            }
            break;
        }
        case WND_M_OTHER_WORK_RESULT:
            if(uParam==UISTOREBOX_WAIT_SAVEMONEY && nParam>0)
                g_pCoreShell->OperationRequest(GOI_MONEY_INOUT_STORE_BOX,true,nParam);
            else if (uParam == UISTOREBOX_WAIT_GETMONEY)
                OnGetMoney(nParam);
            break;
        default:
            return KWndShowAnimate::WndProc(uMsg, uParam, nParam);
    }
    return 0;
}

void KUiStoreBox::OnGetMoney(int nMoney) {
    if (nMoney > 0) {
        g_pCoreShell->OperationRequest(GOI_MONEY_INOUT_STORE_BOX,
                                       false, nMoney);
    }
}

void KUiStoreBox::OnClickItem(KUiDraggedObject *pItem) {
    if(!PageAccessible())return;
    KUiItem* bag=KUiItem::GetIfVisible();
    if(bag)bag->PutExternalItem(pItem,GetPageContainer());
}

#include "UiStoreBoxActions.inl"

void KUiStoreBox::OnItemPickDrop(ITEM_PICKDROP_PLACE *pPickPos, ITEM_PICKDROP_PLACE *pDropPos) {
    if (!PageAccessible() || g_UiBase.GetStatus() != UIS_S_IDLE)
        return;
    KUiObjAtContRegion Pick, Drop;
    KUiDraggedObject Obj;

    if (pPickPos) {
        _ASSERT(pPickPos->pWnd);
        ((KWndObjectMatrix *) (pPickPos->pWnd))->GetObject(
                Obj, pPickPos->h, pPickPos->v);
        Pick.Obj.uGenre = Obj.uGenre;
        Pick.Obj.uId = Obj.uId;
        Pick.Region.Width = Obj.DataW;
        Pick.Region.Height = Obj.DataH;
        Pick.Region.h = Obj.DataX;
        Pick.Region.v = Obj.DataY;
        switch (m_nCurPage) {
            case REPOSITORY_ONE:
                Pick.eContainer = UOC_STORE_BOX1;
                break;
            case REPOSITORY_TWO:
                Pick.eContainer = UOC_STORE_BOX2;
                break;
            case REPOSITORY_THREE:
                Pick.eContainer = UOC_STORE_BOX3;
                break;
            case REPOSITORY_FOUR:
                Pick.eContainer = UOC_STORE_BOX4;
                break;
            case REPOSITORY_FIVE:
                Pick.eContainer = UOC_STORE_BOX5;
                break;
            default:
                Pick.eContainer = UOC_STORE_BOX;
                break;
        }
    }

    if (pDropPos) {
        Wnd_GetDragObj(&Obj);
        Drop.Obj.uGenre = Obj.uGenre;
        Drop.Obj.uId = Obj.uId;
        Drop.Region.Width = Obj.DataW;
        Drop.Region.Height = Obj.DataH;
        Drop.Region.h = pDropPos->h;
        Drop.Region.v = pDropPos->v;
        switch (m_nCurPage) {
            case REPOSITORY_ONE:
                Drop.eContainer = UOC_STORE_BOX1;
                break;
            case REPOSITORY_TWO:
                Drop.eContainer = UOC_STORE_BOX2;
                break;
            case REPOSITORY_THREE:
                Drop.eContainer = UOC_STORE_BOX3;
                break;
            case REPOSITORY_FOUR:
                Drop.eContainer = UOC_STORE_BOX4;
                break;
            case REPOSITORY_FIVE:
                Drop.eContainer = UOC_STORE_BOX5;
                break;
            default:
                Drop.eContainer = UOC_STORE_BOX;
                break;
        }
    }
    g_pCoreShell->OperationRequest(GOI_SWITCH_OBJECT,
                                   pPickPos ? (unsigned int) &Pick : 0,
                                   pDropPos ? (int) &Drop : 0);
}

bool KUiStoreBox::PageAccessible() const {
    if(!g_pCoreShell||g_pCoreShell->GetGameData(PT_BANK_LOCK_GET,0,0))return false;
    if(m_nCurPage<0)return true;
    int expiry=g_pCoreShell->GetGameData(PT_BANK_EXPIRY_GET,0,m_nCurPage+2);
    return PTBankActive(expiry,(int)time(NULL));
}
void KUiStoreBox::PaintWindow() {
    if(g_pCoreShell){
        int expiry=m_nCurPage<0?PT_BANK_PERMANENT:g_pCoreShell->GetGameData(PT_BANK_EXPIRY_GET,0,m_nCurPage+2);
        char text[64];
        if(expiry==PT_BANK_PERMANENT)strcpy(text,"\126\334\156\150\040\166\151\324\156");
        else if(expiry>(int)time(NULL))sprintf(text,"\045\144\040\156\147\265\171",(expiry-(int)time(NULL)+86399)/86400);
        else strcpy(text,expiry?"\110\325\164\040\150\271\156":"\103\150\255\141\040\155\353");
        m_TimeLeft.SetText(text);m_RentInfo.SetText("");m_nExpandBoxNum=g_pCoreShell->GetGameData(GDI_EXBOX_NUM,0,0);
        if(m_nCurPage>=0 && expiry!=PT_BANK_PERMANENT)m_RentBtn.Show();else m_RentBtn.Hide();
    }
    KWndShowAnimate::PaintWindow();
    if (g_pRepresentShell) {
        if (!PageAccessible()) {
            KRUShadow bg;
            m_ItemBox.GetAbsolutePos(&bg.oPosition.nX, &bg.oPosition.nY);
            bg.oPosition.nX;
            bg.oPosition.nY;
            bg.Color.Color_dw = 0x191079a5;
            m_ItemBox.GetSize(&bg.oEndPos.nX, &bg.oEndPos.nY);
            bg.oEndPos.nX += bg.oPosition.nX;
            bg.oEndPos.nY += bg.oPosition.nY;
            g_pRepresentShell->DrawPrimitives(1, &bg, RU_T_SHADOW, true);
        }
    }
}

#include "UiBankPassword.inl"
