// Inventory transaction shared with the native regression host. Inputs retain
// their original item IDs until the local map transition has committed.
typedef bool (*PhongThanEventMove)(void* context);
static int PhongThanEventTicketsPayment(int player,const PhongThanQuestExchangeRow* needs,int needCount,
    int task,int expected,int next,PhongThanEventMove move,void* context)
{
    if(player<=0||player>=MAX_PLAYER||task<0||task>=MAX_TASK||needCount<0||needCount>4||(needCount&&!needs)||
       Player[player].m_cTask.GetSaveVal(task)!=expected||Player[player].GetLockState()||Player[player].CheckTrading()||!move)return -2;
    KItemList& bag=Player[player].m_ItemList;
    struct Take {int index,quantity,stack;ItemPos position;bool changed;} take[64];
    int used=0;
    for(int n=0;n<needCount;++n){
    if(needs[n].count<0||needs[n].count>1000)return -2;
    int left=expected==next?0:needs[n].count;
    PlayerItem* item=bag.GetFirstItem();
    while(item&&left>0){
        int index=item->nIdx;
        const bool currency=needs[n].item.genre==item_task&&(needs[n].item.detail==46||needs[n].item.detail==47);
        const bool carried=item->nPlace==pos_equiproom||(currency&&(item->nPlace==pos_equiproomex||item->nPlace==pos_ptbag2||item->nPlace==pos_ptbag3));
        if(carried&&index>0&&index<MAX_ITEM&&Item[index].GetID()&&
           PhongThanQuestItemMatches(Item[index],needs[n].item)&&!Item[index].GetLock()->IsLock()){
            int slot=-1;for(int k=0;k<used;++k)if(take[k].index==index)slot=k;
            int available=Item[index].GetStackNum()-(slot<0?0:take[slot].quantity);
            if(available>0){
                if(slot<0){if(used>=64)return -2;slot=used++;take[slot].index=index;take[slot].stack=Item[index].GetStackNum();
                    take[slot].quantity=0;take[slot].changed=false;
                    take[slot].position.nPlace=item->nPlace;take[slot].position.nX=item->nX;take[slot].position.nY=item->nY;}
                int amount=available<left?available:left;take[slot].quantity+=amount;left-=amount;
            }
        }
        item=bag.GetNextItem();
    }
    if(left)return -1;
    }
    bool ok=true;int i;
    for(i=0;i<used&&ok;++i){
        Take& part=take[i];
        if(part.quantity==part.stack){if(!bag.Remove(part.index)){ok=false;break;}}
        else{Item[part.index].SetStackNum(part.stack-part.quantity);bag.SyncItem(part.index);}
        part.changed=true;
    }
    if(ok)ok=move(context);
    if(ok){
        Player[player].m_cTask.SetSaveVal(task,next,TRUE);
        for(i=0;i<used;++i)if(take[i].quantity==take[i].stack)ItemSet.Remove(take[i].index);
        return 1;
    }
    for(i=0;i<used;++i)if(take[i].changed){
        Take& part=take[i];
        if(part.quantity==part.stack){
            if(!bag.Add(part.index,part.position.nPlace,part.position.nX,part.position.nY,false))
                g_DebugLog("[EventTicket] restore invariant failed player=%d item=%d",player,part.index);
        }else{Item[part.index].SetStackNum(part.stack);bag.SyncItem(part.index);}
    }
    return 0;
}
static int PhongThanEventTicketPayment(int player,const PhongThanQuestItemTuple& ticket,int count,
    int task,int expected,int next,PhongThanEventMove move,void* context)
{
    PhongThanQuestExchangeRow row;row.item=ticket;row.count=count;
    return PhongThanEventTicketsPayment(player,&row,1,task,expected,next,move,context);
}
