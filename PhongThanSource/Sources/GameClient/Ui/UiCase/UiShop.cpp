// -------------------------------------------------------------------------
//	�ļ���		��	UiTrade.h
//	������		��	Wooy(Wu yue)
//	����ʱ��	��	2002-12-21
//	��������	��	npc����ϵͳ����
// -------------------------------------------------------------------------
#include "KWin32.h"
#include "KIniFile.h"
#include "../Elem/WndMessage.h"
#include "../elem/wnds.h"
#include "UiShop.h"
#include "../../../../Headers/PhongThanUiLayoutRules.h"
#include "UiItem.h"
#include "UiStatus.h"
#include "UiTradeConfirmWnd.h"
#include "../../../Core/Src/CoreObjGenreDef.h"
#include "../../../Core/Src/CoreShell.h"
#include "../UiSoundSetting.h"
#include "../UiBase.h"
#include "UiInformation.h"
#include "UiSysMsgCentre.h"
#include <crtdbg.h>
#include "../../../Engine/src/KDebug.h"

extern iCoreShell *g_pCoreShell;
#include "UiPtShopLayout.inl"

#define SCHEME_INI        "UiShop.ini"

KUiShop *KUiShop::m_pSelf = NULL;

KUiShop::KUiShop() {
    m_bQuestShop = false;
    m_bPtLayout = false;
    m_nCurrentPage = 0;
    m_pObjsList = NULL;
    m_nObjCount = 0;
    m_nPageCount = 0;
}

//--------------------------------------------------------------------------
//	���ܣ��������������ʾ���򷵻�ʵ��ָ��
//--------------------------------------------------------------------------
KUiShop *KUiShop::GetIfVisible() {
    return m_pSelf;
}

//--------------------------------------------------------------------------
//	���ܣ��򿪴��ڣ�����Ψһ��һ�������ʵ��
//--------------------------------------------------------------------------
KUiShop *KUiShop::OpenWindow() {
    if (m_pSelf == NULL) {
        m_pSelf = new KUiShop;
        if (m_pSelf)
            m_pSelf->Initialize();
    }
    if (m_pSelf) {
        m_pSelf->m_bQuestShop=false;
        for(int qi=0;qi<14;++qi){m_pSelf->m_QuestIcons[qi].Hide();m_pSelf->m_PtSlots[qi].Show();}
        m_pSelf->m_RepairBtn.Show();m_pSelf->m_RepairAllBtn.Show();
        if (KUiItem::GetIfVisible() == FALSE)
            KUiItem::OpenWindow();
        KUiItem::OnNpcTradeMode(true);
        KUiItem* inventory=KUiItem::GetIfVisible();
        if(inventory){int ix,iy,iw,ih,sx,sy,sw,sh,screenW,screenH;
            inventory->GetAbsolutePos(&ix,&iy);inventory->GetSize(&iw,&ih);
            m_pSelf->GetPosition(&sx,&sy);m_pSelf->GetSize(&sw,&sh);Wnd_GetScreenSize(screenW,screenH);
            m_pSelf->m_oFixPos.y=PhongThanShopAlignedTop(iy,ih,sh,screenH);
            m_pSelf->SetPosition(sx,m_pSelf->m_oFixPos.y);
        }
        // NPC shop does not own the character-status (F3) window.
        g_UiBase.SetStatus(UIS_S_TRADE_NPC);
        UiSoundPlay(UI_SI_WND_OPENCLOSE);
        m_pSelf->UpdateData();
        m_pSelf->BringToTop();
        m_pSelf->Show();
    }
    return m_pSelf;
}

//--------------------------------------------------------------------------
//	���ܣ��رմ��ڣ�ͬʱ����ѡ���Ƿ�ɾ������ʵ��
//--------------------------------------------------------------------------
void KUiShop::CloseWindow() {
    if (m_pSelf) {
        if(m_pSelf->m_bQuestShop&&g_pCoreShell)g_pCoreShell->OperationRequest(PT_EVENT_SHOP_DISMISS,0,0);
        KUiItem::OnNpcTradeMode(false);
        KUiTradeConfirm::CloseWindow(true);
        g_UiBase.SetStatus(UIS_S_IDLE);
        m_pSelf->Destroy();
        m_pSelf = NULL;
    }
}

