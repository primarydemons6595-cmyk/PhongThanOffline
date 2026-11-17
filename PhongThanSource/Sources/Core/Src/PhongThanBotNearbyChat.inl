static bool PhongThanProcessBotNearbyChat(int player,const char* data,int size){
 if(!data||size<(int)(sizeof(tagExtendProtoHeader)+sizeof(CHAT_CHANNELCHAT_CMD)))return false;
 const tagExtendProtoHeader* header=(const tagExtendProtoHeader*)data;
 if(header->ProtocolType!=c2s_extendchat)return false;
 const CHAT_CHANNELCHAT_CMD* chat=(const CHAT_CHANNELCHAT_CMD*)(header+1);
 if(chat->ProtocolType!=chat_channelchat||chat->channelid!=1)return false;
 int length=size-sizeof(*header);
 if(!PTWhisperLive(player)||header->wLength+1!=size||!chat->sentlen||chat->cost||chat->wSize+1!=length||length!=sizeof(*chat)+chat->sentlen||!memchr(chat->item,0,sizeof(chat->item)))return true;
 PhongThanBotNearby(player,(const char*)(chat+1),chat->sentlen);return true;
}
