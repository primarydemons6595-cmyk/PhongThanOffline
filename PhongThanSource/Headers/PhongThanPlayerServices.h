#ifndef PHONGTHAN_PLAYER_SERVICES_H
#define PHONGTHAN_PLAYER_SERVICES_H
#include "PhongThanProtocol.h"
#include <string.h>
enum {PT_PS_QUERY=0x4128,PT_PS_REPLY=0x4028,PT_PS_ACTION=0x50545058,PT_PS_GET=0x50545059,PT_PS_PREVIEW=0x5054505a,PT_PS_RELEASE=0x5054505b};
enum {PT_PS_INBOX=0,PT_PS_SEND_MAIL=1,PT_PS_READ_MAIL=2,PT_PS_CLAIM_MAIL=3,
 PT_PS_DELETE_MAIL=4,PT_PS_POSITION=5,PT_PS_INVITE_PUPIL=6,PT_PS_REPLY_PUPIL=7,PT_PS_REPORT=8};
enum {PT_PS_MAIL_LIMIT=18,PT_PS_ATTACH_LIMIT=9};
#pragma pack(push,1)
struct PT_PS_ITEM {
 unsigned int Serial;int Genre,Detail,Particular,Row,Series,Level,Luck,Version;
 unsigned int Seed;int Generator[40],Stack,Durability,Expire,Param,Fortune,Upgrade,Physical,Magic;
 unsigned int AddedBaseMask,AddedRequireMask;
 int Base[20][4],Require[6][4],Attributes[20][4];char Name[80],Image[256];
};
struct PT_PS_MAIL {
 unsigned int Id,Created;int Claimed,Deleted,Money,Fee,Count;
 char Sender[32],Recipient[32],Subject[32],Body[257];PT_PS_ITEM Items[PT_PS_ATTACH_LIMIT];
};
struct PT_PS_MAIL_HEADER {unsigned int Id,Created;int Claimed,Money,Count;char Sender[32],Subject[32];};
struct PT_PS_COMMAND {
 PHONGTHAN_WIRE_HEADER Header;unsigned int ActorId,MapId,TargetId,MailId;
 int Operation,Money,Value;unsigned int Items[PT_PS_ATTACH_LIMIT];char Target[32],Subject[32],Text[257];
};
struct PT_PS_STATE {
 PHONGTHAN_WIRE_HEADER Header;unsigned int ActorId,MapId,TargetId;
 int Operation,Result,Count,X,Y,TargetMap,PendingPupil,SendFee;char Target[32],Message[256];
 PT_PS_MAIL_HEADER Mails[PT_PS_MAIL_LIMIT];PT_PS_MAIL Mail;
};
#pragma pack(pop)
inline bool PtPsText(const char* text,int capacity,bool multiline=false){
 if(!text||capacity<1||!memchr(text,0,capacity))return false;
 for(int i=0;text[i];++i){unsigned char c=(unsigned char)text[i];
  if((c<32&&!(multiline&&(c==10||c==13)))||c==127||c=='<'||c=='>')return false;}
 return true;
}
inline unsigned int PtPsHash(const void* data,int size){const unsigned char* p=(const unsigned char*)data;unsigned int h=2166136261U;for(int i=0;i<size;++i)h=(h^p[i])*16777619U;return h;}
inline bool PtPsValidMail(const PT_PS_MAIL& m){
 if(!m.Id)return true;
 if(!PtPsText(m.Sender,32)||!m.Sender[0]||!PtPsText(m.Recipient,32)||!m.Recipient[0]||
 !PtPsText(m.Subject,32)||!PtPsText(m.Body,257,true)||m.Count<0||m.Count>PT_PS_ATTACH_LIMIT||
 m.Money<0||m.Money>100000000||m.Fee<0||m.Fee>1000000||m.Claimed<0||m.Claimed>4)return false;
 for(int i=0;i<m.Count;++i){const PT_PS_ITEM& d=m.Items[i];
  if(!d.Serial||d.Stack<=0||!memchr(d.Name,0,sizeof(d.Name))||!memchr(d.Image,0,sizeof(d.Image)))return false;}
 return true;
}
#ifndef _SERVER
int PhongThanPlayerServiceAction(const PT_PS_COMMAND*);
int PhongThanPlayerServiceGet(PT_PS_STATE*);
#endif
#endif
