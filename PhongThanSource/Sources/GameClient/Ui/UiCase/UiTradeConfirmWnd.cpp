// -------------------------------------------------------------------------
//	文件名		：	交易确认框
//	创建者		：	Wooy(Wu yue)
//	创建时间	：	2003-5-28
// -------------------------------------------------------------------------
#include "KWin32.h"
#include "KIniFile.h"
#include "../Elem/AutoLocateWnd.h"
#include "../Elem/WndMessage.h"
#include "../Elem/Wnds.h"
#include "../UiBase.h"
#include "UiTradeConfirmWnd.h"
#include "UiItem.h"
#include "../UiSoundSetting.h"
#include "../../../core/src/coreshell.h"
#include "../../../engine/src/kdebug.h"

extern iCoreShell *g_pCoreShell;

#define    SCHEME_INI    "UiTradeConfirmWnd.ini"

KUiTradeConfirm *KUiTradeConfirm::m_pSelf = NULL;


//--------------------------------------------------------------------------
//	功能：打开窗口，返回唯一的一个类对象实例
//--------------------------------------------------------------------------
KUiTradeConfirm *KUiTradeConfirm::OpenWindow(KUiObjAtContRegion *pObj,
                                             KUiItemBuySelInfo *pPriceInfo, TRADE_CONFIRM_ACTION eAction) {
    if (pObj == NULL || pPriceInfo == NULL)
        return NULL;
    if (m_pSelf == NULL) {
        m_pSelf = new KUiTradeConfirm;
        if (m_pSelf && !m_pSelf->Initialize()) {
            g_DebugLog("[PTTradeConfirm] OPEN_REJECT resource initialization failed");
            m_pSelf->Destroy();
            m_pSelf = NULL;
            return NULL;
        }
    }
    if (m_pSelf) {
        m_pSelf->m_BuyImg.Hide();
        m_pSelf->m_SaleImg.Hide();
        m_pSelf->m_RepairImg.Hide();
        m_pSelf->m_BuyNumber.Hide();
        m_pSelf->m_Increase.Enable(false);
        m_pSelf->m_Decrease.Enable(false);
        m_pSelf->m_MoneyUnitText.Hide();
        m_pSelf->m_OkBtn.Enable(true);
        if (eAction == TCA_SALE) {
            m_pSelf->m_SaleImg.Show();
            m_pSelf->m_Money.SetTextColor(m_pSelf->m_uNormalPriceColor);
        } else {
            if (eAction == TCA_BUY) {
                m_pSelf->m_BuyImg.Show();
                m_pSelf->m_BuyNumber.Show();
                m_pSelf->m_Increase.Enable(true);
                m_pSelf->m_Decrease.Enable(true);
                m_pSelf->m_MoneyUnitText.Show();
            } else if (eAction == TCA_BUYPLAYER) {
                m_pSelf->m_BuyImg.Show();
                m_pSelf->m_BuyNumber.Show();
                m_pSelf->m_MoneyUnitText.Show();
            } else {
                m_pSelf->m_RepairImg.Show();
            }
            m_pSelf->m_nCurrentScore = g_pCoreShell->GetOwnValue(pPriceInfo->nMoneyUnit);
            m_pSelf->m_OkBtn.Enable(m_pSelf->m_nCurrentScore >= pPriceInfo->nCurPrice);
            m_pSelf->m_Money.SetTextColor((m_pSelf->m_nCurrentScore >= pPriceInfo->nCurPrice) ?
                                          m_pSelf->m_uNormalPriceColor : m_pSelf->m_uNotEnoughMoneyPriceColor);
        }
        m_pSelf->m_Action = eAction;
        m_pSelf->m_ItemInfo = *pObj;
        m_pSelf->m_PriceInfo = *pPriceInfo;
        UiSoundPlay(UI_SI_WND_OPENCLOSE);
        m_pSelf->BringToTop();
        m_pSelf->Show();
    }
    return m_pSelf;
}

