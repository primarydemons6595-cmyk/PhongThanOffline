// Original event APIs. Inventory and mission state remain engine-owned.
#include <time.h>
#include "../../../Headers/PhongThanCommerceMarket.h"
static PhongThanCommerceMarket g_PhongThanCommerce((unsigned long)time(NULL));

static int PhongThanEventInt(Lua_State *L,int index,int minimum,int maximum,int *result)
{
    if(Lua_GetTopIndex(L)<index || !Lua_IsNumber(L,index))return 0;
    const double value=Lua_ValueToNumber(L,index);
    if(!(value>=minimum && value<=maximum) || value!=(int)value)return 0;
    *result=(int)value;return 1;
}
static int PhongThanGoodsArgs(Lua_State *L,int *shop,int *good)
{
    return PhongThanEventInt(L,1,1,9,shop) && PhongThanEventInt(L,2,1,27,good);
}
int LuaGetGoodsNameCompat(Lua_State *L)
{
    int good=0;Lua_PushString(L,PhongThanEventInt(L,1,1,27,&good)?g_PhongThanCommerce.Name(good):"");return 1;
}
int LuaGetGoodsWeightCompat(Lua_State *L)
{
    int good=0;Lua_PushNumber(L,PhongThanEventInt(L,1,1,27,&good)?g_PhongThanCommerce.Weight(good):0);return 1;
}
int LuaGetGoodsNumCompat(Lua_State *L)
{
    int shop=0,good=0;Lua_PushNumber(L,PhongThanGoodsArgs(L,&shop,&good)?g_PhongThanCommerce.Stock(shop,good,(unsigned long)time(NULL)):0);return 1;
}
int LuaGetGoodsPurchasePriceCompat(Lua_State *L)
{
    int shop=0,good=0;Lua_PushNumber(L,PhongThanGoodsArgs(L,&shop,&good)?g_PhongThanCommerce.Purchase(shop,good,(unsigned long)time(NULL)):0);return 1;
}
int LuaGetGoodsSalePriceCompat(Lua_State *L)
{
    int shop=0,good=0;Lua_PushNumber(L,PhongThanGoodsArgs(L,&shop,&good)?g_PhongThanCommerce.Sale(shop,good,(unsigned long)time(NULL)):0);return 1;
}
static int PhongThanChangeGoods(Lua_State *L,int selling)
{
    const int top=Lua_GetTopIndex(L);int shop=0,good=0,amount=0,result=0;
    if(PhongThanGoodsArgs(L,&shop,&good) && PhongThanEventInt(L,3,selling?1:-1000,1000,&amount))
    {
        const int player=GetPlayerIndex(L);Lua_SetTopIndex(L,top);
        if(player>0 && player<MAX_PLAYER && Player[player].m_nIndex>0 && Player[player].m_nIndex<MAX_NPC)
            result=g_PhongThanCommerce.ChangeStock(shop,good,amount,(unsigned long)time(NULL));
    }
    Lua_PushNumber(L,result);return 1;
}
int LuaChangeGoodsNumCompat(Lua_State *L){return PhongThanChangeGoods(L,0);}
// Original Lua already removes cargo and credits cash before this notification.
int LuaPlayerSellGoodsCompat(Lua_State *L){return PhongThanChangeGoods(L,1);}

int LuaHaveHandItemCompat(Lua_State *L)
{
    const int top=Lua_GetTopIndex(L);
    const int player=GetPlayerIndex(L);
    Lua_SetTopIndex(L,top);
    int genre=-1,detail=-1,level=-1,series=-1,particular=-1,count=0;
    if(player>0 && player<MAX_PLAYER &&
        GetVngNormalTuple(L,1,&genre,&detail,&level,&series,&particular))
        count=CountVngOwnedItems(player,genre,detail,level,series,pos_hand,particular);
    Lua_PushNumber(L,count);
    return 1;
}

int LuaDelHandItemCompat(Lua_State *L)
{
    const int top=Lua_GetTopIndex(L);
    const int player=GetPlayerIndex(L);
    Lua_SetTopIndex(L,top);
    int genre=-1,detail=-1,level=-1,series=-1,particular=-1,removed=0;
    if(player>0 && player<MAX_PLAYER &&
        GetVngNormalTuple(L,1,&genre,&detail,&level,&series,&particular))
        removed=RemoveVngItems(player,genre,detail,level,series,pos_hand,FALSE,particular);
    Lua_PushNumber(L,removed);
    return 1;
}

int LuaLockCampCompat(Lua_State *L)
{
    const int top=Lua_GetTopIndex(L);
    int result=0;
    if(top==1 && Lua_IsNumber(L,1))
    {
        const int flag=(int)Lua_ValueToNumber(L,1);
        const int player=GetPlayerIndex(L);
        Lua_SetTopIndex(L,top);
        if(player>0 && player<MAX_PLAYER && (flag==0 || flag==1))
        {
            Player[player].ForbidCamp(flag!=0);
            result=1;
        }
    }
    Lua_PushNumber(L,result);
    return 1;
}

// GetNextPlayer(mission, cursor [, group]): return mission slot and player slot.
// Zero group iterates all participants. No global PlayerIndex mutation.
int LuaGetNextMissionPlayerCompat(Lua_State *L)
{
    unsigned long cursor=0,player=0;
    const int top=Lua_GetTopIndex(L);
    if(top>=2 && Lua_IsNumber(L,1) && Lua_IsNumber(L,2))
    {
        const int id=(int)Lua_ValueToNumber(L,1);
        const int previous=(int)Lua_ValueToNumber(L,2);
        const int group=top>=3?(int)Lua_ValueToNumber(L,3):0;
        Lua_GetGlobal(L,SCRIPT_SUBWORLDINDEX);
        const int world=lua_isnumber(L,-1)?(int)Lua_ValueToNumber(L,-1):-1;
        Lua_SetTopIndex(L,top);
        if(id>0 && previous>=0 && previous<MAX_PLAYER && group>=0 && group<=255 &&
            world>=0 && world<MAX_SUBWORLD && SubWorld[world].m_SubWorldID>=0)
        {
            KMission *mission=SubWorld[world].m_MissionArray.FindByMissionId(id);
            if(mission)
            {
                cursor=previous;
                while((cursor=mission->GetNextPlayer(cursor,player))!=0)
                {
                    const TMissionPlayerInfo &info=mission->m_MissionPlayer.m_Data[cursor];
                    if(player>0 && player<MAX_PLAYER && Player[player].m_nIndex>0 &&
                        Player[player].m_nIndex<MAX_NPC &&
                        Npc[Player[player].m_nIndex].m_SubWorldIndex==world &&
                        Player[player].m_dwID==info.m_ulPlayerID &&
                        (group==0 || info.m_ucPlayerGroup==group))
                        break;
                }
                if(!cursor)player=0;
            }
        }
    }
    Lua_PushNumber(L,cursor);Lua_PushNumber(L,player);
    return 2;
}
