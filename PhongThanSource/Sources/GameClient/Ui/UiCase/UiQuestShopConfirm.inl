KUiTradeConfirm* KUiTradeConfirm::OpenQuestShop(const PTEventShopRow& row,unsigned token){
 KUiObjAtContRegion item;KUiItemBuySelInfo price;memset(&item,0,sizeof(item));memset(&price,0,sizeof(price));lstrcpynA(price.szItemName,row.Name,sizeof(price.szItemName));
 KUiTradeConfirm* window=OpenWindow(&item,&price,TCA_BUY);if(!window)return 0;window->m_Action=TCA_QUEST_SHOP;window->m_QuestRow=row;window->m_QuestToken=token;window->m_BuyNumber.SetIntText(1);window->UpdateQuestShop();return window;
}
void KUiTradeConfirm::UpdateQuestShop(){
 PTEventShopData data;if(!g_pCoreShell||!g_pCoreShell->GetGameData(PT_EVENT_SHOP_GET,(unsigned int)&data,sizeof(data))||data.Token!=m_QuestToken){CloseWindow(false);return;}
 unsigned lots=m_BuyNumber.GetIntNumber(),max=PTEventShopMaxLots(m_QuestRow.Count,m_QuestRow.UnitPrice);if(lots<1||lots>max){m_OkBtn.Enable(false);return;}
 m_OkBtn.Enable(true);m_Increase.Enable(lots<max);m_Decrease.Enable(lots>1);char name[120];sprintf(name,"%s x%u",m_QuestRow.Name,m_QuestRow.Count);m_ItemName.SetText(name);m_Money.SetIntText(m_QuestRow.UnitPrice*lots);m_MoneyUnitText.SetText(m_QuestRow.PaymentName);m_Money.SetTextColor(m_uNormalPriceColor);
}
