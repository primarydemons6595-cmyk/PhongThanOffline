#include <windows.h>
#include <stdio.h>
#include <string.h>
#define MAX_PLAYER 4
#define MAX_NPC 8
#define MAX_ITEM 20
#define MAX_SUBWORLD 3
enum {item_equip=0,item_townportal=5,item_magicscript=6,item_ibitem=8,pos_equiproom=3,pos_equiproomex=4,pos_ptbag2=20,pos_ptbag3=21,pos_immediacy=22,LOCK_STATE_NORMAL=0,do_death=10,do_revive=11,SAVE_REQUEST=1};
static int assertions=0,op=0,arg=0,result=0,changeOk=1,mounted=0,nowUtc=1000,worldAvailable=1;
#define CHECK(x) do{++assertions;if(!(x)){printf("FAIL line=%d %s\n",__LINE__,#x);return 1;}}while(0)
struct Lock {int nState;};
struct KItem {int genre,detail,part,count,expiry;DWORD serial;Lock lock;
 int GetGenre(){return genre;}int GetDetailType(){return detail;}int GetParticular(){return part;}
 int GetStackNum(){return count;}void SetStackNum(int n){count=n;}int GetExpireTime(){return expiry;}DWORD GetID(){return serial;}Lock* GetLock(){return &lock;}
}Item[MAX_ITEM];
struct Bag {struct Position {int nPlace;}m_Items[MAX_ITEM];int owned[MAX_ITEM],sync;
 int FindSame(int i){return owned[i]?i:0;}void SyncItem(int){++sync;}
 bool RemoveItem(int i,int){if(!owned[i])return false;owned[i]=0;Item[i].count=0;return true;}
};
typedef Bag KItemList;
struct PLAYER_REVIVAL_POS {int m_nSubWorldID,m_nMpsX,m_nMpsY;};
struct Tasks {int v[5000];int GetSaveVal(int i){return v[i];}void SetSaveVal(int i,int n,BOOL){v[i]=n;}};
struct Move {bool bLock;int nPlace;};
struct PlayerType {int m_nIndex,m_uMustSave;bool locked,trading,m_bForbidUseTownP;Bag m_ItemList;Tasks m_cTask;Move move;PLAYER_REVIVAL_POS revive;
 bool GetLockState(){return locked;}bool CheckTrading(){return trading;}Move* GetLockMove(){return &move;}PLAYER_REVIVAL_POS* GetDeathRevivalPos(){return &revive;}
 bool ExecuteScript(char*,char*,int){return true;}
}Player[MAX_PLAYER];
struct KNpc {int m_SubWorldIndex,m_CurrentLife,m_Doing,map,x,y,fight;DWORD m_dwID;
 void GetMpsPos(int* a,int* b){*a=x;*b=y;}
 bool ChangeWorld(int m,int a,int b){if(!changeOk)return false;map=m;x=a;y=b;return true;}void SetFightMode(int f){fight=f;}
}Npc[MAX_NPC];
static int barrier=0,occupancy=0;
struct Region {int GetBarrierMin(int,int,int,int,BOOL,BOOL){return barrier;}};
struct World {Region m_Region[1];int m_SubWorldID;int TestBarrier(int,int){return barrier?barrier:occupancy;}void Mps2Map(int,int,int* r,int*,int*,int*,int*){*r=0;}}SubWorld[MAX_SUBWORLD];
struct Worlds {int SearchWorld(int){return worldAvailable?1:-1;}}g_SubWorldSet;
struct PHONGTHAN_PLAYER_NPC_STATE {int nMorphTemplate;};static PHONGTHAN_PLAYER_NPC_STATE morph;
static PHONGTHAN_PLAYER_NPC_STATE* PhongThanPlayerState(int){return &morph;}
static bool PhongThanEscortMayChangeMap(int){return !mounted;}static int KSG_GetCurSec(){return nowUtc;}
struct Lua_State{};static int GetPlayerIndex(Lua_State*){return 1;}static double Lua_ValueToNumber(Lua_State*,int i){return i==1?op:arg;}static void Lua_PushNumber(Lua_State*,int n){result=n;}
static int Lua_GetTopIndex(Lua_State*){return 2;}static bool Lua_IsNumber(Lua_State*,int){return true;}
#include "../../Sources/Core/Src/PhongThanTravelItems.inl"
static int Call(int operation,int value){op=operation;arg=value;LuaTravelItem(0);return result;}
static void Reset(int genre=8,int detail=35,int count=10){ZeroMemory(Player,sizeof(Player));ZeroMemory(Item,sizeof(Item));ZeroMemory(Npc,sizeof(Npc));ZeroMemory(PtTravel::pending,sizeof(PtTravel::pending));
 Player[1].m_nIndex=1;Player[1].revive.m_nSubWorldID=1002;Player[1].revive.m_nMpsX=51200;Player[1].revive.m_nMpsY=102400;
 Npc[1].m_CurrentLife=100;Npc[1].m_SubWorldIndex=1;Npc[1].m_dwID=100;SubWorld[1].m_SubWorldID=1015;
 Item[2].genre=genre;Item[2].detail=detail;Item[2].part=2;Item[2].count=count;Item[2].serial=200;
 Player[1].m_ItemList.owned[2]=1;Player[1].m_ItemList.m_Items[2].nPlace=3;changeOk=1;mounted=0;worldAvailable=1;morph.nMorphTemplate=0;
}
int main(){int n;
 Reset();CHECK(Call(0,2)==1);CHECK(Item[2].count==10);CHECK(Call(1,1)==1);CHECK(Item[2].count==9&&Npc[1].map==1001);CHECK(Call(1,1)==0&&Item[2].count==9);
 for(n=9;n>0;--n){CHECK(Call(0,2)==1);CHECK(Call(1,1)==1);CHECK(Item[2].count==n-1);}CHECK(!Player[1].m_ItemList.owned[2]);CHECK(Call(0,2)==0);
 Reset();CHECK(Call(0,2)==1);changeOk=0;CHECK(Call(1,1)==0&&Item[2].count==10);
 Reset();CHECK(Call(0,2)==1);worldAvailable=0;CHECK(Call(1,1)==0&&Item[2].count==10);
 Reset();CHECK(Call(0,2)==1);Player[1].m_ItemList.owned[2]=0;CHECK(Call(1,1)==0&&Item[2].count==10);
 Reset();CHECK(Call(0,2)==1);Item[2].serial++;CHECK(Call(1,1)==0&&Item[2].count==10);
 Reset();CHECK(Call(0,2)==1);Npc[1].m_dwID++;CHECK(Call(1,1)==0);
 Reset();CHECK(Call(0,2)==1);Player[1].locked=true;CHECK(Call(1,1)==0&&Item[2].count==10);
 Reset();Player[1].trading=true;CHECK(Call(0,2)==0);Reset();Item[2].lock.nState=-1;CHECK(Call(0,2)==0);
 Reset();Item[2].expiry=999;CHECK(Call(0,2)==0);Reset();mounted=1;CHECK(Call(0,2)==0);Reset();morph.nMorphTemplate=364;CHECK(Call(0,2)==0);
 Reset();CHECK(Call(0,2)==1);PtTravel::pending[1].until=GetTickCount()-1;CHECK(Call(1,1)==0);
 Reset();CHECK(Call(0,2)==1);Npc[1].m_SubWorldIndex=2;CHECK(Call(1,1)==0);
 Reset();SubWorld[1].m_SubWorldID=1072;CHECK(Call(0,2)==0);
 for(n=0;n<PT_TRAVEL_DESTINATION_COUNT;++n){const PtTravelDestination& d=ptTravelDestinations[n];Reset(8,d.profile==1?35:117+d.profile,10);CHECK(Call(0,2)==d.profile);CHECK(Call(1,n+1)==1);CHECK(Item[2].count==9&&Npc[1].map==d.map&&Npc[1].x==d.x*32&&Npc[1].y==d.y*32);}
 Reset(8,119);CHECK(Call(0,2)==2);CHECK(Call(1,1)==0&&Item[2].count==10);
 for(n=0;n<PT_TRAVEL_DESTINATION_COUNT;++n){const PtTravelDestination& d=ptTravelDestinations[n];Reset(8,d.profile<=5?9582:9583,20);CHECK(Call(0,2)==(d.profile<=5?9:10));CHECK(Call(1,n+1)==(d.profile==1?0:1));}
 Reset(6,5658,1);CHECK(Call(0,2)==1);CHECK(Item[2].count==1);CHECK(Call(1,1)==1&&!Player[1].m_ItemList.owned[2]);
 Reset(6,264,1);CHECK(Call(0,2)==1);CHECK(Item[2].count==1);CHECK(Call(1,1)==1&&!Player[1].m_ItemList.owned[2]);
 Reset(8,291,100);CHECK(Call(0,2)==8);CHECK(Call(1,0)==1&&Item[2].count==99&&Npc[1].map==1002);
 Reset(5,0,1);CHECK(Call(0,2)==8);CHECK(Call(1,0)==1&&!Player[1].m_ItemList.owned[2]);
 Reset(8,258);CHECK(Call(0,2)==11);CHECK(Call(1,5)==0&&Item[2].count==10);Player[1].m_cTask.v[333]=1;CHECK(Call(1,5)==1&&Item[2].count==9&&Npc[1].map==1002);
 Reset(8,999);CHECK(Call(0,2)==0);CHECK(PhongThanOpenTravelItem(1,2)==-1);
 printf("PASS travel %d native checks, all %d original destinations, charge/replay/ownership/failure\n",assertions,PT_TRAVEL_DESTINATION_COUNT);int map,x,y;
 for(int detail=30;detail<=391;detail+=361){
 Reset(8,detail,10);Npc[1].x=50000;Npc[1].y=100000;CHECK(Call(0,2)==12);
 occupancy=2;
 for(int slot=1;slot<=5;++slot){CHECK(PtTravel::Compass(1,1,slot,map,x,y)==1);CHECK(map==1015&&x==50000&&y==100000&&Item[2].count==10);}
 occupancy=0;
 ZeroMemory(PtTravel::pending,sizeof(PtTravel::pending));CHECK(!PtTravel::Compass(1,0,1,map,x,y));CHECK(Call(0,2)==12);CHECK(PtTravel::Compass(1,0,1,map,x,y)==1&&map==1015);
 Player[1].m_cTask.v[714]=15;CHECK(PtTravel::Compass(1,0,1,map,x,y)==1&&map==1015);
 barrier=1;CHECK(!PtTravel::Compass(1,2,1,map,x,y)&&Item[2].count==10);barrier=0;
 changeOk=0;CHECK(!PtTravel::Compass(1,2,1,map,x,y)&&Item[2].count==10);changeOk=1;
 for(int left=10;left>0;--left){CHECK(Call(0,2)==12);CHECK(PtTravel::Compass(1,2,1,map,x,y)==1);CHECK(Item[2].count==left-1);CHECK(!PtTravel::Compass(1,2,1,map,x,y));}
 CHECK(!Player[1].m_ItemList.owned[2]);
 Reset(8,detail,10);CHECK(Call(0,2)==12);SubWorld[1].m_SubWorldID=1067;CHECK(!PtTravel::Compass(1,1,1,map,x,y)&&!Player[1].m_cTask.v[714]);
 Reset(8,detail,10);CHECK(Call(0,2)==12);Npc[1].x=50000;Npc[1].y=100000;CHECK(PtTravel::Compass(1,1,1,map,x,y));CHECK(PtTravel::Compass(1,3,1,map,x,y)&&!Player[1].m_cTask.v[714]&&Item[2].count==10);
 }
 Reset(8,258,10);morph.nMorphTemplate=364;Player[1].m_cTask.v[333]=1;CHECK(Call(0,2)==11);CHECK(Call(1,5)==1&&Item[2].count==9);
 Reset(8,258,10);morph.nMorphTemplate=364;CHECK(Call(0,2)==11);CHECK(!Call(1,5)&&Item[2].count==10);
 puts("PASS compass: five persistent slots, runtime/content IDs, exact ten charges, failed map/barrier rollback, clearing, session and forbidden-map guards; camel commerce return");
 return 0;
}
