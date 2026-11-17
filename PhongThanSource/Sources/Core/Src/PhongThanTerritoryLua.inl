// Trusted Lua entry points use the native nation identity and one receipt CAS.
#include "PhongThanTerritory.h"
#include "../../../Headers/PhongThanTerritoryState.h"
int LuaTerritoryCount(Lua_State* L){
#ifdef _SERVER
 Lua_PushNumber(L,PhongThanTerritoryCount());
#else
 Lua_PushNumber(L,0);
#endif
 return 1;
}
#ifdef _SERVER
static int LuaTerritoryValue(Lua_State* L){Lua_PushNumber(L,PhongThanTerritoryPlayerValue(GetPlayerIndex(L),(int)Lua_ValueToNumber(L,1)));return 1;}
static int LuaTerritoryGlory(Lua_State* L){Lua_PushNumber(L,PhongThanTerritoryPlayerValue(GetPlayerIndex(L),0));return 1;}
static int LuaTerritoryCreated(Lua_State* L){Lua_PushNumber(L,PhongThanTerritoryPlayerValue(GetPlayerIndex(L),1)>0?1:0);return 1;}
static int LuaTerritoryRight(Lua_State* L){int p=GetPlayerIndex(L);Lua_PushNumber(L,PhongThanNationDuty(p)==1?1:0);return 1;}
static int LuaTerritoryOpenCreate(Lua_State* L){PhongThanTerritoryOpen(GetPlayerIndex(L),0);return 0;}
static int LuaTerritoryOpenGate(Lua_State* L){PhongThanTerritoryOpen(GetPlayerIndex(L),1);return 0;}
static int LuaTerritoryOpenBuilding(Lua_State* L){PhongThanTerritoryOpen(GetPlayerIndex(L),2);return 0;}
static int LuaTerritoryAction(Lua_State* L){Lua_PushNumber(L,PhongThanTerritoryAction(GetPlayerIndex(L),(int)Lua_ValueToNumber(L,1),(int)Lua_ValueToNumber(L,2),0,""));return 1;}
static int LuaTerritoryUpgrade(Lua_State* L){Lua_PushNumber(L,PhongThanTerritoryAction(GetPlayerIndex(L),2,0,0,""));return 1;}
static int LuaTerritoryCityAt(Lua_State* L){char name[32]="";int level=0;int map=PhongThanTerritoryCityAt((int)Lua_ValueToNumber(L,1),name,level);Lua_PushNumber(L,map);Lua_PushString(L,name);Lua_PushNumber(L,level);return 3;}
static int LuaTerritoryExchange(Lua_State* L,bool create){int p=GetPlayerIndex(L),first=(int)Lua_ValueToNumber(L,1),second=(int)Lua_ValueToNumber(L,2),after=0,result=0;
 if(p<=0||p>=MAX_PLAYER){Lua_PushNumber(L,0);return 1;}
 PhongThanQuestExchangeRow needs[4];int count=0,cash=0;
 if(create){for(int i=0;i<4;++i){if(!PhongThanDecodeQuestItemTuple(3,58+i,0,0,0,0,needs[count].item)){Lua_PushNumber(L,0);return 1;}needs[count++].count=1;}}
 else if(first==1){int total=second==1?250:second==2?2500:0;if(!total){Lua_PushNumber(L,0);return 1;}int available[2]={0,0};KItemList& bag=Player[p].m_ItemList;
  for(PlayerItem* owned=bag.GetFirstItem();owned;owned=bag.GetNextItem()){int id=owned->nIdx;if(id<=0||id>=MAX_ITEM||!Item[id].GetID()||Item[id].GetLock()->IsLock()||Item[id].GetGenre()!=3)continue;
   if(owned->nPlace!=pos_equiproom&&owned->nPlace!=pos_equiproomex&&owned->nPlace!=pos_ptbag2&&owned->nPlace!=pos_ptbag3)continue;
   if(Item[id].GetDetailType()==6)available[0]+=Item[id].GetStackNum();else if(Item[id].GetDetailType()==3)available[1]+=Item[id].GetStackNum();
  }if(available[0]+available[1]<total){Lua_PushNumber(L,0);return 1;}
  for(int j=0;j<2&&total;++j){int amount=available[j]<total?available[j]:total;if(!amount)continue;PhongThanDecodeQuestItemTuple(3,j==0?6:3,0,0,0,0,needs[count].item);needs[count++].count=amount;total-=amount;}
 }
 int before=Player[p].m_cTask.GetSaveVal(PT_TERRITORY_RECEIPT_TASK);
 int amount=create?PhongThanTerritoryBeginCreate(p,first,second,before,after):PhongThanTerritoryBeginDonation(p,first,second,before,after);
 if((create&&amount>=0)||(!create&&amount>0)){
  cash=create||first==0?amount:0;
  if(!cash||Player[p].m_ItemList.DecMoney(cash)){
   if(PhongThanQuestExchange(p,PT_TERRITORY_RECEIPT_TASK,before,after,needs,count,0,0,false,0,true,0,true))result=PhongThanTerritoryFinishDonation(p,false)?1:2;
   else {if(cash)Player[p].m_ItemList.AddMoney(room_equipment,cash);PhongThanTerritoryCancelDonation(p,after);}
  }else PhongThanTerritoryCancelDonation(p,after);
 }
 Lua_PushNumber(L,result);return 1;
}
static int LuaTerritoryDonate(Lua_State* L){return LuaTerritoryExchange(L,false);}
static int LuaTerritoryCreate(Lua_State* L){return LuaTerritoryExchange(L,true);}

static int LuaTerritoryBookCount(Lua_State* L){Lua_PushNumber(L,PhongThanTerritoryBookCount(GetPlayerIndex(L),(int)Lua_ValueToNumber(L,1)));return 1;}
static int LuaTerritoryBook(Lua_State* L){int p=GetPlayerIndex(L),pool=(int)Lua_ValueToNumber(L,1),book=(int)Lua_ValueToNumber(L,2),after=0;if(p<=0||p>=MAX_PLAYER){Lua_PushNumber(L,0);return 1;}
 int before=Player[p].m_cTask.GetSaveVal(PT_TERRITORY_RECEIPT_TASK);if(!PhongThanTerritoryBeginBook(p,pool,book,before,after)){Lua_PushNumber(L,0);return 1;}
 PhongThanQuestExchangeRow row;memset(&row,0,sizeof(row));row.count=1;if(book)PhongThanDecodeQuestItemTuple(3,book,0,0,0,0,row.item);else PhongThanDecodeQuestItemTuple(8,pool?1422:193,5,0,0,0,row.item);
 int result=0;if(PhongThanQuestExchange(p,PT_TERRITORY_RECEIPT_TASK,before,after,&row,book?0:1,&row,book?1:0,true,0,true,0,true)){
  if(!book&&pool==0){int day=(int)(time(0)/86400);if(Player[p].m_cTask.GetSaveVal(892)!=day){Player[p].m_cTask.SetSaveVal(892,day,TRUE);Player[p].DirectAddExp(Npc[Player[p].m_nIndex].m_Level*5000);}}
  result=PhongThanTerritoryFinishDonation(p,false)?1:2;
 }else PhongThanTerritoryCancelDonation(p,after);
 Lua_PushNumber(L,result);return 1;
}
#endif
