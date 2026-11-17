// Shared server transaction for trusted NPC Lua. A quest phase is a compare-
// and-set guard; no requirement is destroyed until all rewards fit in F4.
#ifdef _SERVER
#ifndef PT_QUEST_REWARD_NOTICE
#define PT_QUEST_REWARD_NOTICE(player,index) ((void)0)
#endif
#ifndef PT_QUEST_REWARD_NOTICE_AMOUNT
#define PT_QUEST_REWARD_NOTICE_AMOUNT(player,index,amount) PT_QUEST_REWARD_NOTICE(player,index)
#endif
template<class T> static bool PhongThanQuestStackCompatible(T& a,T& b){
 return a.IsStack()&&b.IsStack()&&!a.GetLock()->IsLock()&&!b.GetLock()->IsLock()&&a.GetRow()==b.GetRow()&&
 a.GetGenre()==b.GetGenre()&&a.GetDetailType()==b.GetDetailType()&&a.GetParticular()==b.GetParticular()&&
 a.GetSeries()==b.GetSeries()&&a.GetLevel()==b.GetLevel()&&a.GetExpireTime()==b.GetExpireTime()&&
 a.GetLockSell()==b.GetLockSell()&&a.GetLockTrade()==b.GetLockTrade()&&a.GetLockDrop()==b.GetLockDrop()&&a.GetParam()==b.GetParam();
}
#ifndef PT_QUEST_ITEM_PARAM
#define PT_QUEST_ITEM_PARAM(item) ((item).GetParam())
#endif
#ifndef PT_QUEST_ITEM_NOW
#define PT_QUEST_ITEM_NOW() KSG_GetCurSec()
#endif
struct PhongThanQuestExchangeRow { PhongThanQuestItemTuple item;int count; };
static bool PhongThanReadQuestExchangeRows(Lua_State* L,int index,
    PhongThanQuestExchangeRow* rows,int& count)
{
    if(!Lua_IsTable(L,index))return false;
    count=Lua_GetN(L,index);if(count<0 || count>16)return false;
    const int top=Lua_GetTopIndex(L);
    for(int i=0;i<count;++i){
        Lua_RawGetI(L,index,i+1);const int row=Lua_GetTopIndex(L);
        if(!Lua_IsTable(L,row)){Lua_SetTopIndex(L,top);return false;}
        int values[7];bool valid=true;
        for(int j=0;j<7;++j){
            Lua_RawGetI(L,row,j+1);double v=Lua_ValueToNumber(L,-1);
            if(!Lua_IsNumber(L,-1) || v<0 || v>2147483647.0 || v!=(int)v)valid=false;
            values[j]=valid?(int)v:0;Lua_Pop(L,1);
        }
        if(!valid || values[6]<1 || values[6]>10000 ||
           !PhongThanDecodeQuestItemTuple(values[0],values[1],values[2],values[3],values[4],values[5],rows[i].item))
        {Lua_SetTopIndex(L,top);return false;}
        rows[i].count=values[6];Lua_Pop(L,1);
    }
    return true;
}

