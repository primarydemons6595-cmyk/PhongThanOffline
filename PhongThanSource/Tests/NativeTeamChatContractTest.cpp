// Authorization/lifecycle unit harness. Production builds validate real protocol declarations.
#include <stdio.h>
#include <string.h>
#include <assert.h>
typedef unsigned long DWORD;
typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned char PHONGTHAN_U8;
typedef long HRESULT;
#define MAX_PLAYER 5
#define MAX_TEAM 2
#define MAX_TEAM_MEMBER 2
#define MAX_NPC 5
#define PHONGTHAN_SESSION_TICKET_SIZE 16
#define TRUE 1
#define S_OK 0
enum { c2s_extend=1, c2s_extendchat, s2c_extend, s2c_extendchat,
    pf_playercommunity, playercomm_c2s_querychannelid, playercomm_s2c_notifychannelid,
    playercomm_c2s_subscribe, chat_channelchat };
#pragma pack(push, 1)
struct tagExtendProtoHeader { BYTE ProtocolType; WORD wLength; };
struct EXTEND_HEADER { BYTE ProtocolFamily, ProtocolID; };
struct PLAYERCOMM_QUERYCHANNELID : EXTEND_HEADER { char channel[32]; };
struct PLAYERCOMM_NOTIFYCHANNELID : EXTEND_HEADER { char channel[32]; DWORD channelid; BYTE cost; };
struct PLAYERCOMM_SUBSCRIBE : EXTEND_HEADER { DWORD channelid; int subscribe; };
struct CHAT_CHANNELCHAT_CMD {
    BYTE ProtocolType; WORD wSize; DWORD packageID; BYTE filter; DWORD channelid;
    BYTE cost, sentlen; char item[256]; BYTE someflag;
};
struct CHAT_CHANNELCHAT_SYNC {
    BYTE ProtocolType; WORD wSize; DWORD packageID; char someone[32]; DWORD channelid;
    BYTE sentlen; char item[256]; BYTE someflag;
};
#pragma pack(pop)
struct FakePlayerTeam { int m_nFlag, m_nID; DWORD m_dwChatGeneration; };
struct FakePlayer {
    DWORD m_dwID; int m_nIndex, m_nNetConnectIdx; bool m_bExchangeServer, m_bIsBot;
    FakePlayerTeam m_cTeam; BYTE m_SessionTicket[16];
} Player[MAX_PLAYER];
struct FakeTeam {
    int m_nCaptain, m_nMember[MAX_TEAM_MEMBER]; DWORD generation;
    DWORD GetChatGeneration() const { return generation; }
} g_Team[MAX_TEAM];
struct FakeNpc { char Name[32]; } Npc[MAX_NPC];
struct FakeServer {
    int delivered[MAX_PLAYER]; DWORD lastChannel; char lastText[256];
    HRESULT PackDataToClient(int connection, const void* bytes, int size) {
        const tagExtendProtoHeader* header = (const tagExtendProtoHeader*)bytes;
        assert(header->wLength + 1 == size);
        if (header->ProtocolType == s2c_extendchat) {
            const CHAT_CHANNELCHAT_SYNC* message = (const CHAT_CHANNELCHAT_SYNC*)(header + 1);
            assert(message->wSize + 1 == size - sizeof(*header));
            ++delivered[connection]; lastChannel=message->channelid;
            memcpy(lastText, message+1, message->sentlen); lastText[message->sentlen]=0;
        }
        return S_OK;
    }
} server;
FakeServer* g_pServer=&server;
#include "../Sources/Core/Src/PhongThanNativeTeamChat.inl"

