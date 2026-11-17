#ifndef PHONGTHAN_TRADE_INVITE_H
#define PHONGTHAN_TRADE_INVITE_H
#include "KPlayerMenuState.h"
// PT invitations do not require the old JX open-trade sign above the target.
// Other modal activities remain unavailable.
inline bool PhongThanCanReceiveTradeInvite(int state)
{
    return state == PLAYER_MENU_STATE_NORMAL ||
        state == PLAYER_MENU_STATE_TEAMOPEN ||
        state == PLAYER_MENU_STATE_TRADEOPEN;
}
#endif
