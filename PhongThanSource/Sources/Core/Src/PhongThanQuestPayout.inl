// One world-thread transaction: reversible currency first, inventory + phase
// CAS second, infallible bounded credit/XP last. Character persistence observes
// this only after the world-thread call returns.
static int PhongThanQuestPayout(int player,int task,int expected,int next,
    PhongThanQuestExchangeRow* rewards,int count,int xp,int money,int credit)
{
    if(player<=0||player>=MAX_PLAYER||task<0||task>=MAX_TASK||xp<0||money<0||credit<0||
       Player[player].m_cTask.GetSaveVal(task)!=expected||Player[player].GetLockState()||Player[player].CheckTrading())return 0;
    int oldCredit=Player[player].m_cTask.GetSaveVal(TASKVALUE_PT_CREDIT);
    if(oldCredit<0||oldCredit>MAX_INT-credit)return 0;
    if(money&&!Player[player].Earn(money))return 0;
    if(!PhongThanQuestExchange(player,task,expected,next,NULL,0,rewards,count)){
        if(money&&!Player[player].Pay(money))g_DebugLog("[EventPayout] money rollback invariant player=%d task=%d",player,task);
        return 0;
    }
    if(credit)Player[player].m_cTask.SetSaveVal(TASKVALUE_PT_CREDIT,oldCredit+credit,TRUE);
    if(xp)Player[player].DirectAddExp(xp);
    return 1;
}
