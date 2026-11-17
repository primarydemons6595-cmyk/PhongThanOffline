int PhongThanBuyWithF2Currency(int player,KItem& prototype,int quantity,int unitPrice,int slot) {
    if(player<=0 || player>=MAX_PLAYER || quantity<=0 || quantity>255 || unitPrice<=0 || unitPrice>INT_MAX/quantity)return 0;
    PhongThanQuestExchangeRow need,reward;
    const PTF2Currency* currency=PhongThanF2CurrencyConfig(slot);if(!currency)return 0;
    int type=currency->Query.Type;if(type<0||type>3)return 0;
    int cost=unitPrice*quantity;if(PhongThanF2CurrencyCount(player,slot)<cost)return 0;
    if(type==0&&!PhongThanDecodeQuestItemTuple(4,47,0,0,0,0,need.item))return 0;
    if(type==2){const PTF2BalanceQuery& q=currency->Query;if(!PhongThanDecodeQuestItemTuple(q.Genre,q.Detail,q.Particular,q.Level,q.Series,0,need.item))return 0;}
    // The shop prototype already has a runtime identity. Lua tuple decoding
    // would reinterpret script-book IDs and grant a different item.
    reward.item.genre=prototype.GetGenre();reward.item.detail=prototype.GetDetailType();
    reward.item.particular=prototype.GetParticular();reward.item.level=prototype.GetLevel();
    reward.item.series=prototype.GetSeries();reward.item.luck=0;
    need.count=unitPrice*quantity;reward.count=quantity;
    if(prototype.GetExpirePoint())prototype.SetExpireTime(KSG_GetCurSec()+prototype.GetExpirePoint());
    int task=TASKVALUE_PT_XU_EXCHANGE,value=Player[player].m_cTask.GetSaveVal(task);
    int boundBefore=Player[player].m_cTask.GetSaveVal(TASKVALUE_PT_BOUND_MONEY);
    if(type==1&&!Player[player].m_ItemList.CostMoney(cost))return 0;
    if(type==3){if(boundBefore<cost)return 0;Player[player].m_cTask.SetSaveVal(TASKVALUE_PT_BOUND_MONEY,boundBefore-cost,TRUE);}
    int ok=PhongThanQuestExchange(player,task,value,value==1?0:1,&need,(type==0||type==2)?1:0,&reward,1,true,&prototype,true,0,true);
    if(!ok&&type==1)Player[player].Earn(cost);
    if(!ok&&type==3)Player[player].m_cTask.SetSaveVal(TASKVALUE_PT_BOUND_MONEY,boundBefore,TRUE);
    if(ok)Player[player].Save();return ok;
}
