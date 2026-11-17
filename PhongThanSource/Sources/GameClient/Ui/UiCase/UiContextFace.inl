void KUiPlayerBar::OpenContextFaceSelector(const char* recipient)
{
    if (!m_pSelf || !recipient || !recipient[0]) return;
    InputNameMsg(false, recipient, true);
    KUiFaceSelector* selector = KUiFaceSelector::OpenWindow(m_pSelf, 0);
    if (selector)
    {
        int left, top, width, height;
        m_pSelf->m_InputEdit.GetAbsolutePos(&left, &top);
        selector->GetSize(&width, &height);
        selector->SetPosition(left, top - height);
    }
}
