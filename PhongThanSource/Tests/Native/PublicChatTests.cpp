#define g_pServer HiddenServer
#define Npc HiddenNpc
#include "KCore.h"
#undef g_pServer
#undef Npc
#include "../../Headers/PhongThanCharacter.h"
#undef MAX_PLAYER
#define MAX_PLAYER 8
struct TestTong {DWORD m_dwTongNameID;int m_nFlag;};
struct TestPk {int aim,spar;int GetExercisePKAim(){return aim;}int IsEnmitySpar(){return spar;}};
struct TestPlayer {DWORD m_dwID;BYTE m_SessionTicket[PHONGTHAN_SESSION_TICKET_SIZE];int m_nIndex,m_nNetConnectIdx;bool m_bExchangeServer,m_bIsBot,m_PlayerDBLoad;TestTong m_cTong;TestPk m_cPK;}Player[MAX_PLAYER];
struct TestNpc {int m_SubWorldIndex,m_CurrentLife,m_Level,m_nPlayerIdx,m_FightMode,m_nCurPKPunishState;char Name[32];bool IsPlayer(){return m_nPlayerIdx>0;}int GetPlayerIdx(){return m_nPlayerIdx;}}Npc[MAX_PLAYER];
struct TestServer {int count,connections[16],sizes[16];char packets[16][2048];int PackDataToClient(int p,const void* data,int size){assert(count<16&&size<2048);connections[count]=p;sizes[count]=size;memcpy(packets[count++],data,size);return 0;}void Clear(){count=0;}}server;
TestServer* g_pServer=&server;
struct TestPlayerSet {int current;int GetFirstPlayer(){current=1;return current;}int GetNextPlayer(){return ++current<MAX_PLAYER?current:0;}}PlayerSet;
static bool PTNationReady(){return false;}static int PTNationSelf(int){return -1;}
struct {struct {struct {unsigned NationId;}Members[8];}State;}s_PtNation;
static const char* s_PTFriendFile="nonexistent-test-friends.ini";
static void PTFriendKey(const char* name,char* key){strcpy(key,name);}
static bool PhongThanConsumableChatColor(const TestPlayer&){return false;}
static DWORD now=1000;static DWORD TestTick(){return now;}
#define GetTickCount TestTick
int PhongThanBotPublicChat(int,DWORD,const char*,int){return 0;}
#include "../../Sources/Core/Src/PhongThanPublicChat.inl"
#undef GetTickCount
static int checks=0;
#define CHECK(c) do{++checks;if(!(c)){printf("FAIL line %d: %s\n",__LINE__,#c);return 1;}}while(0)
static void Setup(){ZeroMemory(Player,sizeof(Player));ZeroMemory(Npc,sizeof(Npc));ZeroMemory(s_PublicChat,sizeof(s_PublicChat));server.Clear();for(int i=1;i<=3;++i){Player[i].m_dwID=100+i;Player[i].m_nIndex=i;Player[i].m_nNetConnectIdx=i;Npc[i].m_SubWorldIndex=i;sprintf(Npc[i].Name,"Player%d",i);}}
static DWORD Query(const char* name){char packet[sizeof(tagExtendProtoHeader)+sizeof(PLAYERCOMM_QUERYCHANNELID)];ZeroMemory(packet,sizeof(packet));tagExtendProtoHeader* h=(tagExtendProtoHeader*)packet;h->ProtocolType=c2s_extend;h->wLength=sizeof(packet)-1;PLAYERCOMM_QUERYCHANNELID* q=(PLAYERCOMM_QUERYCHANNELID*)(h+1);q->ProtocolFamily=pf_playercommunity;q->ProtocolID=playercomm_c2s_querychannelid;strcpy(q->channel,name);server.Clear();PhongThanProcessPublicChat(1,packet,sizeof(packet));return server.count==1?((PLAYERCOMM_NOTIFYCHANNELID*)(server.packets[0]+sizeof(*h)))->channelid:0;}
static bool Subscribe(int player,DWORD channel,bool value){char packet[sizeof(tagExtendProtoHeader)+sizeof(PLAYERCOMM_SUBSCRIBE)];ZeroMemory(packet,sizeof(packet));tagExtendProtoHeader* h=(tagExtendProtoHeader*)packet;h->ProtocolType=c2s_extend;h->wLength=sizeof(packet)-1;PLAYERCOMM_SUBSCRIBE* q=(PLAYERCOMM_SUBSCRIBE*)(h+1);q->ProtocolFamily=pf_playercommunity;q->ProtocolID=playercomm_c2s_subscribe;q->channelid=channel;q->subscribe=value;return PhongThanProcessPublicChat(player,packet,sizeof(packet));}
static bool Say(int player,DWORD channel,int cost=0){char packet[sizeof(tagExtendProtoHeader)+sizeof(CHAT_CHANNELCHAT_CMD)+2];ZeroMemory(packet,sizeof(packet));tagExtendProtoHeader* h=(tagExtendProtoHeader*)packet;h->ProtocolType=c2s_extendchat;h->wLength=sizeof(packet)-1;CHAT_CHANNELCHAT_CMD* q=(CHAT_CHANNELCHAT_CMD*)(h+1);q->ProtocolType=chat_channelchat;q->wSize=sizeof(*q)+1;q->channelid=channel;q->sentlen=2;q->cost=cost;memcpy(q+1,"hi",2);return PhongThanProcessPublicChat(player,packet,sizeof(packet));}
int main(){Setup();CHECK(Query("\xBD\xBB\xD2\xD7")==PT_CHAT_TRADE);CHECK(Query("\xCA\xC0\xBD\xE7")==PT_CHAT_WORLD);CHECK(PT_CHAT_TRADE!=PT_CHAT_WORLD);CHECK(Subscribe(1,PT_CHAT_WORLD,true));CHECK(Subscribe(2,PT_CHAT_WORLD,true));CHECK(Subscribe(3,PT_CHAT_TRADE,true));server.Clear();CHECK(Say(1,PT_CHAT_WORLD));CHECK(server.count==2&&server.connections[0]==1&&server.connections[1]==2);server.Clear();CHECK(Say(3,PT_CHAT_TRADE));CHECK(server.count==1&&server.connections[0]==3);CHECK(Subscribe(2,PT_CHAT_WORLD,false));server.Clear();Say(1,PT_CHAT_WORLD);CHECK(server.count==1&&server.connections[0]==1);server.Clear();Say(1,PT_CHAT_WORLD);CHECK(server.count==0);now+=90000;Say(1,PT_CHAT_WORLD);CHECK(server.count==1);server.Clear();Say(1,PT_CHAT_WORLD,1);CHECK(server.count==0);Player[1].m_SessionTicket[0]=1;CHECK(!PTPublicSubscribed(1,PT_CHAT_WORLD));CHECK(!PTBotPublicChannelAllowed(9,10,PT_CHAT_WORLD));CHECK(PTBotPublicChannelAllowed(10,10,PT_CHAT_WORLD));CHECK(PTBotPublicChannelAllowed(10,10,PT_CHAT_TRADE));CHECK(!PTBotPublicChannelAllowed(100,10,PT_CHAT_NATION));Setup();now+=100000;Player[4].m_dwID=404;Player[4].m_nIndex=4;Player[4].m_bIsBot=true;Player[4].m_PlayerDBLoad=true;Npc[4].m_CurrentLife=30;Npc[4].m_Level=9;Npc[4].m_SubWorldIndex=1;strcpy(Npc[4].Name,"ServerBot");Subscribe(1,PT_CHAT_WORLD,true);Subscribe(2,PT_CHAT_TRADE,true);server.Clear();CHECK(!PhongThanBotEmitPublic(4,PT_CHAT_WORLD,"hi",2));Npc[4].m_Level=10;CHECK(PhongThanBotEmitPublic(4,PT_CHAT_WORLD,"hi",2));CHECK(server.count==1&&server.connections[0]==1);CHECK(!PhongThanBotEmitPublic(4,PT_CHAT_WORLD,"hi",2));now+=60000;server.Clear();CHECK(!PhongThanBotEmitPublic(4,PT_CHAT_TRADE,"hi",2));now+=90000;CHECK(PhongThanBotEmitPublic(4,PT_CHAT_TRADE,"hi",2));CHECK(server.count==1&&server.connections[0]==2);CHECK(!PhongThanBotEmitPublic(1,PT_CHAT_WORLD,"hi",2));printf("PASS %d public-channel checks: distinct world/trade, subscriptions, session identity, rate limits and newbie policy.\n",checks);return 0;}