extern int PhongThanConsumableInitialCharges(int,int,int,int,const char*);
static int PhongThanQuestExchange(int player,int task,int expected,int next,
    PhongThanQuestExchangeRow* needs,int needCount,
    PhongThanQuestExchangeRow* rewards,int rewardCount,bool stackRewards=false,KItem* prototype=0,bool carriedNeeds=false,int exactNeed=0,bool bagNeedsOnly=false,const int* needParams=0,bool mergeExisting=false)
{
    if(player<=0 || player>=MAX_PLAYER || task<0 || task>=MAX_TASK || expected==next ||
       needCount<0 || needCount>16 || rewardCount<0 || rewardCount>16 ||
       Player[player].m_cTask.GetSaveVal(task)!=expected ||
       Player[player].GetLockState() || Player[player].CheckTrading())return 0;
    if(mergeExisting&&needCount)return 0; // Reward-only opt-in; existing quest exchanges keep their original path.
    KItemList& bag=Player[player].m_ItemList;
    struct Merge {int index,stack;} merged[128];int mergedCount=0;
    bool absorbed[128];int awarded[128];ZeroMemory(absorbed,sizeof(absorbed));ZeroMemory(awarded,sizeof(awarded));
    struct Take {int index,quantity,stack;ItemPos position;bool changed;} take[64];
    int takeCount=0;
    for(int n=0;n<needCount;++n){
        int left=needs[n].count;
        PlayerItem* owned=bag.GetFirstItem();
        while(owned && left>0){
            int index=owned->nIdx;
            const bool physicalCurrency=needs[n].item.genre==item_task && (needs[n].item.detail==46 || needs[n].item.detail==47);
            const bool carried=owned->nPlace==pos_equiproom || ((physicalCurrency || carriedNeeds) &&
                (owned->nPlace==pos_equiproomex || owned->nPlace==pos_ptbag2 || owned->nPlace==pos_ptbag3 || (carriedNeeds && !bagNeedsOnly && owned->nPlace==pos_immediacy)));
            if(carried && (!exactNeed || index==exactNeed) && index>0 && index<MAX_ITEM && Item[index].GetID() &&
               (!needParams || !needParams[n] || PT_QUEST_ITEM_PARAM(Item[index])==needParams[n]) &&
               ((PT_QUEST_ITEM_PARAM(Item[index])&0xff000000)!=0x2c000000 || (needParams&&needParams[n]==PT_QUEST_ITEM_PARAM(Item[index])&&Item[index].GetExpireTime()>(int)PT_QUEST_ITEM_NOW())) &&
               PhongThanQuestItemMatches(Item[index],needs[n].item) && !Item[index].GetLock()->IsLock()){
                int slot=-1;for(int j=0;j<takeCount;++j)if(take[j].index==index)slot=j;
                int used=slot<0?0:take[slot].quantity;
                int available=Item[index].GetStackNum()-used;
                if(available>0){
                    if(slot<0){
                        if(takeCount==64)return 0;slot=takeCount++;
                        take[slot].index=index;take[slot].quantity=0;
                        take[slot].stack=Item[index].GetStackNum();take[slot].changed=false;
                        take[slot].position.nPlace=owned->nPlace;take[slot].position.nX=owned->nX;take[slot].position.nY=owned->nY;
                    }
                    int amount=available<left?available:left;take[slot].quantity+=amount;left-=amount;
                }
            }
            owned=bag.GetNextItem();
        }
        if(left)return 0;
    }
    int created[128],createdCount=0;const int createdLimit=stackRewards?128:64;bool success=true;
    for(int r=0;r<rewardCount && success;++r){
        if(!stackRewards && rewards[r].count>createdLimit-createdCount){success=false;break;}
        for(int j=0;j<rewards[r].count;){
            if(createdCount>=createdLimit){success=false;break;}
            const PhongThanQuestItemTuple& t=rewards[r].item;
            int levels[MAX_ITEM_MAGICLEVEL];ZeroMemory(levels,sizeof(levels));
            int index=prototype ? ItemSet.Add(prototype) : ItemSet.Add(t.genre,t.series,t.level,t.luck,t.detail,t.particular,levels,g_SubWorldSet.GetGameVersion());
            if(index<=0){success=false;break;}
            created[createdCount++]=index;
            int units=1;
            // A reusable bottle stores charges, not a quantity of bottles.
            const bool charged = prototype && PhongThanConsumableInitialCharges(prototype->GetGenre(),prototype->GetDetailType(),prototype->GetParticular(),prototype->GetMaxStackNum(),prototype->GetScript())>0;
            if(stackRewards && Item[index].IsStack() && !charged){units=rewards[r].count-j;if(units>Item[index].GetMaxStackNum())units=Item[index].GetMaxStackNum();Item[index].SetStackNum(units);}
            j+=units;
            if(!PhongThanQuestItemMatches(Item[index],t) || Item[index].GetWidth()<=0 || Item[index].GetHeight()<=0)
            {success=false;break;}
        }
    }
    int i;
    // Detach (do not destroy) inputs so their space can be used for rewards.
    // All failure paths restore the same object IDs and original cells.
    for(i=0;i<takeCount && success;++i){
        Take& input=take[i];
        if(input.quantity==input.stack){if(!bag.Remove(input.index)){success=false;break;}}
        else {Item[input.index].SetStackNum(input.stack-input.quantity);bag.SyncItem(input.index);}
        input.changed=true;
    }
    for(i=0;i<createdCount && success;++i){
        int index=created[i];awarded[i]=Item[index].GetStackNum();
        if(mergeExisting&&Item[index].IsStack()){
            int left=Item[index].GetStackNum();
            for(PlayerItem* owned=bag.GetFirstItem();owned&&left>0;owned=bag.GetNextItem()){
                int target=owned->nIdx;bool pending=false;for(int c=0;c<createdCount;++c)if(created[c]==target)pending=true;
                bool carried=owned->nPlace==pos_equiproom||owned->nPlace==pos_equiproomex||owned->nPlace==pos_ptbag2||owned->nPlace==pos_ptbag3;
                if(!carried||pending||target<=0||target>=MAX_ITEM||!Item[target].GetID()||!PhongThanQuestStackCompatible(Item[target],Item[index]))continue;
                int room=Item[target].GetMaxStackNum()-Item[target].GetStackNum();if(room<=0)continue;
                int slot=-1;for(int m=0;m<mergedCount;++m)if(merged[m].index==target)slot=m;
                if(slot<0){if(mergedCount>=128){success=false;break;}slot=mergedCount++;merged[slot].index=target;merged[slot].stack=Item[target].GetStackNum();}
                int amount=room<left?room:left;Item[target].SetStackNum(Item[target].GetStackNum()+amount);bag.SyncItem(target);left-=amount;
            }
            if(!success)break;if(!left){absorbed[i]=true;continue;}Item[index].SetStackNum(left);
        }
        ItemPos pos;
        if(!bag.SearchPosition(Item[created[i]].GetWidth(),Item[created[i]].GetHeight(),&pos,true) ||
           !bag.Add(created[i],pos.nPlace,pos.nX,pos.nY,false)){success=false;break;}
    }
    if(success){
        Player[player].m_cTask.SetSaveVal(task,next,TRUE);
        for(i=0;i<takeCount;++i)if(take[i].quantity==take[i].stack)ItemSet.Remove(take[i].index);
        for(i=0;i<createdCount;++i){
            if(mergeExisting)PT_QUEST_REWARD_NOTICE_AMOUNT(player,created[i],awarded[i]);else PT_QUEST_REWARD_NOTICE(player,created[i]);
            if(absorbed[i])ItemSet.Remove(created[i]);
        }
        return 1;
    }
    for(i=0;i<createdCount;++i){if(bag.FindSame(created[i]))bag.Remove(created[i]);ItemSet.Remove(created[i]);}
    for(i=0;i<mergedCount;++i){Item[merged[i].index].SetStackNum(merged[i].stack);bag.SyncItem(merged[i].index);}
    for(i=0;i<takeCount;++i)if(take[i].changed){
        Take& input=take[i];
        if(input.quantity==input.stack){
            if(!bag.Add(input.index,input.position.nPlace,input.position.nX,input.position.nY,false))
                g_DebugLog("[QuestExchange] restore invariant failed player=%d task=%d item=%d",player,task,input.index);
        }else{Item[input.index].SetStackNum(input.stack);bag.SyncItem(input.index);}
    }
    return 0;
}

