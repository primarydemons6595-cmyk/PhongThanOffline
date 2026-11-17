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
    int id,genre,detail,part,level,series,stack,maxStack,expire;struct {int nParam;} m_CommonAttrib;
    int GetRow(){return 0;}int GetLockSell(){return 0;}int GetLockTrade(){return 0;}int GetLockDrop(){return 0;}int GetID(){return id;} int GetGenre(){return genre;} int GetDetailType(){return detail;}
    int GetParticular(){return part;}int GetLevel(){return level;}int GetSeries(){return series;}
    int GetStackNum(){return stack;}int GetMaxStackNum(){return maxStack>0?maxStack:1;}int GetWidth(){return 1;}int GetHeight(){return 1;}
    bool IsStack(){return GetMaxStackNum()>1 && !PhongThanReservoirTooltip(genre,detail,part,GetMaxStackNum(),"NONE");}
    const char* GetScript(){return "NONE";}int GetExpirePoint(){return 0;}void SetExpireTime(int time){expire=time;}int GetExpireTime(){return expire;}int GetParam(){return m_CommonAttrib.nParam;}void SetParam(int v){m_CommonAttrib.nParam=v;}void SetLockTrade(int){}void SetLockSell(int){}void SetLockDrop(int){}
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
    bool GetLockState(){return locked;}bool CheckTrading(){return trading;}
} Player[MAX_PLAYER];
int GetPlayerIndex(Lua_State*){return 1;}
struct WorldSet {int GetGameVersion(){return 1;}int SearchWorld(int map); } g_SubWorldSet;
struct Items {
    int allocated,failAt;
    int Add(KItem* p){int i=Add(p->genre,p->series,p->level,0,p->detail,p->part,0,1);if(i){int id=Item[i].id;Item[i]=*p;Item[i].id=id;}return i;}
    int Add(int genre,int series,int level,int,int detail,int particular,int*,int){
        if(++allocated==failAt)return 0;
        for(int i=2;i<MAX_ITEM;++i)if(!Item[i].id){ZeroMemory(&Item[i],sizeof(Item[i]));Item[i].id=i+100;Item[i].genre=genre;Item[i].detail=detail;
            Item[i].part=particular;Item[i].level=level;Item[i].series=series;Item[i].stack=1;
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
static unsigned long questTestNow=100000;
#define PT_QUEST_ITEM_NOW() questTestNow
#define PT_QUEST_ITEM_PARAM(item) ((item).GetParam())
#include "../../Sources/Core/Src/PhongThanQuestExchange.inl"
#ifdef PT_TEST_GIFTS
#include "F2GiftCases.inc"
static int GiftInfo(Lua_State* L){
    int i=(int)Lua_ValueToNumber(L,1);
    if(i<1||i>=MAX_ITEM||!Item[i].id){Lua_PushNumber(L,-1);return 1;}
    Lua_PushNumber(L,Item[i].genre);Lua_PushNumber(L,Item[i].detail);Lua_PushNumber(L,Item[i].part);return 3;
}
static int GiftTask(Lua_State* L){Lua_PushNumber(L,Player[1].m_cTask.GetSaveVal((int)Lua_ValueToNumber(L,1)));return 1;}
static int GiftMessage(Lua_State*){return 0;}
static void GiftReset(int detail){
    ZeroMemory(Player,sizeof(Player));ZeroMemory(Item,sizeof(Item));ItemSet.allocated=ItemSet.failAt=0;
    Player[1].m_ItemList.capacity=64;
    Item[1].id=101;Item[1].genre=8;Item[1].detail=detail;Item[1].part=2;Item[1].stack=Item[1].maxStack=1;
    assert(Player[1].m_ItemList.Add(1,pos_equiproom,0,0,false));
}
static void GiftTests(int argc,char**argv){
    assert(argc==2 && SetCurrentDirectoryA(argv[1]));g_SetRootPath(NULL);g_SetFilePath("\\");g_SetPakFileMode(0);
    for(int row=0;row<sizeof(giftCases)/sizeof(giftCases[0]);++row){
        GiftCase& c=giftCases[row];KLuaScript script;assert(script.Init());
        script.RegisterFunction("FindItemEx",(void*)GiftInfo);script.RegisterFunction("GetTask",(void*)GiftTask);
        script.RegisterFunction("QuestExchange",(void*)LuaQuestExchange);script.RegisterFunction("Msg2Player",(void*)GiftMessage);
        assert(script.Load((char*)c.path));GiftReset(c.detail);
        assert(script.CallFunction("main",0,"d",1));assert(!Item[1].id && Player[1].m_cTask.GetSaveVal(4897)==1);
        for(int r=0;r<c.count;++r){
            int total=0;int* v=c.rewards[r];int detail=v[0]==6?v[2]:v[1],part=v[0]==6?0:v[2];
            for(int i=2;i<MAX_ITEM;++i)if(Player[1].m_ItemList.FindSame(i)&&Item[i].genre==v[0]&&Item[i].detail==detail&&Item[i].part==part){
                assert(Item[i].stack<=Item[i].GetMaxStackNum());total+=Item[i].stack;
            }
            assert(total==v[6]);
        }
        int owned=Player[1].m_ItemList.Count();assert(script.CallFunction("main",0,"d",1));assert(Player[1].m_ItemList.Count()==owned);
        GiftReset(c.detail);Player[1].m_ItemList.failSearch=true;
        assert(script.CallFunction("main",0,"d",1));assert(Item[1].id==101&&Player[1].m_ItemList.Count()==1&&Player[1].m_cTask.GetSaveVal(4897)==0);
        GiftReset(c.detail);ItemSet.failAt=1;
        assert(script.CallFunction("main",0,"d",1));assert(Item[1].id==101&&Player[1].m_ItemList.Count()==1&&Player[1].m_cTask.GetSaveVal(4897)==0);
        GiftReset(c.detail);Player[1].trading=true;
        assert(script.CallFunction("main",0,"d",1));assert(Item[1].id==101&&Player[1].m_ItemList.Count()==1);
        GiftReset(c.detail);Item[1].detail=99999;
        assert(script.CallFunction("main",0,"d",1));assert(Item[1].id==101&&Player[1].m_ItemList.Count()==1);
        GiftReset(c.detail);
        assert(Lua_ExecuteString(script.m_LuaState,"assert(QuestExchange(4897,0,1,{}, {},0)==0)")==0);
    }
    puts("PASS F2_GIFTS nine_scripts real_Lua4 native_transaction exact_rewards native_piles 1000_units full_bag generation_failure trading wrong_item repeat invalid_flag");
}
#endif
#ifdef PT_TEST_F2
static void Reset(int capacity);
enum {TASKVALUE_PT_XU_EXCHANGE=21};
static int KSG_GetCurSec(){return 1;}
#include "F2PurchaseNative.inc"
static void F2Reset(int capacity,int xu) {
    Reset(capacity);Item[1].detail=47;Item[1].stack=xu;Item[1].maxStack=10000;
}
static void CheckF2Reservoir(int detail,int part,int capacity) {
    KItem prototype;ZeroMemory(&prototype,sizeof(prototype));
    prototype.genre=8;prototype.detail=detail;prototype.part=part;prototype.series=5;
    prototype.stack=prototype.maxStack=capacity;prototype.id=999;
    assert(!prototype.IsStack());
    F2Reset(4,100);assert(PhongThanBuyWithXu(1,prototype,3,10));
    assert(Item[1].stack==70 && Player[1].m_ItemList.Count()==4);
    int bottles=0;
    for(int i=2;i<MAX_ITEM;++i)if(Player[1].m_ItemList.FindSame(i)){
        assert(Item[i].stack==capacity && !Item[i].IsStack());++bottles;
    }
    assert(bottles==3 && prototype.stack==capacity);
    F2Reset(1,100);assert(!PhongThanBuyWithXu(1,prototype,3,10));
    assert(Item[1].stack==100 && Player[1].m_ItemList.Count()==1);
    F2Reset(4,29);assert(!PhongThanBuyWithXu(1,prototype,3,10));
    assert(Item[1].stack==29 && Player[1].m_ItemList.Count()==1);
    F2Reset(4,100);ItemSet.failAt=2;assert(!PhongThanBuyWithXu(1,prototype,3,10));
    assert(Item[1].stack==100 && Player[1].m_ItemList.Count()==1);
    F2Reset(4,100);assert(PhongThanBuyWithXu(1,prototype,2,35));assert(Item[1].stack==30);
    F2Reset(4,100);assert(PhongThanBuyWithXu(1,prototype,1,0));assert(Item[1].stack==100);
    F2Reset(4,100);assert(!PhongThanBuyWithXu(1,prototype,2,INT_MAX));assert(Item[1].stack==100);
    assert(!PhongThanBuyWithXu(1,prototype,1,-1));
}
#endif
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
int main(int argc,char** argv)
{
    Reset(6);assert(PhongThanQuestExchange(1,20,18,19,need,1,rewards,2)==1);
    assert(Player[1].m_cTask.value==19 && Item[1].id==0 && Player[1].m_ItemList.Count()==6);
    assert(!PhongThanQuestExchange(1,20,18,19,need,1,rewards,2));assert(Player[1].m_ItemList.Count()==6);
    Reset(5);assert(!PhongThanQuestExchange(1,20,18,19,need,1,rewards,2));Unchanged();
    Reset(6);ItemSet.failAt=3;assert(!PhongThanQuestExchange(1,20,18,19,need,1,rewards,2));Unchanged();
    Reset(6);need[0].item.detail=27;assert(!PhongThanQuestExchange(1,20,18,19,need,1,rewards,2));Unchanged();
    Reset(6);assert(!PhongThanQuestExchange(1,20,17,19,need,1,rewards,2));Unchanged();
    Reset(2);Item[1].stack=3;need[0].count=2;assert(!PhongThanQuestExchange(1,20,18,19,need,1,rewards,2));Unchanged();assert(Item[1].stack==3);
    Reset(1);assert(PhongThanQuestExchange(1,20,18,19,need,0,rewards,0));assert(Player[1].m_cTask.value==19);
    Reset(10);KItem proto;ZeroMemory(&proto,sizeof(proto));proto.genre=8;proto.detail=330;proto.maxStack=proto.stack=5;
    assert(PhongThanDecodeQuestItemTuple(8,330,0,1,0,0,rewards[0].item));rewards[0].count=2;proto.level=0;
    assert(PhongThanQuestExchange(1,20,18,19,need,0,rewards,1,true,&proto));
    assert(Item[2].stack==5&&Item[3].stack==5&&Player[1].m_ItemList.Count()==3);
    puts("PASS QUEST_EXCHANGE original_ids full_reward_set phase_guard bag_full_generate_failure_exact_input_partial_stack_restore_repeat");
    Reset(1);ticketMoveCalls=0;ticketMoveOk=false;
    assert(PhongThanEventTicketPayment(1,need[0].item,1,20,18,19,TicketMove,0)==0);Unchanged();assert(ticketMoveCalls==1);
    Reset(1);Item[1].stack=5;ticketMoveOk=false;
    assert(PhongThanEventTicketPayment(1,need[0].item,2,20,18,19,TicketMove,0)==0);Unchanged();assert(Item[1].stack==5);
    Reset(1);Item[1].stack=5;ticketMoveOk=true;
    assert(PhongThanEventTicketPayment(1,need[0].item,2,20,18,19,TicketMove,0)==1);assert(Item[1].id==101&&Item[1].stack==3&&Player[1].m_cTask.value==19);
    assert(PhongThanEventTicketPayment(1,need[0].item,2,20,18,19,TicketMove,0)==-2);assert(Item[1].stack==3);
    Reset(1);ticketMoveOk=true;
    assert(PhongThanEventTicketPayment(1,need[0].item,1,20,18,19,TicketMove,0)==1);assert(!Item[1].id&&Player[1].m_ItemList.Count()==0&&Player[1].m_cTask.value==19);
    Reset(1);ticketMoveOk=true;
    assert(PhongThanEventTicketPayment(1,need[0].item,1,20,18,18,TicketMove,0)==1);Unchanged();
    Reset(1);ticketMoveCalls=0;need[0].item.detail=27;
    assert(PhongThanEventTicketPayment(1,need[0].item,1,20,18,19,TicketMove,0)==-1);Unchanged();assert(ticketMoveCalls==0);
    Reset(1);Player[1].trading=true;
    assert(PhongThanEventTicketPayment(1,need[0].item,1,20,18,19,TicketMove,0)==-2);Unchanged();
    puts("PASS EVENT_TICKET_TRANSFER map_failure_exact_ids partial_stack_restore successful_payment no_reentry_charge stale_phase missing_ticket trading_guard");
    PhongThanQuestExchangeRow pair[2];
    Reset(2);pair[0]=need[0];pair[1]=need[0];ticketMoveCalls=0;ticketMoveOk=true;
    assert(PhongThanEventTicketsPayment(1,pair,2,20,18,19,TicketMove,0)==-1);Unchanged();assert(ticketMoveCalls==0);
    Reset(2);Item[2]=Item[1];Item[2].id=102;Item[2].genre=8;Item[2].detail=144;Item[2].part=2;
    Player[1].m_ItemList.Add(2,pos_equiproom,1,0,false);
    assert(PhongThanDecodeQuestItemTuple(8,144,2,0,0,0,pair[1].item));pair[1].count=1;ticketMoveOk=false;
    assert(PhongThanEventTicketsPayment(1,pair,2,20,18,19,TicketMove,0)==0);
    assert(Item[1].id==101&&Item[2].id==102&&Player[1].m_ItemList.Count()==2&&Player[1].m_cTask.value==18);
    ticketMoveOk=true;assert(PhongThanEventTicketsPayment(1,pair,2,20,18,19,TicketMove,0)==1);
    assert(!Item[1].id&&!Item[2].id&&Player[1].m_ItemList.Count()==0&&Player[1].m_cTask.value==19);
    Reset(2);pair[0]=need[0];pair[1]=need[0];Item[1].stack=2;ticketMoveOk=false;
    assert(PhongThanEventTicketsPayment(1,pair,2,20,18,19,TicketMove,0)==0);Unchanged();assert(Item[1].stack==2);
    ticketMoveOk=true;assert(PhongThanEventTicketsPayment(1,pair,2,20,18,19,TicketMove,0)==1);assert(!Item[1].id);
    puts("PASS MULTI_TICKET exact_original_ids rollback_both_items missing_second_no_charge duplicate_row_no_double_spend shared_stack");
#ifdef PT_TEST_F2
    assert(argc==2);FILE* input=fopen(argv[1],"r");assert(input);
    int detail,part,capacity,effect,rate,rows=0;
    while(fscanf(input,"%d%d%d%d%d",&detail,&part,&capacity,&effect,&rate)==5){CheckF2Reservoir(detail,part,capacity);++rows;}
    fclose(input);assert(rows==25);
    // Ordinary stack rewards still use pile quantities.
    KItem pile;ZeroMemory(&pile,sizeof(pile));pile.genre=1;pile.detail=0;pile.level=1;pile.stack=1;pile.maxStack=100;
    F2Reset(2,100);assert(PhongThanBuyWithXu(1,pile,3,10));assert(Item[1].stack==70 && Item[2].stack==3);
    puts("PASS F2_RESERVOIRS 25_types three_full_bottles exact_xu capacity_retained full_bag insufficient_xu generation_rollback ordinary_piles");
#endif
#ifdef PT_TEST_GIFTS
    GiftTests(argc,argv);
#endif
    return 0;
}

