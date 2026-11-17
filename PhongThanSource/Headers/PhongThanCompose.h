#ifndef PHONGTHAN_COMPOSE_H
#define PHONGTHAN_COMPOSE_H
#include "PhongThanWorldProtocol.h"
enum { PHONGTHAN_SMITH_CORE_OPEN=0x505453, PHONGTHAN_SMITH_CORE_CLICK=0x505454, PHONGTHAN_SMITH_CORE_STATE=0x505455, PHONGTHAN_SMITH_CORE_RESULT=0x505456 };
enum {
    PHONGTHAN_COMPOSE_REQUEST=0x5110, PHONGTHAN_COMPOSE_RESPONSE=0x5111,
    PHONGTHAN_COMPOSE_CORE_OP=0x505443, PHONGTHAN_COMPOSE_CORE_RESULT=0x505444,
    PHONGTHAN_COMPOSE_CORE_PREVIEW=0x505445, PHONGTHAN_COMPOSE_CORE_MODE=0x505446
};
// Local read-only UI quote; this structure is never sent as a transaction.
struct PHONGTHAN_COMPOSE_PREVIEW {
    int Money,RecipeId,Status,Count,MetadataOnly;
    int Quantities[9],Genres[9],Details[9],Stacks[9],Levels[9];
};
enum {
    PT_COMPOSE_OK=0,PT_COMPOSE_NO_RECIPE=1,PT_COMPOSE_NOT_NEAR_NPC=2,
    PT_COMPOSE_INVALID_ITEMS=3,PT_COMPOSE_NO_ROOM=4,PT_COMPOSE_LOCKED=5,
    PT_COMPOSE_TABLE_ERROR=6,PT_COMPOSE_STALE=7,PT_COMPOSE_NO_MONEY=8,
    PT_COMPOSE_NO_COST=9,PT_COMPOSE_UPGRADE_OK=10,PT_COMPOSE_RESTORE_OK=11,
    PT_COMPOSE_FAILED_LEVEL3=12,PT_COMPOSE_FAILED_LOST=13,PT_COMPOSE_FAILED_RESET=14,
    PT_COMPOSE_UPGRADE_RULE_MISSING=15,PT_COMPOSE_UPGRADE_MODE_MIX=16,
    PT_COMPOSE_RATE_UNVERIFIED=17, PT_COMPOSE_SOCKET_FAILED=18, PT_COMPOSE_FABAO_FAILED=19
};
#pragma pack(push,1)
struct PHONGTHAN_COMPOSE_COMMAND {
    PHONGTHAN_WIRE_HEADER Header;
    PHONGTHAN_U32 MapId;
    PHONGTHAN_U32 Items[9];
    // Zero selects the whole stack; -1 excludes it; positive selects a count.
    PHONGTHAN_S32 Quantities[9];
    // 0: first empty hole; 1..4: explicitly selected socket (can replace).
    PHONGTHAN_S32 SocketPosition;
};
struct PHONGTHAN_COMPOSE_RESULT {
    PHONGTHAN_WIRE_HEADER Header;
    PHONGTHAN_U32 MapId,EntityId;
    PHONGTHAN_S32 Result,RecipeId,OutputDetail;
};
#pragma pack(pop)
inline int PhongThanValidateComposeCommand(const void* data,unsigned int size) {
    if(!PhongThanWorldFixedPacket(data,size,PHONGTHAN_COMPOSE_REQUEST,PHONGTHAN_WIRE_FLAG_REQUEST,sizeof(PHONGTHAN_COMPOSE_COMMAND)))return 0;
    const PHONGTHAN_COMPOSE_COMMAND* c=(const PHONGTHAN_COMPOSE_COMMAND*)data;
    if(!c->MapId || c->SocketPosition<0 || c->SocketPosition>4)return 0;
    for(int q=0;q<9;++q)if(c->Quantities[q]<-1 || c->Quantities[q]>10000 || (!c->Items[q] && c->Quantities[q]))return 0;
    for(int i=0;i<9;++i)for(int j=0;j<i;++j)if(c->Items[i] && c->Items[i]==c->Items[j])return 0;
    return 1;
}
inline int PhongThanComposeCountsEqual(const int* details,const int* counts,int count,
                                      const int* wanted,const int* quantities,int required) {
    for(int i=0;i<count;++i){int total=0;for(int j=0;j<required;++j)if(details[i]==wanted[j])total+=quantities[j];if(total!=counts[i])return 0;}
    for(int j=0;j<required;++j){bool found=false;for(int i=0;i<count;++i)if(details[i]==wanted[j])found=true;if(!found)return 0;}
    return count>0 && required>0;
}
#endif
