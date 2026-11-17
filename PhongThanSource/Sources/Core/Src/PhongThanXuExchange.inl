// Shared inventory currency transaction. Existing ownership, personal locks,
// bag placement and rollback are enforced by the quest exchange implementation.
#ifdef _SERVER
static int PhongThanXuCount(int player,int detail) {
    if(player<=0 || player>=MAX_PLAYER)return 0;
    int total=0;KItemList& bag=Player[player].m_ItemList;
    for(PlayerItem* p=bag.GetFirstItem();p;p=bag.GetNextItem()) {
        int i=p->nIdx;
        if((p->nPlace==pos_equiproom || p->nPlace==pos_equiproomex || p->nPlace==pos_ptbag2 || p->nPlace==pos_ptbag3) &&
           i>0 && i<MAX_ITEM && Item[i].GetID() &&
           Item[i].GetGenre()==item_task && Item[i].GetDetailType()==detail && !Item[i].GetLock()->IsLock()) {
            int count=Item[i].GetStackNum();if(count>0 && total<=INT_MAX-count)total+=count;
        }
    }
    return total;
}
static int PhongThanXuTrade(int player,int from,int amount,int to,int resultCount) {
    if(player<=0 || player>=MAX_PLAYER || (from!=46 && from!=47) || (to!=46 && to!=47) ||
       from==to || amount<=0 || amount>100000 || resultCount<=0 || resultCount>100000)return 0;
    PhongThanQuestExchangeRow need,reward;
    if(!PhongThanDecodeQuestItemTuple(4,from,0,0,0,0,need.item) ||
       !PhongThanDecodeQuestItemTuple(4,to,0,0,0,0,reward.item))return 0;
    need.count=amount;reward.count=resultCount;
    int task=TASKVALUE_PT_XU_EXCHANGE,value=Player[player].m_cTask.GetSaveVal(task);
    return PhongThanQuestExchange(player,task,value,value==1?0:1,&need,1,&reward,1,true);
}
int LuaLeQuanCurrency(Lua_State* L) {
    int result=0,player=GetPlayerIndex(L);
    if(PhongThanNearLeQuan(player) && Lua_GetTopIndex(L)==2 && Lua_IsNumber(L,1) && Lua_IsNumber(L,2)) {
        double op=Lua_ValueToNumber(L,1),v=Lua_ValueToNumber(L,2);
        if(v>=1 && v<=1000 && v==(int)v) {
            int count=(int)v;
            if(op==0)result=PhongThanXuTrade(player,46,count,47,count*20);
            else if(op==1)result=PhongThanXuTrade(player,47,count*20,46,count);
        }
    }
    Lua_PushNumber(L,result);return 1;
}
// F2 uses the actual catalogue item template identity and the displayed price.
// Catalogue currency is server-owned; generation/space failures refund payment.
#include "PhongThanF2Purchase.inl"
int PhongThanPayBagUnlock(int player,int page,int rows) {
    if(player<=0 || player>=MAX_PLAYER || page<1 || page>3 || rows<0 || rows>=7)return 0;
    PhongThanQuestExchangeRow cloth,coins;
    if(!PhongThanDecodeQuestItemTuple(8,rows<4?269:270,2,0,0,0,cloth.item) ||
       !PhongThanDecodeQuestItemTuple(4,47,0,0,0,0,coins.item))return 0;
    cloth.count=1;coins.count=rows<4?8:128;
    int task=TASKVALUE_PT_BAG_ROWS_1+page-1;
    // Exact original CostID 50/51: cloth first, then physical Xu. Both paths
    // commit the payment and permanent row together; the losing path is a no-op.
    if(PhongThanQuestExchange(player,task,rows,rows+1,&cloth,1,0,0,true,0,true))return 1;
    return PhongThanQuestExchange(player,task,rows,rows+1,&coins,1,0,0);
}
#endif
