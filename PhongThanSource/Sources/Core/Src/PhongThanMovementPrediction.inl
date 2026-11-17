#ifndef _SERVER
static PhongThanMovementHistory s_PtMovementHistory;
static unsigned int s_PtMovementSequence=0;
void PhongThanClearPredictedMoves(){s_PtMovementHistory.Clear();}
unsigned int PhongThanBeginPredictedMove()
{
    const int index=Player[CLIENT_PLAYER_INDEX].m_nIndex;
    if(index<=0||index>=MAX_NPC||Npc[index].m_RegionIndex<0)return 0;
    s_PtMovementSequence=s_PtMovementSequence%65535+1;
    Npc[index].m_PtMoveSequence=(unsigned short)s_PtMovementSequence;
    Npc[index].m_PtMoveStep=0;
    return s_PtMovementSequence;
}
bool PhongThanMatchesPredictedMove(unsigned int stamp,unsigned int map,
    unsigned int actor,int x,int y)
{
    return s_PtMovementHistory.Matches(stamp,map,actor,GetTickCount(),x,y);
}
#endif

// Count the same native walk/run simulation call on both peers, including a
// blocked attempt. Record after all early returns. Attacks, knockback, jumps,
// teleports and remote actors never become predicted ordinary walking.
class PhongThanMoveStepGuard
{
    KNpc& npc;
    bool active;
public:
    PhongThanMoveStepGuard(KNpc& n):npc(n),active(false)
    {
        if(!npc.IsPlayer()||!npc.m_PtMoveSequence||
            (npc.m_Doing!=do_walk&&npc.m_Doing!=do_run))return;
#ifndef _SERVER
        if(npc.m_Index!=Player[CLIENT_PLAYER_INDEX].m_nIndex)return;
#endif
        if(npc.m_PtMoveStep<65535)++npc.m_PtMoveStep;
        active=true;
    }
    ~PhongThanMoveStepGuard()
    {
#ifndef _SERVER
        if(active&&npc.m_RegionIndex>=0){
            int x,y;npc.GetMpsPos(&x,&y);
            s_PtMovementHistory.Record(npc.m_PtMoveSequence,npc.m_PtMoveStep,
                SubWorld[0].m_SubWorldID,npc.m_dwID,GetTickCount(),x,y);
        }
#endif
    }
};
