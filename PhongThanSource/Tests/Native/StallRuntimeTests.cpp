#include "../../Headers/PhongThanStallMessages.h"
#include "KWin32.h"
#include "KIniFile.h"
#include "KFilePath.h"
#include "KPakFile.h"
#include <map>
#include <vector>
#include <string>
#include <stdio.h>
#include <assert.h>
#include "../../Headers/PhongThanStall.h"
enum {MAX_PLAYER=8,MAX_NPC=8,MAX_ITEM=32,MAX_SUBWORLD=4,MAX_TASK=5400,item_ibitem=8,pos_equiproom=1,pos_equiproomex=2,pos_ptbag2=3,pos_ptbag3=4,do_death=9,do_sit=10,SAVE_REQUEST=1,MESSAGE_SYSTEM_ANNOUCE_HEAD=1};
static DWORD seconds=1000;static DWORD KSG_GetCurSec(){return seconds;}
struct PlayerItem {int nIdx,nPlace,nX,nY;};
struct Lock {bool locked;bool IsLock(){return locked;}};
struct KItem {int id,genre,detail,part,expiry,price;bool lockedTrade,temp;Lock lock;
 int GetOwner(){return 102;}void SetOwner(DWORD){}int GetID(){return id;}int GetGenre(){return genre;}int GetParticular(){return part;}int GetDetailType(){return detail;}int GetExpireTime(){return expiry;}Lock* GetLock(){return &lock;}bool GetLockTrade(){return lockedTrade;}bool IsTemp(){return temp;}int GetTradePrice(){return price;}void SetTradePrice(int p){price=p;}int GetWidth(){return 1;}int GetHeight(){return 1;}
};static KItem Item[MAX_ITEM];
struct Inventory {PlayerItem m_Items[MAX_ITEM];int money;bool failAdd,failRemove,full;
 Inventory():money(0),failAdd(false),failRemove(false),full(false){ZeroMemory(m_Items,sizeof(m_Items));}
 int FindSame(int n){return n>0&&n<MAX_ITEM&&m_Items[n].nIdx==n?n:0;}
 int SearchID(int id){for(int i=1;i<MAX_ITEM;++i)if(m_Items[i].nIdx&&Item[i].id==id)return i;return 0;}
 int Hand(){return 0;}int GetEquipmentMoney(){return money;}
 bool CheckCanPlaceInEquipment(int,int,int* p,int* x,int* y){*p=pos_equiproom;*x=*y=0;return !full;}
 bool Remove(int n){if(failRemove||!FindSame(n))return false;m_Items[n].nIdx=0;return true;}
 bool RemoveItem(int n,int){if(!Remove(n))return false;Item[n].id=0;return true;}
 int Add(int n,int p,int x,int y,bool){if(failAdd)return 0;m_Items[n].nIdx=n;m_Items[n].nPlace=p;m_Items[n].nX=x;m_Items[n].nY=y;return n;}
};
struct Tasks {std::map<int,int> values;const char* GetSaveStr(int key){return values[key]?"used":"";}int GetSaveVal(int key){return values[key];}void SetSaveVal(int key,int value,bool){values[key]=value;}};
struct Shop {int nTrade;};
struct KNpc {int m_SubWorldIndex,m_CurrentLife,m_Doing,m_bRideHorse,m_FightMode;void SendCommand(int cmd){m_Doing=cmd;}};static KNpc Npc[MAX_NPC];
struct KPlayer {int m_PlayerDBLoad,m_nIndex,m_dwID,m_uMustSave;bool busy,locked,failEarn;Inventory m_ItemList;Tasks m_cTask;Shop m_PTrade;
 KPlayer():m_PlayerDBLoad(0),m_nIndex(0),m_dwID(0),m_uMustSave(0),busy(false),locked(false),failEarn(false){m_PTrade.nTrade=0;}
 bool CheckTrading(){return busy||m_PTrade.nTrade;}bool GetLockState(){return locked;}bool Pay(int n){if(m_ItemList.money<n)return false;m_ItemList.money-=n;return true;}bool Earn(int n){if(failEarn)return false;m_ItemList.money+=n;return true;}
 void SendTradeCancel(){m_PTrade.nTrade=0;}void SendTradeCount(int){if(!m_ItemList.FindSame(5))m_PTrade.nTrade=0;}void SendSystemMessage(int,const char*){}
};static KPlayer Player[MAX_PLAYER];
struct World {int m_SubWorldID;};static World SubWorld[MAX_SUBWORLD];
struct NpcSetType {int distance;int GetDistance(int,int){return distance;}};static NpcSetType NpcSet;
struct Entry {int nId,nTimes;DWORD dwExpireTime;};struct PHONGTHAN_IBBUFF_STORE {Entry Entry[48];};static PHONGTHAN_IBBUFF_STORE buffs[MAX_PLAYER];
static PHONGTHAN_IBBUFF_STORE* GetPlayerIBBuffStore(int p){return &buffs[p];}
static int FindIBBuffSlot(PHONGTHAN_IBBUFF_STORE* b,int id){for(int i=0;i<48;++i)if(b->Entry[i].nId==id&&b->Entry[i].nTimes>0&&b->Entry[i].dwExpireTime>seconds)return i;return -1;}
static bool AddIBBuffToStore(PHONGTHAN_IBBUFF_STORE* b,int,int id,int duration,int){for(int i=0;i<48;++i)if(!b->Entry[i].nId){b->Entry[i].nId=id;b->Entry[i].nTimes=1;b->Entry[i].dwExpireTime=seconds+duration;return true;}return false;}
static void PersistPlayerIBBuffStore(int){}
static bool PtPetCarried(int p,int n){return Player[p].m_ItemList.FindSame(n)&&Player[p].m_ItemList.m_Items[n].nPlace==pos_equiproom;}
#include "../../Sources/Core/Src/PhongThanStall.inl"
#include "../../Sources/Core/Src/PhongThanStallPurchase.inl"
static void Reset(){
 for(int p=0;p<MAX_PLAYER;++p){Player[p]=KPlayer();ZeroMemory(&buffs[p],sizeof(buffs[p]));}ZeroMemory(Item,sizeof(Item));ZeroMemory(Npc,sizeof(Npc));
 for(int role=1;role<=2;++role){Player[role].m_PlayerDBLoad=1;Player[role].m_nIndex=role;Player[role].m_dwID=100+role;Npc[role].m_CurrentLife=100;Npc[role].m_SubWorldIndex=0;}
 SubWorld[0].m_SubWorldID=1002;NpcSet.distance=64;seconds=1000;
 Player[1].m_ItemList.money=10000;Player[2].m_ItemList.money=200;Player[2].m_PTrade.nTrade=1;
 Item[5].id=55;Item[5].price=1000;Player[2].m_ItemList.Add(5,pos_equiproom,1,1,false);
}
static void Reload(){PtStall::checked=GetTickCount()-6000;}
static void Policy(const char* body){FILE* f=fopen("settings\\phongthan\\StallPolicy.ini","wb");assert(f);fputs(body,f);fclose(f);Reload();}
int main(int argc,char**argv){
 assert(argc==2&&SetCurrentDirectoryA(argv[1]));g_SetRootPath(NULL);g_SetFilePath("\\");g_SetPakFileMode(0);DeleteFileA("settings\\phongthan\\StallPolicy.ini");CreateDirectoryA("settings",0);CreateDirectoryA("settings\\phongthan",0);
 Reset();assert(PhongThanStallAllowed(1));SubWorld[0].m_SubWorldID=1003;assert(PhongThanStallAllowed(1));SubWorld[0].m_SubWorldID=1004;assert(PhongThanStallAllowed(1));
 SubWorld[0].m_SubWorldID=1005;assert(!PhongThanStallAllowed(1));SubWorld[0].m_SubWorldID=1021;assert(!PhongThanStallAllowed(1));AddIBBuffToStore(&buffs[1],1,38,172800,0);assert(PhongThanStallAllowed(1));SubWorld[0].m_SubWorldID=1020;assert(!PhongThanStallAllowed(1));
 SubWorld[0].m_SubWorldID=1021;seconds+=172800;assert(!PhongThanStallAllowed(1));
 Reset();SubWorld[0].m_SubWorldID=1067;AddIBBuffToStore(&buffs[1],1,129,518400,0);assert(PhongThanStallAllowed(1));SubWorld[0].m_SubWorldID=1068;assert(!PhongThanStallAllowed(1));
 Reset();SubWorld[0].m_SubWorldID=1052;AddIBBuffToStore(&buffs[1],1,186,172800,0);assert(PhongThanStallAllowed(1));SubWorld[0].m_SubWorldID=1005;assert(!PhongThanStallAllowed(1));
 Reset();Item[6].id=66;Item[6].genre=8;Item[6].detail=38;Player[1].m_ItemList.Add(6,pos_equiproom,0,0,false);assert(PhongThanUseStallLicense(1,6)==1);assert(!Item[6].id&&buffs[1].Entry[0].dwExpireTime==173800);
 Item[6].id=66;Player[1].m_ItemList.Add(6,pos_equiproom,0,0,false);assert(PhongThanUseStallLicense(1,6)==0&&Item[6].id==66);
 Reset();Item[6].id=66;Item[6].genre=8;Item[6].detail=38;Player[1].m_ItemList.Add(6,pos_equiproom,0,0,false);Player[1].m_ItemList.failRemove=true;assert(PhongThanUseStallLicense(1,6)==0&&!buffs[1].Entry[0].nId&&Item[6].id==66);
 Reset();Item[6].id=66;Item[6].genre=8;Item[6].detail=38;Player[1].m_ItemList.Add(6,pos_equiproom,0,0,false);for(int taskSlot=0;taskSlot<253;++taskSlot)Player[1].m_cTask.SetSaveVal(taskSlot,1,true);assert(PhongThanUseStallLicense(1,6)==0&&Item[6].id==66&&!buffs[1].Entry[0].nId);
 Reset();assert(PhongThanStallPurchase(1,2,5));assert(Player[1].m_ItemList.SearchID(55)==5&&!Player[2].m_ItemList.SearchID(55));assert(Player[1].m_ItemList.money==9000&&Player[2].m_ItemList.money==1200);assert(Player[2].m_cTask.GetSaveVal(5351)==1&&Player[2].m_cTask.GetSaveVal(5352)==1000);assert(!PhongThanStallPurchase(1,2,5));
 for(int failure=0;failure<11;++failure){Reset();switch(failure){case 0:Item[5].price=0;break;case 1:Item[5].lock.locked=true;break;case 2:Player[1].m_ItemList.full=true;break;case 3:Player[1].m_ItemList.money=999;break;case 4:NpcSet.distance=500;break;case 5:Item[5].expiry=999;break;case 6:Player[2].locked=true;break;case 7:Player[1].m_ItemList.failAdd=true;break;case 8:Player[2].failEarn=true;break;case 9:Player[2].m_ItemList.failRemove=true;break;case 10:Player[2].m_ItemList.m_Items[5].nPlace=9;break;}
 int money=Player[1].m_ItemList.money;assert(!PhongThanStallPurchase(1,2,5));assert(Player[2].m_ItemList.SearchID(55)==5&&!Player[1].m_ItemList.SearchID(55));assert(Player[1].m_ItemList.money==money&&Player[2].m_ItemList.money==200);assert(!Player[2].m_cTask.GetSaveVal(5351));}
 Reset();SubWorld[0].m_SubWorldID=1021;assert(!PhongThanStallPurchase(1,2,5)&&!Player[2].m_PTrade.nTrade);
 Policy("[Main]\nVersion=1\nCount=0\n[Maps]\n1021=2\n1002=0\n");Reset();assert(!PhongThanStallAllowed(1));SubWorld[0].m_SubWorldID=1021;assert(PhongThanStallAllowed(1));
 Policy("[Main]\nVersion=1\nCount=1\n[Maps]\n1005=1\n[License0]\nBuff=9500\nDetail=62000\nSeconds=7200\nEnabled=1\nMaps=1005\n");Reset();SubWorld[0].m_SubWorldID=1005;assert(!PhongThanStallAllowed(1));AddIBBuffToStore(&buffs[1],1,9500,7200,0);assert(PhongThanStallAllowed(1));Npc[1].m_bRideHorse=1;assert(!PhongThanStallAllowed(1));Npc[1].m_bRideHorse=0;Npc[1].m_FightMode=1;assert(!PhongThanStallAllowed(1));
 Policy("[Main]\nVersion=1\nCount=1\n[License0]\nBuff=1\nDetail=1\nSeconds=7200\nEnabled=1\nMaps=1002\n");Npc[1].m_FightMode=0;assert(PhongThanStallAllowed(1));
 Reset();SubWorld[0].m_SubWorldID=1005;Player[1].m_PTrade.nTrade=1;AddIBBuffToStore(&buffs[1],1,9500,1,0);PhongThanStallTick(1);assert(Npc[1].m_Doing==do_sit);++seconds;PhongThanStallTick(1);assert(!Player[1].m_PTrade.nTrade);
 assert(PTStallResumeExpiry(173800,1000,1000000)==1172800&&PTStallResumeExpiry(999,1000,2000)==999&&PTStallResumeExpiry(173800,0,1000000)==173800);
 assert(PTStallLevel(0)==1&&PTStallLevel(9)==1&&PTStallLevel(10)==2&&PTStallLevel(40)==3&&PTStallLevel(810)==10);
 puts("PASS production stall map gates, permit use/rollback, expiry/pose, custom policy reload, settlement conservation, failure rollback and progression");return 0;
}
