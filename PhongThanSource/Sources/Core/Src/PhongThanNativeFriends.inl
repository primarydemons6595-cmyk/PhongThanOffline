// Native friend requests for the standalone world; original client UI packets.
// Persist by role name, never by transient player-slot index.
#include "../../../Headers/KTongProtocol.h"
struct PTFriendRequest {int From,To;DWORD FromId,ToId,Time;};
static PTFriendRequest s_PTFriendRequests[256];
static const char* s_PTFriendFile=".\\CharacterStore\\friends.ini";
static bool PTFriendName(const char* name,int size){
    if(!name||size<2||!memchr(name,0,size)||!name[0])return false;
    for(int i=0;name[i];++i)if((unsigned char)name[i]<32 || name[i]=='<' || name[i]=='>')return false;
    return true;
}
static void PTFriendKey(const char* name,char* key){
    const char* hex="0123456789abcdef";int n=0;
    while(*name && n<62){unsigned char c=(unsigned char)*name++;key[n++]=hex[c>>4];key[n++]=hex[c&15];}key[n]=0;
}
static int PTFriendOnline(const char* name){
    for(int i=1;i<MAX_PLAYER;++i)if(Player[i].m_dwID && Player[i].m_nNetConnectIdx>=0 &&
       Player[i].m_nIndex>0 && Player[i].m_nIndex<MAX_NPC && !Player[i].m_bExchangeServer &&
       !strcmp(Npc[Player[i].m_nIndex].Name,name))return i;
    return 0;
}
static void PTFriendSend(int player,const void* body,int length){
    if(player<=0||player>=MAX_PLAYER||Player[player].m_nNetConnectIdx<0||length<sizeof(EXTEND_HEADER)||length>4096)return;
    char packet[4096+sizeof(tagExtendProtoHeader)];memset(packet,0,sizeof(tagExtendProtoHeader));
    tagExtendProtoHeader* h=(tagExtendProtoHeader*)packet;h->ProtocolType=s2c_extendfriend;h->wLength=sizeof(*h)+length-1;
    memcpy(packet+sizeof(*h),body,length);g_pServer->PackDataToClient(Player[player].m_nNetConnectIdx,packet,sizeof(*h)+length);
}
static void PTFriendReply(int player,const char* name,BYTE answer){
    REP_ADDFRIEND_SYNC response;memset(&response,0,sizeof(response));response.ProtocolFamily=pf_friend;
    response.ProtocolID=friend_c2c_repaddfriend;response.pckgid=255;response.answer=answer;
    lstrcpyn(response.srcrole,name,sizeof(response.srcrole));PTFriendSend(player,&response,sizeof(response));
}
static bool PTFriendStore(const char* owner,const char* other,bool add){
    char section[65],key[65];PTFriendKey(owner,section);PTFriendKey(other,key);
    return WritePrivateProfileString(section,key,add?other:NULL,s_PTFriendFile)!=0;
}
static bool PTFriendStorePair(const char* first,const char* second){
    char section[65],key[65],previous[32];PTFriendKey(first,section);PTFriendKey(second,key);
    GetPrivateProfileString(section,key,"",previous,sizeof(previous),s_PTFriendFile);
    if(!PTFriendStore(first,second,true))return false;
    if(PTFriendStore(second,first,true))return true;
    // Restore the prior row; a failed reciprocal write must not erase an old friendship.
    WritePrivateProfileString(section,key,previous[0]?previous:NULL,s_PTFriendFile);
    return false;
}
static void PTFriendSync(int player){
    int actor=Player[player].m_nIndex;char section[65],keys[8192];PTFriendKey(Npc[actor].Name,section);
    memset(keys,0,sizeof(keys));DWORD used=GetPrivateProfileString(section,NULL,"",keys,sizeof(keys),s_PTFriendFile);
    if(used>=sizeof(keys)-2)return;
    for(char* key=keys;*key;key+=strlen(key)+1){
        char name[32];GetPrivateProfileString(section,key,"",name,sizeof(name),s_PTFriendFile);
        if(!PTFriendName(name,sizeof(name)))continue;
        char buffer[256];memset(buffer,0,sizeof(buffer));REP_SYNCFRIENDLIST* list=(REP_SYNCFRIENDLIST*)buffer;
        list->ProtocolFamily=pf_friend;list->ProtocolID=friend_s2c_repsyncfriendlist;list->pckgid=255;
        char* out=(char*)(list+1);*out++=specGroup;*out++=0;*out++=specRole;*out++=PTFriendOnline(name)?stateOnline:stateOffline;
        strcpy(out,name);out+=strlen(name)+1;*out++=specOver;PTFriendSend(player,buffer,out-buffer);
    }
}
static bool PhongThanProcessNativeFriends(int player,const char* data,int size){
    if(!data||size<sizeof(tagExtendProtoHeader))return false;
    const tagExtendProtoHeader* h=(const tagExtendProtoHeader*)data;if(h->ProtocolType!=c2s_extendfriend)return false;
    if(player<=0||player>=MAX_PLAYER||h->wLength+1!=size||size<sizeof(*h)+sizeof(EXTEND_HEADER))return true;
    int actor=Player[player].m_nIndex;if(actor<=0||actor>=MAX_NPC||!Player[player].m_dwID)return true;
    const EXTEND_HEADER* e=(const EXTEND_HEADER*)(h+1);int length=size-sizeof(*h);if(e->ProtocolFamily!=pf_friend)return true;
    if(e->ProtocolID==friend_c2s_asksyncfriendlist){if(length==sizeof(ASK_SYNCFRIENDLIST))PTFriendSync(player);return true;}
    if(e->ProtocolID==friend_c2c_askaddfriend){
        if(length!=sizeof(ASK_ADDFRIEND_CMD))return true;const ASK_ADDFRIEND_CMD* q=(const ASK_ADDFRIEND_CMD*)e;
        if(!PTFriendName(q->dstrole,sizeof(q->dstrole)))return true;
        int dest=PTFriendOnline(q->dstrole);if(!dest||dest==player){PTFriendReply(player,q->dstrole,answerUnable);return true;}
        DWORD now=GetTickCount();int slot=-1;
        for(int i=0;i<256;++i){
            PTFriendRequest& pending=s_PTFriendRequests[i];
            if(pending.From==player && pending.To==dest && now-pending.Time<60000)return true;
            if(!pending.From || now-pending.Time>=60000)slot=i;
        }
        if(slot<0){PTFriendReply(player,q->dstrole,answerUnable);return true;}
        PTFriendRequest& p=s_PTFriendRequests[slot];p.From=player;p.To=dest;p.FromId=Player[player].m_dwID;p.ToId=Player[dest].m_dwID;p.Time=now;
        ASK_ADDFRIEND_SYNC response;memset(&response,0,sizeof(response));response.ProtocolFamily=pf_friend;
        response.ProtocolID=friend_c2c_askaddfriend;response.pckgid=q->pckgid;
        lstrcpyn(response.srcrole,Npc[actor].Name,sizeof(response.srcrole));PTFriendSend(dest,&response,sizeof(response));return true;
    }
    if(e->ProtocolID==friend_c2c_repaddfriend){
        if(length!=sizeof(REP_ADDFRIEND_CMD))return true;const REP_ADDFRIEND_CMD* q=(const REP_ADDFRIEND_CMD*)e;
        if(!PTFriendName(q->dstrole,sizeof(q->dstrole))||q->answer>answerUnable)return true;
        int from=PTFriendOnline(q->dstrole);if(!from)return true;
        for(int i=0;i<256;++i){PTFriendRequest& p=s_PTFriendRequests[i];
            if(p.From!=from||p.To!=player||p.FromId!=Player[from].m_dwID||p.ToId!=Player[player].m_dwID||GetTickCount()-p.Time>=60000)continue;
            p.From=0;BYTE answer=q->answer;
            if(answer==answerAgree){
                const char* me=Npc[actor].Name;
                if(!PTFriendStorePair(me,q->dstrole))answer=answerUnable;
            }
            PTFriendReply(from,Npc[actor].Name,answer);
            if(answer==answerAgree){PTFriendReply(player,q->dstrole,answer);PTFriendSync(player);PTFriendSync(from);}break;
        }return true;
    }
    if(e->ProtocolID==friend_c2s_erasefriend){
        if(length!=sizeof(ERASE_FRIEND))return true;const ERASE_FRIEND* q=(const ERASE_FRIEND*)e;
        if(!PTFriendName(q->friendrole,sizeof(q->friendrole)))return true;
        if(PTFriendStore(Npc[actor].Name,q->friendrole,false)){
            REP_ERASEFRIEND_SYNC response;memset(&response,0,sizeof(response));response.ProtocolFamily=pf_friend;
            response.ProtocolID=friend_c2s_erasefriend;lstrcpyn(response.srcrole,q->friendrole,sizeof(response.srcrole));PTFriendSend(player,&response,sizeof(response));
        }return true;
    }
    return true;
}
