// Server TradeDecision only, before the unsafe JX guessing branch.
if (m_cTrade.m_bTradeFolkGame && pDecision->m_btDecision != 0)
{
    int dest = m_cTrade.m_nTradeDest;
    if (dest <= 0 || dest >= MAX_PLAYER || dest == m_nPlayerIndex ||
        !Player[dest].CheckTrading(true) ||
        Player[dest].m_cTrade.m_nTradeDest != m_nPlayerIndex ||
        !Player[dest].m_cTrade.m_bTradeFolkGame)
        return;
    KTrade& other = Player[dest].m_cTrade;
    if (pDecision->m_btDecision == 2)
    {
        if (m_cTrade.m_nTradeFolkId != -1 || other.m_nTradeFolkId != -1)
            return;
        m_cTrade.m_nTradeLock = !m_cTrade.m_nTradeLock;
        SyncTradeState();
        return;
    }
    if (pDecision->m_btDecision != 3 ||
        !PtRpsValid(pDecision->m_btFolkGame) ||
        m_cTrade.m_nTradeLock != 1 || other.m_nTradeLock != 1 ||
        m_cTrade.m_nTradeFolkId != -1)
        return;
    m_cTrade.m_nTradeFolkId = pDecision->m_btFolkGame;
    m_cTrade.m_nTradeState = 1;
    if (other.m_nTradeFolkId == -1) { SyncTradeState(); return; }
    int result = PtRpsResult(m_cTrade.m_nTradeFolkId, other.m_nTradeFolkId);
    const char* messages[] = {
        "\x4F\xBC\x6E\x20\x74\xEF\x20\x74\xD7\x3A\x20\x42\xB9\x6E\x20\x74\x68\x75\x61\x2E",
        "\x4F\xBC\x6E\x20\x74\xEF\x20\x74\xD7\x3A\x20\x48\xDF\x61\x2E",
        "\x4F\xBC\x6E\x20\x74\xEF\x20\x74\xD7\x3A\x20\x42\xB9\x6E\x20\x74\x68\xBE\x6E\x67\x2E"
    };
    if (result < -1 || result > 1) return;
    if (!FinishPtRpsRound(result)) {
        const char* failure = "OTT: Kh«ng ®ñ chç trèng hoÆc vËt phÈm/b¹c kh«ng hîp lÖ. §· hoµn ®å.";
        KPlayerChat::SendSystemInfo(1, m_nPlayerIndex, MESSAGE_SYSTEM_ANNOUCE_HEAD,
            (char*)failure, strlen(failure));
        KPlayerChat::SendSystemInfo(1, dest, MESSAGE_SYSTEM_ANNOUCE_HEAD,
            (char*)failure, strlen(failure));
        return;
    }
    const char* selfText = messages[result + 1];
    const char* otherText = messages[1 - result];
    KPlayerChat::SendSystemInfo(1, m_nPlayerIndex, MESSAGE_SYSTEM_ANNOUCE_HEAD,
        (char*)selfText, strlen(selfText));
    KPlayerChat::SendSystemInfo(1, dest, MESSAGE_SYSTEM_ANNOUCE_HEAD,
        (char*)otherText, strlen(otherText));
    g_DebugLog("PT_RPS_RESULT self=%d other=%d result=%d session_kept=1",
        m_nPlayerIndex, dest, result);
    return;
}
