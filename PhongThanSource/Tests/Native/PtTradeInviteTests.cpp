#include <stdio.h>
// Avoid engine dependencies; these are the actual menu-state enum values.
#define KPLAYERMENUSTATE_H
enum { PLAYER_MENU_STATE_NORMAL, PLAYER_MENU_STATE_TEAMOPEN,
    PLAYER_MENU_STATE_TRADEOPEN, PLAYER_MENU_STATE_TRADING,
    PLAYER_MENU_STATE_IDLE, PLAYER_MENU_STATE_ENCHASE,
    PLAYER_MENU_STATE_VIEWSHOP, PLAYER_MENU_STATE_NUM };
#include "../../Sources/Core/Src/PhongThanTradeInvite.h"
int main()
{
    int checks=0;
    for (int state=-1; state<=PLAYER_MENU_STATE_NUM; ++state) {
        bool expected=state>=PLAYER_MENU_STATE_NORMAL && state<=PLAYER_MENU_STATE_TRADEOPEN;
        if (PhongThanCanReceiveTradeInvite(state)!=expected) return 1;
        ++checks;
    }
    printf("PASS %d invitation states: normal/team/open accepted, busy/invalid rejected\n",checks);
    return 0;
}
