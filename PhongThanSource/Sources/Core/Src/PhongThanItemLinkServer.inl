#include "../../../Headers/PhongThanItemLink.h"
static bool PTItemLinkNearby(int a,int b) {
    const KNpc& from=Npc[a];const KNpc& to=Npc[b];
    if(from.m_SubWorldIndex!=to.m_SubWorldIndex || from.m_RegionIndex<0 || to.m_RegionIndex<0)return false;
    static const int offsets[9][2]={{0,0},{0,32},{-16,32},{-16,0},{-16,-32},{0,-32},{16,-32},{16,0},{16,32}};
    for(int i=-1;i<8;++i){
        const int region=i<0?from.m_RegionIndex:SubWorld[from.m_SubWorldIndex].m_Region[from.m_RegionIndex].m_nConnectRegion[i];
        if(region!=to.m_RegionIndex)continue;
        int x=to.m_MapX+offsets[i+1][0]-from.m_MapX,y=to.m_MapY+offsets[i+1][1]-from.m_MapY;
        return x*x+y*y<23*23;
    }return false;
}
static bool PTProcessItemLink(int player,const char* data,int size,bool serverBot=false) {
    if(!data || size<(int)sizeof(PHONGTHAN_WIRE_HEADER) ||
        !PhongThanIsWirePacket(data,size) || ((const PHONGTHAN_WIRE_HEADER*)data)->MessageType!=PT_ITEM_LINK_REQUEST)return false;
    if(!PTValidateItemLinkRequest(data,size) || player<=0 || player>=MAX_PLAYER)return !serverBot;
    const PTItemLinkRequest& request=*(const PTItemLinkRequest*)data;
    const int actor=Player[player].m_nIndex;
    if(actor<=0 || actor>=MAX_NPC || Player[player].m_bExchangeServer || (!serverBot&&Player[player].m_nNetConnectIdx<0))return !serverBot;
    if(serverBot&&(!Player[player].m_bIsBot||!Player[player].m_PlayerDBLoad))return false;
    const int index=Player[player].m_ItemList.SearchID(request.ItemId);
    if(index<=0 || index>=MAX_ITEM || Player[player].m_ItemList.FindSame((DWORD)request.ItemId)<=0)return !serverBot;
    const int team=Player[player].m_cTeam.m_nID;
    const bool direct=request.Target[0]!=0;
    if(!serverBot && !direct && request.Channel!=1 && !PTPublicSubscribed(player,request.Channel) && !PhongThanTeamChatSubscribed(player,team,request.Channel))return !serverBot;
    static DWORD last[MAX_PLAYER]={0},owner[MAX_PLAYER]={0};const DWORD now=GetTickCount();
    if(owner[player]==Player[player].m_dwID && last[player] && now-last[player]<800)return !serverBot;
    last[player]=now;owner[player]=Player[player].m_dwID;
    static unsigned nextToken=0;if(++nextToken==0)++nextToken;
    PTItemLinkMessage message;ZeroMemory(&message,sizeof(message));
    PhongThanInitializeWireHeader(&message.Header,PT_ITEM_LINK_MESSAGE,sizeof(message),PHONGTHAN_WIRE_FLAG_RESPONSE,nextToken);
    message.Token=nextToken;message.Channel=direct?0:request.Channel;
    g_StrCpyLen(message.Sender,Npc[actor].Name,sizeof(message.Sender));
    KItem& item=Item[index];g_StrCpyLen(message.Name,item.GetName(),sizeof(message.Name));
    message.Genre=item.GetGenre();message.Detail=item.GetDetailType();message.Particular=item.GetParticularType();message.Row=item.GetRow();
    message.Series=item.GetSeries();message.Level=item.GetLevel();message.Luck=item.m_GeneratorParam.nLuck;message.Version=item.m_GeneratorParam.nVersion;
    message.Seed=item.m_GeneratorParam.uRandomSeed;message.Stack=item.GetStackNum();message.Durability=Item[index].GetDurability();message.Expire=item.GetExpireTime();
    message.Param=item.GetParam();message.Fortune=item.GetFortune();message.Upgrade=item.GetUpgradeState();message.PhysicalPower=item.GetBasePower();message.MagicPower=item.GetMagicPower();
    memcpy(message.Generator,item.m_GeneratorParam.nGeneratorLevel,sizeof(message.Generator));
    memcpy(message.Base,item.P0BaseAttributes(),sizeof(message.Base));memcpy(message.Require,item.TooltipRequirements(),sizeof(message.Require));memcpy(message.Magic,item.m_aryMagicAttrib,sizeof(message.Magic));
    message.TextSize=request.TextSize;memcpy(message.Text,request.Text,request.TextSize);message.Flags=request.Flags&0x3f;
    if(serverBot)message.Flags|=request.Flags&0xc0;
    else if((request.Flags&0x80) && PhongThanConsumableChatColor(Player[player]))message.Flags|=0x80;
    if(!PTValidateItemLinkMessage(&message,sizeof(message)))return !serverBot;
    if(direct&&!serverBot)PhongThanBotItemOffer(player,request.Target,index);
    for(int recipient=PlayerSet.GetFirstPlayer();recipient;recipient=PlayerSet.GetNextPlayer()) {
        const int target=Player[recipient].m_nIndex;
        if(target<=0 || target>=MAX_NPC || Player[recipient].m_nNetConnectIdx<0 || Player[recipient].m_bExchangeServer)continue;
        bool deliver=false;
        if(direct)deliver=recipient==player || !strcmp(Npc[target].Name,request.Target);
        else if(request.Channel==1)deliver=PTItemLinkNearby(actor,target);
        else if(serverBot)deliver=PTPublicSubscribed(recipient,request.Channel);
        else if(PTPublicSubscribed(player,request.Channel))deliver=PTPublicRecipient(player,recipient,request.Channel);
        else deliver=PhongThanTeamChatSubscribed(recipient,team,request.Channel);
        if(!deliver)continue;
        if(direct)g_StrCpyLen(message.Conversation,recipient==player?request.Target:message.Sender,sizeof(message.Conversation));
        g_pServer->PackDataToClient(Player[recipient].m_nNetConnectIdx,&message,sizeof(message));
    }
    return true;
}

