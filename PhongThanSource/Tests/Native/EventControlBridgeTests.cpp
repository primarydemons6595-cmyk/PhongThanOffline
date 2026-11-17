#include <windows.h>
#include <stdio.h>
#include <string>
#include "KWin32.h"
#include "KLuaScript.h"
#define _SERVER
#define KMISSION_H
#define PHONGTHAN_SCRIPT_WIRE_H
enum {MAX_NPC=16,MAX_NPCSTYLE=2703,MAX_SUBWORLD=5,TASKGLOBALVALUENUM=2000,GAME_FPS=18,UI_NEWSINFO=1,NEWSMESSAGE_NORMAL=1,PHONGTHAN_SCRIPT_SHOW=1,PHONGTHAN_UI_MAX_KEY=64,PHONGTHAN_UI_MAX_CONTENT=1024,obj_npc=1};
typedef unsigned char PHONGTHAN_U8;
struct PHONGTHAN_UI_ACTION_HEADER {int x;};
struct KPhongThanScriptAction {int View,OptionCount,Operation,ServerOwned,ContentLength;char Content[1024];};
static unsigned int broadcasts;
static unsigned int PhongThanBuildScriptPacket(KPhongThanScriptAction&,PHONGTHAN_U8*,unsigned int){return 1;}
static void PhongThanQueueUiBroadcast(PHONGTHAN_U8*,unsigned int){++broadcasts;}
struct KTimerTaskFun {void SetTimer(int,int){}void SetOwner(void*){}};
struct Timers {KTimerTaskFun data;KTimerTaskFun* GetData(KTimerTaskFun*){return NULL;}void Remove(KTimerTaskFun*){}KTimerTaskFun* Add(){return &data;}};
struct KMission {Timers m_cTimerTaskSet;void StopMission(){}};
struct Missions {KMission* FindByMissionId(int){return NULL;}void Remove(KMission*){}};
struct Region {void RemoveNpc(int){}void DecRef(int,int,int){}};
struct World {int m_SubWorldID,m_nTotalRegion;Missions m_MissionArray;Region m_Region[1];} SubWorld[MAX_SUBWORLD];
struct Worlds {int SearchWorld(int map){return map==1015?0:map>=1067&&map<=1070?map-1066:-1;}} g_SubWorldSet;
struct NpcData {int m_SubWorldIndex,m_NpcSettingIdx,m_RegionIndex,m_MapX,m_MapY;unsigned long m_dwID;bool IsPlayer(){return false;}} Npc[MAX_NPC];
struct Npcs {int GetNextIdx(int){return 0;}void Remove(int){}} NpcSet;
static int g_TaskGlobalValue[TASKGLOBALVALUENUM];
#define SCRIPT_SUBWORLDINDEX "SubWorld"
static void* g_GetScript(const char*){return NULL;}
static int ReLoadScript(char*){return 0;}
#include "../../Sources/Core/Src/PhongThanEventControl.inl"
static int checks;
#define CHECK(x) do{++checks;if(!(x)){printf("FAIL line=%d %s\n",__LINE__,#x);return 1;}}while(0)
static std::string Header(long now,int rev,int leaseOffset){char h[256];sprintf(h,"PT_EVENT_SNAPSHOT\t2\t%d\t%ld\t1\n",rev,now+leaseOffset);return h;}
static std::string Rule(int enabled){char text[400];sprintf(text,"2001\t%d\t300\t1800\t8\t268\t2\t0\t0\t1\t0\t-\t-\t-\t-\t55\t20\t1\t8\t267\t0\t0\t0\t1\t100000\t0\t0\t1\t0\t0\n",enabled);return text;}
static bool Command(const char* id,long now,int pid,int revision,long expires,const char* op){char path[128],line[512];sprintf(path,"EventService/pending/%s.cmd",id);sprintf(line,"PT_EVENT_COMMAND\t1\t%s\t%d\t%ld\t%ld\t%ld\t2001\t%s\t%d\t0\n",id,pid,PtEvents::boot,now,expires,op,revision);return PtEvents::Write(path,line);}
static bool AckHas(const char* id,const char* expected){char path[128];sprintf(path,"EventService/acks/%s.json",id);std::string bytes;return PtEvents::Read(path,bytes,4096)&&bytes.find(expected)!=std::string::npos;}
int main(int argc,char** argv){
    if(argc!=2||!SetCurrentDirectoryA(argv[1]))return 2;
    CreateDirectoryA("EventService",NULL);CreateDirectoryA("EventService/pending",NULL);CreateDirectoryA("EventService/acks",NULL);
    CHECK(PtEvents::Write("EventService/catalog.tsv","2001\t3\t1015\t15\t0\t556\t0\t-1\t\t76616e5f6c756f6e67\n"));
    CHECK(PtEvents::LoadCatalog());PtEvents::managed=true;PtEvents::boot=(long)time(NULL);long now=PtEvents::boot;
    CHECK(PtEvents::Write("EventService/snapshot.tsv",Header(now,1,15)+Rule(1)));CHECK(PtEvents::LoadSnapshot(now));
    CHECK(PhongThanEventOwns(2001)&&PhongThanEventAccepting(2001));CHECK(PhongThanEventValue(2001,10)==55&&PhongThanEventValue(2001,20)==0);
    const char* id="00000000000000000000000000000001";
    CHECK(Command(id,now,GetCurrentProcessId()+1,1,now+60,"open"));PtEvents::Commands(now);CHECK(AckHas(id,"stale_server_boot")&&broadcasts==0);
    id="00000000000000000000000000000002";CHECK(Command(id,now,GetCurrentProcessId(),2,now+60,"open"));PtEvents::Commands(now);CHECK(AckHas(id,"revision_changed"));
    id="00000000000000000000000000000003";CHECK(Command(id,now,GetCurrentProcessId(),1,now-1,"open"));PtEvents::Commands(now);CHECK(AckHas(id,"expired_command"));
    id="00000000000000000000000000000004";CHECK(Command(id,now,GetCurrentProcessId(),1,now+60,"warning"));PtEvents::Commands(now);CHECK(AckHas(id,"applied")&&broadcasts==1);
    CHECK(Command(id,now,GetCurrentProcessId(),1,now+60,"warning"));PtEvents::Commands(now);CHECK(broadcasts==1);
    CHECK(PtEvents::Write("EventService/snapshot.tsv",Header(now,2,15)+Rule(0)));CHECK(PtEvents::LoadSnapshot(now)&&!PhongThanEventAccepting(2001));
    CHECK(PtEvents::Write("EventService/snapshot.tsv",Header(now,1,15)+Rule(1)));CHECK(!PtEvents::LoadSnapshot(now)&&PtEvents::revision==2);
    CHECK(PtEvents::Write("EventService/snapshot.tsv",Header(now,3,100)+Rule(1)));CHECK(!PtEvents::LoadSnapshot(now));
    CHECK(PtEvents::Write("EventService/snapshot.tsv",Header(now,3,-1)+Rule(1)));CHECK(PtEvents::LoadSnapshot(now)&&!PhongThanEventAccepting(2001));
    CHECK(PhongThanEventOwns(2001)); // User-approved: expired service never silently restores old scheduler.
    PtEvents::Status(now);std::string status;const char* leaseFalse="\"LeaseValid\":false";
    CHECK(PtEvents::Read("EventService/status.json",status,4096)&&status.find(leaseFalse)!=std::string::npos);
    printf("PASS EVENT_NATIVE_BRIDGE checks=%d production_files_commands_ack_revision_lease_idempotency\n",checks);return 0;
}
