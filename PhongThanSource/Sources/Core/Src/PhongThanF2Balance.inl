static int PhongThanF2BalanceValue(int player,const PTF2BalanceQuery& q){
        if(player<=0||player>=MAX_PLAYER)return 0;
        KPlayer& p=Player[player];
        if(q.Type==1)return p.m_ItemList.GetMoney(room_equipment);
        if(q.Type==3)return p.m_cTask.GetSaveVal(TASKVALUE_PT_BOUND_MONEY);
        if(q.Type!=0&&q.Type!=2)return 0;
        int total=0;
        for(PlayerItem* slot=p.m_ItemList.GetFirstItem();slot;slot=p.m_ItemList.GetNextItem()){
            if(slot->nPlace!=pos_equiproom&&slot->nPlace!=pos_equiproomex&&slot->nPlace!=pos_ptbag2&&slot->nPlace!=pos_ptbag3)continue;
            int i=slot->nIdx;if(i<=0||i>=MAX_ITEM||!Item[i].GetID()||Item[i].GetLock()->IsLock())continue;
            bool matches=q.Type==0?(Item[i].GetGenre()==item_task&&Item[i].GetDetailType()==47):
                (Item[i].GetGenre()==q.Genre&&Item[i].GetDetailType()==q.Detail&&Item[i].GetParticular()==q.Particular&&
                 ((q.Genre!=0&&q.Genre!=1&&q.Genre!=6&&q.Genre!=7)||Item[i].GetLevel()==q.Level)&&
                 ((q.Genre!=0&&q.Genre!=6)||Item[i].GetSeries()==q.Series));
            if(!matches)continue;int count=Item[i].GetStackNum();if(count<=0)count=1;
            if(total>2147483647-count)return 2147483647;total+=count;
        }
        return total;
}
