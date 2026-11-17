// User-selected policy, 2026-10-20: one Kim Bai, five equal material
// groups; four equally likely unpolished jades in group five; 5% extra order.
// Pending token is persisted BEFORE inventory work, so a full bag cannot reroll.
namespace PtEscort {
static bool RedeemAnchor(int p) {
    if(!Live(p))return false;
    int n=Player[p].m_nLastNpcIndex,actor=Player[p].m_nIndex;
    if(n<=0 || n>=MAX_NPC || !Npc[n].m_dwID || Npc[n].m_Kind!=kind_dialoger ||
       Map(n)!=1001 || Map(actor)!=1001)return false;
    int x,y,px,py;Npc[n].GetMpsPos(&x,&y);Npc[actor].GetMpsPos(&px,&py);
    return PtEscortNear(x,y,209*256,206*512,96) && PtEscortNear(px,py,x,y,512);
}
static int Redeem(int p) {
    if(!RedeemAnchor(p))return -2;
    if(Player[p].GetLockState() || Player[p].CheckTrading())return 0;
    if(PhongThanKimBaiCount(p)<1)return -1;
    int pending=Get(p,1,20);
    if(!pending) {
        int group=g_Random(5),index=group<4?group:4+g_Random(4);
        pending=1+index+(g_Random(100)<5?8:0);Set(p,1,20,pending);
    }
    if(pending<1 || pending>16)return -3;
    static const int details[8]={5078,5071,5147,5069,257,264,271,278};
    int code=pending-1,index=code%8;bool extra=code>=8;
    PhongThanQuestExchangeRow rewards[2];
    if(!PhongThanDecodeQuestItemTuple(3,details[index],0,0,0,0,rewards[0].item) ||
       !PhongThanDecodeQuestItemTuple(3,100,0,0,0,0,rewards[1].item))return -3;
    rewards[0].count=rewards[1].count=1;
    if(!PhongThanKimBaiAdjust(p,-1))return 0;
    int result=PhongThanQuestPayout(p,PtEscortBase(1)+20,pending,0,rewards,extra?2:1,0,0,0);
    if(result!=1) {
        if(!PhongThanKimBaiAdjust(p,1))g_DebugLog("[Escort] marker rollback failed player=%d",p);
        return 0;
    }
    return pending; // 1..8 material; 9..16 material plus Tuong Quan Lenh.
}
}
int LuaEventEscortRedeem(Lua_State* L) {
    Lua_PushNumber(L,PtEscort::Redeem(GetPlayerIndex(L)));return 1;
}
int LuaEventEscortKimBai(Lua_State* L) {
    int p=GetPlayerIndex(L);Lua_PushNumber(L,PtEscort::RedeemAnchor(p)?PhongThanKimBaiCount(p):0);return 1;
}