//��ʼ��
void KUiShop::Initialize() {
    AddChild(&m_ItemsBox);
    for (int i = 0; i < 14; ++i) {
        AddChild(&m_QuestIcons[i]);
        AddChild(&m_PtSlots[i]);
        AddChild(&m_PtNames[i]);
        AddChild(&m_PtPrices[i]);
    }
    AddChild(&m_BuyBtn);
    AddChild(&m_SellBtn);
    AddChild(&m_RepairBtn);
    AddChild(&m_RepairAllBtn);
    AddChild(&m_PreBtn);
    AddChild(&m_NextBtn);
    AddChild(&m_CloseBtn);

    m_ItemsBox.SetContainerId((int) UOC_NPC_SHOP);
    Wnd_AddWindow(this);
    char Scheme[256];
    g_UiBase.GetCurSchemePath(Scheme, 256);
    LoadScheme(Scheme);
}

//������淽��
void KUiShop::LoadScheme(const char *pScheme) {
    if (m_pSelf) {
        char Buff[128];
        KIniFile Ini;
        sprintf(Buff, "%s\\"SCHEME_INI, pScheme);
        BOOL loaded = Ini.Load(Buff);
        if (!loaded) loaded = Ini.LoadPakEntry(0x6B593BE4UL);
        g_DebugLog("[PTShop] resource=%s loaded=%d", Buff, loaded);
        if (loaded) {
            char sectionValue[32];
            m_pSelf->m_bPtLayout = Ini.GetString("ItemBox_0", "ImageWidth", "", sectionValue, sizeof(sectionValue)) && sectionValue[0];
            m_pSelf->KWndShowAnimate::Init(&Ini, "Main");
            m_pSelf->m_ItemsBox.Init(&Ini, "ItemBox");
            m_pSelf->m_BuyBtn.Init(&Ini, m_pSelf->m_bPtLayout ? "BuyBtn1" : "BuyBtn");
            m_pSelf->m_SellBtn.Init(&Ini, m_pSelf->m_bPtLayout ? "SellBtn1" : "SellBtn");
            m_pSelf->m_RepairBtn.Init(&Ini, "RepairBtn");
            m_pSelf->m_RepairAllBtn.Init(&Ini, "RepairAllBtn");
            if(m_pSelf->m_bPtLayout){
                // The original PT footer has repair controls and pagination only.
                m_pSelf->m_BuyBtn.Hide();m_pSelf->m_SellBtn.Hide();
            }else m_pSelf->m_RepairAllBtn.Hide();
            m_pSelf->m_PreBtn.Init(&Ini, "LeftBtn");
            m_pSelf->m_NextBtn.Init(&Ini, "RightBtn");
            m_pSelf->m_CloseBtn.Init(&Ini, "CloseBtn");

            m_pSelf->m_ItemsBox.EnablePickPut(false);
            if (m_pSelf->m_bPtLayout) {m_pSelf->InitPtLayout(&Ini);m_pSelf->InitQuestLayout(&Ini);}
        }
    }
}

void KUiShop::CancelTrade() {
    CloseWindow();
}

//���ں���
int KUiShop::WndProc(unsigned int uMsg, unsigned int uParam, int nParam) {
    switch (uMsg) {
        case WND_N_BUTTON_CLICK:
            OnClickButton((KWndButton *) (KWndWindow *) uParam, nParam);
            break;
        case WND_N_LEFT_CLICK_ITEM:
            OnBuyItem((KUiDraggedObject *) uParam,
                      g_UiBase.GetStatus() == UIS_S_TRADE_BUY);
            break;
        case WND_N_RIGHT_CLICK_ITEM:
            if (m_bPtLayout || nParam == (int) (KWndWindow *) &m_ItemsBox)
                OnBuyItem((KUiDraggedObject *) uParam, true);
            break;
        case WM_KEYDOWN:
            if (uParam == VK_ESCAPE) {
                if (g_UiBase.GetStatus() == UIS_S_TRADE_NPC) {
                    CloseWindow();
                } else {
                    m_BuyBtn.CheckButton(false);
                    m_SellBtn.CheckButton(false);
                    m_RepairBtn.CheckButton(false);
                    g_UiBase.SetStatus(UIS_S_TRADE_NPC);
                }
            }
            break;
        default:
            return KWndShowAnimate::WndProc(uMsg, uParam, nParam);
    }
    return 0;
}