static bool PhongThanBotEmitItemLink(int player,DWORD channel,int item,const char* text,int length,BYTE flags){
 if(player<=0||player>=MAX_PLAYER||item<=0||item>=MAX_ITEM||!text||length<1||length>255||!Item[item].GetID()||Player[player].m_ItemList.SearchID(Item[item].GetID())!=item)return false;
 char label[67];if(!PTFormatItemChatLabel(label,sizeof(label),Item[item].GetName())||PTLocateItemLabel(text,length,label,strlen(label))<0)return false;
 if(!PhongThanBotPublicPermit(player,channel,false))return false;
 PTItemLinkRequest request;ZeroMemory(&request,sizeof(request));PhongThanInitializeWireHeader(&request.Header,PT_ITEM_LINK_REQUEST,sizeof(request),PHONGTHAN_WIRE_FLAG_REQUEST,0);request.ItemId=Item[item].GetID();request.Channel=channel;request.TextSize=length;request.Flags=flags;memcpy(request.Text,text,length);return PTProcessItemLink(player,(const char*)&request,sizeof(request),true);
}
static bool PhongThanBotEmitPrivateItemLink(int player,int recipient,int item,const char* text,int length){
 if(player<=0||player>=MAX_PLAYER||recipient<=0||recipient>=MAX_PLAYER||!Player[recipient].m_PlayerDBLoad||Player[recipient].m_nNetConnectIdx<0||item<=0||item>=MAX_ITEM||!text||length<1||length>255||Player[player].m_ItemList.SearchID(Item[item].GetID())!=item)return false;
 char label[67];if(!PTFormatItemChatLabel(label,sizeof(label),Item[item].GetName())||PTLocateItemLabel(text,length,label,strlen(label))<0)return false;
 PTItemLinkRequest request;ZeroMemory(&request,sizeof(request));PhongThanInitializeWireHeader(&request.Header,PT_ITEM_LINK_REQUEST,sizeof(request),PHONGTHAN_WIRE_FLAG_REQUEST,0);request.ItemId=Item[item].GetID();g_StrCpyLen(request.Target,Player[recipient].Name,sizeof(request.Target));request.TextSize=length;memcpy(request.Text,text,length);return PTProcessItemLink(player,(const char*)&request,sizeof(request),true);
}
