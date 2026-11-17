#include <windows.h>
#include <stdio.h>
#include <assert.h>
#include "KWin32.h"
#include "KDebug.h"
#include "KLuaScript.h"
#include <limits.h>
#include "../../Headers/PhongThanReservoirs.h"
#ifdef PT_TEST_GIFTS
#include "KFilePath.h"
#include "KPakFile.h"
static int GiftMaxStack(int,int,int);
#endif
enum {MAX_PLAYER=3,MAX_NPC=8,MAX_ITEM=128,MAX_TASK=5000,MAX_ITEM_MAGICLEVEL=20,
      pos_immediacy=22,pos_equiproom=3,pos_equiproomex=4,pos_ptbag2=19,pos_ptbag3=20,item_equip=0,item_medicine=1,item_materials=3,item_task=4,item_townportal=5,item_magicscript=6,item_skillbook=7,item_ibitem=8,equip_detailnum=16};
#include "../../Sources/Core/Src/PhongThanQuestItemTuple.h"
struct Lock {bool IsLock(){return false;}};
struct KItem {
    int GetParam(){return 0;}int GetExpireTime(){return 0;}

    int id,genre,detail,part,level,series,stack,maxStack;
    int GetID(){return id;} int GetGenre(){return genre;} int GetDetailType(){return detail;}
    int GetParticular(){return part;}int GetLevel(){return level;}int GetSeries(){return series;}
    int GetStackNum(){return stack;}int GetMaxStackNum(){return maxStack>0?maxStack:1;}int GetWidth(){return 1;}int GetHeight(){return 1;}
    bool IsStack(){return GetMaxStackNum()>1 && !PhongThanReservoirTooltip(genre,detail,part,GetMaxStackNum(),"NONE");}
    const char* GetScript(){return "NONE";}int GetExpirePoint(){return 0;}void SetExpireTime(int){}
    void SetStackNum(int value){stack=value;}Lock* GetLock(){static Lock lock;return &lock;}
} Item[MAX_ITEM];
struct ItemPos {int nPlace,nX,nY;};
struct PlayerItem {int nIdx,nPlace,nX,nY;};
struct KItemList {
    PlayerItem m_Items[MAX_ITEM];int capacity,cursor;bool failSearch;
    PlayerItem* GetFirstItem(){cursor=0;return GetNextItem();}
    PlayerItem* GetNextItem(){while(++cursor<MAX_ITEM)if(m_Items[cursor].nIdx)return &m_Items[cursor];return 0;}
    int FindSame(int index){return m_Items[index].nIdx?index:0;}
    bool Remove(int index){if(!FindSame(index))return false;m_Items[index].nIdx=0;return true;}
    void SyncItem(int){}
    bool SearchPosition(int,int,ItemPos* p,bool){
        if(failSearch)return false;
        for(int x=0;x<capacity;++x){bool used=false;for(int i=1;i<MAX_ITEM;++i)if(m_Items[i].nIdx && m_Items[i].nX==x)used=true;
            if(!used){p->nPlace=pos_equiproom;p->nX=x;p->nY=0;return true;}}
        return false;
    }
    int Add(int index,int place,int x,int y,bool){
        if(x<0 || x>=capacity || m_Items[index].nIdx)return 0;
        for(int i=1;i<MAX_ITEM;++i)if(m_Items[i].nIdx && m_Items[i].nX==x)return 0;
        m_Items[index].nIdx=index;m_Items[index].nPlace=place;m_Items[index].nX=x;m_Items[index].nY=y;return index;
    }
    int GetEquipmentMoney(){return 1000;}
    int Count(){int count=0;for(int i=1;i<MAX_ITEM;++i)if(m_Items[i].nIdx)++count;return count;}
};
struct Task {int value,slots[MAX_TASK];int GetSaveVal(int id){return id==20?value:slots[id];}void SetSaveVal(int id,int next,BOOL){if(id==20)value=next;else slots[id]=next;}};
struct PlayerMock {
    int m_nIndex,m_nLastNpcIndex,m_uMustSave;int ExecuteScript(const char*,const char*,int){return 1;}void* m_pStatusLoadPlayerInfo;bool locked,trading;Task m_cTask;KItemList m_ItemList;
    unsigned long m_dwID;int cash,xp;struct {int m_nFlag;} m_cTong;
    bool Earn(int money){if(money<0||cash>2000000000-money)return false;cash+=money;return true;}
    bool Pay(int money){if(money<0||cash<money)return false;cash-=money;return true;}
    void DirectAddExp(int amount){xp+=amount;}
    void SendSystemMessage(const char*,const char*){}
    bool GetLockState(){return locked;}bool CheckTrading(){return trading;}
} Player[MAX_PLAYER];
typedef PlayerMock KPlayer;
int GetPlayerIndex(Lua_State*){return 1;}
struct WorldSet {int GetGameVersion(){return 1;}int SearchWorld(int map); } g_SubWorldSet;
struct Items {
    int allocated,failAt;
    int Add(KItem* p){int i=Add(p->genre,p->series,p->level,0,p->detail,p->part,0,1);if(i){int id=Item[i].id;Item[i]=*p;Item[i].id=id;}return i;}
    int Add(int genre,int series,int level,int,int detail,int particular,int*,int){
        if(++allocated==failAt)return 0;
        for(int i=2;i<MAX_ITEM;++i)if(!Item[i].id){Item[i].id=i+100;Item[i].genre=genre;Item[i].detail=detail;
            Item[i].part=particular;Item[i].level=level;Item[i].series=series;Item[i].stack=1;Item[i].maxStack=genre==3?100:1;
#ifdef PT_TEST_GIFTS
            Item[i].maxStack=GiftMaxStack(genre,detail,particular);
#endif
            return i;}
        return 0;
    }
    void Remove(int index){Item[index].id=0;}
} ItemSet;
#define _SERVER
int PhongThanConsumableInitialCharges(int g,int d,int p,int c,const char*){return g==8&&p==0&&(d==330||d==334)?c:0;}
#define PT_QUEST_ITEM_NOW() 0
#include "../../Sources/Core/Src/PhongThanQuestExchange.inl"
#define MESSAGE_SYSTEM_ANNOUCE_HEAD ""
enum {MAX_SUBWORLD=3,SAVE_REQUEST=1};
struct Actor{int m_CurrentLife,m_Level,m_SubWorldIndex,x,y;DWORD m_dwID;void GetMpsPos(int* a,int* b){*a=x;*b=y;}}Npc[MAX_NPC];
bool barrier=false,worldAvailable=true;
struct World{int m_SubWorldID;int TestBarrier(int,int){return barrier?1:0;}void Mps2Map(int,int,int* r,int*,int*,int*,int*){*r=0;}}SubWorld[MAX_SUBWORLD];
int WorldSet::SearchWorld(int map){if(!worldAvailable)return -1;if(map==1021)return 1;SubWorld[2].m_SubWorldID=map;return 2;}
static unsigned rng=987654321;unsigned g_Random(unsigned n){rng=rng*1664525+1013904223;return n?(rng>>8)%n:0;}
DWORD KSG_GetCurSec(){return 100000;}