int  isCheckCloseShop = 0;
int  isCheckActive = 0;
int  timer = 0;
void KUiShop::Breathe()
{
    if(m_bQuestShop){PTEventShopData data;if(!g_pCoreShell||!g_pCoreShell->GetGameData(PT_EVENT_SHOP_GET,(unsigned int)&data,sizeof(data))||data.Token!=m_QuestData.Token)CloseWindow();return;}
    isCheckCloseShop = g_pCoreShell->GetGameData(GDI_SELL_ITEM, 1, 0);
    isCheckActive = g_pCoreShell->GetGameData(GDI_SELL_ITEM, 2, 0);

    if (isCheckCloseShop && isCheckActive)
    {
        timer++;
        if (timer >= 5)
        {
            KUiItem::CloseWindow(true);
            // Leave independently opened F3 unchanged.
            CancelTrade();
            timer = 0;
        }
    }
    else
    {
        timer = 0;
    }
    //g_DebugLog("isCheckCloseShop: %d", isCheckCloseShop);
    //g_DebugLog("isCheckActive: %d", isCheckActive);
}

void KUiShop::OnBuyItem(KUiDraggedObject *pItem, bool bDoImmed) {
    if (pItem == NULL || g_pCoreShell == NULL)
        return;

    KUiObjAtContRegion Obj;
    Obj.Obj.uGenre = pItem->uGenre;
    Obj.Obj.uId = pItem->uId;
    Obj.Region.h = pItem->DataX;
    Obj.Region.v = pItem->DataY;
    Obj.Region.Width = pItem->DataW;
    Obj.Region.Height = pItem->DataH;
    Obj.eContainer = UOC_NPC_SHOP;

    UISYS_STATUS eStatus = g_UiBase.GetStatus();
    if (bDoImmed == false) {
        KUiItemBuySelInfo Price = {0};
        if (g_pCoreShell->GetGameData(GDI_TRADE_ITEM_PRICE,
                                      (unsigned int) (&Obj), (int) (&Price)) && eStatus != UIS_S_TRADE_SALE &&
            eStatus != UIS_S_TRADE_REPAIR) {
            KUiTradeConfirm::OpenWindow(&Obj, &Price, TCA_BUY);
        }
    } else {
        if (eStatus == UIS_S_TRADE_BUY) {
            g_pCoreShell->OperationRequest(GOI_TRADE_NPC_BUY,
                                           (unsigned int) (&Obj), 0);
            return;
        } else {
            if ((GetKeyState(VK_SHIFT) & 0x8000) != 0) {
                g_pCoreShell->OperationRequest(GOI_TRADE_NPC_BUY,
                                               (unsigned int) (&Obj), 1);
                return;
            } else {
                KSystemMessage Msg;
                Msg.byConfirmType = SMCT_NONE;
                Msg.byParamSize = 0;
                Msg.byPriority = 0;
                Msg.eType = SMT_NORMAL;
                Msg.uReservedForUi = 0;
                strcpy(Msg.szMessage, "Nh�n gi� ph�m Shift ��ng th�i nh�n chu�t ph�i l�p t�c mua ���c v�t ph�m!");
                KUiSysMsgCentre::AMessageArrival(&Msg, NULL);
                return;
            }
        }
    }
}

