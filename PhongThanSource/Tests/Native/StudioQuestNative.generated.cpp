#include <windows.h>
#include <stdio.h>
#include <assert.h>
#include "KWin32.h"
#include <vector>
#include <map>
#include <string>
#include <time.h>
#include "KIniFile.h"
#include "KFilePath.h"
#include "KPakFile.h"
#include "../../Headers/PhongThanF2Balance.h"
#include "KDebug.h"
#include "KLuaScript.h"
#include <limits.h>
#include "../../Headers/PhongThanReservoirs.h"
#ifdef PT_TEST_GIFTS
#include "KFilePath.h"
#include "KPakFile.h"
static int GiftMaxStack(int,int,int);
#endif
enum {MAX_PLAYER=3,MAX_NPC=8,MAX_ITEM=128,MAX_TASK=5400,MAX_ITEM_MAGICLEVEL=20,
      pos_immediacy=22,pos_equiproom=3,pos_equiproomex=4,pos_ptbag2=19,pos_ptbag3=20,item_equip=0,item_medicine=1,item_materials=3,item_task=4,item_townportal=5,item_magicscript=6,item_skillbook=7,item_ibitem=8,equip_detailnum=16};
#include "../../Sources/Core/Src/PhongThanQuestItemTuple.h"
struct Lock {bool IsLock(){return false;}};
struct KItem {
    int id,genre,detail,part,level,series,stack,maxStack,expire;struct {int nParam;} m_CommonAttrib;
    int GetID(){return id;} int GetGenre(){return genre;} int GetDetailType(){return detail;}
    int GetParticular(){return part;}int GetLevel(){return level;}int GetSeries(){return series;}
    int GetStackNum(){return stack;}int GetMaxStackNum(){return maxStack>0?maxStack:1;}int GetWidth(){return 1;}int GetHeight(){return 1;}
    bool IsStack(){return GetMaxStackNum()>1 && !PhongThanReservoirTooltip(genre,detail,part,GetMaxStackNum(),"NONE");}
    const char* GetScript(){return "NONE";}int GetExpirePoint(){return 0;}void SetExpireTime(int time){expire=time;}int GetExpireTime(){return expire;}int GetParam(){return m_CommonAttrib.nParam;}void SetParam(int v){m_CommonAttrib.nParam=v;}void SetLockTrade(int){}void SetLockSell(int){}void SetLockDrop(int){}
    void SetStackNum(int value){stack=value;}Lock* GetLock(){static Lock lock;return &lock;}
} Item[MAX_ITEM];
struct ItemPos {int nPlace,nX,nY;};
struct PlayerItem {int nIdx,nPlace,nX,nY;};
struct KItemList {
    PlayerItem m_Items[MAX_ITEM];int capacity,cursor;bool failSearch;int GetEquipmentMoney();int CostMoney(int);
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
struct Task {int value,slots[5400];char text[5400][16];int GetSaveVal(int id){return id==20?value:slots[id];}const char* GetSaveStr(int id){if(!text[id][0]&&GetSaveVal(id))itoa(GetSaveVal(id),text[id],10);return text[id];}void SetSaveVal(int id,int next,BOOL){if(id==20)value=next;else slots[id]=next;itoa(next,text[id],10);}void SetSaveVal(int id,char* v,BOOL){strncpy(text[id],v,15);slots[id]=atoi(v);}};
struct KPhongThanScriptAction;
struct PlayerMock {
    void DoScriptAction(KPhongThanScriptAction*){}
    int m_nIndex,m_nLastNpcIndex,m_PlayerDBLoad,m_nNextLevelExp,m_nExp,saves,lastParam,m_nPaceBarTime,m_nPaceBarTimeMax;DWORD m_dwTaskExcuteScriptId;char m_szTaskExcuteFun[80];DWORD lastScript;void Save(){++saves;}BOOL ExecuteScript(DWORD s,char*,int p){lastScript=s;lastParam=p;return TRUE;}BOOL ExecuteScript(char* s,char* f,int p){return ExecuteScript(g_FileName2Id(s),f,p);}void* m_pStatusLoadPlayerInfo;bool locked,trading;Task m_cTask;KItemList m_ItemList;
    unsigned long m_dwID;int cash,xp;struct {int m_nFlag;} m_cTong;
    bool Earn(int money){if(money<0||cash>2000000000-money)return false;cash+=money;return true;}
    bool Pay(int money){if(money<0||cash<money)return false;cash-=money;return true;}
    void DirectAddExp(int amount){xp+=amount;m_nExp+=amount;}
    bool GetLockState(){return locked;}bool CheckTrading(){return trading;}
} Player[MAX_PLAYER];
int GetPlayerIndex(Lua_State*){return 1;}
struct WorldSet {int tick;int GetGameTime(){return tick;}int GetGameVersion(){return 1;}int SearchWorld(int map); } g_SubWorldSet;
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

typedef PlayerMock KPlayer;
int KItemList::GetEquipmentMoney(){return Player[1].cash;}int KItemList::CostMoney(int n){if(n<0||Player[1].cash<n)return 0;Player[1].cash-=n;return 1;}
enum{MAX_SUBWORLD=2,MAX_LEVEL=200,kind_normal=0,kind_player=1,kind_dialoger=3,TASKVALUE_PT_BOUND_MONEY=4847,TASKVALUE_STATTASK_REPUTE=100,TASKVALUE_PT_TITLE_QUALIFY_BEGIN=200};
#define MAX_INT INT_MAX
struct NpcMock{int m_Kind,m_CurrentLife,m_SubWorldIndex,m_RegionIndex,m_NpcSettingIdx,m_Series,m_Level,x,y,m_OriginX,m_OriginY;DWORD m_dwID,m_ActionScriptID;char ActionScript[128];int IsPlayer(){return m_Kind==1;}int IsAlive(){return m_CurrentLife>0;}int GetPlayerIdx(){return IsPlayer()?1:0;}void SendSyncData(int,int){}void GetMpsPos(int* a,int* b){*a=x;*b=y;}}Npc[MAX_NPC];
struct NpcSetMock{int GetDistance(int a,int b){return abs(Npc[a].x-Npc[b].x)+abs(Npc[a].y-Npc[b].y);}}NpcSet;
struct World{int m_SubWorldID;void Mps2Map(int x,int y,int* r,int* mx,int* my,int* ox,int* oy){*r=0;*mx=x;*my=y;*ox=*oy=0;}}SubWorld[2];
void* g_GetScript(const char*){return (void*)1;}BOOL ReLoadScript(const char*){return TRUE;}
int journalCalls,lastJournalAction;static void SendPhongThanTaskRecord(int,int,int,int a,const char*){++journalCalls;lastJournalAction=a;}
unsigned long GetTitleQualifyWord(int p,int w){return strtoul(Player[p].m_cTask.GetSaveStr(TASKVALUE_PT_TITLE_QUALIFY_BEGIN+w),0,16);}
enum{PHONGTHAN_SCRIPT_SHOW=1,UI_TALKDIALOG=1};struct KPhongThanScriptAction{int Operation,View,OptionCount,ServerOwned,ContentLength;char Content[2048];};

int PhongThanF2WalletCount(int p,const PTF2BalanceQuery& q){int total=0;for(PlayerItem* i=Player[p].m_ItemList.GetFirstItem();i;i=Player[p].m_ItemList.GetNextItem()){int n=i->nIdx;if(n>0&&n<MAX_ITEM&&Item[n].id&&(i->nPlace==pos_equiproom||i->nPlace==pos_equiproomex||i->nPlace==pos_ptbag2||i->nPlace==pos_ptbag3)&&Item[n].genre==q.Genre&&Item[n].detail==q.Detail&&Item[n].part==q.Particular)total+=Item[n].stack;}return total;}
#include "StudioQuestGatherMocks.inc"
#include "StudioQuestServiceMocks.inc"
#include "../../Sources/Core/Src/PhongThanStudioQuests.inl"
#include "../../Sources/Core/Src/PhongThanStudioQuestGather.inl"
static void Setup(){PhongThanStudioQuestNodesChanged();ZeroMemory(Object,sizeof(Object));ZeroMemory(questBuffs,sizeof(questBuffs));questSeconds=100000;g_SubWorldSet.tick=100;Reset(12);ZeroMemory(Npc,sizeof(Npc));SubWorld[0].m_SubWorldID=1052;Player[1].m_PlayerDBLoad=1;Player[1].m_nIndex=1;Player[1].m_nLastNpcIndex=2;Player[1].m_dwID=1001;Player[1].m_nNextLevelExp=100000;
 Npc[1].m_Kind=1;Npc[1].m_CurrentLife=100;Npc[1].m_dwID=1001;Npc[1].m_Level=40;Npc[1].x=10000;Npc[1].y=15000;
 Npc[2].m_Kind=3;Npc[2].m_dwID=12;Npc[2].m_NpcSettingIdx=151;Npc[2].x=10000;Npc[2].y=15000;strcpy(Npc[2].ActionScript,"\\script\\old.lua");Npc[2].m_ActionScriptID=g_FileName2Id(Npc[2].ActionScript);
 Npc[4].m_Kind=3;Npc[4].m_dwID=14;Npc[4].m_NpcSettingIdx=154;Npc[4].x=10032;Npc[4].y=15000;strcpy(Npc[4].ActionScript,"\\script\\oldb.lua");Npc[4].m_ActionScriptID=g_FileName2Id(Npc[4].ActionScript);
 Npc[3].m_Kind=0;Npc[3].m_dwID=33;Npc[3].m_NpcSettingIdx=900;Npc[3].x=10000;Npc[3].y=15000;
 PtStudioQuest::checkedAt=-100;PtStudioQuest::loadedRevision=-1;PtStudioQuest::quests.clear();PtStudioQuest::bindings.clear();ZeroMemory(PtStudioQuest::seenEntity,sizeof(PtStudioQuest::seenEntity));ZeroMemory(PtStudioQuest::seenLife,sizeof(PtStudioQuest::seenLife));for(int n=0;n<MAX_NPC;n++)PtStudioQuest::lives[n]=PhongThanNpcLifeToken();assert(PtStudioQuest::Load());PtStudioGather::SpawnNodes();}
#define CHECK(c) do{if(!(c)){printf("FAIL native quest line=%d %s\n",__LINE__,#c);return 1;}}while(0)
#include "StudioQuestGatherCases.inc"
#include "StudioQuestSealCases.inc"
#include "StudioQuestClassRewardCases.inc"
#include "StudioQuestServiceCases.inc"
#include "StudioQuestBountyCases.inc"
int main(int argc,char**argv){if(argc==3){CHECK(SetCurrentDirectoryA(argv[1]));g_SetRootPath(NULL);g_SetFilePath("\\");g_SetPakFileMode(0);BountySetup();CHECK(PtStudioQuest::quests.size()==1&&PtStudioQuest::questBosses.size()==4&&PtStudioQuest::services.offers.size()>=33);CHECK(PtStudioQuest::Act(1,1,0,1));puts("PASS published server configuration: native loader accepts real NPC, four private bosses, class shops and daily quest acceptance (mock character only)");return 0;}CHECK(argc==2&&SetCurrentDirectoryA(argv[1]));g_SetRootPath(NULL);g_SetFilePath("\\");g_SetPakFileMode(0);Setup();CHECK(PtStudioQuest::quests.size()==10&&PtStudioQuest::bindings.size()==3);
 CHECK(PtStudioQuest::Act(1,1,0,1)==1);PtStudioQuest::Quest* q=PtStudioQuest::Find(1);CHECK(PtStudioQuest::Status(1,*q)==1);Npc[1].x=11000;CHECK(!PtStudioQuest::Act(1,1,1,1));Npc[1].x=10000;CHECK(PtStudioQuest::Act(1,1,1,1)==1&&PtStudioQuest::Status(1,*q)==2);
 Npc[3].m_NpcSettingIdx=901;PhongThanStudioQuestKilled(3,1);CHECK((PtStudioQuest::Raw(1,*q)&65535)==0);Npc[3].m_NpcSettingIdx=900;PhongThanStudioQuestRevived(3);PhongThanStudioQuestKilled(3,1);CHECK((PtStudioQuest::Raw(1,*q)&65535)==1);PhongThanStudioQuestKilled(3,1);CHECK((PtStudioQuest::Raw(1,*q)&65535)==1);
 Npc[1].m_Level=50;PhongThanStudioQuestRevived(3);PhongThanStudioQuestKilled(3,1);CHECK(PtStudioQuest::Status(1,*q)==3);Player[1].m_nLastNpcIndex=4;Player[1].m_ItemList.failSearch=true;CHECK(!PtStudioQuest::Act(1,1,1,2));CHECK(Item[1].id==101&&Player[1].cash==0&&Player[1].m_cTask.GetSaveVal(4847)==0&&PtStudioQuest::Status(1,*q)==3);
 Player[1].m_ItemList.failSearch=false;CHECK(PtStudioQuest::Act(1,1,1,2)==2);CHECK((PtStudioQuest::Raw(1,*q)&PtStudioQuest::Done)&&!Item[1].id&&Player[1].cash==10000&&Player[1].m_cTask.GetSaveVal(4847)==500&&Player[1].m_cTask.GetSaveVal(100)==10&&Player[1].xp==100&&(GetTitleQualifyWord(1,0)&32));CHECK(!PtStudioQuest::Act(1,1,1,2)&&Player[1].cash==10000);
 puts("PASS native story: NPC A -> exact-ID/map kill with life receipt -> NPC B -> delivery + all reward types; full bag refunds; no replay; level cap only checked at acceptance");
 Setup();CHECK(PtStudioQuest::Act(1,2,0,1));Item[2]=Item[1];Item[2].id=102;Item[2].detail=27;Item[2].stack=1;CHECK(Player[1].m_ItemList.Add(2,pos_equiproom,1,0,false));CHECK(!PtStudioQuest::Act(1,2,1,1)&&Item[1].id&&Item[2].stack==1);Item[2].stack=2;Player[1].m_ItemList.failSearch=true;CHECK(!PtStudioQuest::Act(1,2,1,1)&&Item[1].id&&Item[2].stack==2);Player[1].m_ItemList.failSearch=false;CHECK(PtStudioQuest::Act(1,2,1,1)==2&&!Item[1].id&&!Item[2].id);CHECK(PtStudioQuest::Status(1,*PtStudioQuest::Find(2))==0&&!PtStudioQuest::Act(1,2,1,1));
 puts("PASS native exchange: all ingredients checked together, shortages consume nothing, generation failure restores both stacks, successful exact exchange and repeat guard");
 Setup();CHECK(!PtStudioQuest::Act(1,3,0,1));Npc[1].m_Series=1;CHECK(!PtStudioQuest::Act(1,3,0,1));Player[1].m_cTask.SetSaveVal(5160,PtStudioQuest::Done|PtStudioQuest::Day(),TRUE);CHECK(PtStudioQuest::Act(1,3,0,1));Setup();CHECK(PtStudioQuest::Act(1,4,0,1)&&PtStudioQuest::Act(1,4,1,1)==2);q=PtStudioQuest::Find(4);CHECK(PtStudioQuest::Status(1,*q)==-1);Player[1].m_cTask.SetSaveVal(5163,PtStudioQuest::Done|(q->generation<<22)|((PtStudioQuest::Day()-1)&65535),TRUE);CHECK(PtStudioQuest::Status(1,*q)==0);
 Setup();q=PtStudioQuest::Find(5);Player[1].m_cTask.SetSaveVal(5164,(1<<22)|(1<<16),TRUE);CHECK(PtStudioQuest::Status(1,*q)==-2&&PtStudioQuest::Act(1,5,0,1));CHECK(PtStudioQuest::Status(1,*q)==1);journalCalls=0;PhongThanStudioQuestLogin(1);CHECK(journalCalls>0);
 puts("PASS native profession/prerequisite gates, daily reset, changed-definition restart and relog journal restore");
 Setup();CHECK(PtStudioQuest::Act(1,6,0,1)&&PtStudioQuest::Act(1,6,1,1));q=PtStudioQuest::Find(6);CHECK(PtStudioQuest::Status(1,*q)==4);Player[1].m_nLastNpcIndex=4;CHECK(PtStudioQuest::Act(1,6,1,2)==2&&Item[1].id==101);
 Setup();BOOL result=FALSE;CHECK(PhongThanStudioQuestTryRoute(1,Npc[2].m_ActionScriptID,"main",55,&result)&&result&&Player[1].lastParam==55&&!PtStudioQuest::routing);CHECK(!PhongThanStudioQuestTryRoute(1,Npc[2].m_ActionScriptID,"buy",0,&result));CHECK(!PhongThanStudioQuestTryRoute(1,12345,"main",0,&result));
 puts("PASS native shared quest per-profession steps and scoped NPC-main routing preserving unrelated callbacks and original parameter");CHECK(GatherCases()==0);CHECK(SealCases()==0);CHECK(ClassRewardCases()==0);CHECK(ServiceCases()==0);CHECK(BountyCases()==0);return 0;}
