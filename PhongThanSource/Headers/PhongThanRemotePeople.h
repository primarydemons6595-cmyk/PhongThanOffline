#ifndef PHONGTHAN_REMOTE_PEOPLE_H
#define PHONGTHAN_REMOTE_PEOPLE_H
#include "PhongThanWorldProtocol.h"
#include <string.h>
enum {PT_PEOPLE_QUERY=0x4130,PT_PEOPLE_REPLY=0x4030,PT_PEOPLE_ACTION=0x50544351,PT_PEOPLE_GET=0x50544347};
#pragma pack(push,1)
struct PT_PEOPLE_COMMAND {
 PHONGTHAN_WIRE_HEADER Header;unsigned int ActorId,MapId,TargetId;int Action;char Name[32];
};
struct PT_PEOPLE_RESULT {
 PHONGTHAN_WIRE_HEADER Header;unsigned int ActorId,MapId,TargetId;int Action,Result,MenuState,TeamId;char Name[32];
};
#pragma pack(pop)
inline bool PtPeopleActionAllowed(int a){return a>=0&&a<=19&&a!=2&&a!=12;}
inline bool PtPeopleNameValid(const char* s){
 if(!s||!s[0]||!memchr(s,0,32))return false;
 for(int i=0;s[i];++i)if((unsigned char)s[i]<32||(unsigned char)s[i]==127)return false;
 return true;
}
int PhongThanRemotePeopleAction(const PT_PEOPLE_COMMAND*);
int PhongThanRemotePeopleGet(PT_PEOPLE_RESULT*);
#endif
