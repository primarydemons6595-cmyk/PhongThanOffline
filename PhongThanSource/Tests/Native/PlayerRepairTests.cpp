#include <windows.h>
#include <stdio.h>
#include <string.h>
enum {MAX_ITEM=3,item_equip=0,s2c_msgshow=1,enumMSG_ID_ITEM_CANTREPAIR=2};
#define MSG_ITEM_REPAIRED "%d %s"
#define MESSAGE_SYSTEM_ANNOUCE_HEAD "game"
struct SHOW_MSG_SYNC {int ProtocolType,m_wMsgID,m_wLength;void* data;};
struct ItemMock {DWORD id;int dur,maxDur,cost;int GetGenre(){return item_equip;}int GetRepairPrice(){return dur>=maxDur?0:cost;}int GetMaxDurability(){return maxDur;}int GetDurability(){return dur;}void SetDurability(int n){dur=n;}const char* GetName(){return "owned weapon";}}Item[MAX_ITEM];
struct Bag {int synced;int SearchID(DWORD id){return id==Item[1].id?1:0;}void SyncItemDurability(int){++synced;}};
struct KPlayer {Bag m_ItemList;int m_nNetConnectIdx,m_nPlayerIndex,money,pays;int Pay(int cost){++pays;if(cost>money)return 0;money-=cost;return 1;}void RepairItem(DWORD);};
struct Server {int packets;void PackDataToClient(int,void*,int){++packets;}}server,*g_pServer=&server;
static int messages;
struct KPlayerChat{static void SendSystemInfo(int,int,const char*,char*,int){++messages;}};
#include "../../Output/LegacyDropNativeTest/RepairItemBody.inl"
int main(){KPlayer p;memset(&p,0,sizeof(p));p.m_nNetConnectIdx=-1;p.money=50;Item[1].id=123;Item[1].dur=20;Item[1].maxDur=100;Item[1].cost=10;
 p.RepairItem(123);if(p.money!=40||Item[1].dur!=100||p.m_ItemList.synced!=1||messages||p.pays!=1)return 1;
 Item[1].dur=0;p.RepairItem(123);if(p.money!=40||Item[1].dur||p.pays!=1)return 2;
 Item[1].dur=20;p.money=2;p.RepairItem(123);if(p.money!=2||Item[1].dur!=20||p.m_ItemList.synced!=1)return 3;
 p.money=50;p.m_nNetConnectIdx=3;p.RepairItem(123);if(p.money!=40||Item[1].dur!=100||messages!=1)return 4;
 p.RepairItem(999);if(p.money!=40)return 5;
 Item[1].dur=20;Item[1].cost=0;p.RepairItem(123);if(p.money!=40||Item[1].dur!=20)return 6;
 puts("PASS actual KPlayer repair: server bot charged once and repaired without client; human notification, insufficient money, unknown item, zero-durability original rule");return 0;}
