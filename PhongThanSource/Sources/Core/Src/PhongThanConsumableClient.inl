#include "../../../Headers/PhongThanTimedItems.h"
#pragma once
#ifndef _SERVER
#include "PhongThanConsumableData.h"
#include "PhongThanWorldProtocol.h"
static BOOL ptClientPowders[PT_CONSUMABLE_SLOTS];
static DWORD ptClientPowderSeconds[PT_CONSUMABLE_SLOTS];
static DWORD ptClientPowderTick;
int PhongThanConsumableUi(void* output,int size)
{
    if(!output || size!=sizeof(DWORD)*PT_CONSUMABLE_SLOTS) return 0;
    DWORD elapsed=(GetTickCount()-ptClientPowderTick)/1000;
    for(int i=0;i<PT_CONSUMABLE_SLOTS;++i) ((DWORD*)output)[i]=ptClientPowderSeconds[i]>elapsed ? ptClientPowderSeconds[i]-elapsed : 0;
    return 1;
}
void PhongThanConsumableClear(KPlayer& p)
{
    if(&p==&Player[CLIENT_PLAYER_INDEX]) {
        ZeroMemory(ptClientPowders,sizeof(ptClientPowders));
        ZeroMemory(ptClientPowderSeconds,sizeof(ptClientPowderSeconds));
    }
}
void PhongThanConsumableApply(KPlayer& p)
{
    if(&p!=&Player[CLIENT_PLAYER_INDEX] || p.m_nIndex<=0 || p.m_nIndex>=MAX_NPC || !PTConsumableLoad()) return;
    for(int i=0;i<ptConsumableCount;++i) {
        PTConsumableDef& d=ptConsumableDefs[i];
        if((d.kind!=3 && d.kind!=5) || d.detail==133 || !ptClientPowders[PhongThanTimedItemSlot(d.detail)]) continue;
        for(int a=0;a<2;++a) if(d.id[a]) {
            KMagicAttrib attr;ZeroMemory(&attr,sizeof(attr));attr.nAttribType=d.id[a];attr.nValue[0]=d.value[a];
            Npc[p.m_nIndex].ModifyAttrib(p.m_nIndex,&attr);
        }
    }
    if(Npc[p.m_nIndex].m_bRideHorse && ptClientPowders[PhongThanTimedItemSlot(133)]){Npc[p.m_nIndex].m_CurrentRunSpeed*=1.2f;Npc[p.m_nIndex].m_CurrentWalkSpeed*=1.2f;}
}
BOOL PhongThanConsumableReceive(KPlayer& p,const void* packet)
{
    const PHONGTHAN_SELF_VITALS* v=(const PHONGTHAN_SELF_VITALS*)packet;
    if(p.m_nIndex<=0 || p.m_nIndex>=MAX_NPC || v->EntityId!=Npc[p.m_nIndex].m_dwID) return FALSE;
    BOOL changed=FALSE;
    const PHONGTHAN_SELF_CONSUMABLES* extended=NULL;
    if(v->Header.PacketSize==sizeof(PHONGTHAN_SELF_CONSUMABLES)) extended=(const PHONGTHAN_SELF_CONSUMABLES*)packet;
    // A vitals-only update carries no buff snapshot. It must not cancel the
    // timed items already received; expiry and a full snapshot remove them.
    if(!extended)return FALSE;
    ptClientPowderTick=GetTickCount();
    for(int i=0;i<PT_CONSUMABLE_SLOTS;++i) {
        ptClientPowderSeconds[i]=extended ? extended->RemainingSeconds[i] : 0;
        BOOL active=extended && extended->RemainingSeconds[i]>0;
        if(ptClientPowders[i]!=active){ptClientPowders[i]=active;changed=TRUE;}
    }
    if(changed) {
        p.UpdataCurData();
        FILE* f=fopen("pt_consumable_client_trace.log","a");
        if(f){fprintf(f,"tick=%lu event=BUFF_SNAPSHOT_APPLIED entity=%lu life_max=%d mana_max=%d\n",GetTickCount(),v->EntityId,Npc[p.m_nIndex].m_CurrentLifeMax,Npc[p.m_nIndex].m_CurrentManaMax);fclose(f);}
    }
    return changed;
}
#endif
