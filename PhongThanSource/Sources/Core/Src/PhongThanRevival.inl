#ifdef _SERVER
#include "PhongThanRevival.h"
namespace PtRevival {
struct Loss {DWORD owner,entity;int experience;Loss():owner(0),entity(0),experience(0){}};
static Loss losses[MAX_PLAYER];
struct Context {int player,choice;};
static bool Move(void* opaque){
    Context& c=*(Context*)opaque;int index=Player[c.player].m_nIndex;
    if(index<=0||index>=MAX_NPC||!Npc[index].IsPlayer()||Npc[index].m_CurrentLife>0)return false;
    Player[c.player].Revive(c.choice==2?LOCAL_REVIVE_TYPE:REMOTE_REVIVE_TYPE);
    return Npc[index].m_CurrentLife>0;
}
}
void PhongThanRecordDeathLoss(int p,int beforeExperience){
    if(p<=0||p>=MAX_PLAYER)return;int index=Player[p].m_nIndex;if(index<=0||index>=MAX_NPC)return;
    PtRevival::Loss& loss=PtRevival::losses[p];loss.owner=Player[p].m_dwID;loss.entity=Npc[index].m_dwID;
    loss.experience=beforeExperience>Player[p].m_nExp?beforeExperience-Player[p].m_nExp:0;
}
bool PhongThanTryRevive(int p,int choice){
    if(p<=0||p>=MAX_PLAYER||choice<0||choice>2||!Player[p].m_dwID||
       Player[p].GetLockState()||Player[p].CheckTrading())return false;
    int index=Player[p].m_nIndex;
    // OnDeath advances a player to do_revive while awaiting this choice.
    // Life is authoritative; also recover old zero-life actors left standing.
    if(index<=0||index>=MAX_NPC||!Npc[index].IsPlayer()||Npc[index].m_CurrentLife>0)return false;
    PtRevival::Context context;context.player=p;context.choice=choice;
    bool ok=false;
    if(choice==0)ok=PtRevival::Move(&context);
    else {
        PhongThanQuestExchangeRow cost;
        if(!PhongThanDecodeQuestItemTuple(choice==1?8:4,choice==1?32:47,choice==1?5:0,0,0,0,cost.item))return false;
        cost.count=1;
        // Payment and map transition share rollback; no regeneration of an IB item on failure.
        int task=TASKVALUE_PT_REVIVAL_PAYMENT,old=Player[p].m_cTask.GetSaveVal(task),next=old==2147483647?1:old+1;
        ok=PhongThanEventTicketsPayment(p,&cost,1,task,old,next,PtRevival::Move,&context)==1;
    }
    if(ok){
        PtRevival::Loss& loss=PtRevival::losses[p];
        if((choice==1||choice==2)&&loss.owner==Player[p].m_dwID&&loss.entity==Npc[index].m_dwID&&loss.experience>0)
            Player[p].DirectAddExp(loss.experience);
        loss.owner=0;loss.entity=0;loss.experience=0;
    }
    return ok;
}
#endif
