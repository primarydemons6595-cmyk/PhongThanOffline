bool KUiItem::HandleF4ResourceButton(unsigned int window)
{
    if (window == (unsigned int)(KWndWindow*)&m_PtF4.close)
    {
        if (g_UiBase.GetStatus() != UIS_S_TRADE_SETPRICE)
            CloseWindow(false);
        return true;
    }
    for (int page = 0; page < 4; ++page)
        if (window == (unsigned int)(KWndWindow*)&m_PageBtn[page])
        {
            SetInventoryPage(page);
            return true;
        }
    if(m_PtF4.IsRow(window)) {s_PtF4Unlock.Request(m_nInventoryPage);return true;}
    if(window==(unsigned int)(KWndWindow*)&m_PtF4.arrange) {
        if(g_pCoreShell && g_UiBase.GetStatus()==UIS_S_IDLE && !g_pCoreShell->GetTradeState())
            g_pCoreShell->OperationRequest(PT_GOI_F4,PT_F4_SORT,m_nInventoryPage);
        return true;
    }
    if(window==(unsigned int)(KWndWindow*)&m_PtF4.alchemy) {
        KPtAlchemy::Open();return true;
    }
    return false;
}
