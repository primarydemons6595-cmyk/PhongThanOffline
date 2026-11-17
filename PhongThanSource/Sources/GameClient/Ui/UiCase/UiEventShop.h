#pragma once
#include "../Elem/WndShowAnimate.h"
#include "../Elem/WndLabeledButton.h"
#include "../Elem/WndText.h"
#include "../Elem/WndEdit.h"
#include "../../../../Headers/PhongThanEventShop.h"
class KUiEventShop : public KWndShowAnimate {
 class ShopButton : public KWndLabeledButton {public:void PaintWindow();};
 class ItemIcon : public KWndButton {
 public:unsigned int goodsId;void SetItem(const PTEventShopRow&);int WndProc(unsigned int,unsigned int,int);
 } m_Icons[6];
 KWndText256 m_Names[6],m_Prices[6],m_Info;
 KWndText80 m_Title,m_PageText;
 ShopButton m_Buy[6],m_Previous,m_Next,m_Close;
 class ConfirmPane : public KWndImage {public:KUiEventShop* owner;void PaintWindow();int WndProc(unsigned int,unsigned int,int);} m_Confirm;
 KWndText256 m_ConfirmTitle,m_ConfirmName,m_ConfirmPrice,m_ConfirmHint,m_QuantityLabel;
 KWndEdit32 m_Quantity;ShopButton m_ConfirmBuy,m_Cancel,m_Plus,m_Minus;
 unsigned int m_Selected;bool m_Initializing;
 PTEventShopData m_Data;int m_Page;bool m_Pending;static KUiEventShop* s_Window;
 bool Initialize();void Update(bool);void Dismiss();
 void SelectGoods(unsigned int);void UpdateConfirmation();void CancelConfirmation();
 void InitConfirmation(KIniFile&);bool ConfirmClick(unsigned int);
 void PaintWindow();void Breathe();
public:static void Open();int WndProc(unsigned int,unsigned int,int);
};
