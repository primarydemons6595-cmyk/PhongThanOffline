// Called only after reciprocal session, both locks and both hands are verified.
// Return own offers first, then transfer only the loser's offer (never double Pay).
bool KPlayer::FinishPtRpsRound(int result)
{
    const int dest = m_cTrade.m_nTradeDest;
    if(result==0) {
        // A tie keeps both offers in escrow. Only choices/locks reset.
        // Tie fence shares native transport ordering with item updates.
        for(int side=0;side<2;++side) {
            KPlayer& p=Player[side?dest:m_nPlayerIndex];
            p.m_cTrade.m_nTradeFolkId=-1;p.m_cTrade.m_nTradeState=p.m_cTrade.m_nTradeLock=0;
            PhongThanSendRpsRound(p,8);
        }
        SyncTradeState();return true;
    }
    const int capacity = TRADE_ROOM_WIDTH * TRADE_ROOM_HEIGHT;
    int winner = result > 0 ? m_nPlayerIndex : dest;
    int loser = result > 0 ? dest : m_nPlayerIndex;
    TRADE_ITEM_INFO offers[TRADE_ROOM_WIDTH * TRADE_ROOM_HEIGHT];
    PlayerItem original[TRADE_ROOM_WIDTH * TRADE_ROOM_HEIGHT];
    memset(offers, 0, sizeof(offers));
    int money = result ? Player[loser].m_ItemList.GetMoney(room_trade) : 0;
    if (result) {
        Player[loser].m_ItemList.GetTradeRoomItemInfo();
        memcpy(offers, ItemSet.m_psItemInfo, sizeof(offers));
    }
    // Ordered marker: client restores its local trade backup BEFORE item deltas.
    // Restore fence is sent before any ownership delta.
    for (int side = 0; side < 2; ++side) {
        int id = side ? dest : m_nPlayerIndex;
        Player[id].m_ItemList.RecoverTrade();
        Player[id].m_ItemList.ClearRoom(room_trade);
        Player[id].m_ItemList.ClearRoom(room_trade1);
        PhongThanSendRpsRound(Player[id],2);
    }
    bool ok = true;
    int count = 0;
    if (result) {
        KItemList& win = Player[winner].m_ItemList;
        KItemList& lose = Player[loser].m_ItemList;
        int winMoney = win.GetMoney(room_equipment);
        int loseMoney = lose.GetMoney(room_equipment);
        ok = money >= 0 && loseMoney >= money && winMoney >= 0 &&
            (__int64)winMoney + money <= 2147483647;
        while (count < capacity && offers[count].m_nIdx) {
            int item = offers[count].m_nIdx;
            if (item <= 0 || item >= MAX_ITEM) { ok = false; break; }
            int index = lose.FindSame(item);
            if (index <= 0 || Item[item].IsTemp() ||
                Item[item].GetLockTrade() || Item[item].GetLock()->IsLock()) {
                ok = false; break;
            }
            original[count] = lose.m_Items[index];
            ++count;
        }
        int freeSlots = 0;
        for (int slot = 1; slot < MAX_PLAYER_ITEM; ++slot)
            if (!win.m_Items[slot].nIdx) ++freeSlots;
        if (freeSlots < count) ok = false;
        // Plan the entire incoming offer against the RESTORED winner inventory.
        // This includes the winner's returned items, so no occupied-cell overlap.
        memcpy(ItemSet.m_psItemInfo, offers, sizeof(offers));
        if (ok && !win.TradeCheckCanPlace()) ok = false;
        if (ok) memcpy(offers, ItemSet.m_psItemInfo, sizeof(offers));
        int moved = 0;
        if (ok) {
            for (; moved < count; ++moved) {
                int item = offers[moved].m_nIdx;
                if (!lose.Remove(item)) { ok = false; break; }
                if (!win.Add(item, offers[moved].m_nPlace, offers[moved].m_nX,
                             offers[moved].m_nY, false)) {
                    // Include the just-removed item in rollback below.
                    ++moved; ok = false; break;
                }
            }
        }
        if (!ok) {
            while (moved > 0) {
                --moved;
                int item = offers[moved].m_nIdx;
                if (win.FindSame(item)) win.Remove(item);
                if (!lose.FindSame(item) && !lose.Add(item, original[moved].nPlace,
                        original[moved].nX, original[moved].nY, false))
                    g_DebugLog("PT_RPS_ROLLBACK_FATAL player=%d item=%d", loser, item);
            }
        } else {
            // Prevalidated signed range; do not use CostMoney (may take bank money).
            lose.SetRoomMoney(room_equipment, loseMoney - money);
            win.SetRoomMoney(room_equipment, winMoney + money);
        }
    }
    // A round ends, not the invitation/session. A fresh snapshot is essential:
    // closing a later round must never undo an earlier completed round.
    // Final fence: 5 loss, 7 win; 3 failed settlement (returned offers).
    for (int next = 0; next < 2; ++next) {
        int id = next ? dest : m_nPlayerIndex;
        int decision=ok?6+(id==m_nPlayerIndex?result:-result):3;
        KPlayer& p = Player[id];
        p.m_cTrade.m_nTradeFolkId = -1;
        p.m_cTrade.m_nTradeState = p.m_cTrade.m_nTradeLock = 0;
        p.m_ItemList.StartTrade();
        p.m_ItemList.SendMoneySync();
        PhongThanSendRpsRound(p,decision);
    }
    SyncTradeState();
    g_DebugLog("PT_RPS_ROUND self=%d other=%d result=%d items=%d money=%d settled=%d session_kept=1",
        m_nPlayerIndex, dest, result, count, money, ok);
    return ok;
}
