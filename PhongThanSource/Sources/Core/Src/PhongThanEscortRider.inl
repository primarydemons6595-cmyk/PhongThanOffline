#ifndef _SERVER
struct PTEscortVisual {DWORD actor,cart,until;int actorIndex;};
static PTEscortVisual ptEscortVisual[MAX_NPC],ptEscortOwners[MAX_NPC];
void PhongThanSetEscortOwner(int npc,unsigned long actor) {
    if(npc<=0 || npc>=MAX_NPC)return;
    ptEscortOwners[npc].actorIndex=NpcSet.SearchID(actor);
    ptEscortOwners[npc].actor=actor;ptEscortOwners[npc].cart=Npc[npc].m_dwID;ptEscortOwners[npc].until=GetTickCount()+3000;
}
int PhongThanEscortRelation(int first,int second) {
    if(first<=0 || first>=MAX_NPC || second<=0 || second>=MAX_NPC)return -1;
    if(!Npc[first].IsPlayer())return -1;
    PTEscortVisual& state=ptEscortOwners[second];
    if(!state.actor || state.cart!=Npc[second].m_dwID || (LONG)(GetTickCount()-state.until)>=0 ||
       Npc[first].m_SubWorldIndex!=Npc[second].m_SubWorldIndex || Npc[second].m_CurrentLife<=0)return -1;
    return state.actor==Npc[first].m_dwID?relation_dialog:relation_enemy;
}
void PhongThanSetEscortRider(int npc,unsigned long cart) {
    if(npc<=0 || npc>=MAX_NPC)return;
    ptEscortVisual[npc].actor=Npc[npc].m_dwID;ptEscortVisual[npc].cart=cart;
    ptEscortVisual[npc].until=GetTickCount()+3000;
}
bool PhongThanIsEscortRider(int npc) {
    if(npc<=0 || npc>=MAX_NPC)return false;
    if(Npc[npc].IsPlayer() && Npc[npc].m_NpcSettingIdx==556)return true;
    PTEscortVisual& state=ptEscortVisual[npc];
    if(!state.cart || state.actor!=Npc[npc].m_dwID || (LONG)(GetTickCount()-state.until)>=0)return false;
    return true; // The cart may arrive after the owner snapshot during a map load.
}
int PhongThanClientEscortCart(int actor){
    if(actor<=0 || actor>=MAX_NPC || !PhongThanIsEscortRider(actor))return 0;
    PTEscortVisual& s=ptEscortVisual[actor];
    return s.actor==Npc[actor].m_dwID?NpcSet.SearchID(s.cart):0;
}
int PhongThanClientEscortDriver(int npc){
    if(npc<=0 || npc>=MAX_NPC || Npc[npc].IsPlayer() || Npc[npc].m_NpcSettingIdx!=556)return 0;
    PTEscortVisual& owner=ptEscortOwners[npc];
    if(!owner.actor || owner.cart!=Npc[npc].m_dwID || (LONG)(GetTickCount()-owner.until)>=0)return 0;
    int actor=owner.actorIndex;
    if(actor<=0 || actor>=MAX_NPC || Npc[actor].m_dwID!=owner.actor)
        actor=owner.actorIndex=NpcSet.SearchID(owner.actor);
    if(actor<=0 || actor>=MAX_NPC || !PhongThanIsEscortRider(actor))return 0;
    return ptEscortVisual[actor].actor==owner.actor && ptEscortVisual[actor].cart==Npc[npc].m_dwID?actor:0;
}

bool PhongThanEscortHiddenEntity(int npc){return PhongThanClientEscortDriver(npc)>0;}
#endif
