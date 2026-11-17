#pragma once

// Native, server-local compatibility for the original \T<TeamServerID> contract.
// No new client opcode or fixed TEAM channel. IDs are opaque runtime allocations.
#include <stdio.h>

struct PhongThanTeamChatChannel
{
    DWORD id;
    DWORD generation;
};

struct PhongThanTeamChatSubscription
{
    DWORD id;
    DWORD playerGeneration;
    PHONGTHAN_U8 ticket[PHONGTHAN_SESSION_TICKET_SIZE];
};

static PhongThanTeamChatChannel s_PhongThanTeamChannels[MAX_TEAM];
static PhongThanTeamChatSubscription s_PhongThanTeamSubscriptions[MAX_PLAYER];
static DWORD s_PhongThanNextTeamChannel = 3; // 1=proven nearby, 2=receive-only GM

static bool PhongThanTeamChatMember(int player, int team)
{
    if (player <= 0 || player >= MAX_PLAYER || team < 0 || team >= MAX_TEAM)
        return false;
    if (!Player[player].m_dwID || Player[player].m_nIndex <= 0 ||
        Player[player].m_nIndex >= MAX_NPC || Player[player].m_nNetConnectIdx < 0 ||
        Player[player].m_bExchangeServer || Player[player].m_bIsBot ||
        !Player[player].m_cTeam.m_nFlag || Player[player].m_cTeam.m_nID != team)
        return false;
    if (g_Team[team].m_nCaptain == player)
        return true;
    for (int i = 0; i < MAX_TEAM_MEMBER; ++i)
        if (g_Team[team].m_nMember[i] == player)
            return true;
    return false;
}

static bool PhongThanTeamChatSubscribed(int player, int team, DWORD channel)
{
    if (!PhongThanTeamChatMember(player, team))
        return false;
    const PhongThanTeamChatChannel& route = s_PhongThanTeamChannels[team];
    const PhongThanTeamChatSubscription& sub = s_PhongThanTeamSubscriptions[player];
    return route.id == channel && route.generation == g_Team[team].GetChatGeneration() &&
        sub.id == channel && sub.playerGeneration == Player[player].m_cTeam.m_dwChatGeneration &&
        memcmp(sub.ticket, Player[player].m_SessionTicket, sizeof(sub.ticket)) == 0;
}

