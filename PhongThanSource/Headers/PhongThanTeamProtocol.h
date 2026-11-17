#ifndef PHONGTHAN_TEAM_PROTOCOL_H
#define PHONGTHAN_TEAM_PROTOCOL_H
#include "PhongThanProtocol.h"
#define PT_TEAM_REQUEST 0x4110
#define PT_TEAM_SNAPSHOT 0x4010
#define PT_TEAM_UI_GET 0x50540002
#define PT_TEAM_UI_NAME 0x50540003
#define PT_TEAM_UI_VISIBLE 0x50540004
#pragma pack(push,1)
struct PT_TEAM_QUERY {PHONGTHAN_WIRE_HEADER Header;PHONGTHAN_U32 ActorId;PHONGTHAN_U32 Rename;char Name[32];};
struct PT_TEAM_MEMBER {PHONGTHAN_U32 Id,MapId;PHONGTHAN_S32 Life,MaxLife,Level,Profession;char Name[32];};
struct PT_TEAM_STATE {PHONGTHAN_WIRE_HEADER Header;PHONGTHAN_U32 ActorId,Generation,CaptainId,Count,Result,ShowName;char Name[32];PT_TEAM_MEMBER Members[8];};
#pragma pack(pop)
inline bool PhongThanValidTeamName(const char* name){
    bool nonspace=false;for(int i=0;i<32;++i){unsigned char c=(unsigned char)name[i];if(!c)return nonspace;if(c<32||c=='<'||c=='>'||c==127)return false;if(c!=' ')nonspace=true;}return false;
}
#endif
