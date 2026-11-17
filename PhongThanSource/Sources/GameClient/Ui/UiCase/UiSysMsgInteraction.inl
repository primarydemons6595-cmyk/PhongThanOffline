// Resource-anchored icon layout. Empty queues must not intercept live icons.
void KUiSysMsgCentre::SyncMessageButtons()
{
    int width = 0, height = 0;
    m_MsgIconBtn[0].GetSize(&width, &height);
    int occupied = 0;
    for (int i = 0; i < SMT_WAR; ++i)
    {
        if (!m_MsgHeap[i].nNumValid) continue;
        m_MsgIconBtn[i].SetPosition(m_nIconAnchorX - occupied * width, m_nIconAnchorY);
        m_MsgIconBtn[i].Enable(true);
        m_MsgIconBtn[i].Show();
        ++occupied;
    }
    for (int empty = 0; empty < SMT_WAR; ++empty)
    {
        if (m_MsgHeap[empty].nNumValid) continue;
        // Keep an empty destination for the next incoming icon animation.
        m_MsgIconBtn[empty].SetPosition(m_nIconAnchorX - occupied * width, m_nIconAnchorY);
        m_MsgIconBtn[empty].Enable(false);
        m_MsgIconBtn[empty].Hide();
    }
}