static bool PhongThanProcessNativeTeamChat(int player, const char* data, int size)
{
    if (!data || size < (int)sizeof(tagExtendProtoHeader))
        return false;
    const tagExtendProtoHeader* header = (const tagExtendProtoHeader*)data;
    if (header->ProtocolType != c2s_extend && header->ProtocolType != c2s_extendchat)
        return false;
    // This bridge owns only forwarded community/chat envelopes; malformed input fails closed.
    if (header->wLength + 1 != size || player <= 0 || player >= MAX_PLAYER)
        return true;
    int team = Player[player].m_cTeam.m_nID;
    const char* payload = data + sizeof(*header);
    int payloadSize = size - sizeof(*header);
    if (header->ProtocolType == c2s_extend)
    {
        if (payloadSize < (int)sizeof(EXTEND_HEADER))
            return true;
        const EXTEND_HEADER* extension = (const EXTEND_HEADER*)payload;
        if (extension->ProtocolFamily != pf_playercommunity)
            return true;
        if (extension->ProtocolID == playercomm_c2s_querychannelid)
        {
            if (payloadSize != sizeof(PLAYERCOMM_QUERYCHANNELID))
                return true;
            const PLAYERCOMM_QUERYCHANNELID* query = (const PLAYERCOMM_QUERYCHANNELID*)payload;
            const char* end = (const char*)memchr(query->channel, 0, sizeof(query->channel));
            if (!end || query->channel[0] != '\\' || query->channel[1] != 'T' ||
                end <= query->channel + 2 || !PhongThanTeamChatMember(player, team))
                return true;
            unsigned int requested = 0;
            for (const char* digit = query->channel + 2; digit < end; ++digit)
            {
                if (*digit < '0' || *digit > '9' || requested >= MAX_TEAM)
                    return true;
                requested = requested * 10 + (*digit - '0');
            }
            if (requested != (unsigned int)team)
                return true;
            PhongThanTeamChatChannel& route = s_PhongThanTeamChannels[team];
            if (!route.id || route.generation != g_Team[team].GetChatGeneration())
            {
                if (s_PhongThanNextTeamChannel >= 0x40000000)
                    return true;
                route.id = s_PhongThanNextTeamChannel++;
                route.generation = g_Team[team].GetChatGeneration();
            }
            char reply[sizeof(tagExtendProtoHeader) + sizeof(PLAYERCOMM_NOTIFYCHANNELID)];
            memset(reply, 0, sizeof(reply));
            tagExtendProtoHeader* replyHeader = (tagExtendProtoHeader*)reply;
            PLAYERCOMM_NOTIFYCHANNELID* notify = (PLAYERCOMM_NOTIFYCHANNELID*)(replyHeader + 1);
            replyHeader->ProtocolType = s2c_extend;
            replyHeader->wLength = sizeof(reply) - 1;
            notify->ProtocolFamily = pf_playercommunity;
            notify->ProtocolID = playercomm_s2c_notifychannelid;
            memcpy(notify->channel, query->channel, end - query->channel + 1);
            notify->channelid = route.id;
            notify->cost = 0;
            g_pServer->PackDataToClient(Player[player].m_nNetConnectIdx, reply, sizeof(reply));
            return true;
        }
        if (extension->ProtocolID == playercomm_c2s_subscribe)
        {
            if (payloadSize != sizeof(PLAYERCOMM_SUBSCRIBE))
                return true;
            const PLAYERCOMM_SUBSCRIBE* request = (const PLAYERCOMM_SUBSCRIBE*)payload;
            PhongThanTeamChatSubscription& sub = s_PhongThanTeamSubscriptions[player];
            if (!request->subscribe)
            {
                if (sub.id == request->channelid)
                    memset(&sub, 0, sizeof(sub));
                return true;
            }
            if (request->subscribe != TRUE || !PhongThanTeamChatMember(player, team))
                return true;
            const PhongThanTeamChatChannel& route = s_PhongThanTeamChannels[team];
            if (!route.id || request->channelid != route.id ||
                route.generation != g_Team[team].GetChatGeneration())
                return true;
            sub.id = route.id;
            sub.playerGeneration = Player[player].m_cTeam.m_dwChatGeneration;
            memcpy(sub.ticket, Player[player].m_SessionTicket, sizeof(sub.ticket));
            return true;
        }
        return true;
    }
    if (payloadSize < sizeof(CHAT_CHANNELCHAT_CMD))
        return true;
    const CHAT_CHANNELCHAT_CMD* command = (const CHAT_CHANNELCHAT_CMD*)payload;
    // Local Loose testing only. Do not expose arbitrary Lua execution through
    // the public GM notification channel. Text follows the fixed item-link field.
    if (command->ProtocolType == chat_channelchat && command->channelid == 2 &&
        command->sentlen && payloadSize == sizeof(*command) + command->sentlen &&
        command->wSize + 1 == payloadSize && command->cost == 0)
    {
        FILE* marker = fopen("loose_only.flag", "rb");
        if (!marker) return true;
        fclose(marker);
        char text[256];
        memcpy(text, command + 1, command->sentlen);
        text[command->sentlen] = 0;
        int particular = 0;
        if (!strcmp(text, "?gm ds AddNormalItem(0,10,400,1,1000,0)")) particular = 400;
        if (!strcmp(text, "?gm ds AddNormalItem(0,10,27,1,1000,0)")) particular = 27;
        char script[256];
        if (particular)
            sprintf(script, "local i=AddNormalItem(0,10,%d,1,1000,0); if i>0 then Msg2Player('TEST ITEM OK: index='..i) else Msg2Player('TEST ITEM FAILED: check bag space and template') end", particular);
        else
            strcpy(script, "Msg2Player('GM test: only two horse test commands are enabled on Loose')");
        BOOL result = Player[player].DoScript(script);
        FILE* trace = fopen("gm_test_trace.log", "a");
        if (trace) { fprintf(trace, "player=%d channel=2 particular=%d script_ok=%d\n", player, particular, result); fclose(trace); }
        return true;
    }
    if (command->ProtocolType != chat_channelchat || !command->sentlen ||
        payloadSize != sizeof(*command) + command->sentlen || command->wSize + 1 != payloadSize ||
        command->cost != 0 || !memchr(command->item, 0, sizeof(command->item)) ||
        !PhongThanTeamChatSubscribed(player, team, command->channelid))
        return true;
    char packet[sizeof(tagExtendProtoHeader) + sizeof(CHAT_CHANNELCHAT_SYNC) + 255];
    int packetSize = sizeof(tagExtendProtoHeader) + sizeof(CHAT_CHANNELCHAT_SYNC) + command->sentlen;
    memset(packet, 0, packetSize);
    tagExtendProtoHeader* outbound = (tagExtendProtoHeader*)packet;
    CHAT_CHANNELCHAT_SYNC* message = (CHAT_CHANNELCHAT_SYNC*)(outbound + 1);
    outbound->ProtocolType = s2c_extendchat;
    outbound->wLength = packetSize - 1;
    message->ProtocolType = chat_channelchat;
    message->wSize = sizeof(*message) + command->sentlen - 1;
    message->packageID = command->packageID;
    strncpy(message->someone, Npc[Player[player].m_nIndex].Name, sizeof(message->someone) - 1);
    message->channelid = command->channelid;
    message->sentlen = command->sentlen;
    message->someflag = command->someflag & 0x7f;
    if((command->someflag&0x80) && PhongThanConsumableChatColor(Player[player]))message->someflag|=0x80;
    memcpy(message->item, command->item, sizeof(message->item));
    memcpy(message + 1, command + 1, command->sentlen);
    int queued = 0;
    HRESULT result = S_OK;
    for (int slot = -1; slot < MAX_TEAM_MEMBER; ++slot)
    {
        int recipient = slot < 0 ? g_Team[team].m_nCaptain : g_Team[team].m_nMember[slot];
        if (!PhongThanTeamChatSubscribed(recipient, team, command->channelid))
            continue;
        result = g_pServer->PackDataToClient(Player[recipient].m_nNetConnectIdx, packet, packetSize);
        ++queued;
    }
    FILE* trace = fopen("player_chat_trace.log", "a");
    if (trace)
    {
        fprintf(trace, "TEAM_SEND player=%d team=%d channel=%lu text_len=%u recipients=%d last_hr=%ld\n",
            player, team, (unsigned long)command->channelid, command->sentlen, queued, (long)result);
        fclose(trace);
    }
    return true;
}
