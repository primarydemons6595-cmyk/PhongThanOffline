#include <stdio.h>
typedef unsigned char BYTE;
#include <stdlib.h>
#include <string.h>
enum { MAX_PLAYER=4, MAX_ITEM=20, MAX_PLAYER_ITEM=8, TRADE_ROOM_WIDTH=3,
    TRADE_ROOM_HEIGHT=1, room_equipment=0, room_trade=1, room_trade1=2,
    pos_equiproom=1, s2c_tradedecision=9 };
struct TRADE_ITEM_INFO { int m_nIdx,m_nWidth,m_nHeight,m_nX,m_nY,m_nPlace; };
struct PlayerItem { int nIdx,nPlace,nX,nY; };
struct TRADE_DECISION_SYNC { int ProtocolType,m_btDecision; };
struct Lock { bool locked; bool IsLock() { return locked; } };
struct FakeItem { bool bound,temp; Lock lock;
    bool GetLockTrade() { return bound; } bool IsTemp() { return temp; }
    Lock* GetLock() { return &lock; }
} Item[MAX_ITEM];
struct { TRADE_ITEM_INFO m_psItemInfo[3]; } ItemSet;
static int failAddAt=0,addCalls=0,markers=0,plannedPlace=pos_equiproom;
struct Server { void PackDataToClient(int,void*,int) { ++markers; } } server,*g_pServer=&server;
void g_DebugLog(const char*,...) {}
struct KItemList {
    PlayerItem m_Items[MAX_PLAYER_ITEM],backup[MAX_PLAYER_ITEM];
    int offer[3],money,stake,roomCapacity,snapshots;
    int GetMoney(int room) { return room==room_trade?stake:money; }
    int GetEquipmentMoney() { return money; }
    int FindSame(int item) { for(int i=1;i<MAX_PLAYER_ITEM;++i) if(m_Items[i].nIdx==item) return i; return 0; }
    void GetTradeRoomItemInfo() {
        memset(ItemSet.m_psItemInfo,0,sizeof(ItemSet.m_psItemInfo));
        for(int i=0;i<3;++i) ItemSet.m_psItemInfo[i].m_nIdx=offer[i];
    }
    void RecoverTrade() { memcpy(m_Items,backup,sizeof(backup)); }
    void ClearRoom(int room) { if(room==room_trade) { memset(offer,0,sizeof(offer));stake=0; } }
    bool TradeCheckCanPlace() {
        int occupied=0,count=0;
        for(int i=1;i<MAX_PLAYER_ITEM;++i) if(m_Items[i].nIdx) ++occupied;
        for(int j=0;j<3&&ItemSet.m_psItemInfo[j].m_nIdx;++j) {
            ++count;ItemSet.m_psItemInfo[j].m_nX=occupied+j;ItemSet.m_psItemInfo[j].m_nPlace=plannedPlace;
        }
        return occupied+count<=roomCapacity;
    }
    bool Remove(int item) { int index=FindSame(item);if(!index) return false;m_Items[index].nIdx=0;return true; }
    int Add(int item,int place,int x,int y,bool stack) {
        if(stack) abort();
        if(++addCalls==failAddAt) return 0;
        for(int i=1;i<MAX_PLAYER_ITEM;++i) if(!m_Items[i].nIdx) {
            m_Items[i].nIdx=item;m_Items[i].nPlace=place;m_Items[i].nX=x;m_Items[i].nY=y;return i;
        }
        return 0;
    }
    void SetRoomMoney(int,int value) { money=value; }
    void SendMoneySync() {}
    void StartTrade() { memcpy(backup,m_Items,sizeof(backup));ClearRoom(room_trade);++snapshots; }
};
struct KPlayer {
    KItemList m_ItemList;
    struct { int m_nTradeDest,m_nTradeFolkId,m_nTradeState,m_nTradeLock; } m_cTrade;
    int m_nPlayerIndex,m_nNetConnectIdx,sync;
    void SyncTradeState() { ++sync; }
    bool FinishPtRpsRound(int);
} Player[MAX_PLAYER];
static void PhongThanSendRpsRound(KPlayer&,int){++markers;}
#include "../../Sources/Core/Src/PhongThanRpsRound.inl"
int checks=0;
void Check(bool ok,const char* label) { ++checks;if(!ok) { printf("FAIL %s\n",label);exit(1); } }
void Reset() {
    memset(Player,0,sizeof(Player));memset(Item,0,sizeof(Item));
    failAddAt=addCalls=markers=0;plannedPlace=pos_equiproom;
    for(int i=1;i<=2;++i) {
        KPlayer& p=Player[i];p.m_nPlayerIndex=i;p.m_cTrade.m_nTradeDest=3-i;
        p.m_cTrade.m_nTradeLock=p.m_cTrade.m_nTradeState=1;p.m_cTrade.m_nTradeFolkId=1;
        p.m_ItemList.money=100;p.m_ItemList.stake=i*10;p.m_ItemList.roomCapacity=7;
        p.m_ItemList.m_Items[1].nIdx=i;p.m_ItemList.m_Items[1].nPlace=pos_equiproom;
        p.m_ItemList.m_Items[1].nX=i;p.m_ItemList.offer[0]=i;
        memcpy(p.m_ItemList.backup,p.m_ItemList.m_Items,sizeof(p.m_ItemList.backup));
        // Offered items are no longer occupying normal inventory cells.
        p.m_ItemList.m_Items[1].nPlace=2;
    }
}
int main() {
    Reset();Check(Player[1].FinishPtRpsRound(1),"winner settlement");
    Check(Player[1].m_ItemList.FindSame(1)&&Player[1].m_ItemList.FindSame(2)&&!Player[2].m_ItemList.FindSame(2),"only loser offer changes owner");
    Check(Player[1].m_ItemList.money==120&&Player[2].m_ItemList.money==80,"money conserved, loser charged once");
    Check(markers==4&&Player[1].m_cTrade.m_nTradeDest==2&&Player[1].m_cTrade.m_nTradeLock==0,"round markers retain session and unlock");
    Player[1].m_ItemList.RecoverTrade();Player[2].m_ItemList.RecoverTrade();
    Check(Player[1].m_ItemList.FindSame(2)&&!Player[2].m_ItemList.FindSame(2),"next cancellation cannot undo prior winnings");
    Reset();Check(Player[1].FinishPtRpsRound(-1)&&Player[2].m_ItemList.FindSame(1)&&Player[1].m_ItemList.money==90,"other player wins");
    Reset();for(int ties=0;ties<100;++ties){
        Check(Player[1].FinishPtRpsRound(0),"tie accepted");
        Check(Player[1].m_ItemList.m_Items[1].nPlace==2 && Player[2].m_ItemList.m_Items[1].nPlace==2,"tie retains both escrow items");
        Check(Player[1].m_ItemList.stake==10 && Player[2].m_ItemList.stake==20,"tie retains both money stakes");
        Check(Player[1].m_ItemList.snapshots==0 && Player[2].m_ItemList.snapshots==0,"tie never replaces cancellation backup");
        Check(!Player[1].m_cTrade.m_nTradeLock && !Player[2].m_cTrade.m_nTradeLock,"tie unlocks next round");
    }
    Check(Player[1].FinishPtRpsRound(1),"win after repeated ties");
    Check(Player[1].m_ItemList.FindSame(2) && !Player[2].m_ItemList.FindSame(2),"post-tie win transfers once");
    Reset();Player[1].m_ItemList.roomCapacity=1;Check(!Player[1].FinishPtRpsRound(1)&&Player[2].m_ItemList.FindSame(2)&&Player[1].m_ItemList.money==100,"full bag returns offers without transfer");
    Reset();Player[1].m_ItemList.money=2147483640;Check(!Player[1].FinishPtRpsRound(1)&&Player[2].m_ItemList.money==100,"money overflow rejected");
    Reset();Player[2].m_ItemList.stake=101;Check(!Player[1].FinishPtRpsRound(1)&&Player[2].m_ItemList.money==100,"unfunded stake rejected");
    Reset();Item[2].bound=true;Check(!Player[1].FinishPtRpsRound(1)&&Player[2].m_ItemList.FindSame(2),"bound item rejected");
    Reset();Item[2].temp=true;Check(!Player[1].FinishPtRpsRound(1),"temporary item rejected");
    Reset();Item[2].lock.locked=true;Check(!Player[1].FinishPtRpsRound(1),"locked item rejected");
    Reset();failAddAt=1;Check(!Player[1].FinishPtRpsRound(1)&&Player[2].m_ItemList.FindSame(2)&&!Player[1].m_ItemList.FindSame(2)&&Player[1].m_ItemList.money==100,"failed insertion rolls item back, no money transfer");
    Reset();Player[2].m_ItemList.offer[0]=MAX_ITEM;Check(!Player[1].FinishPtRpsRound(1),"invalid item index rejected before access");
    Reset();Player[2].m_ItemList.offer[0]=7;Check(!Player[1].FinishPtRpsRound(1),"unowned item rejected");
    Reset();Player[2].m_ItemList.offer[1]=3;
    Player[2].m_ItemList.backup[2].nIdx=3;Player[2].m_ItemList.backup[2].nPlace=pos_equiproom;
    failAddAt=2;
    Check(!Player[1].FinishPtRpsRound(1)&&Player[2].m_ItemList.FindSame(2)&&Player[2].m_ItemList.FindSame(3)&&!Player[1].m_ItemList.FindSame(2)&&!Player[1].m_ItemList.FindSame(3),"second insertion failure rolls entire batch back");
    Reset();Player[2].m_ItemList.stake=-1;Check(!Player[1].FinishPtRpsRound(1),"negative stake rejected");
    Reset();Player[2].m_ItemList.stake=100;Check(Player[1].FinishPtRpsRound(1)&&Player[2].m_ItemList.money==0&&Player[1].m_ItemList.money==200,"exact balance allowed, total conserved");
    Reset();for(int i=2;i<MAX_PLAYER_ITEM;++i) { Player[1].m_ItemList.backup[i].nIdx=5+i; }
    Check(!Player[1].FinishPtRpsRound(1)&&Player[2].m_ItemList.FindSame(2),"full item-index pool rejected");
    for(int page=1;page<=4;++page){Reset();plannedPlace=page;Check(Player[1].FinishPtRpsRound(1),"planned carried bag settlement");int slot=Player[1].m_ItemList.FindSame(2);Check(slot>0&&Player[1].m_ItemList.m_Items[slot].nPlace==page,"preserve four-bag planner destination");Player[1].m_ItemList.RecoverTrade();Check(Player[1].m_ItemList.m_Items[slot].nPlace==page,"later cancel preserves awarded bag");}
    printf("PASS %d production RPS settlement checks\n",checks);return 0;
}