KUiTradeConfirm::KUiTradeConfirm() {
    m_Action = TCA_BUY;
    m_bResourceReady = false;
    m_bCheckingInput = false;
    m_PriceInfo.szItemName[0] = 0;
    m_PriceInfo.nCurPrice = 0;
    m_PriceInfo.nMoneyUnit = 0;
    m_nCurrentScore = 0;
    m_ItemInfo.Obj.uGenre = CGOG_NOTHING;
}

//如果窗口正被显示，则返回实例指针
KUiTradeConfirm *KUiTradeConfirm::GetIfVisible() {
    if (m_pSelf && m_pSelf->IsVisible())
        return m_pSelf;
    return NULL;
}

//--------------------------------------------------------------------------
//	功能：关闭销毁窗口
//--------------------------------------------------------------------------
void KUiTradeConfirm::CloseWindow(bool bDestroy) {
    if (m_pSelf) {
        if (bDestroy == false)
            m_pSelf->Hide();
        else {
            m_pSelf->Destroy();
            m_pSelf = NULL;
        }
    }
}


//显示窗口
void KUiTradeConfirm::Show() {
    if (!m_bResourceReady) return;
    m_ItemName.SetText(m_PriceInfo.szItemName);
    m_MoneyUnitText.SetMoneyUnitText(m_PriceInfo.nMoneyUnit);
    int Left, Top;
    ALW_GetWndPosition(Left, Top, m_Width, m_Height);
    SetPosition(Left, Top);

    m_BuyNumber.Enable(m_Action!=TCA_BUYPLAYER);
    m_BuyNumber.SetIntText(m_Action==TCA_BUYPLAYER?max(1,g_pCoreShell->GetStackNum(m_ItemInfo.Obj.uId)):1);
    Wnd_SetFocusWnd(&m_BuyNumber);

    KWndImage::Show();
    Wnd_SetExclusive((KWndWindow *) this);
}

//隐藏窗口
void KUiTradeConfirm::Hide() {
    Wnd_ReleaseExclusive((KWndWindow *) this);
    KWndImage::Hide();
}

//--------------------------------------------------------------------------
//	功能：初始化
//--------------------------------------------------------------------------
int KUiTradeConfirm::Initialize() {
    AddChild(&m_ItemName);
    AddChild(&m_BuyNumber);
    AddChild(&m_Increase);
    AddChild(&m_Decrease);
    AddChild(&m_Money);
    AddChild(&m_MoneyUnitText);
    AddChild(&m_BuyImg);
    AddChild(&m_SaleImg);
    AddChild(&m_RepairImg);
    AddChild(&m_OkBtn);
    AddChild(&m_CancelBtn);
    Wnd_AddWindow(this);

    char Scheme[256];
    g_UiBase.GetCurSchemePath(Scheme, 256);
    LoadScheme(Scheme);

    return m_bResourceReady;
}

//--------------------------------------------------------------------------
//	功能：载入窗口的界面方案
//--------------------------------------------------------------------------
void KUiTradeConfirm::LoadScheme(const char *pScheme) {
    char Buff[128];
    KIniFile Ini;
    sprintf(Buff, "%s\\%s", pScheme, SCHEME_INI);
    m_bResourceReady = false;
    BOOL loaded = Ini.Load(Buff);
    if (!loaded) loaded = Ini.LoadPakEntry(0x80247C8CUL);
    int maxLen = 0, width = 0, height = 0;
    if (loaded) {
        Ini.GetInteger("BuyNumber", "MaxLen", 0, &maxLen);
        Ini.GetInteger("BuyNumber", "Width", 0, &width);
        Ini.GetInteger("BuyNumber", "Height", 0, &height);
    }
    g_DebugLog("[PTTradeConfirm] resource=%s loaded=%d maxlen=%d size=%dx%d",
        Buff, loaded, maxLen, width, height);
    if (loaded && maxLen > 0 && maxLen < 32 && width > 0 && height > 0) {
        if (!Init(&Ini, "Main")) return;
        m_ItemName.Init(&Ini, "ItemName");

        m_Money.Init(&Ini, "Price");
        char moneySection[32];
        bool hasMoneyUnit = Ini.GetString("MoneyUnit", "Width", "", moneySection, sizeof(moneySection)) && moneySection[0];
        m_MoneyUnitText.Init(&Ini, hasMoneyUnit ? "MoneyUnit" : "TxtMoneyName");
        if (!m_BuyNumber.Init(&Ini, "BuyNumber")) return;
        m_Increase.Init(&Ini, "Increase");
        m_Decrease.Init(&Ini, "Decrease");
        Ini.GetString("Price", "Color", "", Buff, sizeof(Buff));
        m_uNormalPriceColor = GetColor(Buff);
        Ini.GetString("Price", "CantBuyColor", "", Buff, sizeof(Buff));
        m_uNotEnoughMoneyPriceColor = GetColor(Buff);

        m_BuyImg.Init(&Ini, "BuyImg");
        m_SaleImg.Init(&Ini, "SaleImg");
        m_RepairImg.Init(&Ini, "RepairImg");
        m_OkBtn.Init(&Ini, "OkBtn");
        m_CancelBtn.Init(&Ini, "CancelBtn");
        m_bResourceReady = true;
    }
}