int LuaQuestExchange(Lua_State* L)
{
    int result=0;const int player=GetPlayerIndex(L);
    const int argc=Lua_GetTopIndex(L);
    if(player>0 && (argc==5 || argc==6)){
        int args[3];bool valid=true;
        for(int i=0;i<3;++i){double v=Lua_ValueToNumber(L,i+1);
            if(!Lua_IsNumber(L,i+1) || v<0 || v>2147483647.0 || v!=(int)v)valid=false;
            args[i]=valid?(int)v:0;}
        // Optional numeric flag: batch stackable rewards into their native
        // piles; reservoirs remain separate full objects through IsStack().
        bool stackRewards=false;
        if(argc==6){
            if(!Lua_IsNumber(L,6) || Lua_ValueToNumber(L,6)!=1)valid=false;
            else stackRewards=true;
        }
        PhongThanQuestExchangeRow needs[16],rewards[16];int needCount=0,rewardCount=0;
        if(valid && PhongThanReadQuestExchangeRows(L,4,needs,needCount) && PhongThanReadQuestExchangeRows(L,5,rewards,rewardCount))
            result=PhongThanQuestExchange(player,args[0],args[1],args[2],needs,needCount,rewards,rewardCount,stackRewards);
    }
    Lua_PushNumber(L,result);return 1;
}
#endif
