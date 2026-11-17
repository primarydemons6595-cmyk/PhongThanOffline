// Extended trade-decision values, only valid inside a live PT RPS session.
if ((pSync->m_btDecision == 2 || pSync->m_btDecision == 3 ||
     (pSync->m_btDecision >= 5 && pSync->m_btDecision <= 8)) &&
    CheckTrading(true) && m_cTrade.m_bTradeFolkGame)
{
    if (pSync->m_btDecision == 2) {
        m_ItemList.RecoverTrade();
        m_ItemList.SetMoney(m_cTrade.m_nBackEquipMoney,
            m_cTrade.m_nBackRepositoryMoney, 0);
        m_ItemList.ClearRoom(room_trade);
        m_ItemList.RemoveRoom(room_trade1);
    } else if(pSync->m_btDecision!=8) {
        m_ItemList.StartTrade();
        m_cTrade.m_nBackEquipMoney = m_ItemList.GetEquipmentMoney();
        m_cTrade.m_nBackRepositoryMoney = m_ItemList.GetRepositoryMoney();
    }
    m_cTrade.m_nTradeState = m_cTrade.m_nTradeLock = 0;
    m_cTrade.m_nTradeDestState = m_cTrade.m_nTradeDestLock = 0;
    CoreDataChanged(PT_GDCNI_RPS_ROUND, 0, pSync->m_btDecision >= 5 ? pSync->m_btDecision : 2);
    CoreDataChanged(GDCNI_CONTAINER_OBJECT_CHANGED, UOC_ITEM_TAKE_WITH, 0);
    m_ItemList.MenuSetMouseItem();
    return;
}
