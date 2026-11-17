#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../Sources/Core/Src/PhongThanRpsRules.h"
typedef unsigned char BYTE;
enum { MAX_PLAYER=4, room_trade=1 };
#define MESSAGE_SYSTEM_ANNOUCE_HEAD ""
struct TRADE_DECISION_COMMAND { BYTE m_btDecision,m_btFolkGame; };
struct ItemInfo { int m_nIdx; };
struct Items { ItemInfo m_psItemInfo[1]; } ItemSet;
struct ItemList {
    int stake,items;
    int GetMoney(int) { return stake; }
    void GetTradeRoomItemInfo() { ItemSet.m_psItemInfo[0].m_nIdx=items; }
};
struct KTrade {
    bool m_bTradeFolkGame;
    int m_nTradeDest,m_nTradeLock,m_nTradeFolkId,m_nTradeState;
};
struct KPlayer {
    KTrade m_cTrade; ItemList m_ItemList;
    int m_nPlayerIndex,sync,cancel,legacy,rounds; bool active;
    bool FinishPtRpsRound(int) {
        ++rounds;
        for(int side=1;side<=2;++side) {
            PlayerReset(side);
        }
        return true;
    }
    void PlayerReset(int);
    bool CheckTrading(bool) { return active; }
    void SyncTradeState() { ++sync; }
    void TradeDecision(BYTE*);
} Player[MAX_PLAYER];
void KPlayer::PlayerReset(int side) {
    Player[side].m_cTrade.m_nTradeFolkId=-1;
    Player[side].m_cTrade.m_nTradeLock=Player[side].m_cTrade.m_nTradeState=0;
}
static int texts=0;
struct KPlayerChat {
 static void SendSystemInfo(int,int,const char*,char*,int) { ++texts; }
};
void g_DebugLog(const char*,...) {}
void KPlayer::TradeDecision(BYTE* protocol)
{
    if(!active) return;
    TRADE_DECISION_COMMAND* pDecision=(TRADE_DECISION_COMMAND*)protocol;
    #include "../../Sources/Core/Src/PhongThanRpsDecision.inl"
    if(pDecision->m_btDecision==0) {
        ++cancel; active=false; Player[m_cTrade.m_nTradeDest].active=false;return;
    }
    ++legacy;
}
static int checks=0;
void Check(bool good,const char* label) {
    ++checks; if(!good) { printf("FAIL: %s\n",label);exit(1); }
}
void Reset() {
    memset(Player,0,sizeof(Player)); texts=0;
    for(int i=1;i<=2;++i) {
        Player[i].m_nPlayerIndex=i; Player[i].active=true;
        Player[i].m_cTrade.m_bTradeFolkGame=true;
        Player[i].m_cTrade.m_nTradeDest=3-i;
        Player[i].m_cTrade.m_nTradeLock=1;
        Player[i].m_cTrade.m_nTradeFolkId=-1;
    }
}
void Send(int player,int decision,int choice=0) {
    TRADE_DECISION_COMMAND command;command.m_btDecision=decision;command.m_btFolkGame=choice;
    Player[player].TradeDecision((BYTE*)&command);
}
int main() {
    const int results[3][3]={{0,-1,1},{1,0,-1},{-1,1,0}};
    for(int a=0;a<3;++a) for(int b=0;b<3;++b) {
        Check(PtRpsResult(a,b)==results[a][b],"all nine hand outcomes");
        Reset();Send(1,3,a);
        Check(Player[1].active&&Player[1].m_cTrade.m_nTradeState==1&&!texts,"first hand waits privately");
        Send(2,3,b);
        Check(Player[1].active&&Player[2].active&&texts==2&&Player[2].rounds==1&&!Player[2].cancel,"both results and session retained");
        Check(!Player[1].legacy&&!Player[2].legacy,"never enters wager settlement");
    }
    Reset();Send(1,3,3);Check(Player[1].m_cTrade.m_nTradeFolkId==-1&&!texts,"invalid hand rejected");
    Reset();Player[2].m_cTrade.m_nTradeLock=0;Send(1,3,1);
    Check(Player[1].m_cTrade.m_nTradeFolkId==-1,"both locks required");
    Reset();Send(1,3,1);Send(1,3,2);Send(1,2);
    Check(Player[1].m_cTrade.m_nTradeFolkId==1&&Player[1].m_cTrade.m_nTradeLock==1,"immutable committed hand");
    Reset();Send(1,3,1);Send(2,2);
    Check(Player[2].m_cTrade.m_nTradeLock==1,"peer cannot unlock after commit");
    Reset();Player[1].m_ItemList.stake=12;Send(1,3,0);Send(2,3,1);
    Check(Player[2].rounds==1&&!Player[1].cancel&&!Player[1].legacy,"money stake uses new settlement only");
    Reset();Player[2].m_ItemList.items=9;Send(1,3,0);Send(2,3,1);
    Check(Player[2].rounds==1&&!Player[1].cancel,"item stake uses new settlement only");
    Reset();Player[2].m_cTrade.m_nTradeDest=3;Send(1,3,0);
    Check(Player[1].m_cTrade.m_nTradeFolkId==-1,"reciprocal session required");
    Reset();Player[1].m_cTrade.m_nTradeDest=-1;Send(1,3,0);
    Check(!texts&&!Player[1].legacy,"negative peer rejected");
    Reset();Player[1].m_cTrade.m_nTradeDest=MAX_PLAYER;Send(1,3,0);
    Check(!texts&&!Player[1].legacy,"out of bounds peer rejected");
    Reset();Send(1,1);Check(!Player[1].legacy&&Player[1].active,"legacy decision blocked in RPS");
    Reset();Send(1,0);Check(Player[1].cancel==1,"cancel remains available");
    Reset();Player[1].m_cTrade.m_bTradeFolkGame=false;Send(1,1);
    Check(Player[1].legacy==1,"normal trade unaffected");
    printf("PASS: %d RPS checks\n",checks);return 0;
}
