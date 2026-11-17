// Production packet declarations and handlers, isolated world connections.
#define g_pServer UnusedNativeServer
#define Npc UnusedNativeNpcs
#define SubWorld UnusedNativeWorlds
#include "KCore.h"
#undef g_pServer
#undef Npc
#undef SubWorld
#include "KPlayerDef.h"
#include "../../Headers/PhongThanRemotePeople.h"
#include "../../Headers/PhongThanChatRouting.h"
#include <assert.h>
#undef MAX_PLAYER
#undef MAX_NPC
#undef MAX_SUBWORLD
#define MAX_PLAYER 8
#define MAX_NPC 8
#define MAX_SUBWORLD 4
#define MAX_TEAM 8
static DWORD tick=10000;
static DWORD PtTestTick(){return tick;}
#define GetTickCount PtTestTick
struct FakeTeamInfo {
 int m_nFlag,m_nID,m_nFigure,invited;bool accept;
 BOOL GetCreatTeamFlag(){return accept;}
 void InviteAdd(int,TEAM_INVITE_ADD_COMMAND* q,int target){assert(q->m_dwNpcID==200);invited=target;}
};
struct FakePlayer {
 bool m_PlayerDBLoad,m_bExchangeServer,m_bIsBot,trading,locked;
 DWORD m_dwID;int m_nIndex,m_nNetConnectIdx,viewed,joined;char Name[32];
 FakeTeamInfo m_cTeam;struct {int m_nState;}m_cMenuState;
 bool CheckTrading(){return trading;}bool GetLockState(){return locked;}
 void SendEquipItemInfo(int p){viewed=p;}
 BOOL S2CSendAddTeamInfo(BYTE*,int target){joined=target;return TRUE;}
}Player[MAX_PLAYER];
struct FakeNpc {DWORD m_dwID;int m_SubWorldIndex,m_RegionIndex,m_Kind;char Name[32];}Npc[MAX_NPC];
struct FakeWorld {int m_SubWorldID;}SubWorld[MAX_SUBWORLD];
struct FakeTeam {bool open;bool IsOpen(){return open;}}g_Team[MAX_TEAM];
struct FakeServer {
 int count,connections[16],sizes[16];char packets[16][2048];
 void Clear(){count=0;}
 int PackDataToClient(int connection,const void* data,int size){assert(count<16&&size<2048);connections[count]=connection;sizes[count]=size;memcpy(packets[count++],data,size);return 0;}
}server;
FakeServer* g_pServer=&server;
#ifndef PT_REMOTE_CLIENT_TEST
static int botMessages=0;
int PhongThanBotWhisper(int,const char* name,const char*,int){if(!stricmp(name,"TestBot")){++botMessages;return 1;}return 0;}
int PhongThanBotNearby(int,const char*,int){++botMessages;return 1;}
#include "../../Sources/Core/Src/PhongThanRemotePeople.inl"
#include "../../Sources/Core/Src/PhongThanNativeWhisper.inl"
#include "../../Sources/Core/Src/PhongThanBotNearbyChat.inl"
#else
#undef _SERVER
struct FakeClient {int count;PT_PEOPLE_COMMAND last;int SendPackToServer(const void* data,int size){assert(size==sizeof(last));memcpy(&last,data,size);++count;return 0;}}client;
FakeClient* g_pClient=&client;
#include "../../Sources/Core/Src/PhongThanRemotePeople.inl"
#endif
static int checks=0;
#define CHECK(x) do{++checks;if(!(x)){printf("FAIL %d: %s\n",__LINE__,#x);return 1;}}while(0)
static void Setup(){
 ZeroMemory(Player,sizeof(Player));ZeroMemory(Npc,sizeof(Npc));ZeroMemory(g_Team,sizeof(g_Team));server.Clear();tick+=1000;
 for(int i=1;i<=2;++i){Player[i].m_PlayerDBLoad=true;Player[i].m_dwID=100+i;Player[i].m_nIndex=i;Player[i].m_nNetConnectIdx=i;Player[i].m_cTeam.accept=true;
  Npc[i].m_dwID=i*100;Npc[i].m_Kind=kind_player;Npc[i].m_SubWorldIndex=i-1;Npc[i].m_RegionIndex=1;SubWorld[i-1].m_SubWorldID=1000+i;}
 strcpy(Player[1].Name,"Alice");strcpy(Player[2].Name,"Bob");strcpy(Npc[1].Name,"Alice");strcpy(Npc[2].Name,"Bob");
}
int main(){
 Setup();PT_PEOPLE_COMMAND q;ZeroMemory(&q,sizeof(q));q.ActorId=100;q.MapId=1001;q.Action=6;strcpy(q.Name,"Bob");
#ifndef PT_REMOTE_CLIENT_TEST
 for(int action=0;action<20;++action){Setup();q.Action=action;Player[2].m_PlayerDBLoad=false;PhongThanRemotePeopleRequest(1,q);
  CHECK(server.count==(PtPeopleActionAllowed(action)?1:0));if(server.count)CHECK(((PT_PEOPLE_RESULT*)server.packets[0])->Result==0);}
 Setup();q.Action=6;PhongThanRemotePeopleRequest(1,q);CHECK(server.count==1&&Player[2].viewed==1);CHECK(((PT_PEOPLE_RESULT*)server.packets[0])->Result==2);
 Setup();q.TargetId=201;PhongThanRemotePeopleRequest(1,q);CHECK(!Player[2].viewed&&((PT_PEOPLE_RESULT*)server.packets[0])->Result==0);q.TargetId=0;
 Setup();q.ActorId=999;PhongThanRemotePeopleRequest(1,q);CHECK(!server.count);q.ActorId=100;
 Setup();q.MapId=999;PhongThanRemotePeopleRequest(1,q);CHECK(!server.count);q.MapId=1001;
 Setup();Player[2].m_bExchangeServer=true;PhongThanRemotePeopleRequest(1,q);CHECK(((PT_PEOPLE_RESULT*)server.packets[0])->Result==0);
 Setup();Player[2].m_nNetConnectIdx=-1;PhongThanRemotePeopleRequest(1,q);CHECK(((PT_PEOPLE_RESULT*)server.packets[0])->Result==0);
 Setup();q.Action=0;Player[2].m_nNetConnectIdx=-999;Player[2].m_bIsBot=true;PhongThanRemotePeopleRequest(1,q);CHECK(server.count==1&&((PT_PEOPLE_RESULT*)server.packets[0])->Result==1&&((PT_PEOPLE_RESULT*)server.packets[0])->TargetId==200);
 Setup();Player[2].m_nNetConnectIdx=-999;Player[2].m_bIsBot=true;Player[2].m_PlayerDBLoad=false;PhongThanRemotePeopleRequest(1,q);CHECK(((PT_PEOPLE_RESULT*)server.packets[0])->Result==0);q.Action=6;
 Setup();Npc[2].m_RegionIndex=-1;PhongThanRemotePeopleRequest(1,q);CHECK(((PT_PEOPLE_RESULT*)server.packets[0])->Result==0);
 Setup();Npc[2].m_Kind=kind_normal;PhongThanRemotePeopleRequest(1,q);CHECK(((PT_PEOPLE_RESULT*)server.packets[0])->Result==0);
 Setup();memset(q.Name,'x',32);PhongThanRemotePeopleRequest(1,q);CHECK(!server.count);strcpy(q.Name,"Bob");
 Setup();Player[1].locked=true;PhongThanRemotePeopleRequest(1,q);CHECK(!Player[2].viewed&&((PT_PEOPLE_RESULT*)server.packets[0])->Result==-2);
 Setup();q.Action=3;Player[2].m_cTeam.m_nFlag=1;Player[2].m_cTeam.m_nFigure=TEAM_CAPTAIN;Player[2].m_cTeam.m_nID=2;g_Team[2].open=true;
 PhongThanRemotePeopleRequest(1,q);CHECK(Player[1].joined==2&&((PT_PEOPLE_RESULT*)server.packets[0])->Result==2);
 tick+=1000;server.Clear();g_Team[2].open=false;Player[1].joined=0;PhongThanRemotePeopleRequest(1,q);CHECK(!Player[1].joined&&((PT_PEOPLE_RESULT*)server.packets[0])->Result==-4);
 Setup();q.Action=4;Player[1].m_cTeam.m_nFlag=1;Player[1].m_cTeam.m_nFigure=TEAM_CAPTAIN;Player[1].m_cTeam.m_nID=1;g_Team[1].open=true;
 PhongThanRemotePeopleRequest(1,q);CHECK(Player[1].m_cTeam.invited==2&&((PT_PEOPLE_RESULT*)server.packets[0])->Result==2);
 char bytes[sizeof(tagExtendProtoHeader)+sizeof(CHAT_SOMEONECHAT_CMD)+255];ZeroMemory(bytes,sizeof(bytes));
 tagExtendProtoHeader* h=(tagExtendProtoHeader*)bytes;h->ProtocolType=c2s_extendchat;CHAT_SOMEONECHAT_CMD* msg=(CHAT_SOMEONECHAT_CMD*)(h+1);msg->ProtocolType=chat_someonechat;msg->packageID=42;strcpy(msg->someone,"Bob");msg->sentlen=5;memcpy(msg+1,"hello",5);
 int size=sizeof(*h)+sizeof(*msg)+5;h->wLength=size-1;msg->wSize=sizeof(*msg)+4;
 CHECK(PhongThanChatWireKind(bytes,size)==PT_CHAT_WIRE_PRIVATE);
 CHECK(PhongThanChatWireKind(bytes,size-1)==PT_CHAT_WIRE_INVALID);
 Setup();CHECK(PhongThanProcessNativeWhisper(1,bytes,size));CHECK(server.count==2&&server.connections[0]==2&&server.connections[1]==1);
 CHAT_SOMEONECHAT_SYNC* received=(CHAT_SOMEONECHAT_SYNC*)((tagExtendProtoHeader*)server.packets[0]+1);CHECK(!strcmp(received->someone,"Alice")&&received->sentlen==5&&!memcmp(received+1,"hello",5));
 CHAT_FEEDBACK* feedback=(CHAT_FEEDBACK*)((tagExtendProtoHeader*)server.packets[1]+1);CHECK(feedback->code==codeSucc&&feedback->packageID==42);
 CHECK(*(DWORD*)(feedback+1)==(DWORD)-1);CHECK(!strcmp((char*)(feedback+1)+sizeof(DWORD),"Bob"));
 Setup();Player[2].m_PlayerDBLoad=false;CHECK(PhongThanProcessNativeWhisper(1,bytes,size));CHECK(server.count==1&&server.connections[0]==1);feedback=(CHAT_FEEDBACK*)((tagExtendProtoHeader*)server.packets[0]+1);CHECK(feedback->code==codeFail);
 Setup();h->wLength--;CHECK(PhongThanProcessNativeWhisper(1,bytes,size)&&server.count==0);h->wLength++;
 Setup();memset(msg->someone,'x',sizeof(msg->someone));CHECK(PhongThanProcessNativeWhisper(1,bytes,size)&&server.count==0);strcpy(msg->someone,"Bob");
 Setup();memset(msg->item,'x',sizeof(msg->item));CHECK(PhongThanProcessNativeWhisper(1,bytes,size)&&server.count==0);memset(msg->item,0,sizeof(msg->item));
 Setup();CHECK(PhongThanProcessNativeWhisper(1,bytes,size));server.Clear();CHECK(PhongThanProcessNativeWhisper(1,bytes,size)&&server.count==0);
 Setup();msg->ProtocolType=chat_channelchat;CHECK(!PhongThanProcessNativeWhisper(1,bytes,size)&&!server.count);msg->ProtocolType=chat_someonechat;
 Setup();msg->sentlen=255;memset(msg+1,'z',255);size=sizeof(*h)+sizeof(*msg)+255;h->wLength=size-1;msg->wSize=sizeof(*msg)+254;CHECK(PhongThanProcessNativeWhisper(1,bytes,size)&&server.count==2);
 Setup();strcpy(msg->someone,"testbot");botMessages=0;CHECK(PhongThanProcessNativeWhisper(1,bytes,size)&&botMessages==1&&server.count==1);feedback=(CHAT_FEEDBACK*)((tagExtendProtoHeader*)server.packets[0]+1);CHECK(feedback->code==codeSucc);
 char nearBytes[sizeof(tagExtendProtoHeader)+sizeof(CHAT_CHANNELCHAT_CMD)+5];ZeroMemory(nearBytes,sizeof(nearBytes));tagExtendProtoHeader* nearHead=(tagExtendProtoHeader*)nearBytes;nearHead->ProtocolType=c2s_extendchat;nearHead->wLength=sizeof(nearBytes)-1;CHAT_CHANNELCHAT_CMD* nearMsg=(CHAT_CHANNELCHAT_CMD*)(nearHead+1);nearMsg->ProtocolType=chat_channelchat;nearMsg->wSize=sizeof(*nearMsg)+4;nearMsg->channelid=1;nearMsg->sentlen=5;memcpy(nearMsg+1,"hello",5);
 CHECK(PhongThanChatWireKind(nearBytes,sizeof(nearBytes))==PT_CHAT_WIRE_CHANNEL);Setup();botMessages=0;CHECK(PhongThanProcessBotNearbyChat(1,nearBytes,sizeof(nearBytes))&&botMessages==1);nearMsg->channelid=3;CHECK(!PhongThanProcessBotNearbyChat(1,nearBytes,sizeof(nearBytes)));nearMsg->channelid=1;nearMsg->sentlen=6;CHECK(PhongThanProcessBotNearbyChat(1,nearBytes,sizeof(nearBytes))&&botMessages==1);
 printf("PASS REMOTE_SERVER_WHISPER %d checks: online/offline, forged identity, remote view/team, private delivery+echo, no bystanders, validation/rate/255 bytes\n",checks);
#else
 CHECK(PhongThanRemotePeopleAction(&q));CHECK(client.count==1&&client.last.ActorId==100&&client.last.MapId==1001);
 PT_PEOPLE_RESULT out,r;ZeroMemory(&r,sizeof(r));r.Header.Sequence=client.last.Header.Sequence;r.ActorId=100;r.MapId=1001;r.Action=6;r.Result=1;strcpy(r.Name,"Bob");
 CHECK(!PhongThanRemotePeopleGet(&out));r.Header.Sequence--;PhongThanRemotePeopleReceive(r);CHECK(!PhongThanRemotePeopleGet(&out));r.Header.Sequence++;
 r.ActorId=999;PhongThanRemotePeopleReceive(r);CHECK(!PhongThanRemotePeopleGet(&out));r.ActorId=100;
 r.MapId=999;PhongThanRemotePeopleReceive(r);CHECK(!PhongThanRemotePeopleGet(&out));r.MapId=1001;
 strcpy(r.Name,"Eve");PhongThanRemotePeopleReceive(r);CHECK(!PhongThanRemotePeopleGet(&out));strcpy(r.Name,"Bob");
 r.Action=3;PhongThanRemotePeopleReceive(r);CHECK(!PhongThanRemotePeopleGet(&out));r.Action=6;
 PhongThanRemotePeopleReceive(r);CHECK(PhongThanRemotePeopleGet(&out)&&!strcmp(out.Name,"Bob"));
 CHECK(PhongThanRemotePeopleAction(&q));CHECK(!PhongThanRemotePeopleGet(&out));r.Header.Sequence=client.last.Header.Sequence;r.Result=0;PhongThanRemotePeopleReceive(r);CHECK(PhongThanRemotePeopleGet(&out)&&!out.Result);
 Npc[1].m_SubWorldIndex=1;CHECK(!PhongThanRemotePeopleGet(&out));Npc[1].m_SubWorldIndex=0;Npc[1].m_dwID=999;CHECK(!PhongThanRemotePeopleGet(&out));
 q.Action=2;CHECK(!PhongThanRemotePeopleAction(&q));q.Action=12;CHECK(!PhongThanRemotePeopleAction(&q));
 printf("PASS REMOTE_CLIENT %d checks: pending reset, latest sequence/name/action/actor/map, offline result, forbidden actions\n",checks);
#endif
 return 0;
}
