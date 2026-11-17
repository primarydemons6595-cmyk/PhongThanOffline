#include <windows.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include "../../Headers/PhongThanPet.h"
enum{MAX_ITEM=20,MAX_OBJECT=8,MAX_ITEM_MAGICATTRIB=20,item_equip=0,item_materials=3,item_medicine=1,item_skillbook=7,equip_normal=0,equip_magic=1,Obj_Kind_Item=1,pos_equiproom=3,pos_equip=2,PLAYER_PICKUP_SERVER_DISTANCE=40000,PHONGTHAN_MSG_INVENTORY_PICKUP_REQUEST=1,PHONGTHAN_WIRE_FLAG_REQUEST=1,MESSAGE_BROADCAST_ANNOUCE_HEAD=0};
struct Header{int x;};struct PHONGTHAN_ITEM_PICKUP_REQUEST{Header Header;DWORD WorldObjectId;int DestinationContainer,DestinationX,DestinationY;};
void PhongThanInitializeWireHeader(Header*,int,int,int,int){}
struct Attr{int nAttribType;};struct Lock{bool locked;bool IsLock(){return locked;}};
struct ItemType{int genre,detail,quality,level,set,upgrade,param,durability;bool sellLock,tradeLock;Attr m_aryMagicAttrib[20];Lock lock;
 int GetGenre(){return genre;}int GetDetailType(){return detail;}int GetQuality(){return quality;}int GetLevel(){return level;}int GetSetID(){return set;}int GetParam(){return param;}int GetUpgradeState(){return upgrade;}Lock* GetLock(){return &lock;}bool GetLockSell(){return sellLock;}bool GetLockTrade(){return tradeLock;}int GetDurability(){return durability;}int GetMaxDurability(){return 100;}int GetRepairPrice(){return 10;}void SetDurability(int v){durability=v;}int GetWidth(){return 1;}int GetHeight(){return 1;}char*GetName(){return "loot";}}Item[MAX_ITEM];
int PhongThanForgeRecipe(int p){return p>0?p:0;}
struct ItemPos{int nPlace,nX,nY;};struct PlayerItem{int nIdx,nPlace;};
struct Bag{PlayerItem it;bool room;PlayerItem*GetFirstItem(){return it.nIdx?&it:0;}PlayerItem*GetNextItem(){return 0;}bool FindPickupPosition(int,ItemPos*p){p->nPlace=3;p->nX=2;p->nY=4;return room;}void SyncItemDurability(int){}};
static int picks=0,sales=0;
struct Actor{int m_nIndex;Bag m_ItemList;bool locked,trading;bool GetLockState(){return locked;}bool CheckTrading(){return trading;}bool Pay(int){return true;}bool ServerPickUpItem(BYTE*bytes){PHONGTHAN_ITEM_PICKUP_REQUEST*q=(PHONGTHAN_ITEM_PICKUP_REQUEST*)bytes;assert(q->DestinationContainer==3&&q->DestinationX==2&&q->DestinationY==4);++picks;return true;}}Player[3];
struct NpcType{int m_SubWorldIndex;bool IsAlive(){return true;}void GetMpsPos(int*x,int*y){*x=*y=1000;}}Npc[3];
struct Obj{DWORD m_nID;int m_nRegionIdx,m_nSubWorldID,m_nKind,m_nItemDataID,x,y;void GetMpsPos(int*a,int*b){*a=x;*b=y;}}Object[MAX_OBJECT];typedef Obj KObj;
struct PT_PET_RUNTIME{bool Journal;PT_PET_DATA Data;DWORD ChatTick,AutomationTick;int PickupCursor,Dirty;};
struct KIniFile{bool Load(char*){return false;}void GetInteger(char*,char*,int,int*){}void GetString(char*,char*,char*,char*,int){}};
struct KPlayerChat{static void SendSystemInfo(int,int,int,char*,int,int){}};
int g_Random(int){return 0;}int PtPetSetting(const char*,const char*key,int def){return def;}void PtPetLog(PT_PET_DATA&,int,int){}
struct Shop{bool Sell(int,int,int,int){++sales;return true;}}BuySell;
int PtPetNearbyObjects(int,int* result,int){result[0]=1;return 1;}
#include "../../Sources/Core/Src/PhongThanPetAutomation.inl"
static PT_PET_RUNTIME Reset(){ZeroMemory(Player,sizeof(Player));ZeroMemory(Item,sizeof(Item));ZeroMemory(Object,sizeof(Object));picks=sales=0;Player[1].m_nIndex=1;Player[1].m_ItemList.room=true;Item[1].genre=0;Item[1].detail=0;Player[1].m_ItemList.it.nIdx=1;Player[1].m_ItemList.it.nPlace=3;Object[1].m_nID=10;Object[1].m_nKind=1;Object[1].m_nItemDataID=1;Object[1].x=1180;Object[1].y=1000;PT_PET_RUNTIME r;ZeroMemory(&r,sizeof(r));r.Data.Owned=1;r.Data.AgeSeconds=20000;r.Data.Strength=200000;r.Data.Flags=PT_PET_PICK;r.AutomationTick=GetTickCount();return r;}
int main(){PT_PET_RUNTIME r=Reset();PtPetAutomate(1,&r);assert(picks==1&&r.Data.Strength==199996);
 r=Reset();Player[1].m_ItemList.room=false;PtPetAutomate(1,&r);assert(!picks&&r.Data.Strength==200000);
 r=Reset();r.Data.PickExclude=1;PtPetAutomate(1,&r);assert(!picks);r.Data.PickExclude=0;r.Data.AgeSeconds=7199;PtPetAutomate(1,&r);assert(!picks);r.Data.AgeSeconds=7200;r.Data.Strength=104399;PtPetAutomate(1,&r);assert(!picks);
 r=Reset();r.Data.Flags|=PT_PET_REST;PtPetAutomate(1,&r);assert(!picks);r=Reset();Player[1].trading=true;PtPetAutomate(1,&r);assert(!picks);
 for(int detail=0;detail<14;++detail){r=Reset();Item[1].detail=detail;r.Data.Flags=PT_PET_SELL;r.AutomationTick=GetTickCount()-5001;PtPetAutomate(1,&r);assert(sales==(detail==0||detail==1||detail==2||detail==3||detail==5||detail==6||detail==7||detail==8));}
 r=Reset();r.Data.SaleKeep=1;assert(PtPetSaleKeep(1,r.Data));Item[1].quality=1;r.Data.SaleKeep=2;assert(PtPetSaleKeep(1,r.Data));r.Data.SaleKeep=0;assert(!PtPetSaleKeep(1,r.Data));Item[1].set=3;assert(PtPetSaleKeep(1,r.Data));Item[1].set=0;Item[1].upgrade=1;assert(PtPetSaleKeep(1,r.Data));
 puts("PASS actual pet automation: valid destination, pickup at 180px, full bag/filters/age/strength/rest/trade, family whitelist protects fabao/wings/mount/jade/seal/instrument, independent white/blue keep, set/upgrade protection");return 0;}
