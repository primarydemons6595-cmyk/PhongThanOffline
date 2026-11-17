// Reconstructed standalone routing for original trade/area/nation channels.
// Uses existing community envelopes; identity and group membership come from server state.
#include "../../../Headers/PhongThanChatChannels.h"
struct PTPublicChatSession {DWORD Role;BYTE Ticket[PHONGTHAN_SESSION_TICKET_SIZE];unsigned Mask;DWORD Window[PT_CHAT_CHANNEL_COUNT];int Used[PT_CHAT_CHANNEL_COUNT];};
static PTPublicChatSession s_PublicChat[MAX_PLAYER];
static bool PTPublicLive(int p){return p>0&&p<MAX_PLAYER&&Player[p].m_dwID&&Player[p].m_nIndex>0&&Player[p].m_nIndex<MAX_NPC&&Player[p].m_nNetConnectIdx>=0&&!Player[p].m_bExchangeServer&&!Player[p].m_bIsBot;}
static bool PTPublicChannel(DWORD id){return id>=PT_CHAT_TRADE&&id<=PT_CHAT_TONG;}
static PTPublicChatSession& PTPublicSession(int p){
    PTPublicChatSession& s=s_PublicChat[p];
    if(s.Role!=Player[p].m_dwID||memcmp(s.Ticket,Player[p].m_SessionTicket,sizeof(s.Ticket))){
        memset(&s,0,sizeof(s));s.Role=Player[p].m_dwID;memcpy(s.Ticket,Player[p].m_SessionTicket,sizeof(s.Ticket));
    }return s;
}
static bool PTPublicSubscribed(int p,DWORD id){return PTPublicLive(p)&&PTPublicChannel(id)&&(PTPublicSession(p).Mask&(1u<<(id-PT_CHAT_TRADE)))!=0;}
static unsigned PTPublicNation(int p){if(!PTPublicLive(p)||!PTNationReady())return 0;int member=PTNationSelf(p);return member<0?0:s_PtNation.State.Members[member].NationId;}
static DWORD PTPublicTong(int p){return PTPublicLive(p)&&Player[p].m_cTong.m_nFlag?Player[p].m_cTong.m_dwTongNameID:0;}
static bool PTPublicRecipient(int from,int to,DWORD id){
    if(!PTPublicSubscribed(from,id)||!PTPublicSubscribed(to,id))return false;
    if(id==PT_CHAT_TRADE||id==PT_CHAT_WORLD)return true;
    if(id==PT_CHAT_AREA)return Npc[Player[from].m_nIndex].m_SubWorldIndex==Npc[Player[to].m_nIndex].m_SubWorldIndex;
    if(id==PT_CHAT_NATION){unsigned nation=PTPublicNation(from);return nation&&nation==PTPublicNation(to);}
    if(id==PT_CHAT_TONG){DWORD tong=PTPublicTong(from);return tong&&tong==PTPublicTong(to);}
    if(id==PT_CHAT_FRIENDS){
        if(from==to)return true;
        char section[65],key[65],stored[32];PTFriendKey(Npc[Player[from].m_nIndex].Name,section);PTFriendKey(Npc[Player[to].m_nIndex].Name,key);
        GetPrivateProfileString(section,key,"",stored,sizeof(stored),s_PTFriendFile);
        return !strcmp(stored,Npc[Player[to].m_nIndex].Name);
    }return false;
}
static bool PhongThanProcessPublicChat(int p,const char* data,int size){
    if(!data||size<(int)sizeof(tagExtendProtoHeader))return false;
    const tagExtendProtoHeader* h=(const tagExtendProtoHeader*)data;
    if(h->ProtocolType!=c2s_extend&&h->ProtocolType!=c2s_extendchat)return false;
    const char* body=data+sizeof(*h);int length=size-sizeof(*h);
    if(h->ProtocolType==c2s_extend){
        if(length<(int)sizeof(EXTEND_HEADER))return false;
        const EXTEND_HEADER* e=(const EXTEND_HEADER*)body;if(e->ProtocolFamily!=pf_playercommunity)return false;
        if(e->ProtocolID==playercomm_c2s_querychannelid){
            if(length!=sizeof(PLAYERCOMM_QUERYCHANNELID))return true;
            const PLAYERCOMM_QUERYCHANNELID* q=(const PLAYERCOMM_QUERYCHANNELID*)body;
            if(!memchr(q->channel,0,sizeof(q->channel)))return true;
            DWORD id=0;
            if(!strcmp(q->channel,"\xBD\xBB\xD2\xD7"))id=PT_CHAT_TRADE;
            else if(!strcmp(q->channel,"\xCA\xC0\xBD\xE7"))id=PT_CHAT_WORLD;
            else if(!strcmp(q->channel,"\\B"))id=PT_CHAT_AREA;
            else if(!strcmp(q->channel,"\\O"))id=PT_CHAT_NATION;
            else if(!strcmp(q->channel,"PT_FRIENDS"))id=PT_CHAT_FRIENDS;
            else if(q->channel[0]=='\\'&&q->channel[1]=='O'&&q->channel[2]){
                // Original guild channel is \\O<Tong#>; bare \\O remains nation.
                char expected[40];sprintf(expected,"\\O%lu",(unsigned long)PTPublicTong(p));
                if(!PTPublicLive(p)||!PTPublicTong(p)||strcmp(expected,q->channel))return true;
                id=PT_CHAT_TONG;
            }
            else return false;
            if(!PTPublicLive(p)||h->wLength+1!=size)return true;
            char packet[sizeof(tagExtendProtoHeader)+sizeof(PLAYERCOMM_NOTIFYCHANNELID)];memset(packet,0,sizeof(packet));
            tagExtendProtoHeader* out=(tagExtendProtoHeader*)packet;out->ProtocolType=s2c_extend;out->wLength=sizeof(packet)-1;
            PLAYERCOMM_NOTIFYCHANNELID* reply=(PLAYERCOMM_NOTIFYCHANNELID*)(out+1);reply->ProtocolFamily=pf_playercommunity;reply->ProtocolID=playercomm_s2c_notifychannelid;
            strcpy(reply->channel,q->channel);reply->channelid=id;reply->cost=0;
            g_pServer->PackDataToClient(Player[p].m_nNetConnectIdx,packet,sizeof(packet));return true;
        }
        if(e->ProtocolID==playercomm_c2s_subscribe){
            if(length!=sizeof(PLAYERCOMM_SUBSCRIBE))return true;
            const PLAYERCOMM_SUBSCRIBE* q=(const PLAYERCOMM_SUBSCRIBE*)body;if(!PTPublicChannel(q->channelid))return false;
            if(!PTPublicLive(p)||h->wLength+1!=size||(q->subscribe!=0&&q->subscribe!=TRUE))return true;
            unsigned bit=1u<<(q->channelid-PT_CHAT_TRADE);
            if(q->subscribe)PTPublicSession(p).Mask|=bit;else PTPublicSession(p).Mask&=~bit;return true;
        }return false;
    }
    if(length<(int)sizeof(CHAT_CHANNELCHAT_CMD))return false;
    const CHAT_CHANNELCHAT_CMD* q=(const CHAT_CHANNELCHAT_CMD*)body;if(!PTPublicChannel(q->channelid))return false;
    if(!PTPublicSubscribed(p,q->channelid)||h->wLength+1!=size||q->ProtocolType!=chat_channelchat||!q->sentlen||
        q->cost||q->wSize+1!=length||length!=sizeof(*q)+q->sentlen||!memchr(q->item,0,sizeof(q->item)))return true;
    if(q->channelid==PT_CHAT_NATION&&!PTPublicNation(p))return true;
    if(q->channelid==PT_CHAT_TONG&&!PTPublicTong(p))return true;
    PTPublicChatSession& session=PTPublicSession(p);int slot=q->channelid-PT_CHAT_TRADE;
    DWORD interval=(slot==0||slot==4)?90000:(slot==1?20000:(slot==2?10000:2000));DWORD now=GetTickCount();
    if(!session.Window[slot]||now-session.Window[slot]>=interval){session.Window[slot]=now;session.Used[slot]=0;}
    if(session.Used[slot]>=2)return true;++session.Used[slot];
    char packet[sizeof(tagExtendProtoHeader)+sizeof(CHAT_CHANNELCHAT_SYNC)+255];int bytes=sizeof(tagExtendProtoHeader)+sizeof(CHAT_CHANNELCHAT_SYNC)+q->sentlen;memset(packet,0,bytes);
    tagExtendProtoHeader* out=(tagExtendProtoHeader*)packet;out->ProtocolType=s2c_extendchat;out->wLength=bytes-1;
    CHAT_CHANNELCHAT_SYNC* msg=(CHAT_CHANNELCHAT_SYNC*)(out+1);msg->ProtocolType=chat_channelchat;msg->wSize=sizeof(*msg)+q->sentlen-1;
    msg->channelid=q->channelid;msg->packageID=q->packageID;msg->sentlen=q->sentlen;msg->someflag=q->someflag&0x7f;
    if((q->someflag&0x80)&&PhongThanConsumableChatColor(Player[p]))msg->someflag|=0x80;
    strncpy(msg->someone,Npc[Player[p].m_nIndex].Name,sizeof(msg->someone)-1);memcpy(msg->item,q->item,sizeof(msg->item));memcpy(msg+1,q+1,q->sentlen);
    int sent=0;for(int recipient=PlayerSet.GetFirstPlayer();recipient;recipient=PlayerSet.GetNextPlayer())if(PTPublicRecipient(p,recipient,q->channelid)){g_pServer->PackDataToClient(Player[recipient].m_nNetConnectIdx,packet,bytes);++sent;}
    PhongThanBotPublicChat(p,q->channelid,(const char*)(q+1),q->sentlen);
    FILE* log=fopen("player_chat_trace.log","a");if(log){fprintf(log,"PUBLIC_SEND player=%d channel=%lu recipients=%d bytes=%u\n",p,(unsigned long)q->channelid,sent,q->sentlen);fclose(log);}return true;
}

