// Standalone private messages use the original client envelopes and feedback.
// Sender identity is always derived from the authenticated world connection.
static bool PTWhisperLive(int p){
 if(p<=0||p>=MAX_PLAYER||!Player[p].m_PlayerDBLoad||!Player[p].m_dwID||Player[p].m_nNetConnectIdx<0||Player[p].m_bExchangeServer||Player[p].m_bIsBot)return false;
 int n=Player[p].m_nIndex;return n>0&&n<MAX_NPC&&Npc[n].m_dwID&&Npc[n].m_Kind==kind_player&&Npc[n].m_RegionIndex>=0&&Npc[n].m_SubWorldIndex>=0&&Npc[n].m_SubWorldIndex<MAX_SUBWORLD;
}
static bool PhongThanProcessNativeWhisper(int p,const char* bytes,int size){
 if(!bytes||size<(int)(sizeof(tagExtendProtoHeader)+1))return false;
 const tagExtendProtoHeader* h=(const tagExtendProtoHeader*)bytes;const char* body=bytes+sizeof(*h);int length=size-sizeof(*h);
 if(h->ProtocolType!=c2s_extendchat||*(const BYTE*)body!=chat_someonechat)return false;
 if(length<(int)sizeof(CHAT_SOMEONECHAT_CMD)||!PTWhisperLive(p)||h->wLength+1!=size)return true;
 const CHAT_SOMEONECHAT_CMD* q=(const CHAT_SOMEONECHAT_CMD*)body;
 if(!q->sentlen||q->wSize+1!=length||length!=sizeof(*q)+q->sentlen||!q->someone[0]||!memchr(q->someone,0,sizeof(q->someone))||!memchr(q->item,0,sizeof(q->item)))return true;
 for(int k=0;q->someone[k];++k)if((unsigned char)q->someone[k]<32||(unsigned char)q->someone[k]==127)return true;
 static DWORD owner[MAX_PLAYER],last[MAX_PLAYER];DWORD now=GetTickCount();
 if(owner[p]==Player[p].m_dwID&&now-last[p]<350)return true;owner[p]=Player[p].m_dwID;last[p]=now;
 int target=0;for(int i=1;i<MAX_PLAYER;++i)if(PTWhisperLive(i)&&!stricmp(Player[i].Name,q->someone)){target=i;break;}
 if(target==p)target=0;
 extern int PhongThanBotWhisper(int,const char*,const char*,int);
 int botTarget=target?0:PhongThanBotWhisper(p,q->someone,(const char*)(q+1),q->sentlen);
 if(target){
  char packet[sizeof(tagExtendProtoHeader)+sizeof(CHAT_SOMEONECHAT_SYNC)+255];int total=sizeof(*h)+sizeof(CHAT_SOMEONECHAT_SYNC)+q->sentlen;ZeroMemory(packet,total);
  tagExtendProtoHeader* out=(tagExtendProtoHeader*)packet;out->ProtocolType=s2c_extendchat;out->wLength=total-1;
  CHAT_SOMEONECHAT_SYNC* msg=(CHAT_SOMEONECHAT_SYNC*)(out+1);msg->ProtocolType=chat_someonechat;msg->wSize=sizeof(*msg)+q->sentlen-1;msg->packageID=q->packageID;msg->sentlen=q->sentlen;
  lstrcpyn(msg->someone,Npc[Player[p].m_nIndex].Name,sizeof(msg->someone));memcpy(msg->item,q->item,sizeof(msg->item));memcpy(msg+1,q+1,q->sentlen);
  g_pServer->PackDataToClient(Player[target].m_nNetConnectIdx,packet,total);
 }
 char reply[sizeof(tagExtendProtoHeader)+sizeof(CHAT_FEEDBACK)+sizeof(DWORD)+_NAME_LEN+1+255];
 int total=sizeof(*h)+sizeof(CHAT_FEEDBACK)+sizeof(DWORD)+_NAME_LEN+1+q->sentlen;ZeroMemory(reply,total);
 tagExtendProtoHeader* out=(tagExtendProtoHeader*)reply;out->ProtocolType=s2c_extendchat;out->wLength=total-1;
 CHAT_FEEDBACK* feedback=(CHAT_FEEDBACK*)(out+1);feedback->ProtocolType=chat_feedback;feedback->packageID=q->packageID;feedback->code=(target||botTarget)?codeSucc:codeFail;memcpy(feedback->item,q->item,sizeof(feedback->item));
 char* tail=(char*)(feedback+1);DWORD channel=(DWORD)-1;memcpy(tail,&channel,sizeof(channel));tail+=sizeof(channel);lstrcpyn(tail,q->someone,_NAME_LEN);tail+=_NAME_LEN;*tail++=q->sentlen;memcpy(tail,q+1,q->sentlen);
 g_pServer->PackDataToClient(Player[p].m_nNetConnectIdx,reply,total);return true;
}
