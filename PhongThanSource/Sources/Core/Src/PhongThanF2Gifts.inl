// Exact clicked-item costs and all-or-nothing reward placement.
#include "../../../Headers/PhongThanF2GiftData.h"
int PhongThanUseF2Gift(int p,int item){
 if(item<=0||item>=MAX_ITEM||Item[item].GetGenre()!=item_ibitem||Item[item].GetParticular()!=2)return -1;
 const PTF2Gift* gift=0;for(int i=0;i<PT_F2_GIFT_COUNT;++i)if(ptF2Gifts[i].detail==Item[item].GetDetailType())gift=&ptF2Gifts[i];
 if(!gift)return -1;
 if(p<=0||p>=MAX_PLAYER||!PhongThanIsLivePlayer(p)||!PtTravel::Owned(p,item)||Npc[Player[p].m_nIndex].m_CurrentLife<=0)return 0;
 PhongThanQuestExchangeRow need,rewards[7];need.count=1;
 if(!PhongThanDecodeQuestItemTuple(8,gift->detail,2,0,0,0,need.item))return 0;
 for(i=0;i<gift->count;++i){const PTF2Reward& r=gift->rewards[i];rewards[i].count=r.count;
  if(!PhongThanDecodeQuestItemTuple(r.genre,r.detail,r.part,r.level,0,0,rewards[i].item))return 0;}
 int old=Player[p].m_cTask.GetSaveVal(4897);
 int result=PhongThanQuestExchange(p,4897,old,old==0?1:0,&need,1,rewards,gift->count,true,0,true,item);
 Player[p].SendSystemMessage(MESSAGE_SYSTEM_ANNOUCE_HEAD,result==1?ptF2GiftSuccess:ptF2GiftFull);
 return result;
}
int LuaUseF2Gift(Lua_State* L){
 int result=0;if(Lua_GetTopIndex(L)==1&&Lua_IsNumber(L,1)){
  double item=Lua_ValueToNumber(L,1);if(item>0&&item<MAX_ITEM&&item==(int)item)result=PhongThanUseF2Gift(GetPlayerIndex(L),(int)item)>0?1:0;
 }Lua_PushNumber(L,result);return 1;
}
