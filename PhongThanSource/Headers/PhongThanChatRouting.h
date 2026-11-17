#ifndef PHONGTHAN_CHAT_ROUTING_H
#define PHONGTHAN_CHAT_ROUTING_H
#include "KProtocol.h"
#include <string.h>
enum {PT_CHAT_WIRE_INVALID=0,PT_CHAT_WIRE_PRIVATE=1,PT_CHAT_WIRE_CHANNEL=2};
// Inspect the nested type before interpreting its different fixed body layout.
inline int PhongThanChatWireKind(const void* data,int size){
 if(!data||size<(int)sizeof(tagExtendProtoHeader)+1)return PT_CHAT_WIRE_INVALID;
 const tagExtendProtoHeader* h=(const tagExtendProtoHeader*)data;
 if(h->ProtocolType!=c2s_extendchat||h->wLength+1!=size)return PT_CHAT_WIRE_INVALID;
 const BYTE* body=(const BYTE*)(h+1);int length=size-sizeof(*h);
 if(*body==chat_someonechat){
  if(length<(int)sizeof(CHAT_SOMEONECHAT_CMD))return PT_CHAT_WIRE_INVALID;
  const CHAT_SOMEONECHAT_CMD* q=(const CHAT_SOMEONECHAT_CMD*)body;
  return q->sentlen&&q->wSize+1==length&&length==sizeof(*q)+q->sentlen&&q->someone[0]&&memchr(q->someone,0,sizeof(q->someone))&&memchr(q->item,0,sizeof(q->item))?PT_CHAT_WIRE_PRIVATE:PT_CHAT_WIRE_INVALID;
 }
 if(*body==chat_channelchat){
  if(length<(int)sizeof(CHAT_CHANNELCHAT_CMD))return PT_CHAT_WIRE_INVALID;
  const CHAT_CHANNELCHAT_CMD* q=(const CHAT_CHANNELCHAT_CMD*)body;
  return q->sentlen&&q->wSize+1==length&&length==sizeof(*q)+q->sentlen&&memchr(q->item,0,sizeof(q->item))?PT_CHAT_WIRE_CHANNEL:PT_CHAT_WIRE_INVALID;
 }return PT_CHAT_WIRE_INVALID;
}
#endif
