#include <windows.h>
#include <stdio.h>
#include <string.h>
#include "KWin32.h"
#include "LuaLib.h"
#include "KLuaScript.h"
#include "../../Sources/Core/Src/PhongThanScriptAction.h"
enum { MAX_PLAYER=16,MAX_NPC=32,MAX_SUBWORLD=8,pos_hand=9 };
#define SCRIPT_SUBWORLDINDEX "SubWorld"
int executed=0,executedValue=0;char executedCallback[32];
int (*executeHook)(DWORD,char*,int)=NULL;
struct KPlayer {
    int m_nIndex;DWORD m_dwID;int locked,trading,index;
    DWORD m_SelUiScriptId,m_TalkUiScriptId,m_UiDialogToken;int m_UiDialogView,m_nAvailableAnswerNum;bool m_bWaitingPlayerFeedBack;
    void ForbidCamp(BOOL b){locked=b;}
    int CheckTrading(){return trading;}
    int GetPlayerIndex(){return index;}
    void DoScriptAction(KPhongThanScriptAction *action){static DWORD serial=10;m_UiDialogToken=++serial;m_UiDialogView=action->View==UI_ROULETTE?PHONGTHAN_VIEW_ROULETTE:PHONGTHAN_VIEW_NUMBER_INPUT;}
    int ExecuteScript(DWORD id,char *callback,int value){++executed;executedValue=value;strcpy(executedCallback,callback);return executeHook?executeHook(id,callback,value):1;}
} Player[MAX_PLAYER];
struct MockNpc {int m_SubWorldIndex;DWORD m_dwID,m_ActionScriptID;} Npc[MAX_NPC];
struct TMissionPlayerInfo {unsigned long m_ulPlayerIndex,m_ulPlayerID;unsigned char m_ucPlayerGroup;};
struct MissionSlots {TMissionPlayerInfo m_Data[MAX_PLAYER];};
struct KMission {
    int id;MissionSlots m_MissionPlayer;
    KMission():id(0){memset(&m_MissionPlayer,0,sizeof(m_MissionPlayer));}
    void SetMissionId(int n){id=n;}
    unsigned long GetNextPlayer(unsigned long cursor,unsigned long &player) {
        for(++cursor;cursor<MAX_PLAYER;++cursor)if(m_MissionPlayer.m_Data[cursor].m_ulPlayerIndex){player=m_MissionPlayer.m_Data[cursor].m_ulPlayerIndex;return cursor;}
        player=0;return 0;
    }
};
KMission mission;
struct MissionArray {KMission *GetData(KMission *key){return key->id==mission.id?&mission:0;} KMission* FindByMissionId(int id){return id==mission.id?&mission:0;}};
struct MockWorld {int m_SubWorldID;MissionArray m_MissionArray;} SubWorld[MAX_SUBWORLD];
int currentPlayer=1,lastPlace=-1,handCount=3,removed=0,tupleSeries=-99;
int GetPlayerIndex(Lua_State *L){Lua_PushNumber(L,currentPlayer);return currentPlayer;}
BOOL GetVngNormalTuple(Lua_State *L,int start,int *genre,int *detail,int *level,int *series,int *particular){
    if(Lua_GetTopIndex(L)<start+3)return FALSE;
    *genre=(int)Lua_ValueToNumber(L,start);*detail=(int)Lua_ValueToNumber(L,start+1);*particular=(int)Lua_ValueToNumber(L,start+2);*level=(int)Lua_ValueToNumber(L,start+3);
    *series=Lua_GetTopIndex(L)>=start+4?(int)Lua_ValueToNumber(L,start+4):-1;tupleSeries=*series;return TRUE;
}
int CountVngOwnedItems(int,int,int,int,int,int place,int){lastPlace=place;return handCount;}
int RemoveVngItems(int,int,int,int,int,int place,BOOL all,int){lastPlace=place;if(all)return -99;if(handCount){--handCount;++removed;return 1;}return 0;}
#include "../../Sources/Core/Src/PhongThanEventLuaCompat.inl"
static KLuaScript *testScript;
KLuaScript *g_GetScript(DWORD id){return id==99?testScript:NULL;}
int PhongThanIsLivePlayer(int p){return p>0 && p<MAX_PLAYER && Player[p].m_nIndex>0 && Player[p].m_nIndex<MAX_NPC && Npc[Player[p].m_nIndex].m_dwID>0;}
#include "../../Sources/Core/Src/PhongThanEventInput.inl"
int finished=0;
static int Finished(Lua_State*){++finished;return 0;}
static int FinishHook(DWORD,char *callback,int){if(!strcmp(callback,"Finished"))++finished;return 1;}
int checks=0;
#define CHECK(x) do{++checks;if(!(x)){printf("FAIL line=%d %s\n",__LINE__,#x);return 1;}}while(0)
static void Reset(Lua_State *L){Lua_SetTopIndex(L,0);}
static void Arg(Lua_State *L,int n){Lua_PushNumber(L,n);}
static void World(Lua_State *L,int n){Arg(L,n);Lua_SetGlobal(L,SCRIPT_SUBWORLDINDEX);}
static void Tuple(Lua_State *L){Reset(L);Arg(L,3);Arg(L,66);Arg(L,0);Arg(L,0);}
static void Next(Lua_State *L,int id,int cursor,int group){Reset(L);Arg(L,id);Arg(L,cursor);Arg(L,group);}
int main(){
    KLuaScript script;CHECK(script.Init());testScript=&script;Lua_State *L=script.m_LuaState;CHECK(L!=0);
    Tuple(L);CHECK(LuaHaveHandItemCompat(L)==1 && Lua_ValueToNumber(L,-1)==3);CHECK(lastPlace==pos_hand && tupleSeries==-1 && Lua_GetTopIndex(L)==5);
    Tuple(L);CHECK(LuaDelHandItemCompat(L)==1 && Lua_ValueToNumber(L,-1)==1);CHECK(handCount==2 && removed==1 && lastPlace==pos_hand && tupleSeries==-1);
    Reset(L);Arg(L,3);CHECK(LuaDelHandItemCompat(L)==1 && Lua_ValueToNumber(L,-1)==0 && removed==1);
    currentPlayer=MAX_PLAYER;Tuple(L);CHECK(LuaHaveHandItemCompat(L)==1 && Lua_ValueToNumber(L,-1)==0);currentPlayer=1;
    Reset(L);Arg(L,1);CHECK(LuaLockCampCompat(L)==1 && Player[1].locked==1);
    Reset(L);Arg(L,0);CHECK(LuaLockCampCompat(L)==1 && Player[1].locked==0);
    Reset(L);Arg(L,7);CHECK(LuaLockCampCompat(L)==1 && Lua_ValueToNumber(L,-1)==0 && Player[1].locked==0);
    mission.id=1;SubWorld[0].m_SubWorldID=1067;World(L,0);
    for(int i=1;i<=4;++i){Player[i].m_nIndex=i;Player[i].m_dwID=100+i;Npc[i].m_SubWorldIndex=0;mission.m_MissionPlayer.m_Data[i].m_ulPlayerIndex=i;mission.m_MissionPlayer.m_Data[i].m_ulPlayerID=100+i;mission.m_MissionPlayer.m_Data[i].m_ucPlayerGroup=i%2+1;}
    Player[1].m_dwID=999; // Recycled player slot must be ignored.
    Npc[2].m_SubWorldIndex=1; // Player already teleported out.
    Next(L,1,0,0);CHECK(LuaGetNextMissionPlayerCompat(L)==2 && Lua_ValueToNumber(L,-2)==3 && Lua_ValueToNumber(L,-1)==3);
    Next(L,1,3,0);CHECK(LuaGetNextMissionPlayerCompat(L)==2 && Lua_ValueToNumber(L,-2)==4 && Lua_ValueToNumber(L,-1)==4);
    Next(L,1,0,1);CHECK(LuaGetNextMissionPlayerCompat(L)==2 && Lua_ValueToNumber(L,-2)==4 && Lua_ValueToNumber(L,-1)==4);
    Next(L,1,4,0);CHECK(LuaGetNextMissionPlayerCompat(L)==2 && Lua_ValueToNumber(L,-2)==0 && Lua_ValueToNumber(L,-1)==0);
    Next(L,99,0,0);CHECK(LuaGetNextMissionPlayerCompat(L)==2 && Lua_ValueToNumber(L,-2)==0 && Lua_ValueToNumber(L,-1)==0);
    Next(L,1,MAX_PLAYER,0);CHECK(LuaGetNextMissionPlayerCompat(L)==2 && Lua_ValueToNumber(L,-1)==0);
    World(L,-1);Next(L,1,0,0);CHECK(LuaGetNextMissionPlayerCompat(L)==2 && Lua_ValueToNumber(L,-1)==0);
    // Commodity tables: all 243 rules, refresh boundaries and stock exhaustion.
    PhongThanCommerceMarket market(123);
    for(int s=1;s<=9;++s)for(int g=1;g<=27;++g){
        int price=market.Purchase(s,g,100),sale=market.Sale(s,g,100);
        CHECK(sale>0 && sale<100000 && market.Weight(g)>0 && market.Name(g)[0]);
        const PhongThanPriceRule &rule=pt_price_rules[s-1][g-1];
        int minimum=100000,maximum=-100000;
        for(int range=0;range<9;range+=3)if(rule.purchase[range+2]){if(rule.purchase[range]<minimum)minimum=rule.purchase[range];if(rule.purchase[range+1]>maximum)maximum=rule.purchase[range+1];}
        if(price)CHECK(price>=rule.reference+minimum && price<=rule.reference+maximum);else CHECK(market.Stock(s,g,100)==0);
    }
    CHECK(market.Stock(1,1,100)==1500);CHECK(market.ChangeStock(1,1,-1500,100)==1);CHECK(market.Stock(1,1,1279)==0);
    CHECK(!market.ChangeStock(1,1,-1,1279));CHECK(market.Stock(1,1,1280)==1500);CHECK(!market.ChangeStock(1,1,100001,1280));CHECK(!market.Purchase(0,1,100));
    // A valid numeric response dispatches the server-owned callback exactly once.
    Player[1].index=1;Player[1].m_dwID=101;Player[1].m_SelUiScriptId=99;Player[1].m_nIndex=1;
    Npc[1].m_dwID=201;Npc[1].m_SubWorldIndex=0;currentPlayer=1;
    Reset(L);Lua_PushString(L,"Quantity");Arg(L,2);Lua_PushString(L,"OK/inputok1");Lua_PushString(L,"Cancel/no_buy");
    CHECK(LuaInputDialogCompat(L)==1 && Lua_ValueToNumber(L,-1)==1);
    PHONGTHAN_UI_NUMBER_REQUEST request;memset(&request,0,sizeof(request));request.MapId=1067;request.DialogToken=Player[1].m_UiDialogToken;request.Value=10;
    CHECK(PhongThanEventNumberInput(Player[1],request));CHECK(executed==1 && executedValue==10 && !strcmp(executedCallback,"inputok1"));
    CHECK(!PhongThanEventNumberInput(Player[1],request) && executed==1);
    Reset(L);Lua_PushString(L,"Quantity");Arg(L,2);Lua_PushString(L,"OK/output1");Lua_PushString(L,"Cancel/no_sell");CHECK(LuaInputDialogCompat(L)==1);
    request.DialogToken=Player[1].m_UiDialogToken;request.Value=-1;CHECK(PhongThanEventNumberInput(Player[1],request));CHECK(executed==2 && !strcmp(executedCallback,"no_sell"));
    Reset(L);Lua_PushString(L,"Quantity");Arg(L,2);Lua_PushString(L,"OK/output1");Lua_PushString(L,"Cancel/no_sell");CHECK(LuaInputDialogCompat(L)==1);
    request.DialogToken=Player[1].m_UiDialogToken;request.Value=1001;CHECK(PhongThanEventNumberInput(Player[1],request) && executed==2);
    Reset(L);Lua_PushString(L,"Quantity");Arg(L,2);Lua_PushString(L,"OK/output1");Lua_PushString(L,"Cancel/no_sell");CHECK(LuaInputDialogCompat(L)==1);
    request.DialogToken=Player[1].m_UiDialogToken;request.Value=1;Npc[1].m_dwID=202;CHECK(PhongThanEventNumberInput(Player[1],request) && executed==2);
    CHECK(script.RegisterFunction("Finished",Finished));executeHook=FinishHook;Reset(L);Arg(L,4);CHECK(LuaRouletteCompat(L)==1 && Lua_ValueToNumber(L,-1)==1 && finished==0);
    CHECK(Player[1].m_UiDialogView==PHONGTHAN_VIEW_ROULETTE);
    request.DialogToken=Player[1].m_UiDialogToken;request.Value=11;
    CHECK(PhongThanEventNumberInput(Player[1],request) && finished==0); // Client cannot select a prize.
    request.Value=1;request.MapId=1002;CHECK(PhongThanEventNumberInput(Player[1],request) && finished==0);
    request.MapId=1067;CHECK(PhongThanEventNumberInput(Player[1],request) && finished==1);
    CHECK(!PhongThanEventNumberInput(Player[1],request) && finished==1);
    Reset(L);Arg(L,12);CHECK(LuaRouletteCompat(L)==1 && Lua_ValueToNumber(L,-1)==0 && finished==1);
    printf("PASS EVENT_LUA_COMPAT checks=%d hand_only=PASS mission_identity=PASS commodity_rules=243 input_replay=PASS roulette_dispatch=PASS\n",checks);return 0;
}
