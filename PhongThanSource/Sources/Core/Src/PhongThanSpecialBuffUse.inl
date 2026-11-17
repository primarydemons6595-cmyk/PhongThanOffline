#include "../../../Headers/PhongThanOriginalItemBuffData.h"
#include "../../../Headers/PhongThanWeeklyDragon.h"
#include "../../../Headers/PhongThanWeeklyDragonMessages.h"
int PhongThanUseOriginalBuffItem(int player,int item)
{
    if(item<=0 || item>=MAX_ITEM)return -1;
    if(Item[item].GetGenre()!=item_ibitem)return -1;
    const int detail=Item[item].GetDetailType(),part=Item[item].GetParticular();
    if(!((detail==418&&part==0)||(detail==7445&&part==2)||(detail==9444&&part==2)))return -1;
    if(player<=0 || player>=MAX_PLAYER || !PhongThanIsLivePlayer(player) ||
       Player[player].GetLockState() || Player[player].CheckTrading() || !PtPetCarried(player,item) ||
       Npc[Player[player].m_nIndex].m_CurrentLife<=0)return 0;
    if(Item[item].m_CommonAttrib.nExpireTime>0 && Item[item].m_CommonAttrib.nExpireTime<=(int)KSG_GetCurSec()){if(detail==7445)Player[player].SendSystemMessage(MESSAGE_SYSTEM_ANNOUCE_HEAD,ptDragonExpired);return 0;}
    if(detail==9444){
        PHONGTHAN_IBBUFF_STORE* store=GetPlayerIBBuffStore(player);if(!store||FindIBBuffSlot(store,9445)>=0)return 0;
        PHONGTHAN_IBBUFF_STORE previous=*store;
        if(!AddIBBuffToStore(store,Player[player].m_nIndex,9445,604800,0))return 0;
        const int slot=FindIBBuffSlot(store,9445);if(slot<0){*store=previous;return 0;}store->Entry[slot].nTimes=1;
        if(!Player[player].m_ItemList.RemoveItem(item,1)){*store=previous;return 0;}
        PersistPlayerIBBuffStore(player);Player[player].UpdataCurData();return 1;
    }
    if(detail==418){
        if(!PhongThanPetHopeAdjust(player,1)){Player[player].SendSystemMessage(MESSAGE_SYSTEM_ANNOUCE_HEAD,ptOriginalHopeDenied);return 0;}
        if(!Player[player].m_ItemList.RemoveItem(item,1)){PhongThanPetHopeAdjust(player,-1);return 0;}
        Player[player].SendSystemMessage(MESSAGE_SYSTEM_ANNOUCE_HEAD,ptOriginalHopeAdded);return 1;
    }
    PHONGTHAN_IBBUFF_STORE* store=GetPlayerIBBuffStore(player);if(!store)return 0;
    const DWORD now=KSG_GetCurSec();int uses=0,lastDay=-1;
    if(!PtWeeklyDragonState(Item[item].GetParam(),uses,lastDay))return 0;
    const int day=PtWeeklyDragonDay(now);
    if(uses>=7||day<=lastDay){Player[player].SendSystemMessage(MESSAGE_SYSTEM_ANNOUCE_HEAD,ptDragonDailyUsed);return 0;}
    if(Player[player].m_uMustSave==SAVE_DOING)return 0;
    DWORD expiry=PtWeeklyDragonExpiry(Item[item].GetParam(),Item[item].m_CommonAttrib.nExpireTime,now);
    if(expiry<=now){Player[player].SendSystemMessage(MESSAGE_SYSTEM_ANNOUCE_HEAD,ptDragonExpired);return 0;}
    const int cost=PtWeeklyDragonCost(uses);
    if(cost>Player[player].m_ItemList.GetEquipmentMoney()){Player[player].SendSystemMessage(MESSAGE_SYSTEM_ANNOUCE_HEAD,ptDragonMoneyNeeded);return 0;}
    const int profession=Player[player].m_cProfession.GetProfession();if(profession<0 || profession>3)return 0;
    const PTOriginalItemBuffDef& def=ptOriginalItemBuffs[profession==1||profession==2?1:0];
    const PHONGTHAN_IBBUFF_STORE previous=*store;
    RemoveIBBuffFromStore(store,Player[player].m_nIndex,7446);
    RemoveIBBuffFromStore(store,Player[player].m_nIndex,7447);
    const int seconds=(int)min((DWORD)86400,expiry-now);
    if(!AddIBBuffToStore(store,Player[player].m_nIndex,def.id,seconds,0)){*store=previous;return 0;}
    const int slot=FindIBBuffSlot(store,def.id);if(slot<0){*store=previous;return 0;}
    store->Entry[slot].nTimes=1;
    if(cost&&!Player[player].m_ItemList.CostMoney(cost)){*store=previous;return 0;}
    Item[item].SetParam(PtWeeklyDragonPack(uses+1,day));Item[item].SetExpireTime(expiry);
    PersistPlayerIBBuffStore(player);Player[player].m_ItemList.SyncItem(item);
    Player[player].m_uMustSave=SAVE_REQUEST;Player[player].UpdataCurData();
    Player[player].SendSystemMessage(MESSAGE_SYSTEM_ANNOUCE_HEAD,ptOriginalDragonAdded);return 1;
}
int LuaUseOriginalBuffItem(Lua_State* L)
{
    const int top=Lua_GetTopIndex(L);const int player=GetPlayerIndex(L);Lua_SetTopIndex(L,top);
    int result=0;
    if(top==1 && Lua_IsNumber(L,1)){
        const double value=Lua_ValueToNumber(L,1);
        if(value>0 && value<MAX_ITEM && value==(int)value)result=PhongThanUseOriginalBuffItem(player,(int)value)>0?1:0;
    }
    Lua_PushNumber(L,result);return 1;
}