static void Query(int player) {
    char bytes[sizeof(tagExtendProtoHeader)+sizeof(PLAYERCOMM_QUERYCHANNELID)]={0};
    tagExtendProtoHeader* header=(tagExtendProtoHeader*)bytes;
    header->ProtocolType=c2s_extend; header->wLength=sizeof(bytes)-1;
    PLAYERCOMM_QUERYCHANNELID* q=(PLAYERCOMM_QUERYCHANNELID*)(header+1);
    q->ProtocolFamily=pf_playercommunity; q->ProtocolID=playercomm_c2s_querychannelid;
    strcpy(q->channel,"\\T0");
    assert(PhongThanProcessNativeTeamChat(player,bytes,sizeof(bytes)));
}
static void Subscribe(int player,DWORD id) {
    char bytes[sizeof(tagExtendProtoHeader)+sizeof(PLAYERCOMM_SUBSCRIBE)]={0};
    tagExtendProtoHeader* header=(tagExtendProtoHeader*)bytes;
    header->ProtocolType=c2s_extend; header->wLength=sizeof(bytes)-1;
    PLAYERCOMM_SUBSCRIBE* s=(PLAYERCOMM_SUBSCRIBE*)(header+1);
    s->ProtocolFamily=pf_playercommunity; s->ProtocolID=playercomm_c2s_subscribe;
    s->channelid=id; s->subscribe=TRUE;
    assert(PhongThanProcessNativeTeamChat(player,bytes,sizeof(bytes)));
}
static void Send(int player,DWORD id,bool malformed=false) {
    char bytes[sizeof(tagExtendProtoHeader)+sizeof(CHAT_CHANNELCHAT_CMD)+3]={0};
    tagExtendProtoHeader* header=(tagExtendProtoHeader*)bytes;
    header->ProtocolType=c2s_extendchat; header->wLength=sizeof(bytes)-1;
    CHAT_CHANNELCHAT_CMD* command=(CHAT_CHANNELCHAT_CMD*)(header+1);
    command->ProtocolType=chat_channelchat; command->wSize=sizeof(*command)+2;
    command->channelid=id; command->sentlen=3; memcpy(command+1,"alo",3);
    if(malformed) --header->wLength;
    assert(PhongThanProcessNativeTeamChat(player,bytes,sizeof(bytes)));
}
int main() {
    g_Team[0].m_nCaptain=1; g_Team[0].m_nMember[0]=2; g_Team[0].m_nMember[1]=-1;
    g_Team[0].generation=1;
    for(int p=1;p<=3;++p) {
        Player[p].m_dwID=p; Player[p].m_nIndex=p; Player[p].m_nNetConnectIdx=p;
        Player[p].m_cTeam.m_nFlag=p<3; Player[p].m_cTeam.m_nID=p<3?0:-1;
        Player[p].m_cTeam.m_dwChatGeneration=1; Player[p].m_SessionTicket[0]=(BYTE)p;
        sprintf(Npc[p].Name,"Player%d",p);
    }
    Query(1); DWORD id=s_PhongThanTeamChannels[0].id; assert(id>2);
    Query(2); assert(s_PhongThanTeamChannels[0].id==id);
    Subscribe(1,id); Subscribe(2,id); Subscribe(3,id);
    Send(1,id); assert(server.delivered[1]==1 && server.delivered[2]==1 && !server.delivered[3]);
    assert(server.lastChannel==id && strcmp(server.lastText,"alo")==0);
    Send(3,id); Send(1,id,true); assert(server.delivered[1]==1);
    Player[2].m_cTeam.m_nFlag=0; ++Player[2].m_cTeam.m_dwChatGeneration;
    Send(1,id); assert(server.delivered[1]==2 && server.delivered[2]==1);
    Player[2].m_cTeam.m_nFlag=1; Send(2,id); assert(server.delivered[1]==2);
    Subscribe(2,id); Send(2,id); assert(server.delivered[1]==3 && server.delivered[2]==2);
    ++Player[1].m_SessionTicket[0]; Send(1,id); assert(server.delivered[1]==3);
    ++g_Team[0].generation; Send(2,id); assert(server.delivered[1]==3);
    Query(2); assert(s_PhongThanTeamChannels[0].id!=id);
    Subscribe(2,id); Send(2,id); assert(server.delivered[1]==3);
    puts("PASS: dynamic identity, membership, outsider exclusion, malformed length, leave/rejoin, relogin, disband/reuse, preserved text/channel");
    return 0;
}