//--------------------------------------------------------------------------
//	功能：窗口消息函数
//--------------------------------------------------------------------------
int KUiTradeConfirm::WndProc(unsigned int uMsg, unsigned int uParam, int nParam) {
    int nRet = 0;
    switch (uMsg) {
        case WND_N_BUTTON_CLICK:
            if (uParam == (unsigned int) (KWndWindow *) &m_OkBtn) {
                if (m_BuyImg.IsVisible())
                    OnBuy();
                else if (m_SaleImg.IsVisible())
                    OnSale();
                else
                    OnRepair();
            } else if (uParam == (unsigned int) (KWndWindow *) &m_CancelBtn)
                OnCancel();
            else if (uParam == (unsigned int) (KWndWindow *) &m_Increase)
                OnIncrease();
            else if (uParam == (unsigned int) (KWndWindow *) &m_Decrease)
                OnDecrease();
            break;
        case WM_KEYDOWN:
            if (uParam == VK_RETURN) {
                if (m_SaleImg.IsVisible()) {
                    OnSale();
                } else if (m_BuyImg.IsVisible()) {
                    if (m_BuyImg.IsDisable())
                        OnCancel();
                    else
                        OnBuy();
                } else if (m_RepairImg.IsDisable()) {
                    OnCancel();
                } else {
                    OnRepair();
                }
                nRet = 1;
            } else if (uParam == VK_ESCAPE) {
                OnCancel();
                nRet = 1;
            }
            break;
        case WND_N_EDIT_CHANGE:
            OnCheckInput();
            break;
        default:
            nRet = KWndImage::WndProc(uMsg, uParam, nParam);
    }
    return nRet;
}

//--------------------------------------------------------------------------
//	功能：响应点击买按钮
//--------------------------------------------------------------------------
void KUiTradeConfirm::OnBuy() {
    if(m_OkBtn.IsDisable())return;
    if(m_Action==TCA_QUEST_SHOP){PTEventShopData data;if(!g_pCoreShell||!g_pCoreShell->GetGameData(PT_EVENT_SHOP_GET,(unsigned int)&data,sizeof(data))||data.Token!=m_QuestToken){CloseWindow(false);return;}UpdateQuestShop();if(m_OkBtn.IsDisable())return;unsigned lots=m_BuyNumber.GetIntNumber();if(g_pCoreShell&&g_pCoreShell->OperationRequest(PT_EVENT_SHOP_PURCHASE,m_QuestRow.Id,lots))CloseWindow(false);return;}
    int nBuyNumber = m_BuyNumber.GetIntNumber();
    if (m_ItemInfo.Obj.uGenre == CGOG_NPCSELLITEM) {
        if (g_pCoreShell && nBuyNumber) {
            g_pCoreShell->OperationRequest(GOI_TRADE_NPC_BUY,
                                           (unsigned int) (&m_ItemInfo), nBuyNumber);
        }
    } else if (m_ItemInfo.Obj.uGenre == CGOG_PLAYERSELLITEM) {
        if (g_pCoreShell) {
            g_pCoreShell->OperationRequest(GOI_PLAYER_BUYTRADE,
                                           (unsigned int) (&m_ItemInfo), NULL);
        }
    }
    CloseWindow(false);
}

