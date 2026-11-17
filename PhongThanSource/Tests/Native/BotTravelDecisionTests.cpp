// Exercise the production bot controller, including ordinary-use vs auto-dose.
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <vector>
#include "../../Headers/PhongThanTravelItemPolicy.h"
#include "../../Headers/PhongThanTravelDestinations.h"
#define CHECK(x) do {if(!(x)){printf("FAIL line %d: %s\n",__LINE__,#x);return 1;}++checks;}while(0)
enum {Capacity=1,MAX_ITEM=8,pos_equiproom=3,pos_equiproomex=12,pos_ptbag2=13,pos_ptbag3=14,pos_immediacy=4,do_stand=0};
enum {PHONGTHAN_MSG_UI_CHOICE_REQUEST=1,PHONGTHAN_MSG_INVENTORY_USE_REQUEST=2,PHONGTHAN_WIRE_FLAG_REQUEST=1};
struct Header {int unused;};
struct PHONGTHAN_ITEM_USE_REQUEST {struct Header Header;DWORD ItemId;int Container,SlotX,SlotY,Reserved;};
struct PHONGTHAN_UI_CHOICE_REQUEST {struct Header Header;int MapId;DWORD DialogToken;int View,Selection;};
static void PhongThanInitializeWireHeader(Header*,int,int,int,int){}
struct Lock {int nState;};
struct KItem {int detail,count,expiry;DWORD serial;Lock lock;int GetGenre(){return 8;}int GetDetailType(){return detail;}int GetParticular(){return 2;}int GetStackNum(){return count;}DWORD GetID(){return serial;}Lock* GetLock(){return &lock;}int GetExpireTime(){return expiry;}}Item[MAX_ITEM];
struct PlayerItem {int nIdx,nPlace,nX,nY;};
struct Bag {PlayerItem entry;PlayerItem* GetFirstItem(){return entry.nIdx?&entry:0;}PlayerItem* GetNextItem(){return 0;}int Hand(){return 0;}int SearchID(DWORD id){return entry.nIdx&&Item[entry.nIdx].serial==id?entry.nIdx:0;}};
struct Task {int val[850];int GetSaveVal(int n){return val[n];}};
static int uses=0,autoDoses=0,pageTurns=0,blocked=0,saves=0,selectedOption=0,checks=0;
struct World {int m_SubWorldID;}SubWorld[2];
struct KNpc {int m_SubWorldIndex,m_CurrentLife;void SendCommand(int){}}Npc[2];
struct KPlayer {int m_nIndex,page;DWORD m_UiDialogToken;int m_UiDialogView;Bag m_ItemList;Task m_cTask;
 int GetLockState(){return 0;}bool CheckTrading(){return false;}void Save(){++saves;}
 void EatItem(BYTE* raw){PHONGTHAN_ITEM_USE_REQUEST& q=*(PHONGTHAN_ITEM_USE_REQUEST*)raw;if(q.Reserved){++autoDoses;return;}++uses;if(blocked)return;m_UiDialogToken++;page=1;}
 void ProcessPlayerSelectFromUI(BYTE* raw){PHONGTHAN_UI_CHOICE_REQUEST& q=*(PHONGTHAN_UI_CHOICE_REQUEST*)raw;if(q.DialogToken!=m_UiDialogToken)return;
  if(q.Selection==1){page=2;++pageTurns;++m_UiDialogToken;return;}if(q.Selection==0){const PtTravelDestination& d=ptTravelDestinations[selectedOption-1];SubWorld[0].m_SubWorldID=d.map;--Item[2].count;m_UiDialogToken=0;}}
}Player[1];
struct Config {char Id[33];};struct Runtime {int Player;Config Config;char State[64];}bots[Capacity];
struct Training {std::vector<int> Path;}training[Capacity];
static bool BotBag(int p){return p==pos_equiproom||p==pos_equiproomex||p==pos_ptbag2||p==pos_ptbag3||p==pos_immediacy;}
static int KSG_GetCurSec(){return 1000;}
static void Log(const char*,const char*,const char*){}
static int QuestChoice(KPlayer& p,const char* callback){char exact[40];sprintf(exact,"pt_travel_%d",selectedOption);
 if(!strcmp(callback,exact)&&((selectedOption<=16&&p.page==1)||(selectedOption>16&&p.page==2)))return 0;
 if(!strcmp(callback,"pt_travel_page2")&&p.page==1)return 1;return -1;}
