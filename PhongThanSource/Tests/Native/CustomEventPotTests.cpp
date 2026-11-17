#include <windows.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <map>
#include <time.h>
#define _SERVER
#include "../../Sources/Core/Src/PhongThanCustomEvents.h"
#include "../../Headers/PhongThanPotProgress.h"
#define MAX_NPC 64
#define MAX_PLAYER 3
#define MAX_SUBWORLD 2
#define kind_dialoger 3
#define camp_free 0
#define obj_npc 1
struct Region {void RemoveNpc(int){}void DecRef(int,int,int){}};
struct World {int m_SubWorldID,m_nTotalRegion;Region m_Region[1];int GetBarrier(int,int);};
static bool blocked=false,spawnFailed=false,owned=true;int World::GetBarrier(int,int){return blocked?1:0;}
struct KNpc {DWORD m_dwID,m_ActionScriptID,m_DeathScriptID,m_TimerScriptID,m_DropScriptID;int m_NpcSettingIdx,m_Kind,m_SubWorldIndex,m_RegionIndex,m_MapX,m_MapY;char Name[32],ActionScript[120];bool human;bool IsPlayer(){return human;}void SendDataToNearRegion(BYTE*,int){}void SetFightMode(int){}void SetCurrentCamp(int){}void GetMpsPos(int*x,int*y){*x=m_MapX;*y=m_MapY;}};
static KNpc Npc[MAX_NPC];static World SubWorld[MAX_SUBWORLD];
struct Actor {int m_nIndex,m_nLastNpcIndex;const char* key;const char* Name;};static Actor Player[4];
struct Worlds {int SearchWorld(int map){return map==1?0:-1;}} g_SubWorldSet;
struct Npcs {int Add(int templateId,int w,int x,int y,BOOL){if(spawnFailed)return 0;for(int i=3;i<MAX_NPC;++i)if(!Npc[i].m_dwID){static DWORD next=100;KNpc& n=Npc[i];ZeroMemory(&n,sizeof(n));n.m_dwID=++next;n.m_NpcSettingIdx=HIWORD(templateId);n.m_SubWorldIndex=w;n.m_RegionIndex=0;n.m_MapX=x;n.m_MapY=y;return i;}return 0;}void Remove(int i){ZeroMemory(&Npc[i],sizeof(Npc[i]));}int GetDistance(int a,int b){int dx=abs(Npc[a].m_MapX-Npc[b].m_MapX),dy=abs(Npc[a].m_MapY-Npc[b].m_MapY);return dx+dy;}} NpcSet;
static DWORD g_FileName2Id(char* s){DWORD n=0;while(*s)n=n*31+(unsigned char)*s++;return n;}
static PTCustomEvent eventConfig;const PTCustomEvent* PhongThanCustomEvent(int id){return id==eventConfig.id?&eventConfig:0;}const PTCustomRecipe* PhongThanCustomRecipe(int){return 0;}
namespace PtCustomActions {
static unsigned long potNow=100000;static time_t TestPotTime(time_t*){return potNow;}
#define time TestPotTime
static bool Ready(int p){return p>0&&p<3;}static bool OwnsTool(int,const PTCustomEvent&){return owned&&eventConfig.enabled;}
static std::string Format(std::string text,const PTCustomRecipe&,unsigned long){return text;}static void Tell(int,const char*){}
static bool JobPath(int p,int eventId,int,char* path){sprintf(path,"EventService\\CustomEvents\\jobs\\%s-%d-0.ini",Player[p].key,eventId);return true;}
#include "../../Sources/Core/Src/PhongThanCustomEventPots.inl"
#undef time
}
#define CHECK(c) do{if(!(c)){printf("FAIL line %d: %s\n",__LINE__,#c);return 1;}++checks;}while(0)
int main(int argc,char** argv){if(argc!=2)return 2;CreateDirectoryA(argv[1],0);SetCurrentDirectoryA(argv[1]);CreateDirectoryA("EventService",0);CreateDirectoryA("EventService\\CustomEvents",0);using namespace PtCustomActions;int checks=0;
 eventConfig.id=1;eventConfig.enabled=true;eventConfig.toolNpc=261;eventConfig.toolScript="\\script\\phongthan\\custom_events\\e1\\v1\\tool.lua";SubWorld[0].m_SubWorldID=1;SubWorld[0].m_nTotalRegion=1;
 eventConfig.toolName="Noi Nau Banh";
 Player[1].m_nIndex=1;Player[1].key="alice-5175616e6748757933-99";Player[1].Name="QuangHuy3";Player[2].m_nIndex=2;Player[2].key="bob";Player[2].Name="Bob";Npc[1].m_dwID=1;Npc[1].human=true;Npc[1].m_MapX=Npc[1].m_MapY=1000;Npc[2]=Npc[1];Npc[2].m_dwID=2;
 CHECK(PlacePot(1,eventConfig)&&pots.size()==1&&LivePot(pots[0]));CHECK(Npc[pots[0].index].m_NpcSettingIdx==261&&Npc[pots[0].index].m_Kind==kind_dialoger);
 CHECK(!strcmp(Npc[pots[0].index].Name,"Noi Nau Banh (QuangHuy3)"));
 CHECK(PlacePot(2,eventConfig)&&pots.size()==2&&!strcmp(Npc[pots[1].index].Name,"Noi Nau Banh (Bob)"));FinishPot(2,1,0);CHECK(pots.size()==1);
 CHECK(!ToolAccess(1,eventConfig));Player[1].m_nLastNpcIndex=pots[0].index;CHECK(ToolAccess(1,eventConfig));Player[2].m_nLastNpcIndex=pots[0].index;CHECK(!ToolAccess(2,eventConfig));
 owned=false;CHECK(ToolAccess(1,eventConfig));owned=true;Npc[1].m_MapX+=1000;CHECK(!ToolAccess(1,eventConfig));Npc[1].m_MapX-=1000;
 blocked=true;DWORD before=pots[0].entity;CHECK(!PlacePot(1,eventConfig)&&pots[0].entity==before);blocked=false;
 CHECK(PlacePot(1,eventConfig)&&pots.size()==1&&pots[0].entity!=before);Player[1].m_nLastNpcIndex=pots[0].index;
 eventConfig.enabled=false;TickPots();CHECK(!LivePot(pots[0]));eventConfig.enabled=true;TickPots();CHECK(LivePot(pots[0]));
 eventConfig.toolNpc=262;TickPots();CHECK(LivePot(pots[0])&&Npc[pots[0].index].m_NpcSettingIdx==262);
 int reused=pots[0].index;Npc[reused].m_dwID=999;Npc[reused].human=true;eventConfig.enabled=false;TickPots();CHECK(Npc[reused].human&&Npc[reused].m_dwID==999);eventConfig.enabled=true;
 pots.clear();potsLoaded=false;TickPots();CHECK(pots.size()==1&&LivePot(pots[0])&&Npc[pots[0].index].m_NpcSettingIdx==262);
 CHECK(!strcmp(Npc[pots[0].index].Name,"Noi Nau Banh (QuangHuy3)"));
 WritePrivateProfileStringA("Pot","OwnerHex",0,pots[0].path.c_str());pots.clear();potsLoaded=false;TickPots();CHECK(pots.size()==1&&pots[0].owner=="QuangHuy3");
 unsigned long placed=pots[0].placed;std::string idlePath=pots[0].path;CHECK(placed==potNow);potNow+=299;TickPots();CHECK(pots.size()==1);Player[1].m_nLastNpcIndex=pots[0].index;CHECK(ToolAccess(1,eventConfig));potNow++;CHECK(!ToolAccess(1,eventConfig));TickPots();CHECK(pots.empty()&&GetFileAttributesA(idlePath.c_str())==(DWORD)-1);
 CHECK(PlacePot(1,eventConfig));placed=pots[0].placed;RemovePot(pots[0]);pots.clear();potsLoaded=false;potNow+=150;TickPots();CHECK(pots.size()==1&&pots[0].placed==placed);potNow+=150;TickPots();CHECK(pots.empty());
 eventConfig.toolIdleSeconds=60;CHECK(PlacePot(1,eventConfig));potNow+=60;TickPots();CHECK(pots.empty());eventConfig.toolIdleSeconds=300;
 CHECK(PlacePot(1,eventConfig));pots[0].recipe=1;pots[0].started=potNow;pots[0].ready=potNow+600;pots[0].burn=potNow+900;CHECK(SavePot(pots[0]));potNow+=301;TickPots();CHECK(pots.size()==1&&pots[0].recipe==1);potNow+=599;TickPots();CHECK(pots.empty());
 CHECK(PlacePot(1,eventConfig));pots[0].placed=0;CHECK(SavePot(pots[0]));RemovePot(pots[0]);pots.clear();potsLoaded=false;TickPots();CHECK(pots.size()==1&&pots[0].placed==potNow);placed=potNow;RemovePot(pots[0]);pots.clear();potsLoaded=false;potNow+=200;TickPots();CHECK(pots.size()==1&&pots[0].placed==placed);potNow+=100;TickPots();CHECK(pots.empty());
 CHECK(PlacePot(1,eventConfig));eventConfig.id=2;TickPots();CHECK(pots.empty()&&Npc[reused].human);printf("PASS CUSTOM_EVENT_POTS checks=%d production_include=1\n",checks);return 0;
}