static bool PhongThanIsLivePlayer(int p){return p==1;}
namespace PtTravel{static bool Owned(int p,int item){return p==1&&Player[p].m_ItemList.FindSame(item)>0&&Player[p].m_ItemList.m_Items[item].nPlace!=99;}}
#include "../../Sources/Core/Src/PhongThanF2Gifts.inl"
#include "../../Sources/Core/Src/PhongThanEventTicketPayment.inl"
static int ticketMoveCalls;
static bool ticketMoveOk;
static bool TicketMove(void*){++ticketMoveCalls;return ticketMoveOk;}
static PhongThanQuestExchangeRow need[1],rewards[2];
static void Reset(int capacity)
{
    ZeroMemory(Player,sizeof(Player));ZeroMemory(Item,sizeof(Item));ItemSet.allocated=0;ItemSet.failAt=0;
    Player[1].m_cTask.value=18;Player[1].m_ItemList.capacity=capacity;
    Item[1].id=101;Item[1].genre=4;Item[1].detail=26;Item[1].stack=1;
    Player[1].m_ItemList.Add(1,pos_equiproom,0,0,false);
    assert(PhongThanDecodeQuestItemTuple(4,26,0,0,0,0,need[0].item));need[0].count=1;
    assert(PhongThanDecodeQuestItemTuple(1,0,1,1,0,0,rewards[0].item));rewards[0].count=3;
    assert(PhongThanDecodeQuestItemTuple(1,3,1,1,0,0,rewards[1].item));rewards[1].count=3;
}
static void Unchanged(){assert(Player[1].m_cTask.value==18 && Item[1].id==101 && Player[1].m_ItemList.Count()==1);}