#include "../../Sources/Core/Src/PhongThanBotTravel.inl"
static void Reset(int detail=35,int place=pos_equiproom,int destination=1016){
 ZeroMemory(Player,sizeof(Player));ZeroMemory(Npc,sizeof(Npc));ZeroMemory(Item,sizeof(Item));bots[0].Player=0;botTravelJobs[0]=BotTravelJob();
 Player[0].m_nIndex=1;Player[0].m_ItemList.entry.nIdx=2;Player[0].m_ItemList.entry.nPlace=place;Player[0].m_ItemList.entry.nX=4;Player[0].m_ItemList.entry.nY=2;
 Npc[1].m_CurrentLife=100;Npc[1].m_SubWorldIndex=0;SubWorld[0].m_SubWorldID=1002;Item[2].serial=555;Item[2].count=10;Item[2].detail=detail;
 uses=autoDoses=pageTurns=blocked=saves=0;selectedOption=0;for(int n=0;n<PT_TRAVEL_DESTINATION_COUNT;++n)if(ptTravelDestinations[n].map==destination){selectedOption=n+1;break;}
}
int main(){
 int places[]={pos_equiproom,pos_equiproomex,pos_ptbag2,pos_ptbag3,pos_immediacy};
 for(int kind=0;kind<2;++kind)for(int at=0;at<5;++at){Reset(kind?159:35,places[at]);CHECK(BotHasTravelItem(Player[0],1016));CHECK(BotTryTravel(bots[0],1016,100));CHECK(uses==1&&autoDoses==0&&Item[2].count==10);CHECK(BotTryTravel(bots[0],1016,1600));CHECK(pageTurns==1&&SubWorld[0].m_SubWorldID==1002&&Item[2].count==10);CHECK(BotTryTravel(bots[0],1016,3100));CHECK(SubWorld[0].m_SubWorldID==1016&&Item[2].count==9&&saves==1);CHECK(!BotTryTravel(bots[0],1016,5000));CHECK(Item[2].count==9);}
 Reset(35,pos_equiproom,1001);CHECK(BotTryTravel(bots[0],1001,100));CHECK(BotTryTravel(bots[0],1001,1600));CHECK(!pageTurns&&SubWorld[0].m_SubWorldID==1001&&Item[2].count==9);
 Reset();blocked=1;Player[0].m_UiDialogToken=77;CHECK(!BotTryTravel(bots[0],1016,100));CHECK(BotHasTravelItem(Player[0],1016)&&Item[2].count==10);
 Reset();Item[2].lock.nState=1;CHECK(!BotHasTravelItem(Player[0],1016));CHECK(!BotTryTravel(bots[0],1016,100));CHECK(!uses);
 Reset();Item[2].expiry=999;CHECK(!BotHasTravelItem(Player[0],1016));CHECK(!BotTryTravel(bots[0],1016,100));CHECK(!uses);
 Reset();CHECK(BotTryTravel(bots[0],1016,100));Item[2].serial++;CHECK(BotTryTravel(bots[0],1016,1600));CHECK(Item[2].count==10&&pageTurns==0&&uses==2&&botTravelJobs[0].serial==Item[2].serial);
 Reset(119);CHECK(!BotHasTravelItem(Player[0],1016));CHECK(!BotTryTravel(bots[0],1016,100));CHECK(!uses);
 printf("PASS %d production bot travel checks: both Di Ngoai Phu variants, every bag, Tam Son page2, ordinary use, charges and failure guards\n",checks);return 0;
}