// Server agents use the same subscribed recipients, bounded public-channel rate
// and restored cost-zero protocol as live clients. No forged client identity.
static bool PhongThanBotPublicPermit(int player,DWORD channel,bool conversation){
    if(player<=0||player>=MAX_PLAYER||!Player[player].m_bIsBot||!Player[player].m_PlayerDBLoad||Player[player].m_bExchangeServer)return false;
    int actor=Player[player].m_nIndex;if(actor<=0||actor>=MAX_NPC||Npc[actor].m_CurrentLife<=0||Npc[actor].m_SubWorldIndex<0)return false;
    const char* policy="BotAIgent\\channel-policy.ini";int minimum=GetPrivateProfileIntA("Policy","PublicMinLevel",10,policy);if(minimum<1||minimum>200)minimum=10;
    if(!PTBotPublicChannelAllowed(Npc[actor].m_Level,minimum,channel))return false;
    if(channel==PT_CHAT_WORLD&&!GetPrivateProfileIntA("Policy","WorldEnabled",1,policy))return false;
    if(channel==PT_CHAT_TRADE&&!GetPrivateProfileIntA("Policy","TradeEnabled",1,policy))return false;
    static DWORD global=0,nextAdvert=0,role[MAX_PLAYER],window[MAX_PLAYER][2];static int used[MAX_PLAYER][2];DWORD now=GetTickCount();if(now-global<3000||(!conversation&&(int)(nextAdvert-now)>0))return false;int slot=channel==PT_CHAT_TRADE?0:1;
    if(role[player]!=Player[player].m_dwID){role[player]=Player[player].m_dwID;ZeroMemory(window[player],sizeof(window[player]));ZeroMemory(used[player],sizeof(used[player]));}
    if(!window[player][slot]||now-window[player][slot]>=90000){window[player][slot]=now;used[player][slot]=0;}if(used[player][slot]>=2)return false;
    ++used[player][slot];global=now;if(!conversation)nextAdvert=now+90000+((now^Player[player].m_dwID)%60000);return true;
}
static bool PTBotPublicPacket(int player,DWORD channel,const char* text,int length){
    int actor=Player[player].m_nIndex;
    char packet[sizeof(tagExtendProtoHeader)+sizeof(CHAT_CHANNELCHAT_SYNC)+255];ZeroMemory(packet,sizeof(packet));int size=sizeof(tagExtendProtoHeader)+sizeof(CHAT_CHANNELCHAT_SYNC)+length;
    tagExtendProtoHeader* h=(tagExtendProtoHeader*)packet;h->ProtocolType=s2c_extendchat;h->wLength=size-1;CHAT_CHANNELCHAT_SYNC* msg=(CHAT_CHANNELCHAT_SYNC*)(h+1);msg->ProtocolType=chat_channelchat;msg->wSize=sizeof(*msg)+length-1;msg->channelid=channel;msg->packageID=(DWORD)-1;msg->sentlen=(BYTE)length;lstrcpynA(msg->someone,Npc[actor].Name,sizeof(msg->someone));memcpy(msg+1,text,length);
    for(int to=PlayerSet.GetFirstPlayer();to;to=PlayerSet.GetNextPlayer())if(PTPublicSubscribed(to,channel)&&(channel!=PT_CHAT_AREA||Npc[Player[to].m_nIndex].m_SubWorldIndex==Npc[actor].m_SubWorldIndex))g_pServer->PackDataToClient(Player[to].m_nNetConnectIdx,packet,size);
    return true;
}
static bool PhongThanBotEmitPublic(int player,DWORD channel,const char* text,int length){
 if(!text||length<1||length>255||!PhongThanBotPublicPermit(player,channel,false))return false;return PTBotPublicPacket(player,channel,text,length);
}
static bool PhongThanBotReplyPublic(int player,int from,DWORD channel,const char* text,int length){
 if(!PTPublicLive(from)||!text||length<1||length>255)return false;
 if(channel==PT_CHAT_WORLD||channel==PT_CHAT_TRADE||channel==PT_CHAT_AREA)return PhongThanBotPublicPermit(player,channel,true)&&PTBotPublicPacket(player,channel,text,length);
 return false;
}
