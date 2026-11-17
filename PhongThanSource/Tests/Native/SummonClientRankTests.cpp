#include <windows.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../../Headers/PhongThanGameplayProtocol.h"
#include "../../Headers/PhongThanSummonRules.h"
#define MAX_NPC 8
struct Actor {DWORD m_dwID;} Npc[MAX_NPC];
struct Lookup {
    int SearchID(DWORD id){for(int i=1;i<MAX_NPC;++i)if(Npc[i].m_dwID==id)return i;return 0;}
} NpcSet;
#include "SummonClientCache.inl"
int main() {
    PHONGTHAN_SUMMON_OWNER_SYNC p;memset(&p,0,sizeof(p));
    PhongThanInitializeWireHeader(&p.Header,PHONGTHAN_MSG_GAMEPLAY_SUMMON_OWNER,sizeof(p),PHONGTHAN_WIRE_FLAG_RESPONSE,0);
    p.MapId=1002;p.PetId=21;p.OwnerId=10;p.SkillLevel=10;
    assert(PhongThanValidateSummonOwnerSync(&p,sizeof(p)));
    assert(!PhongThanValidateSummonOwnerSync(&p,sizeof(p)-1));
    p.SkillLevel=0;assert(!PhongThanValidateSummonOwnerSync(&p,sizeof(p)));
    p.SkillLevel=64;assert(!PhongThanValidateSummonOwnerSync(&p,sizeof(p)));
    p.SkillLevel=10;p.TargetLocked=2;assert(!PhongThanValidateSummonOwnerSync(&p,sizeof(p)));p.TargetLocked=1;
    Npc[1].m_dwID=10;Npc[2].m_dwID=21;
    PhongThanSummonBindClientOwner(21,10,10,1);
    assert(PhongThanClientSummonOwner(2)==1&&PhongThanClientSummonRank(2)==10);
    assert(PhongThanSummonShownLevel(90,PhongThanClientSummonRank(2))==10);
    Npc[2].m_dwID=22;assert(!PhongThanIsClientSummon(2)&&!PhongThanClientSummonRank(2));
    PhongThanSummonBindClientOwner(22,10,1,0);assert(PhongThanClientSummonRank(2)==1&&!PhongThanClientSummonLocked(2));
    PhongThanSummonBindClientOwner(22,10,64,1);assert(PhongThanClientSummonRank(2)==1);
    puts("PASS actual client cache + wire validation: rank10 display, owner binding, malformed packets, reused NPC slot and invalid rank");
    return 0;
}