void KUiShop::OnClickButton(KWndButton *pWnd, int bCheck) {
    if(m_bQuestShop){
        for(int i=0;i<14;++i)if(pWnd==&m_QuestIcons[i]){QuestBuy(m_QuestIcons[i].goods);return;}
        if(pWnd==&m_PreBtn&&m_nCurrentPage>0)--m_nCurrentPage;
        else if(pWnd==&m_NextBtn&&m_nCurrentPage+1<m_nPageCount)++m_nCurrentPage;
        else if(pWnd==&m_CloseBtn){CloseWindow();return;}
        UpdateQuestShop(false);return;
    }
    if (Wnd_GetDragObj(NULL))
        return;
    if (pWnd == &m_BuyBtn) {
        if (bCheck) {
            m_SellBtn.CheckButton(false);
            m_RepairBtn.CheckButton(false);
            g_UiBase.SetStatus(UIS_S_TRADE_BUY);
        } else
            g_UiBase.SetStatus(UIS_S_TRADE_NPC);
    } else if (pWnd == &m_SellBtn) {
        if (bCheck) {
            m_BuyBtn.CheckButton(false);
            m_RepairBtn.CheckButton(false);
            g_UiBase.SetStatus(UIS_S_TRADE_SALE);
        } else
            g_UiBase.SetStatus(UIS_S_TRADE_NPC);
    } else if (pWnd == &m_RepairBtn) {
        if (bCheck) {
            m_BuyBtn.CheckButton(false);
            m_SellBtn.CheckButton(false);
            g_UiBase.SetStatus(UIS_S_TRADE_REPAIR);
        } else
            g_UiBase.SetStatus(UIS_S_TRADE_NPC);
    } else if(pWnd == &m_RepairAllBtn){
        m_BuyBtn.CheckButton(false);m_SellBtn.CheckButton(false);m_RepairBtn.CheckButton(false);
        g_UiBase.SetStatus(UIS_S_TRADE_NPC);
        KUiItem::ConfirmRepairAll();
    } else if (pWnd == &m_PreBtn)
        SetPage(m_nCurrentPage - 1);
    else if (pWnd == &m_NextBtn)
        SetPage(m_nCurrentPage + 1);
    else if (pWnd == &m_CloseBtn)
        CloseWindow();
}

void KUiShop::SetPage(int nIndex) {
    if (nIndex >= 0 && nIndex < m_nPageCount && m_pObjsList) {
        if (m_bPtLayout) {
            SetPtPage(nIndex);
            return;
        }
        m_ItemsBox.Clear();
        for (int i = 0; i < m_nObjCount; i++) {
            if (m_pObjsList[i].nContainer == nIndex)
                // m_ItemsBox.AddObject((KUiDraggedObject*)&m_pObjsList[i], 1);
                UpdateItem(&m_pObjsList[i], true);
        }
        m_nCurrentPage = nIndex;
    }
}

void KUiShop::UpdateItem(KUiObjAtContRegion *pItem, int bAdd) {
    if (m_bPtLayout) {
        SetPage(m_nCurrentPage);
        return;
    }
    if (pItem) {
        //UiSoundPlay(UI_SI_PICKPUT_ITEM);
        if (pItem->Obj.uGenre != CGOG_MONEY) {
            KUiDraggedObject Obj;
            Obj.uGenre = pItem->Obj.uGenre;
            Obj.uId = pItem->Obj.uId;
            Obj.DataX = pItem->Region.h;
            Obj.DataY = pItem->Region.v;
            Obj.DataW = pItem->Region.Width;
            Obj.DataH = pItem->Region.Height;
            if (bAdd)
                m_ItemsBox.AddObject(&Obj, 1);
            else
                m_ItemsBox.RemoveObject(&Obj);
        }

    } else
        SetPage(m_nCurrentPage);
}

void KUiShop::UpdateData() {
    if(m_bQuestShop){UpdateQuestShop(true);return;}
    Clear();
    m_nObjCount = g_pCoreShell->GetGameData(GDI_TRADE_NPC_ITEM, 0, 0);
    if (m_nObjCount == 0)
        return;

    if (m_pObjsList = (KUiObjAtContRegion *) malloc(sizeof(KUiObjAtContRegion) * m_nObjCount)) {
        g_pCoreShell->GetGameData(GDI_TRADE_NPC_ITEM, (unsigned int) m_pObjsList,
                                  m_nObjCount);//���߳�ִ�У�nCountֵ����
        m_nPageCount = m_bPtLayout ? (m_nObjCount + 13) / 14 : m_pObjsList[m_nObjCount - 1].nContainer + 1;
        SetPage(0);
        m_PreBtn.Enable(m_nPageCount > 1);
        m_NextBtn.Enable(m_nPageCount > 1);
    } else
        m_nObjCount = 0;
}

void KUiShop::Clear() {
    if (m_bPtLayout) {
        for (int i = 0; i < 14; ++i) {
            m_PtSlots[i].Celar();
            m_PtNames[i].Clear();
            m_PtPrices[i].Clear();
        }
    }
    m_nObjCount = 0;
    m_nPageCount = 0;
    if (m_pObjsList) {
        free(m_pObjsList);
        m_pObjsList = NULL;
    }
}

#include "UiQuestShop.inl"
