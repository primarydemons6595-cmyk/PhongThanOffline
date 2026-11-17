bool KUiTrade::LoadRpsScheme(KIniFile* ini)
{
    if (!m_bTradeFolkGame) {
        m_RpsImage[0].Hide(); m_RpsImage[1].Hide();
        return false;
    }
    // Original RPS has exactly five slots per side. All coordinates,
    // money edits and choice controls come from its own original scheme.
    m_PtClose.Hide();
    Init(ini, "Main");
    m_OtherName.Init(ini, "OtherName");
    m_OtherName.SetText(m_OtherData.Name);
    m_Info.Init(ini, "TxtInfo");
    m_Ok.Init(ini, "BtnLock");
    m_Trade.Init(ini, "BtnConfirm");
    m_Cancel.Init(ini, "BtnClose");
    const char* sections[] = { "BtnForfex", "BtnStone", "BtnCloth" };
    for (int i = 0; i < 3; ++i) m_RpsChoice[i].Init(ini, sections[i]);
    const char* leftKeys[] = { "SelfLeft", "OtherLeft" };
    const char* topKeys[] = { "SelfTop", "OtherTop" };
    for (int side = 0; side < 2; ++side)
    {
        m_RpsImage[side].Init(ini, "ChoiceImage");
        int left = 0, top = 0;
        ini->GetInteger("ChoiceImage", leftKeys[side], 0, &left);
        ini->GetInteger("ChoiceImage", topKeys[side], 0, &top);
        m_RpsImage[side].SetPosition(left, top);
        m_RpsImage[side].SetFrame(3);
        m_RpsImage[side].Hide();
    }
    // PT has a separate F4 inventory, not the embedded JX take-with panel.
    m_TakewithMoney.Hide(); m_TakewithItemsBox.Hide();
    m_SelfItemsBox.Init(ini, "SelfItemsBox");
    m_OtherItemsBox.Init(ini, "OtherItemsBox");
    m_SelfMoney.Init(ini, "SelfMoney");
    m_OtherMoney.Init(ini, "OtherMoney");
    m_SelfItemsBox.EnableTracePutPos(true);
    m_OtherItemsBox.EnablePickPut(false);
    m_AddMoney.Hide(); m_ReduceMoney.Hide();
    m_Info.SetText("\x43\x68\xE4\x6E\x20\x42\xF3\x61\x2F\x4B\xD0\x6F\x2F\x42\x61\x6F\x2E", -1);
    return true;
}

bool KUiTrade::UpdateRpsState()
{
    if (!m_bTradeFolkGame) return false;
    int locked = g_pCoreShell->GetGameData(GDI_TRADE_OPER_DATA, UTOD_IS_LOCKED, 0);
    int otherLocked = g_pCoreShell->GetGameData(GDI_TRADE_OPER_DATA, UTOD_IS_OTHER_LOCKED, 0);
    int committed = g_pCoreShell->GetGameData(GDI_TRADE_OPER_DATA, UTOD_IS_TRADING, 0);
    int otherCommitted = g_pCoreShell->GetGameData(GDI_TRADE_OPER_DATA, UTOD_IS_OTHER_TRADING, 0);
    // Original ChoiceImage frames verified: scissors=0, rock=1, paper=2, unknown=3.
    // The peer's secret hand is not transmitted by the existing lock packet.
    // Keep result pictures visible, but always reset interaction controls.
    if (!m_bShowRpsResult) {
    if (locked && m_nRpsChoice >= 0 && m_nRpsChoice < 3) {
        m_RpsImage[0].SetFrame(m_nRpsChoice); m_RpsImage[0].Show();
    } else m_RpsImage[0].Hide();
    if (otherLocked) {
        m_RpsImage[1].SetFrame(3); m_RpsImage[1].Show();
    } else m_RpsImage[1].Hide();
    }
    m_SelfItemsBox.EnablePickPut(!locked && !committed && !otherCommitted);
    m_SelfMoney.Enable(!locked && !committed && !otherCommitted);
    m_Ok.CheckButton(locked);
    m_Ok.Enable(m_nRpsChoice >= 0 && !committed && !otherCommitted);
    m_Trade.Enable(locked && otherLocked && !committed && m_nRpsChoice >= 0);
    m_Trade.CheckButton(committed);
    for (int i = 0; i < 3; ++i)
    {
        m_RpsChoice[i].Enable(!locked && !committed && !otherCommitted);
        m_RpsChoice[i].CheckButton(i == m_nRpsChoice);
    }
    return true;
}

bool KUiTrade::HandleRpsButton(unsigned int control)
{
    if (!m_bTradeFolkGame) return false;
    for (int i = 0; i < 3; ++i)
        if (control == (unsigned int)(KWndWindow*)&m_RpsChoice[i])
        {
            m_bShowRpsResult = false;
            if (!g_pCoreShell->GetGameData(GDI_TRADE_OPER_DATA, UTOD_IS_LOCKED, 0))
                m_nRpsChoice = i;
            UpdateRpsState();
            return true;
        }
    return false;
}
