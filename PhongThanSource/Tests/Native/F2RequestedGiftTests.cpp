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
    int Count(){int count=0;for(int i=1;i<MAX_ITEM;++i)if(m_Items[i].nIdx)++count;return count;}
};
struct Task {int value,slots[MAX_TASK];int GetSaveVal(int id){return id==20?value:slots[id];}void SetSaveVal(int id,int next,BOOL){if(id==20)value=next;else slots[id]=next;}};
struct PlayerMock {
    int m_nIndex,m_nLastNpcIndex;void* m_pStatusLoadPlayerInfo;bool locked,trading;Task m_cTask;KItemList m_ItemList;
    unsigned long m_dwID;int cash,xp;struct {int m_nFlag;} m_cTong;
    bool Earn(int money){if(money<0||cash>2000000000-money)return false;cash+=money;return true;}
    bool Pay(int money){if(money<0||cash<money)return false;cash-=money;return true;}
    void DirectAddExp(int amount){xp+=amount;}
    void SendSystemMessage(const char*,const char*){}
    bool GetLockState(){return locked;}bool CheckTrading(){return trading;}
} Player[MAX_PLAYER];
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
struct Actor{int m_CurrentLife;}Npc[MAX_NPC];
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
int main(){
 for(int bucket=0;bucket<2;++bucket)for(int n=0;n<PT_F2_GIFT_COUNT;++n){const PTF2Gift& g=ptF2Gifts[n];
  Reset(100);Player[1].m_nIndex=1;Npc[1].m_CurrentLife=100;Item[1].genre=8;Item[1].detail=g.detail;Item[1].part=2;
  Item[2]=Item[1];Item[2].id=102;Player[1].m_ItemList.Add(2,bucket?pos_immediacy:pos_equiproomex,1,0,false);
  Player[1].m_ItemList.failSearch=true;assert(!PhongThanUseF2Gift(1,2));assert(Item[1].id==101&&Item[2].id==102&&Player[1].m_ItemList.Count()==2);
  Player[1].m_ItemList.failSearch=false;assert(PhongThanUseF2Gift(1,2)==1);assert(Item[1].id==101&&!Item[2].id&&Player[1].m_ItemList.FindSame(1));
  for(int r=0;r<g.count;++r){int quantity=0;for(int i=1;i<MAX_ITEM;++i)if(Player[1].m_ItemList.FindSame(i)&&Item[i].genre==g.rewards[r].genre&&Item[i].detail==g.rewards[r].detail&&Item[i].part==g.rewards[r].part&&Item[i].level==g.rewards[r].level)quantity+=Item[i].stack;assert(quantity==g.rewards[r].count);}
  assert(PhongThanUseF2Gift(1,2)<=0);Player[1].locked=true;assert(!PhongThanUseF2Gift(1,1));Player[1].locked=false;Player[1].trading=true;assert(!PhongThanUseF2Gift(1,1));
  printf("PASS gift %d: exact rewards, clicked copy, bag rollback, duplicate/lock/trade guards\n",g.detail);
 }
 return 0;
}