//--------------------------------------------------------------------------
//	功能：响应点击卖按钮
//--------------------------------------------------------------------------
void KUiTradeConfirm::OnSale() {
    if (g_pCoreShell) {
        g_pCoreShell->OperationRequest(GOI_TRADE_NPC_SELL,
                                       (unsigned int) (&m_ItemInfo), 1);
    }
    CloseWindow(false);
}

//--------------------------------------------------------------------------
//	功能：响应点修理按钮
//--------------------------------------------------------------------------
void KUiTradeConfirm::OnRepair() {
    if(m_OkBtn.IsDisable())return;
    if(m_Action==TCA_REPAIR_ALL){
        int quote=m_PriceInfo.nCurPrice;CloseWindow(false);
        KUiItem::RepairAllEquipped(quote);return;
    }
    if (g_pCoreShell) {
        g_pCoreShell->OperationRequest(GOI_TRADE_NPC_REPAIR,
                                       (unsigned int) (&m_ItemInfo), 0);
    }
    CloseWindow(false);
}


void KUiTradeConfirm::OnIncrease() {
    int nBuyNumber = m_BuyNumber.GetIntNumber();

    if(m_Action==TCA_QUEST_SHOP&&nBuyNumber>=(int)PTEventShopMaxLots(m_QuestRow.Count,m_QuestRow.UnitPrice))return;
    nBuyNumber++;

    m_BuyNumber.SetIntText(nBuyNumber);

    Wnd_SetFocusWnd(&m_pSelf->m_BuyNumber);
}


void KUiTradeConfirm::OnDecrease() {
    int nBuyNumber = m_BuyNumber.GetIntNumber();

    nBuyNumber--;

    m_BuyNumber.SetIntText(nBuyNumber);

    Wnd_SetFocusWnd(&m_pSelf->m_BuyNumber);
}


void KUiTradeConfirm::OnCheckInput() {
    // SetIntText synchronously sends WND_N_EDIT_CHANGE back to this window.
    if (!m_bResourceReady || m_bCheckingInput || m_Action==TCA_BUYPLAYER) return;
    m_bCheckingInput = true;
    int nBuyNumber = m_BuyNumber.GetIntNumber();

    if(m_Action==TCA_QUEST_SHOP){m_bCheckingInput=false;UpdateQuestShop();return;}
    if (m_BuyImg.IsVisible()) {
        if (m_PriceInfo.nCurPrice) {
            if (nBuyNumber > (m_nCurrentScore / m_PriceInfo.nCurPrice))
                nBuyNumber = (m_nCurrentScore / m_PriceInfo.nCurPrice);
        }
    }

    if (nBuyNumber < 0)
        nBuyNumber = 0;

    char szBuff1[16], szBuff2[16];
    itoa(nBuyNumber, szBuff1, 10);
    m_BuyNumber.GetText(szBuff2, sizeof(szBuff2), true);
    if (strcmp(szBuff1, szBuff2))
        m_BuyNumber.SetIntText(nBuyNumber);
    m_bCheckingInput = false;
}

//---------------------------------------
void KUiTradeConfirm::OnCancel() {
    CloseWindow(false);
}

void KUiTradeConfirm::Breathe() {
    if(m_Action==TCA_QUEST_SHOP){UpdateQuestShop();return;}
    int nBuyNumber = m_BuyNumber.GetIntNumber();
    // Player stalls quote a complete stack, independent of the NPC unit price.
    m_Money.SetIntText(m_Action==TCA_BUYPLAYER?m_PriceInfo.nCurPrice:m_PriceInfo.nCurPrice * nBuyNumber);
}

#include "UiQuestShopConfirm.inl"