#include "../../Sources/Core/Src/PhongThanTreasure.inl"
void Setup(int trigram=1){Reset(100);ZeroMemory(PtTreasure::contexts,sizeof(PtTreasure::contexts));
 Player[1].m_nIndex=1;Npc[1].m_CurrentLife=100;Npc[1].m_Level=90;Npc[1].m_dwID=90;Npc[1].m_SubWorldIndex=1;SubWorld[1].m_SubWorldID=1021;
 Item[1].genre=3;Item[1].detail=14+trigram;Item[1].part=0;Item[1].level=0;Item[1].series=5;barrier=false;worldAvailable=true;
}
int MapItem(){for(int i=1;i<MAX_ITEM;++i)if(Player[1].m_ItemList.FindSame(i)&&Item[i].genre==6&&Item[i].detail==110)return i;return 0;}
void Near(int index){Npc[1].m_SubWorldIndex=2;SubWorld[2].m_SubWorldID=PtTreasure::Map(Player[1].m_cTask.GetSaveVal(381));Npc[1].x=Player[1].m_cTask.GetSaveVal(382)*32;Npc[1].y=Player[1].m_cTask.GetSaveVal(383)*32;
 PtTreasure::Context& c=PtTreasure::contexts[1];c.item=index;c.serial=Item[index].GetID();c.actor=Npc[1].m_dwID;c.until=GetTickCount()+120000;}
int main(){
 for(int trigram=1;trigram<=7;++trigram){Setup(trigram);assert(PtTreasure::Accept(1,trigram));int item=MapItem();assert(item&&Player[1].m_cTask.GetSaveVal(382)>0);assert(!PtTreasure::Accept(1,trigram));
  Near(item);DWORD serial=Item[item].GetID();int x=Player[1].m_cTask.GetSaveVal(382);int distance;assert(PtTreasure::Position(1,distance)&&distance==0);
  for(int i=0;i<20;++i){assert(PtTreasure::Position(1,distance));assert(Item[item].GetID()==serial&&Player[1].m_cTask.GetSaveVal(382)==x);}
  Npc[1].x+=3200;assert(!PtTreasure::Dig(1)&&Item[item].GetID()==serial);Near(item);
  Player[1].m_ItemList.m_Items[item].nPlace=pos_immediacy;PtTreasure::contexts[1].roll=1;PtTreasure::contexts[1].amount=3;
  Player[1].m_ItemList.failSearch=true;assert(!PtTreasure::Dig(1)&&Item[item].GetID()==serial&&Player[1].m_cTask.GetSaveVal(382)==x);
  Player[1].m_ItemList.failSearch=false;assert(PtTreasure::Dig(1)&&!Player[1].m_cTask.GetSaveVal(382));assert(!PtTreasure::Dig(1));
 }
 Setup();barrier=true;assert(!PtTreasure::Accept(1,1)&&Item[1].id==101);barrier=false;worldAvailable=false;assert(!PtTreasure::Accept(1,1)&&Item[1].id==101);
 Setup();Player[1].m_ItemList.failSearch=true;assert(!PtTreasure::Accept(1,1)&&Item[1].id==101&&!Player[1].m_cTask.GetSaveVal(382));
 Setup();assert(PtTreasure::Accept(1,1));int item=MapItem();Near(item);++Npc[1].m_dwID;assert(!PtTreasure::Dig(1));Near(item);PtTreasure::contexts[1].until=GetTickCount()-1;assert(!PtTreasure::Dig(1));
 // Every original 1..1000 outcome completes without a second use/reward.
 for(int roll=1;roll<=1000;++roll){Setup();assert(PtTreasure::Accept(1,1));item=MapItem();Near(item);PtTreasure::contexts[1].roll=roll;PtTreasure::contexts[1].amount=1;assert(PtTreasure::Dig(1));assert(!PtTreasure::Dig(1));}
 Setup();assert(PtTreasure::Accept(1,1));item=MapItem();Near(item);Player[1].m_cTask.SetSaveVal(826,1,TRUE);Player[1].m_cTask.SetSaveVal(827,1,TRUE);assert(PtTreasure::Dig(1));assert(Player[1].m_cTask.GetSaveVal(827)==2);
 Setup();assert(PtTreasure::Accept(1,1));item=MapItem();Player[1].m_ItemList.Remove(item);ItemSet.Remove(item);
 KLuaScript lua;assert(lua.Init());TLua_Funcs funs[]={{"TreasureItem",LuaTreasureItem}};lua.RegisterFunctions(funs,1);
 const char* code="function TestRestore() assert(TreasureItem(3,0)==1); assert(TreasureItem(3,0)==0) end";
 assert(lua.LoadBuffer((BYTE*)code,strlen(code))&&lua.ExecuteCode()&&lua.CallFunction("TestRestore",0,""));assert(MapItem());
 puts("PASS production treasure: seven trigrams, repeated hints preserve map, hotbar consume on successful dig only, full-bag rollback, wrong location, failed map, stale actor/session, 1000 reward rolls, no replay, legacy progression, recover missing map without duplicate");return 0;
}
