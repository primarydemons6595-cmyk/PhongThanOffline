#include <stdio.h>
#include "../../Sources/Core/Src/PhongThanRpsRules.h"
#include <stdlib.h>
enum { room_trade,room_trade1,GDCNI_TRADE_END,GDCNI_CONTAINER_OBJECT_CHANGED,UOC_ITEM_TAKE_WITH };
struct TRADE_DECISION_SYNC { int m_btDecision; };
static int kept=0,refresh=0;
void CoreDataChanged(int type,int,int value) {
    if(type==PT_GDCNI_RPS_ROUND) { if(value!=2 && value!=8) abort();++kept; }
    else ++refresh;
}
struct Items {
    int recover,backup,clear,removed,mouse,money,bank;
    void RecoverTrade() { ++recover; }
    void SetMoney(int m,int b,int) { money=m;bank=b; }
    void ClearRoom(int) { ++clear; }
    void RemoveRoom(int) { ++removed; }
    void StartTrade() { ++backup; }
    int GetEquipmentMoney() { return money; }
    int GetRepositoryMoney() { return bank; }
    void MenuSetMouseItem() { ++mouse; }
};
struct Client {
    Items m_ItemList;
    struct Trade { bool m_bTradeFolkGame; int m_nBackEquipMoney,m_nBackRepositoryMoney,
        m_nTradeState,m_nTradeLock,m_nTradeDestState,m_nTradeDestLock; } m_cTrade;
    bool active; int legacy;
    bool CheckTrading(bool) { return active; }
    void Handle(int decision) {
        TRADE_DECISION_SYNC msg;msg.m_btDecision=decision;
        TRADE_DECISION_SYNC* pSync=&msg;
        #include "../../Sources/Core/Src/PhongThanRpsRoundClient.inl"
        ++legacy;
    }
};
int main() {
    Client c={0};c.active=true;c.m_cTrade.m_bTradeFolkGame=true;
    c.m_cTrade.m_nBackEquipMoney=100;c.m_cTrade.m_nBackRepositoryMoney=50;
    c.m_cTrade.m_nTradeLock=c.m_cTrade.m_nTradeState=1;
    c.Handle(2);
    if(c.m_ItemList.recover!=1||c.m_ItemList.money!=100||c.m_cTrade.m_nTradeLock||kept!=1||c.legacy) return 1;
    c.m_ItemList.money=120;c.Handle(3);
    if(c.m_ItemList.backup!=1||c.m_cTrade.m_nBackEquipMoney!=120||kept!=2||c.legacy) return 2;
    c.Handle(2);
    if(c.m_ItemList.money!=120) return 3;
    int recoverBefore=c.m_ItemList.recover,backupBefore=c.m_ItemList.backup;
    c.Handle(8);
    if(c.m_ItemList.recover!=recoverBefore || c.m_ItemList.backup!=backupBefore || kept!=4)return 5;
    c.m_cTrade.m_bTradeFolkGame=false;c.Handle(2);
    if(c.legacy!=1) return 4;
    c.m_cTrade.m_bTradeFolkGame=true;c.active=false;c.Handle(3);
    if(c.legacy!=2) return 5;
    printf("PASS 5 production client round checks\n");
    return 0;
}
